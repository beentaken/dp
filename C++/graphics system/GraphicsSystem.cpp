/******************************************************************************/
/*!
\file		GraphicsSystem.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GraphicsSystem.h"
#include "Components\ModelRenderer.h"
#include "Components\Transform.h"
#include "Cameras\FPSCamera.h"
#include "Cameras\EditorCamera.h"
#include "Window\GameWindow.h"
#include "Utility\ResourceManager.h"
#include "Utility\Logging.h"
#include "Lighting\LightingManager.h"
#include "Editor\Editor.h"
#include "Core\Level.h"
#include "Lighting\ShadowMapping.h"
#include "DebugDraw\DebugCollider.h"
#include "Cameras\CameraManager.h"
#include "Utility\Threading.h"
#include "Utility\Functor.h"
#include "Skybox.h"
#include "Billboards\BillboardManager.h"
#include "UI\UIRenderManager.h"
#include "Utility\Functor.h"
#include "UI\2DAnimationLoader.h"
#include "Environment\WaterRenderer.h"

/////////////
// GLOBALS //
/////////////
//near and far plane
static const float SCREEN_FAR = 20000.f; // 100000.0f;
static const float SCREEN_NEAR = 1.f; //0.1f;
static const size_t MAX_BONES = 128;                //should be same as shader

static const Vec3 level_position(-400.f, 0.0f, 400.f);
static const Vec3 level_scale(1.f, 1.f, 1.f);
static const Vec3 level_rotate = Vec3();
static const Vec3 bgPosition(-400.f, 0.f, 400.f);
static const Vec3 bgRotate = Vec3();
static const Vec3 bgScale(80.f, 80.f, 80.f);



//Image directory
const std::string directory = "Assets/";

GraphicsSystem::GraphicsSystem() : modelList(ResourceManager::GetInstance().GetModelList()), m_lighting(false), windowx(0), windowy(0), m_loading(false) {}

//Init system and save world position edges
void GraphicsSystem::Init()
{
  //init pointers
  m_window = &(GameWindow::GetInstance());
  ModelRenderer::s_pGraphics = this;

  //get device context
  m_deviceContext = m_window->GetDeviceContext();

  //init camera manager
  m_camMgr = &(CameraManager::GetInstance());
  m_camMgr->Init();

// #define _EDITOR
#ifdef _EDITOR
  m_camMgr->SetActiveCamera("Editor");
#else
  m_camMgr->SetActiveCamera("FPS");
#endif

  //init camera
  m_camMgr->GetActiveCamera()->Init();

  // Moved up init systems for draw
  //init font systems
  FontManager::GetInstance().Init();
  //init ui render manager
  m_uiMgr = &(UIRenderManager::GetInstance());
  m_uiMgr->Init();

  //Model textures must be loaded before model
  LoadModelTextures(); // 2,000 ms
  LoadModels(); // 10,000 ms

  //init tex manager
  TextureManager::GetInstance().LoadTextures(); // 9,000 ms

  //construct and compile main shaders
  m_renderShader = AddShader("toon", "animated.vert", "toon.frag");
  m_debugShader = AddShader("debugdraw", "debug.vert", "debug.frag");
  m_outlineShader = AddShader("outline", "outline.vert", "outline.frag");

  //load 2d animations
  AnimationLoader::GetInstance().LoadSpriteSheets();

  //init billboard manager
  m_bbMgr = &(BillboardManager::GetInstance());
  m_bbMgr->Init();

  //init instance mgr
  m_instMgr = &(InstanceManager::GetInstance());
  m_instMgr->Init();

  //init light manager
  LightingManager::GetInstance().Init();

  //create shadow map
  ShadowMap::GetInstance().Init(m_aspectRatio, m_camMgr->GetActiveCamera()->GetFOV(), SCREEN_NEAR);
  m_shadowMap = &(ShadowMap::GetInstance());

  //create screen texture
  InitScreenTex();

  //resize viewport after making screen texture
  ResizeViewport(m_window->GetWidth(), m_window->GetHeight());


  //init point renderer
  m_pointRenderer = &PointRenderer::GetInstance();
  m_pointRenderer->Init();

  //create skybox
  m_skybox = Construct<Skybox>();
  m_skybox->Init();

  //init water renderer
  WaterRenderer::GetInstance().Init();
}

void GraphicsSystem::InitScreenTex()
{
  //quad that fills the entire screen in NDC
  float quadVertices[] = 
  {
    // pos          //UV
    -1.0f,  1.0f,   0.0f, 1.0f,
    -1.0f, -1.0f,   0.0f, 0.0f,
    1.0f, -1.0f,    1.0f, 0.0f,

    -1.0f,  1.0f,   0.0f, 1.0f,
    1.0f, -1.0f,    1.0f, 0.0f,
    1.0f,  1.0f,    1.0f, 1.0f
  };

  //initialize vertex data
  glGenVertexArrays(1, &m_screenQuadVAO);
  glGenBuffers(1, &m_screenQuadVBO);
  glBindVertexArray(m_screenQuadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_screenQuadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  //initialize render texture
  m_screenTex.Init(m_window->GetWidth(), m_window->GetHeight());

  //create shaders
  m_screenShader = AddShader("screen", "screen.vert", "screen.frag");
}

void GraphicsSystem::LightingUpdate()
{
  //check for active lights
  if(!m_lighting)
    LightingManager::GetInstance().Update();
}

void GraphicsSystem::Update()
{
  //update camera
  m_camMgr->GetActiveCamera()->Update();

  //update skybox
  //m_skybox->Update();

  if (!m_lighting)
    m_lighting = true;
  else
    LightingManager::GetInstance().Update();
}

void GraphicsSystem::Draw()
{
#ifdef _EDITOR
  TimeTracker& timeTracker = TimeTracker::GetInstance();
  timeTracker.TrackStart("Rendering");
#endif
  LightingManager& lightMgr = LightingManager::GetInstance();
  //add to actual drawlist
  RenderInfo info;
  while (m_tempDrawList.Pop(info))
  {
    if (info.highlight)
      m_outlinedObjects.push_back(info);

    m_actualDrawList.push_back(info);
  }

  //sort before rendering
  SortForAlphaBlending();

  //get current field of view
  float fov = m_camMgr->GetActiveCamera()->GetFOV();

  //first pass: render shadows
  if(lightMgr.CheckIfDirectionalLightExist())
    m_shadowMap->RenderShadows(*lightMgr.GetDirectionalLight(), fov, m_aspectRatio);

  //compute matrices
  m_projMtx = Mtx4::glperspective(fov, m_aspectRatio, SCREEN_NEAR, SCREEN_FAR);
  m_viewMtx = m_camMgr->GetCurrentViewMatrix();
#ifndef SCREEN_TEXTURE    
  //reset viewport before second pass
  glViewport(0, 0, m_window->GetWidth(), m_window->GetHeight());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  //Render debug colliders
  RenderDebugColliders();

  //render skybox (if skybox got problem move it to b4 ui)
  m_skybox->Draw(m_projMtx, m_viewMtx);

  //send scene properties, get shadow map and then render the scene
  SendSceneInfoToShader(*m_renderShader);
  
  //normal render pass
  RenderScene(*m_renderShader, RenderPass::Normal);

  //render points
  if (m_pointRenderer->hasData())
  {
    Mtx4 m_viewProj = m_projMtx * m_viewMtx;
    m_pointRenderer->Draw(m_viewProj);
  }

  //render billboards
  m_bbMgr->Draw(m_projMtx, m_viewMtx);

  //render instances
  m_instMgr->Draw(m_projMtx, m_viewMtx);

  //render font, ui and other 2D objects last
  m_uiMgr->Draw();
#else
  //update movement of water and setup reflection and refraction
  WaterRenderer::GetInstance().Setup();

  //render to screen texture
  m_screenTex.Bind();

  /******* RENDERING THE ENTIRE GAME **************************************************/

  //Render debug colliders
  RenderDebugColliders();

  //render skybox last b4 UI(if skybox got problem move it to b4 ui)
  RenderSkybox();

  //render water before skybox
  if (lightMgr.CheckIfDirectionalLightExist())
	WaterRenderer::GetInstance().Draw(m_viewMtx, m_projMtx, *lightMgr.GetDirectionalLight(), m_camMgr->GetActiveCamera()->GetPosition());

  //send scene properties, get shadow map and then render the scene
  SendSceneInfoToShader(*m_renderShader);
  RenderScene(*m_renderShader, RenderPass::Normal);

  //render points
  if (m_pointRenderer->hasData())
  {
    Mtx4 m_viewProj = m_projMtx * m_viewMtx;
    m_pointRenderer->Draw(m_viewProj);
  }

  //render billboards
  m_bbMgr->Draw(m_projMtx, m_viewMtx);

  //render instances
  m_instMgr->Draw(m_projMtx, m_viewMtx);

  //render font, ui and other 2D objects last
  m_uiMgr->Draw();

  /*************************************************************************************/
  m_screenTex.Unbind();
  RenderScreenQuad();

  //debug draw reflection and refraction
  //WaterRenderer::GetInstance().DebugDraw();

#endif
  //clear all containers
  m_actualDrawList.clear();
  m_sortedObjects.clear();
  m_outlinedObjects.clear();
#ifdef _EDITOR
  timeTracker.TrackEnd("Rendering");
#endif
}

void GraphicsSystem::AddToDrawList(const RenderInfo& info)
{
  m_tempDrawList.Push(info);
}

void GraphicsSystem::AddToDebugDrawList(const DebugRenderInfo& data)
{
  m_debugDrawList.Push(data);
}

void GraphicsSystem::SortForAlphaBlending()
{
  m_sortedObjects.reserve(m_actualDrawList.size());
  Vec3 cameraPos = CameraManager::GetInstance().GetActiveCamera()->GetPosition();
  Vec3 normCamForward = Vec3Normalise(CameraManager::GetInstance().GetActiveCamera()->GetCamForward());
  static std::vector<Vec3> bounding_box
  {
    Vec3(-0.5f, 0.5f, 0.5f),     //top left
    Vec3(-0.5f, -0.5f, 0.5f),     //bottom left
    Vec3(0.5f, 0.5f, 0.5f),       //top right
    Vec3(0.5f, -0.5f, 0.5f),      //bottom right
    Vec3(-0.5f, 0.5f, -0.5f),      //top left
    Vec3(-0.5f, -0.5f, -0.5f),     //bottom left
    Vec3(0.5f, 0.5f, -0.5f),       //top right
    Vec3(0.5f, -0.5f, -0.5f)      //bottom right
  };

  for (auto& elem : m_actualDrawList)
  {
    std::vector<float> distVec(8);

    //calculate distance from camera to each vertex of object's AABB
    for (unsigned i = 0; i < 8; ++i)
    {
      Vec4 vertPos = elem.modelMatrix * Vec4(bounding_box[i].x, bounding_box[i].y, bounding_box[i].z, 1.f);
      distVec[i] = Vec3DotProduct(normCamForward, vertPos.ToVec3() - cameraPos);
    }

    //store the maximum distance
    SortedObj obj;
    obj.dist = *std::max_element(distVec.begin(), distVec.end());
    obj.ri = &elem;
    m_sortedObjects.push_back(obj);
  }

  //sorts all objects based on distance to camera to fix alpha issues
  std::sort(m_sortedObjects.begin(), m_sortedObjects.end(),
    [&](const SortedObj& lhs, const SortedObj& rhs)
    {
      return lhs.dist > rhs.dist;
    });
}


void GraphicsSystem::RenderScene(const Shader& shader, RenderPass pass)
{
  //render transparent objects from back to front
  for (auto& elem : m_sortedObjects)
  {
    //optimization: dont render into shadowmap if not casting shadows

    if (pass == RenderPass::Shadow && elem.ri->castshadow == false)
      continue;

    //apparently need this clear call to actually clear the stencil(clear before drawing each model)
    if (pass != RenderPass::Shadow)
    {
      glClear(GL_STENCIL_BUFFER_BIT);
    }

    //draws the model
    RenderModel(shader, *elem.ri, pass);

    //if object is outlined
    if (elem.ri->highlight && pass != RenderPass::Shadow)
    {
      RenderOutlinedModel(*elem.ri);
    }
  }
}

void GraphicsSystem::RenderDebugColliders()
{
  Mtx4 projView = m_projMtx * m_viewMtx;
  m_debugShader->SetMatrix4("VP", projView, true);
  DebugRenderInfo info;
  while (m_debugDrawList.Pop(info))
  {
    m_debugShader->SetMatrix4("model", info.modelMatrix, true);
    info.debugcollider->Draw();
  }
}

void GraphicsSystem::RenderScreenQuad()
{
  // glViewport(0, 0, m_window->GetWidth(), m_window->GetHeight());
  glViewport(windowx, windowy, m_window->GetWidth(), m_window->GetHeight());        //change this to change screen area
  //glViewport(500, 300, 400, 300);

  glDisable(GL_DEPTH_TEST);         //disable depth test so screen space quad isnt discarded due to depth test
  glDisable(GL_STENCIL_TEST);       //disable stencil testing
  glClear(GL_COLOR_BUFFER_BIT);     //clear color before sampling screen texture

  //use screen shader
  m_screenShader->Use();
  m_screenShader->SetBool("debug", false);
  glActiveTexture(GL_TEXTURE0);
  m_screenShader->SetInt("screenTexture", 0);


  //draw the texture over the screen
  glBindVertexArray(m_screenQuadVAO);
  glBindTexture(GL_TEXTURE_2D, m_screenTex.GetTexture());
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

  //enable depth and stencil test again
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
}


//creates the letter box if aspect ratio is not 16:9
void GraphicsSystem::ResizeViewport(unsigned width, unsigned height)
{
#if 1 // Not working correctly
  static const float targetAspect = 16.f / 9.f;
  int inwidth = width;
  int inheight = (int)(inwidth/targetAspect);
  static GameWindow & gw = GameWindow::GetInstance();  

  if (inheight > (int)height)
  {
    //It doesn't fit our height, we must switch to pillarbox then
    inheight = height;
    inwidth = (int)(inheight * targetAspect);
  }

  // set up the new viewport centered in the backbuffer
  windowx = (width - inwidth) / 2;
  windowy = (height - inheight) / 2;

  //update aspect ratio
  m_aspectRatio = static_cast<float>(inwidth) / static_cast<float>(inheight);

  //Set the viewport to center at screen width and height
  gw.Resize(inwidth, inheight);
  glViewport(windowx, windowy, inwidth, inheight);
#else
  GameWindow::GetInstance().Resize(width, height);
  glViewport(0, 0, width, height);
#endif
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GraphicsSystem::RenderModel(const Shader & shader, const RenderInfo& info, RenderPass pass)
{
  //send bone matrices to shader
  std::vector<Mtx4>& finalTransforms = *(info.boneMatrices);
  size_t numBones = finalTransforms.size();
  assert(numBones < MAX_BONES);
  for (unsigned i = 0; i < numBones; ++i)
  {
    shader.SetMatrix4(std::string("gBones") + "[" + std::to_string(i) + "]", finalTransforms[i], true);
  }

  //is model animated?
  shader.SetBool("u_animated", info.useBoneMatrices);

  //send values to shader
  shader.SetMatrix4("model", info.modelMatrix, true);
  shader.SetMatrix3("normalMatrix", info.normalMatrix, true);
  shader.SetVec2("u_tileRepeat", info.tileRepeat);
  shader.SetVec4("u_tintColor", info.tintColor);
  shader.SetFloat("u_tintFactor", info.tintFactor);
  shader.SetFloat("u_alpha", info.alpha);
  shader.SetFloat("u_emiFactor", info.emiFactor);
  shader.SetBool("cast_shadow", info.castshadow);
  shader.SetBool("rim_enabled", info.useRim);
  shader.SetVec3("rim_color", info.rimColor);
  shader.SetVec3("u_emissive", info.emissive);

  GLuint mask = (info.highlight && pass != RenderPass::Shadow) ? 0xFF : 0x00;
  glStencilMask(mask);
  
  info.modelptr->Draw(shader);
}

void GraphicsSystem::RenderSkybox()
{
  //render skybox (if skybox got problem move it to b4 ui)
  m_skybox->Draw(m_projMtx, m_viewMtx);
}

void GraphicsSystem::RenderSkyboxForWaterReflection(const Mtx4 & waterCamMtx)
{
  m_skybox->Draw(m_projMtx, waterCamMtx);
}

void GraphicsSystem::RenderOutlinedModel(const RenderInfo & info)
{
  m_outlineShader->SetMatrix4("projection", m_projMtx, true);
  m_outlineShader->SetMatrix4("view", m_viewMtx, true);

  //passes if value in stencil is not 1, which will only be true for the difference in sizes,
  //making it look like outlines
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

  //disable stencil buffer writing and depth test
  glStencilMask(0x00);
  glDisable(GL_DEPTH_TEST);

  m_outlineShader->SetMatrix4("model", info.modelMatrix, true);
  m_outlineShader->SetFloat("thickness", info.outlineThickness);
  m_outlineShader->SetVec4("line_color", info.highlightColor);

  info.modelptr->DrawWithoutTexture(*m_outlineShader);

  //reset gl state
  glEnable(GL_DEPTH_TEST);
  glStencilMask(0xFF);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
}


void GraphicsSystem::SendSceneInfoToShader(const Shader & shader)
{
  //send values common to scene to shader
  Mtx4 lightSpaceMtx = m_shadowMap->GetLightSpaceMatrix();
  shader.SetVec3("viewPosition", m_camMgr->GetActiveCamera()->GetPosition());
  shader.SetMatrix4("view", m_viewMtx, true);
  shader.SetMatrix4("projection", m_projMtx, true);
  shader.SetMatrix4("lightSpaceMatrix", lightSpaceMtx, true);
  //send lighting info to shader
  LightingManager::GetInstance().SendAllLightPropertiesGL(shader);

  //send shadowmap for this frame to shader (4th texture unit)
  glActiveTexture(GL_TEXTURE0 + 4);
  shader.SetInt("u_shadowmap", 4);
  GLuint shadowmap = m_shadowMap->GetShadowMap();
  glBindTexture(GL_TEXTURE_2D, shadowmap);
}


void GraphicsSystem::StartFrame()
{
  // Set the color to clear the screen to.
  glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, m_backgroundColor.w);

	// Clear the screen and depth buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GraphicsSystem::EndFrame()
{
	// Present the back buffer to the screen since rendering is complete.
	SwapBuffers(m_deviceContext);
}

void GraphicsSystem::Terminate()
{
  //delete buffers for all models
  for (auto& elem : modelList)
  {
    elem.second.Terminate();
  }

  //call shutdown on mgrs
  TextureManager::GetInstance().Terminate();
  LightingManager::GetInstance().Terminate();
  m_camMgr->Terminate();
  m_bbMgr->Terminate();

  //call shutdown on graphic subsystems
  m_shadowMap->Terminate();
  m_skybox->Terminate();
  m_uiMgr->Terminate();
  //m_fontRenderer->Terminate();
  m_pointRenderer->Terminate();

  Destruct(m_skybox);

  //delete shaders
  for (auto& elem : shaderList)
  {
    glDeleteProgram(elem.second.m_id);
  }

  //delete screen quad
  glDeleteVertexArrays(1, &m_screenQuadVAO);
  glDeleteBuffers(1, &m_screenQuadVBO);
}

bool GraphicsSystem::LoadingDone() const
{
  return m_loading;
}

void GraphicsSystem::SetLoadingDone()
{
  m_loading = true;
  GameWindow::GetInstance().ChangeToResizableWindowAfterLoading();
}

Shader* GraphicsSystem::AddShader(const std::string & name, const char * vert, const char * frag)
{
	auto& result = shaderList.insert(std::make_pair(name, Shader(vert, frag)));
  Assert("Failed to add shader", result.second, ErrorLevel::SystemCrash);
  return &result.first->second;
}

Shader* GraphicsSystem::GetShader(const std::string & name)
{
	ShaderList::iterator it = shaderList.find(name);
	Assert("Unable to find shader", it != shaderList.end(), ErrorLevel::SystemCrash);
	return &it->second;
}

Model* GraphicsSystem::AddModel(const std::string & path)
{
	std::string file = path.substr(path.find_last_of("/") + 1, path.length());
	modelNames.emplace_back(file);
  //prevents model destructor from being called
  auto& result = modelList.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(file),
    std::forward_as_tuple(path, file));
	Assert("Unable to add model in GraphicsSystem::AddModel()", result.second);
	return &(result.first->second);
}

Model * GraphicsSystem::GetModel(const std::string & path)
{
	ModelList::iterator it = modelList.find(path);
	Assert("Unable to find model at " + path, it != modelList.end(), ErrorLevel::SystemCrash);
	return &it->second;
}


void GraphicsSystem::SetBackgroundColor(float r, float g, float b, float a)
{
	m_backgroundColor = Vec4(r, g, b);
}

Vec2 GraphicsSystem::ConvertWorldToScreen(const Vec3& worldPos) const
{

	Vec4 pos = Vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
	Mtx4 matProjection = m_projMtx * m_viewMtx;

	pos = matProjection * pos;
	pos /= pos.w;

	int winX = static_cast<int>((pos.x + 1.f) / 2.f * m_window->GetWidth());
	int winY = static_cast<int>((1.f - pos.y) / 2.f * m_window->GetHeight());

	return Vec2((float)winX, (float)winY);
}

Vec3 GraphicsSystem::ConvertScreenToWorld(const Vec2& screenpos) const
{
  //reference: https://stackoverflow.com/questions/7692988/opengl-math-projecting-screen-space-to-world-space-coords

  Vec4 pos;
  Mtx4 matProjection = m_projMtx * m_viewMtx;

  //inverse matrix to unproject
  Mtx4 matInverse = matProjection.inverse();

  Vec4 v;

#define NEARPLANE -1.f //z = -1.f

  //x = mouseposition.x within a range of window x - transform to values between - 1 and 1

  //y = mouseposition.y within a range of window y - transform to values between - 1 and 1 - remember to invert mouseposition.y if needed

  //transform screenpos x and y to NDC
  v.x = (2.0f*((float)(screenpos.x) / (m_window->GetWidth()))) - 1.0f,
  v.y = 1.0f - (2.0f*((float)(screenpos.y) / (m_window->GetHeight())));
  v.z = -1.f;//2.0* NEARPLANE - 1.0;
  v.w = 1.0;

#undef NEARPLANE

  pos = matInverse * v;

  pos.w = 1.0f / pos.w;

  pos.x *= pos.w;
  pos.y *= pos.w;
  pos.z *= pos.w;

  return Vec3(pos);
}

Vec2 GraphicsSystem::GetCurrentMousePosition() const
{
  POINT mousePos;
  GetCursorPos(&mousePos);
  ScreenToClient(m_window->GetWindowHandle(), &mousePos);
  return Vec2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

float GraphicsSystem::GetValueAtScreenPosition(const Vec2 & screenpos, GLenum valType) const
{
  float value;
  glReadPixels((int)screenpos.x, (int)screenpos.y, 1, 1, valType, GL_FLOAT, &value);
  return value;
}

float GraphicsSystem::GetNearPlane() const
{
  return SCREEN_NEAR;
}

float GraphicsSystem::GetFarPlane() const
{
  return SCREEN_FAR;
}

Skybox & GraphicsSystem::GetSkybox()
{
  return *m_skybox;
}


bool GraphicsSystem::CheckDone(bool blocking)
{
  // Loops until render is done
  while (blocking && !thd.Ready())
    std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // thread yield
  return thd.Ready();
}

void GraphicsSystem::StartDraw(bool blocking)
{
  if (blocking)
    Draw();
  else if (CheckDone(true))
  {// Graphics needs to called in the main thread
    thd = std::move(Dispatcher::PrepareThreadData(SingletonCaller<GraphicsSystem>, &GraphicsSystem::Draw));
    thd.Run();
  }
}

std::string GraphicsSystem::GetModelNamesStr() const
{
	return modelNamesStr;
}

std::vector<std::string>& GraphicsSystem::GetModelNames()
{
	return modelNames;
}

void Load(Model * mptr)
{
  mptr->LoadModel();
}

//helper function to load all texture files in a model folder
void LoadTextureInFolder(std::string const & dir)
{
  static TextureManager & tMgr = TextureManager::GetInstance();

  HANDLE hFind;
  WIN32_FIND_DATA data;
  std::string path = dir;
  path += "/*.dds";
  hFind = FindFirstFile(path.c_str(), &data);
  if (hFind != INVALID_HANDLE_VALUE)
  {// No error checking
    do {
      std::string filename(data.cFileName);
      tMgr.LoadModelTexture(filename, dir);
    } while (FindNextFile(hFind, &data));
  }
}

//load all model textures in Models folder
void GraphicsSystem::LoadModelTextures()
{
  HANDLE hFind;
  WIN32_FIND_DATA data;

  std::string path(MODELS_DIR);
  path += "*";
  hFind = FindFirstFile(path.c_str(), &data);

  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      if (data.cFileName[0] != '.')
      {
        std::string foldername(data.cFileName);
        size_t found = foldername.find(".");
        if (found == std::string::npos)
        {// Check if the folder has a dds
          LoadTextureInFolder(MODELS_DIR + foldername);
          // Advance loading animation and render to screen
          UpdateLoadingScreen();
        }
      }
    } while (FindNextFile(hFind, &data));
    FindClose(hFind);
  }
}

void GraphicsSystem::LoadModels()
{
  HANDLE hFind;
  WIN32_FIND_DATA data;

  std::string path(MODELS_DIR);
  path += "*";
  hFind = FindFirstFile(path.c_str(), &data);

  // Vector of threads
  std::vector<Dispatcher::ThreadHandle> thds;
  thds.resize(std::thread::hardware_concurrency());

  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      if (data.cFileName[0] != '.')
      {
        std::string filename(data.cFileName);
        size_t found = filename.find('.');
        if (found == std::string::npos)
        {
          Model * mptr = LoadModel(MODELS_DIR + filename + "/");
          bool wait = true;
          while (wait)
          {
            for (auto & elem : thds)
            {
              if (elem.Ready())
              {
                elem = std::move(Dispatcher::PrepareThreadData(&Load, mptr));
                elem.Run();
                wait = false;
                break;
              }
            }
            if (wait)
            {
              // Advance loading animation and render to screen
              UpdateLoadingScreen();
              std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            }
          }
        }
      }
    } while (FindNextFile(hFind, &data));
    FindClose(hFind);
  }
  // PickupThreads
  for (auto & elem : thds)
  {
    while (!elem.Ready())
    {
      UpdateLoadingScreen();
      std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }
  }
  for (auto & elem : modelList)
  {
    elem.second.LoadMaterials();
    elem.second.SendToGL();
    // Advance loading animation and render to screen
    UpdateLoadingScreen();
  }
  GenerateModelNamesStr();
}

// Assumes 1 file and lmf extension
Model * GraphicsSystem::LoadModel(std::string const& dir)
{
  Model * result = nullptr;
  HANDLE hFind;
  WIN32_FIND_DATA data;
  std::string path = dir;
  path += "*.lmf";
  hFind = FindFirstFile(path.c_str(), &data);
  if (hFind != INVALID_HANDLE_VALUE)
  {// No error checking
    std::string filename(data.cFileName);
    result = AddModel(dir + filename);
  }
  return result;
}

void GraphicsSystem::GenerateModelNamesStr()
{
  modelNamesStr.clear();
  for (auto& str : modelNames)
  {
    modelNamesStr += str + '\0';
  }
}

namespace LoadingDetails
{ // Loading screen data

  Animator2D loading_;
  Animator2D flavour_;
  std::chrono::time_point<std::chrono::high_resolution_clock> tp;
  UIRenderData data;
  bool lready = false;
  unsigned state = 0; // Set animation based on state
  int counter = 0;
}

void GraphicsSystem::InitLoadScreen()
{
  const char filename[12] = "Loading.dds";
  const char xmlname[12] = "Loading.xml";
  // Load loading screen spritesheet
  {
    AnimationLoader & al = AnimationLoader::GetInstance();
    Texture* loadedTex = TextureManager::GetInstance().LoadSpriteSheetTexture(filename, LOADING_DIR);
    
    DataConverter dc;
    if (dc.Open(LOADING_DIR + xmlname))
      LoadingDetails::lready = true;
    else
      return;
    AnimationContainer * acptr = nullptr;
    float height;
    float width;
    dc.GetVar("width", width);
    dc.GetVar("height", height);
	  std::string curraniname = "";
    auto range = dc.GetChildren();
    while (range.first != range.second)
    {
      auto& itemDc = *range.first++;
      std::string tmp;
	    //get animation name
	    itemDc.GetVar("n", tmp);
	    tmp = tmp.substr(0, tmp.find_first_of('%'));
	    if (curraniname != tmp)
	    {
		    acptr = al.CreateAnimationContainer(tmp, width, height, loadedTex->id);
		    curraniname = tmp;
	    }
	    //add in every frame in the animation
	    al.ReadSingleImage(acptr, itemDc, width, height);
    }
    // Run splashscreen animation before loading?
  }
  LoadingDetails::loading_.SetAnimation("Splash_Logo");
  LoadingDetails::flavour_.SetAnimation("LoadingScreenFlavour");
  /*
  // Set fixed settings
  Vec2 displayratio = GameWindow::GetInstance().GetWindowToDisplayRatio();
  LoadingDetails::data.modelMatrix = Mtx4::translate(Vec3{ 0.f * displayratio.x, -200.f * displayratio.y, 0.f }) * Mtx4::scale(Vec3{ 640.f * displayratio.x, 320.f * displayratio.y, 1.f });
  LoadingDetails::data.color = Vec4(0.f, 0.f, 0.f, 1.f);
  LoadingDetails::data.tintFactor = 0.f;
  LoadingDetails::data.isFont = false;
  LoadingDetails::data.is_animated = true;
  */
  Vec2 displayratio = GameWindow::GetInstance().GetWindowToDisplayRatio();
  LoadingDetails::data.modelMatrix = Mtx4::translate(Vec3{ 0.f , 0.f , 0.f }) * Mtx4::scale(Vec3{ 1600.f * displayratio.x, 900.f * displayratio.y, 1.f });
  LoadingDetails::data.color = Vec4(0.f, 0.f, 0.f, 1.f);
  LoadingDetails::data.tintFactor = 0.f;
  LoadingDetails::data.isFont = false;
  LoadingDetails::data.is_animated = true;
  LoadingDetails::data.depth = -1.f;
  // Set correct texture
  FrameData frame = LoadingDetails::loading_.GetCurrentFrame();
  LoadingDetails::data.uv_offset = frame.uv;
  LoadingDetails::data.uv_origin = frame.origin;
  LoadingDetails::data.texture = frame.texture;
}

void GraphicsSystem::UpdateLoadingScreen()
{
  static UIRenderManager & uir = UIRenderManager::GetInstance();
  static InputManager & im = InputManager::GetInstance();
  if (!LoadingDetails::lready)
    return;
  auto currtp = std::chrono::high_resolution_clock::now();
  // Check if frame should be updated
  if (std::chrono::duration<double>(currtp - LoadingDetails::tp).count() > 0.1)
  {
    if (im.Update())
      exit(0);// throw("Exit on loading");
    if (LoadingDetails::state < 3 && (im.state.IsKeyPressed(InputManager::B_A) || im.state.IsKeyPressed(InputManager::B_Start) ||
      im.IsKeyPressed(VK_ESCAPE) || im.IsKeyPressed(VK_SPACE) || im.IsKeyPressed(VK_RETURN) || 
      im.IsKeyPressed(VK_LBUTTON) || im.IsKeyPressed(VK_RBUTTON)))
      LoadingDetails::state = 3;
    StartFrame();
    // Update frame
    LoadingDetails::tp = currtp;
    if (LoadingDetails::state < 3)
    {
      ++LoadingDetails::counter;
      if (LoadingDetails::counter < 10)
      {// 1 secs
        LoadingDetails::data.color = Vec4(0.f, 0.f, 0.f, LoadingDetails::counter / 10.f);
      }
      else if (LoadingDetails::counter < 40)
      {// Display for 3 secs
        LoadingDetails::data.color = Vec4(0.f, 0.f, 0.f, 1.f);
      }
      else if (LoadingDetails::counter < 50)
      {// 1 secs
        LoadingDetails::data.color = Vec4(0.f, 0.f, 0.f, 5.f + (LoadingDetails::counter / -10.f));
      }
      else// 2 secs?
      {
        LoadingDetails::data.color = Vec4(0.f, 0.f, 0.f, 0.f);
        {// Change to same animation, shift frames instead
          LoadingDetails::loading_.SetFrameNumber(++LoadingDetails::state);
          FrameData frame = LoadingDetails::loading_.GetCurrentFrame();
          LoadingDetails::data.uv_offset = frame.uv;
          LoadingDetails::data.uv_origin = frame.origin;
          LoadingDetails::data.texture = frame.texture;
        }
        LoadingDetails::counter = 0;
      }
      uir.AddToDrawList(LoadingDetails::data);
    }
    else if (LoadingDetails::state == 3)
    {// Init Loading
      LoadingDetails::loading_.SetAnimation("LoadingScreen");
      Vec2 displayratio = GameWindow::GetInstance().GetWindowToDisplayRatio();
      LoadingDetails::data.modelMatrix = Mtx4::translate(Vec3{ 0.f * displayratio.x, -200.f * displayratio.y, 0.f }) * Mtx4::scale(Vec3{ 640.f * displayratio.x, 320.f * displayratio.y, 1.f });
      LoadingDetails::data.color = Vec4(0.f, 0.f, 0.f, 1.f);
      ++LoadingDetails::state;
    }
    else
    {// Loading screen
      LoadingDetails::loading_.MoveFrames(1);
      if (++LoadingDetails::counter > 30)
      { // Change text
        LoadingDetails::flavour_.MoveFrames(1);
        LoadingDetails::counter = 0;
      }
      // Send image data
      FrameData frame = LoadingDetails::loading_.GetCurrentFrame();
      LoadingDetails::data.uv_offset = frame.uv;
      LoadingDetails::data.uv_origin = frame.origin;
      LoadingDetails::data.texture = frame.texture;
      LoadingDetails::data.depth = -0.9f;
      uir.AddToDrawList(LoadingDetails::data);

      // Send text data
      frame = LoadingDetails::flavour_.GetCurrentFrame();
      LoadingDetails::data.uv_offset = frame.uv;
      LoadingDetails::data.uv_origin = frame.origin;
      LoadingDetails::data.texture = frame.texture;
      LoadingDetails::data.depth = -1.f;
      uir.AddToDrawList(LoadingDetails::data);

    }
    uir.Draw();
    EndFrame();
  }
}
