/******************************************************************************/
/*!
\file		PointRenderer.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "PointRenderer.h"
#include "Graphics\GraphicsSystem.h"

void PointRenderer::Init()
{
  GraphicsSystem& gsys = GraphicsSystem::GetInstance();
  m_pointShader = gsys.AddShader("points", "points.vert", "points.frag");

  //create gl buffers that will be updated every frame
  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  //maximum 100 points
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * 100, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

//points are in world position
void PointRenderer::Draw(const Mtx4& vp)
{
  m_pointShader->SetMatrix4("viewProjection", vp, true);

  //change point size
  glPointSize(10);

  //bind buffer
  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  //update point data
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3) * m_points.size(), m_points.data());
  
  //draw
  glDrawArrays(GL_POINTS, 0, m_points.size());

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3) * m_line_points.size(), m_line_points.data());

  glDrawArrays(GL_LINES, 0, m_line_points.size());

  //reset values
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glPointSize(1);
  
  //clear for next frame
  m_points.clear();
  m_line_points.clear();
}

void PointRenderer::Terminate()
{
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);
}

bool PointRenderer::hasData() const
{
  return !m_points.empty() || !m_line_points.empty();
}

void PointRenderer::AddPointToRender(const Vec3& point)
{
  m_points.push_back(point);
}

void PointRenderer::AddLineToRender(const Vec3 & start, const Vec3 & end)
{
	m_line_points.push_back(start);
	m_line_points.push_back(end);
}
