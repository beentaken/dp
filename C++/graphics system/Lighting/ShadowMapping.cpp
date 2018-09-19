/******************************************************************************/
/*!
\file		ShadowMapping.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "ShadowMapping.h"
#include "Graphics\ogl.h"
#include "Graphics\GraphicsSystem.h"
#include "Graphics\Lighting\LightingManager.h"
#include "Graphics\Cameras\CameraManager.h"
#include "Math\OMath.h"
#include "Graphics\RenderTexture.h"

#define SHADOW_DIST 300.f
#define SHADOW_RESOLUTION 8192
#define LIGHT_PROJ_NEAR 0.1f
#define LIGHT_PROJ_FAR 100000.f
#define DIRECTION_SCALE 10.f

static const Vec3 interiorPosition = Vec3(0.f, 450.f, 0.f);
static const Vec3 Zone1Position = Vec3(0.f, 1000.f, 0.f);
static const Vec3 Zone4Position = Vec3(0.f, 10000.f, 0.f);


void ShadowMap::Init(float aspect, float fov, float nearP)
{
  //shadow map properties
  BufferProps shadowMapProps = BufferProps();
  //not reading or writing color for shadowmap, only depth
  shadowMapProps.readColor = shadowMapProps.writeColor = false;
  shadowMapProps.texWrap = GL_CLAMP_TO_EDGE;
  shadowMapProps.texInFormat = GL_DEPTH_COMPONENT;
  shadowMapProps.texAttachment = GL_DEPTH_ATTACHMENT;
  shadowMapProps.texPixelType = GL_FLOAT;
  shadowMapProps.writeStencil = false;
  //no renderbuffer needed
  shadowMapProps.hasRB = false;

  //create shadowmap
  m_renderTex.Init(SHADOW_RESOLUTION, SHADOW_RESOLUTION, shadowMapProps);

  //create shadowbox(not working)
  //m_box.Init(SHADOW_DIST, fov, nearP, aspect);

  //create shader
  GraphicsSystem& gph = GraphicsSystem::GetInstance();
  m_Shader = gph.AddShader("shadow", "shadow.vert", "shadow.frag");

  //set size of shadow projection
  m_shadowSize = Vec2(6000.f, 6000.f);

  m_projectionPosition = interiorPosition;

  //get maximum number of texture units
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_tex_unit_count);
}

void ShadowMap::RenderShadows(const Light& light, float fov, float aspect)
{
  GraphicsSystem::GetInstance().StartFrame();

  //check if size of shadowbox needs to be updated(not working)
  //m_box.Setup(fov, aspect);

  //only accept directional light
  Assert("Shadow map only accepts directional lights", light.type == Light::Type::Directional);
  Mtx4 lightViewMatrix = CalculateLightViewMatrix(light.lightDirection);

  //update shadowbox with new view matrix(not working)
  //m_box.Update(lightViewMatrix);

  //compute world to lightspace matrix
  Mtx4 lightProj = CalculateLightProjectionMatrix();
  m_lightSpaceMatrix = lightProj * lightViewMatrix;

  //send matrices to shader
  m_Shader->SetMatrix4("lightSpaceMatrix", m_lightSpaceMatrix, true);

  //bind depth texture and render
  glEnable(GL_DEPTH_TEST);
  m_renderTex.Bind();

  //supposedly culling front face is correct
  //glCullFace(GL_FRONT);
  glDisable(GL_CULL_FACE);
  GraphicsSystem::GetInstance().RenderScene(*m_Shader, RenderPass::Shadow);
  m_renderTex.Unbind();
  glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);
}

GLuint ShadowMap::GetShadowMap() const
{
  return m_renderTex.GetTexture();
}

void ShadowMap::Terminate()
{
}

Mtx4 ShadowMap::GetLightSpaceMatrix() const
{
  return m_lightSpaceMatrix;
}

void ShadowMap::ChangeToInterior()
{
  m_projectionPosition = interiorPosition;
  m_shadowSize = Vec2(1500.f, 1500.f);
}

void ShadowMap::ChangeToExteriorZone1()
{
  m_projectionPosition = Zone1Position;
  m_shadowSize = Vec2(3000.f, 3000.f);
}

void ShadowMap::ChangeToExteriorZone4()
{
  m_projectionPosition = Zone4Position;
  m_shadowSize = Vec2(9000.f, 9000.f);
}

Mtx4 ShadowMap::CalculateLightViewMatrix(const Vec3& lightDir) const
{
  Vec3 target = m_projectionPosition + (DIRECTION_SCALE * lightDir);
  return Mtx4::glLookAtRH(m_projectionPosition, target, Vec3::EY());
}

Mtx4 ShadowMap::CalculateLightProjectionMatrix() const
{
  return Mtx4::glortho(-m_shadowSize.x, m_shadowSize.x, -m_shadowSize.y, m_shadowSize.y, LIGHT_PROJ_NEAR, LIGHT_PROJ_FAR);
}

