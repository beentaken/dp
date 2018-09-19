/******************************************************************************/
/*!
\file		WaterRenderer.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef WATER_RENDERER_H
#define WATER_RENDERER_H
#pragma once

#include "Graphics\RenderTexture.h"
#include "Utility\Singleton.h"
#include "Graphics\Shader.h"
#include "Graphics\GraphicsSystem.h"

//#define ENABLE_REFRACTION


struct WaterTile
{
  RenderInfo info;
  float waterHeight;
};


class WaterRenderer : public Singleton<WaterRenderer>
{
public:
  void Init();
  void Setup();
  void Draw(const Mtx4& view, const Mtx4& proj, const Light& sun, const Vec3& viewPos);
  bool hasWater() const;
  void AddNewWaterLocation(const WaterTile& obj);
  void DebugDraw();


private:
  RenderTexture m_reflectTex;
  RenderTexture m_refractTex;
  std::vector<WaterTile> m_waterObjects;
  Shader* m_waterShader;
  Shader* m_objectsShader;
  Shader* m_screenShader;
  bool m_hasWater;
  GLuint m_waterPlaneVAO;
  GLuint m_waterPlaneVBO;
  Texture* m_dudvMap;
  Texture* m_normalMap;
  float m_movingFactor;
};

#endif


