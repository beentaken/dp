/******************************************************************************/
/*!
\file		Skybox.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef SKYBOX_H
#define SKYBOX_H

#include "Shader.h"
#include "Math\OMath.h"
#include "ogl.h"
#include <vector>
#include <unordered_map>

using CubeMap = std::vector<std::string>;
using SkyboxList = std::unordered_map<std::string, CubeMap>;

class Skybox
{
public:
  Skybox();
  void Init();
  void Update();
  void SetColor(const Vec4& color, float tintf);
  void Draw(const Mtx4& projection, const Mtx4& currViewMtx);
  void Terminate();

private:
  CubeMap* AddSkybox(const std::string& folder);
  Vec4 m_tintColor;
  float m_tintFactor;

  //variables
  GLuint m_skyboxVAO;
  GLuint m_skyboxVBO;
  GLuint m_cubeMapID;
  Shader* m_skyboxShader;
  SkyboxList m_skyboxes;
  CubeMap* m_currentSkybox;
};

#endif

