/******************************************************************************/
/*!
\file		GraphicsSystem.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef _GRAPHICSSYSTEM_H_
#define _GRAPHICSSYSTEM_H_

////////////////////////////////////////////////////////
// NOTE: VERTICES ARE DRAWN IN COUNTER-CLOCKWISE ORDER//
////////////////////////////////////////////////////////

#include "Math\OMath.h"
#include "Utility\Singleton.h"
#include "Skeletal Animation\Model.h" 
#include "Shader.h"
#include "Cameras\CameraBase.h"
#include "Lighting\LightingManager.h"
#include "Utility\SharedQueue.h"
#include "Utility\Threading.h"
#include "DebugDraw\DebugCollider.h"
#include "Font\FontManager.h"
#include "Billboards\BillboardManager.h"
#include "DebugDraw\PointRenderer.h"
#include "Particle System\InstanceManager.h"
#include "RenderTexture.h"

#pragma region POD classes

struct RenderInfo
{
  std::vector<Mtx4>* boneMatrices;
  Model* modelptr;
  Mtx4 normalMatrix;
  Mtx4 modelMatrix;
  Vec4 tintColor;
  Vec4 highlightColor;
  Vec3 rimColor;
  Vec3 emissive;
  Vec2 tileRepeat;
  float alpha;
  float tintFactor;
  float outlineThickness;
  float emiFactor;
  bool useBoneMatrices;
  bool castshadow;
  bool useRim;
  bool highlight;
};

enum class RenderPass
{
  Normal = 0,
  Shadow,
  StencilOnly,
};

struct SortedObj
{
  RenderInfo* ri;
  float dist;
};

struct DebugRenderInfo
{
  DebugCollider* debugcollider;
  Mtx4 modelMatrix;
};

#pragma endregion

#pragma region DECLARATIONS and TYPEDEFS
class Model;
class Shader;
class Skybox;
class CameraBase;
class GameWindow;
class ShadowMap;
class ModelRenderer;
class CameraManager;
class BillboardManager;
class UIRenderManager;
struct DebugCollider;

using ModelID = unsigned;
using ModelList = std::unordered_map<std::string, Model>;
using ShaderList = std::unordered_map<std::string, Shader>;
using TempRenderList = SharedQueue<RenderInfo>;       //thread-safe container to store models that need to be rendered every frame
using DrawList = std::vector<RenderInfo>;
using DebugDrawList = SharedQueue<DebugRenderInfo>;
using SortedDrawList = std::vector<SortedObj>;

#define SCREEN_TEXTURE

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsSystem
////////////////////////////////////////////////////////////////////////////////
class GraphicsSystem : public Singleton<GraphicsSystem>
{
public:
  GraphicsSystem();
  ~GraphicsSystem() = default;

  void Init();
  void LightingUpdate();
  void Update();
  void Draw();
  void StartFrame();          // Clear the buffers to begin the scene.
  void EndFrame();            // Present the rendered scene to the screen.

  void InitScreenTex();

  /**************************************** Shader management ***************************************/
  Shader* AddShader(const std::string& name, const char* vert, const char* frag);
  Shader* GetShader(const std::string& name);

  /*****************************************Model Management*****************************************/
  Model* AddModel(const std::string& path);
  Model* GetModel(const std::string& path);
  void LoadModelTextures();
  void LoadModels();
  Model * LoadModel(std::string const & path);
  void GenerateModelNamesStr();
  std::string GetModelNamesStr() const;
  std::vector<std::string>& GetModelNames();

  /*****************************************Render Lists*********************************************/
  void AddToDrawList(const RenderInfo& info);
  void AddToDebugDrawList(const DebugRenderInfo& data);
  void SortForAlphaBlending();

  /*****************************************Display Functions*****************************************/
  void RenderScene(const Shader& shader, RenderPass pass = RenderPass::Normal);
  void RenderModel(const Shader& shader, const RenderInfo& info, RenderPass pass = RenderPass::Normal);
  void RenderSkybox();
  void RenderSkyboxForWaterReflection(const Mtx4& waterCamMtx);
  void RenderDebugColliders();
  void RenderScreenQuad();

  //windows resizing on openGL api
  void ResizeViewport(unsigned width, unsigned height);
  bool CheckDone(bool = false);
  void StartDraw(bool = false);
  //Send scene shader properties for a given shader
  void SendSceneInfoToShader(const Shader& shader);

  /*****************************************Utility Functions*****************************************/
  void SetBackgroundColor(float r, float g, float b, float a);
  Vec2 ConvertWorldToScreen(const Vec3 & worldPos) const;
  Vec3 ConvertScreenToWorld(const Vec2& screenpos) const;
  Vec2 GetCurrentMousePosition() const;
  float GetValueAtScreenPosition(const Vec2& screenpos, GLenum = GL_DEPTH_COMPONENT) const;
  float GetNearPlane() const;
  float GetFarPlane() const;
  Skybox& GetSkybox();

  // Loading Screen
  void InitLoadScreen();
  void UpdateLoadingScreen();
  //Cleanup
  void Terminate();
  bool LoadingDone() const;
  void SetLoadingDone();

private:
  //internal render function for outlining
  void RenderOutlinedModel(const RenderInfo& info);

  HDC m_deviceContext;
  GameWindow* m_window;
  ShadowMap* m_shadowMap;
  Skybox* m_skybox;
  ShaderList shaderList;
  ModelList& modelList;

  //drawlists
  DrawList m_actualDrawList;
  DrawList m_outlinedObjects;
  SortedDrawList m_sortedObjects;
  TempRenderList m_tempDrawList;
  DebugDrawList m_debugDrawList;

  Dispatcher::ThreadHandle thd;
  Shader* m_renderShader;
  Shader* m_debugShader;
  Shader* m_screenShader;
  Shader* m_outlineShader;
  CameraManager* m_camMgr;
  BillboardManager* m_bbMgr;
  UIRenderManager* m_uiMgr;
  PointRenderer* m_pointRenderer;
  InstanceManager* m_instMgr;
  
  float m_aspectRatio;
  unsigned windowx;
  unsigned windowy;
  std::vector<std::string> modelNames;
  std::string modelNamesStr;
  Mtx4 m_viewMtx;
  Mtx4 m_projMtx;
  Vec4 m_backgroundColor;
  bool m_lighting;      //control if lighting is on
  bool m_loading;

  //render texture that spans the whole screen
  RenderTexture m_screenTex;
  GLuint m_screenQuadVAO;
  GLuint m_screenQuadVBO;
};

#endif