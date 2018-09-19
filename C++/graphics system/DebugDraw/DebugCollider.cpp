/******************************************************************************/
/*!
\file		DebugCollider.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "DebugCollider.h"

//hardcoded cube collider
void DebugCollider::Init()
{        
  m_vertices = std::vector<Vec3>(8);

  //front face
  m_vertices[0] = Vec3(-0.5f, 0.5f, 0.5f);      //top left
  m_vertices[1] = Vec3(-0.5f, -0.5f, 0.5f);     //bottom left
  m_vertices[2] = Vec3(0.5f, 0.5f, 0.5f);       //top right
  m_vertices[3] = Vec3(0.5f, -0.5f, 0.5f);      //bottom right

  //back face                                      
  m_vertices[4] = Vec3(-0.5f, 0.5f, -0.5f);      //top left
  m_vertices[5] = Vec3(-0.5f, -0.5f, -0.5f);     //bottom left
  m_vertices[6] = Vec3(0.5f, 0.5f, -0.5f);       //top right
  m_vertices[7] = Vec3(0.5f, -0.5f, -0.5f);      //bottom right

  m_indices = std::vector<unsigned>
              {
                0, 1, 0, 2, 1, 3, 2, 3,  //front face
                4, 5, 4, 6, 5, 7, 6, 7,  //back face
                0, 4, 2, 6, 1, 5, 3, 7   //lines to connect front and back face
              };

  //generate buffers and objects
  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  glGenBuffers(1, &m_EBO);

  //initialize VAO
  glBindVertexArray(m_VAO);
  //VBO
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vec3), m_vertices.data(), GL_STATIC_DRAW);
  //EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned), m_indices.data(), GL_STATIC_DRAW);

  //vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void DebugCollider::Draw()
{
  // draw collider
  glBindVertexArray(m_VAO);
  glDrawElements(GL_LINES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void DebugCollider::CleanUp()
{
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteBuffers(1, &m_EBO);
}

