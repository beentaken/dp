/******************************************************************************/
/*!
\file		BillboardManager.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "BillboardManager.h"
#include "Graphics\GraphicsSystem.h"
#include "Graphics\TextureManager.h"

void BillboardManager::Init()
{
  m_billboardShader = GraphicsSystem::GetInstance().AddShader("billboard", "billboard.vert", "billboard.frag");
  InitGL();
}

void BillboardManager::InitGL()
{
  //position and uv
  float vertices[] =
  {
    -0.5f, 0.5f, 0.0f,    0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
    0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
    0.5f, -0.5f, 0.0f,    1.0f, 0.0f,
  };

  unsigned int indices[] =
  {
    0, 1, 2, // first triangle
    1, 3, 2  // second triangle
  };

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  glGenBuffers(1, &m_EBO);

  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

  // texture coord attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void BillboardManager::Draw(const Mtx4& projection, const Mtx4& view)
{
  //to get from transform
  m_billboardShader->SetMatrix4("projection", projection, true);

  BbRenderData data;
  while (m_billboards.Pop(data))
  {
    //concatenate view matrix with model matrix
    data.modelMtx = view * data.modelMtx;

    //draw the billboard
    Render(data);
  }

  /*
  for (auto& elem : m_billboards)
  {
    Billboard& bb = *elem.second;
    if (bb.active)
    {
      Mtx4 model = Mtx4::translate(bb.position) *  Mtx4::scale(bb.scale.x, bb.scale.y, 1.f);
      Mtx4 modelView = view * model;
      m_billboardShader->SetMatrix4("modelView", modelView, true);
      m_billboardShader->SetVec3("u_scale", bb.scale);
      m_billboardShader->SetFloat("u_alpha", bb.alpha);
      elem.second->Draw();
    }
  }
  */
}

void BillboardManager::Render(const BbRenderData& bb)
{
  Vec2 uv_origin = Vec2();
  Vec2 uv_offset = Vec2(1.f, 1.f);

  //for animated objects, change uv origin and offset
  if (bb.is_animated)
  {
    uv_origin = bb.uv_origin;
    uv_offset = bb.uv_offset;
  }

  m_billboardShader->SetVec2("uvOrigin", uv_origin);
  m_billboardShader->SetVec2("uvOffset", uv_offset);
  m_billboardShader->SetMatrix4("modelView", bb.modelMtx, true);
  m_billboardShader->SetVec3("u_scale", bb.scale);
  m_billboardShader->SetVec4("tintColor", bb.tintColor);
  m_billboardShader->SetFloat("tintFactor", bb.tintFactor);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, bb.texture);
  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void BillboardManager::Terminate()
{
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);
}

void BillboardManager::AddToDrawList(const BbRenderData & data)
{
  m_billboards.Push(data);
}
