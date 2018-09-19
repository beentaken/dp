/******************************************************************************/
/*!
\file		ShadowMapping.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef SHADOW_MAPPING_H
#define SHADOW_MAPPING_H

#include "Graphics\Shader.h"
#include "Graphics\TextureManager.h"
#include "Utility\Singleton.h"
#include "LightTypes.h"
#include "Graphics\RenderTexture.h"
#include "ShadowBox.h"

#define RENDER_TEXTURE

class ShadowMap : public Singleton<ShadowMap>
{
public:
  void Init(float aspect, float fov, float nearP);

  //assumes light is directional light
  void RenderShadows(const Light& light, float fov, float aspect);
  GLuint GetShadowMap() const;
  void Terminate();

  //invalid every frame until rendershadows is called
  Mtx4 GetLightSpaceMatrix() const;

  //depending on size of level position and projection size need to be changed (hack until CSM is implemented)
  void ChangeToInterior();
  void ChangeToExteriorZone1();
  void ChangeToExteriorZone4();

private:
  Mtx4 CalculateLightViewMatrix(const Vec3& lightDir) const;
  Mtx4 CalculateLightProjectionMatrix() const;

  ShadowBox m_box;
  RenderTexture m_renderTex;
  GLint max_tex_unit_count;          //maximum number of texture unit per shader (shadow will always use the max-1 texture unit)
  Shader* m_Shader;
  Mtx4 m_lightSpaceMatrix;          //the matrix that will bring a object from world space to light space
  Vec2 m_shadowSize;
  Vec3 m_projectionPosition;

  GLuint debugVAO;                  //quad for debugging
  GLuint debugVBO;
};

#endif

