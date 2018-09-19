/******************************************************************************/
/*!
\file		WaterRenderer.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "WaterRenderer.h"
#include "Graphics\Window\GameWindow.h"
#include "Graphics\TextureManager.h"
#include "Graphics\GraphicsSystem.h"
#include "Graphics\Lighting\LightTypes.h"
#include "Graphics\Cameras\CameraManager.h"
#include "Utility\FrameRateController.h"
#include "Utility\Logging.h"

#define MAX_WATER_LOCATIONS 100
#define WAVE_SPEED 0.03f

//#define ENABLE_REFRACTION

void WaterRenderer::Init()
{
  m_waterObjects.reserve(MAX_WATER_LOCATIONS);
  m_hasWater = false;

  m_waterShader = GraphicsSystem::GetInstance().AddShader("water", "water.vert", "water.frag");
  m_objectsShader = GraphicsSystem::GetInstance().GetShader("toon");
  m_screenShader = GraphicsSystem::GetInstance().GetShader("screen");

  int w = GameWindow::GetInstance().GetWidth();
  int h = GameWindow::GetInstance().GetHeight();

  //get dudvmap and store
  TextureManager& txMgr = TextureManager::GetInstance();
  m_dudvMap = txMgr.GetStandaloneTexture("water_dudv.dds");
  m_normalMap = txMgr.GetStandaloneTexture("water_normals.dds");
  m_movingFactor = 0.f;

  //init reflect and refraction textures
  m_reflectTex.Init(w, h);
#ifdef ENABLE_REFRACTION
  m_refractTex.Init(w, h);
#endif

  //init buffers
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
  glGenVertexArrays(1, &m_waterPlaneVAO);
  glGenBuffers(1, &m_waterPlaneVBO);
  glBindVertexArray(m_waterPlaneVAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_waterPlaneVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void WaterRenderer::Setup()
{
  if (!m_hasWater)
    return;

  //update moving water
  float dt = static_cast<float>(FrameRateController::GetInstance().GetDelta(false));
  m_movingFactor += WAVE_SPEED * dt;
  if (m_movingFactor >= 1.f)
    m_movingFactor = 0.f;

  //enable clipping plane
  glEnable(GL_CLIP_PLANE0);

  float waterHeight = m_waterObjects[0].waterHeight;
  GraphicsSystem& gsys = GraphicsSystem::GetInstance();

  //move and rotate camera to capture scene in inverse, send different view matrix and the usual scene shader properties
  Mtx4 waterCamMtx = CameraManager::GetInstance().GetWaterReflectionMatrix(waterHeight);
  gsys.SendSceneInfoToShader(*m_objectsShader);
  m_objectsShader->SetMatrix4("view", waterCamMtx, true);

  //set clipping plane for reflection (rendering everything above the watersurface, so normal is upwards, and distance is negative)
  m_objectsShader->SetVec4("u_clipPlane", Vec4(0.f, 1.f, 0.f, -waterHeight));

  //render to reflection texture
  m_reflectTex.Bind();
  
  //draw scene for reflection
  gsys.RenderSkyboxForWaterReflection(waterCamMtx);
  gsys.RenderScene(*m_objectsShader);
  m_reflectTex.Unbind();

#ifdef ENABLE_REFRACTION
  //get normal projection and view matrix
  gsys.SendSceneInfoToShader(*m_objectsShader);

  //set clipping plane for refraction(render everything under water, normal is downwards and distance is positive)
  m_objectsShader->SetVec4("u_clipPlane", Vec4(0.f, -1.f, 0.f, waterHeight));
  
  //render to refraction texture
  m_refractTex.Bind();
  gsys.RenderScene(*m_objectsShader);
  m_refractTex.Unbind();

#endif
  //disable clipping plane
  glDisable(GL_CLIP_PLANE0);
}

void WaterRenderer::Draw(const Mtx4& view, const Mtx4& proj, const Light& sun, const Vec3& viewPos)
{
  if (m_waterObjects.empty())
    return;

  //hack: take only first water(assuming entire game only has one water object)
  WaterTile& currWater = m_waterObjects[0];

  //set matrixes and other uniforms
  m_waterShader->SetVec3("viewPosition", viewPos);
  m_waterShader->SetVec3("lightDir", -sun.lightDirection);
  m_waterShader->SetVec3("lightColor", sun.color);
  m_waterShader->SetFloat("moveFactor", m_movingFactor);
  m_waterShader->SetMatrix4("view", view, true);
  m_waterShader->SetMatrix4("projection", proj, true);

  //set shader int for reflection (texture unit 5)
  glActiveTexture(GL_TEXTURE0 + 5);
  m_waterShader->SetInt("reflectionTex", 5);
  glBindTexture(GL_TEXTURE_2D, m_reflectTex.GetTexture());

#ifdef ENABLE_REFRACTION
  //set shader int for refraction (texture unit 6)
  glActiveTexture(GL_TEXTURE0 + 6);
  m_waterShader->SetInt("refractionTex", 6);
  glBindTexture(GL_TEXTURE_2D, m_refractTex.GetTexture());
#endif

  //set shader int for dudvmap
  glActiveTexture(GL_TEXTURE0 + 7);
  m_waterShader->SetInt("dudvMap", 7);
  glBindTexture(GL_TEXTURE_2D, m_dudvMap->id);

  //set shader int for normal map
  glActiveTexture(GL_TEXTURE0 + 8);
  m_waterShader->SetInt("normalMap", 8);
  glBindTexture(GL_TEXTURE_2D, m_normalMap->id);

  //draw the water object
  GraphicsSystem::GetInstance().RenderModel(*m_waterShader, currWater.info, RenderPass::Normal);
}

bool WaterRenderer::hasWater() const
{
  return m_hasWater;
}

void WaterRenderer::AddNewWaterLocation(const WaterTile& w)
{
  m_waterObjects.push_back(w);
  m_hasWater = true;
}

void WaterRenderer::DebugDraw()
{
  //only sampling texture, disable tests
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);

  //setup viewport and shader params for reflection texture
  glViewport(100, 600, 400, 225);         //where texture will be located
  m_screenShader->SetBool("debug", true);
  glActiveTexture(GL_TEXTURE0 + 5);
  m_screenShader->SetInt("screenTexture", 5);

  //render reflection texture
  glBindVertexArray(m_waterPlaneVAO);
  glBindTexture(GL_TEXTURE_2D, m_reflectTex.GetTexture());
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

#ifdef ENABLE_REFRACTION
  //setup viewport and shader params for refraction texture
  glViewport(600, 600, 400, 225);         //where texture will be located
  m_screenShader->SetBool("debug", false);
  glActiveTexture(GL_TEXTURE0 + 6);
  m_screenShader->SetInt("screenTexture", 6);

  //render reflection texture
  glBindVertexArray(m_waterPlaneVAO);
  glBindTexture(GL_TEXTURE_2D, m_refractTex.GetTexture());
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
#endif

  //enable depth and stencil test again
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
}

/*Notes on water shader:

two glClipPlane needed to only render things above/below the water
glClipDistance[0] = dot product of object's world position with plane equation
plane equation has d = -distance from origin
need a list of all water objects

reflection texture -> everything above (normal = +y axis),

Disable glClipDistance b4 rendering the scene


For reflection texture:
Camera needs to invert y position and pitch to render stuff under the water
Camera moved down by original y position above the water multiplied by 2

Move it back for refraction texture

*/