/******************************************************************************/
/*!
\file		Skybox.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "Skybox.h"
#include "GraphicsSystem.h"
#include "Graphics\Texture Loading\DDSLoader.h"
#include "Utility\FrameRateController.h"
#include "GameController.h"

#define DDS_SKYBOX

static const std::string DIRECTORY = "Assets/Skyboxes/";


void Skybox::Update()
{
  //float dt = static_cast<float>(FrameRateController::GetInstance().GetDelta());
}

void Skybox::SetColor(const Vec4 & color, float tintf)
{
  m_tintColor = color;
  m_tintFactor = tintf;
}

Skybox::Skybox()
{
  //init all skyboxes here (names must be foldername in Assets folder)
  AddSkybox("notstolen");
  std::string active = "notstolen";

  auto& it = m_skyboxes.find(active);
  if (it != m_skyboxes.end())
  {
    m_currentSkybox = &(it->second);
  }

  //init color and tintfactor
  m_tintColor = Vec4::W();
  m_tintFactor = 0.f;
}

void Skybox::Init()
{
  float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
  };

  m_cubeMapID = LoadCubeMapDDS(*m_currentSkybox);

  //send skybox data
  glGenVertexArrays(1, &m_skyboxVAO);
  glGenBuffers(1, &m_skyboxVBO);
  glBindVertexArray(m_skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  //create skybox shader
  GraphicsSystem& gph = GraphicsSystem::GetInstance();
  m_skyboxShader = gph.AddShader("skybox", "skybox.vert", "skybox.frag");

}

void Skybox::Draw(const Mtx4& projection, const Mtx4 & currViewMtx)
{
  //need to change depthfunc as objects with depth of 1.0(maximum) shld occlude the skybox
  //dont write depth
  glDepthFunc(GL_LEQUAL);
  //glDepthMask(GL_FALSE);

  //remove the translation part of the viewmatrix
  Mtx4 newView = Mtx4(currViewMtx.getNormalizedCoordinateSystem());

  m_skyboxShader->SetMatrix4("view", newView, true);
  m_skyboxShader->SetMatrix4("projection", projection, true);

  m_skyboxShader->SetVec4("tintColor", m_tintColor);
  m_skyboxShader->SetFloat("tintFactor", m_tintFactor);

  //draw skybox
  glBindVertexArray(m_skyboxVAO);
  //glActiveTexture(GL_TEXTURE0);
  //set texture unit
  glActiveTexture(GL_TEXTURE0 + 8);
  m_skyboxShader->SetInt("skybox", 8);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapID);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);

  glDepthFunc(GL_LESS); // set depth function back to default
  //glDepthMask(GL_TRUE);
}

void Skybox::Terminate()
{
  glDeleteVertexArrays(1, &m_skyboxVAO);
  glDeleteBuffers(1, &m_skyboxVBO);
  glDeleteTextures(1, &m_cubeMapID);
}



CubeMap* Skybox::AddSkybox(const std::string & foldername)
{
  std::string folder = foldername + "/";
  CubeMap cubemap
  {
    DIRECTORY + folder + "right.dds",
    DIRECTORY + folder + "left.dds",
    DIRECTORY + folder + "top.dds",
    DIRECTORY + folder + "bottom.dds",
    DIRECTORY + folder + "back.dds",
    DIRECTORY + folder + "front.dds"
  };

  auto& result = m_skyboxes.emplace(foldername, std::move(cubemap));
  Assert("Failed to add skybox", result.second, ErrorLevel::SystemCrash);

  return &result.first->second;
}

/*
GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, i from 0 to 6

Texture target	                    Orientation
GL_TEXTURE_CUBE_MAP_POSITIVE_X	    Right
GL_TEXTURE_CUBE_MAP_NEGATIVE_X	    Left
GL_TEXTURE_CUBE_MAP_POSITIVE_Y	    Top
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	    Bottom
GL_TEXTURE_CUBE_MAP_POSITIVE_Z	    Back
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	    Front
*/
