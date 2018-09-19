/******************************************************************************/
/*!
\File name: InstanceRenderer.cpp
\Project name: 
\Primary Author: Chan Yu Hong
All content © 2017-2018 DigiPen (SINGAPORE) Corporation, All rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "InstanceRenderer.h"

//reminder that our matrix is row major so might need to transpose in gl shader/cpu side

#define VERTICES_POSITION_ATTRIB_LOCATION 0
#define VERTICES_UV_ATTRIB_LOCATION 1
#define WVP_LOCATION 2  //and 3 4 5
#define COLOR_LOCATION 6


InstanceRenderer::InstanceRenderer()
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
  glGenBuffers(1, &m_matrixBuffer);
  glGenBuffers(1, &m_colorsBuffer);
  glGenBuffers(1, &m_scaleBuffer);

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

  //bind world view projection matrix buffer
  glBindBuffer(GL_ARRAY_BUFFER, m_matrixBuffer);

  //for each column, associate an attribute
  for (unsigned i = 0; i < 4; ++i)
  {
    glBufferData(GL_ARRAY_BUFFER, 4 * MAX_PARTICLES * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(WVP_LOCATION + i);
    glVertexAttribPointer(WVP_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(Mtx4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
    glVertexAttribDivisor(WVP_LOCATION + i, 1);
  }

  /****dont initialize data yet for position and colors(pointer in bufferdata is NULL) ***/
  //bind and configure buffer for particle's colors
  glBindBuffer(GL_ARRAY_BUFFER, m_colorsBuffer);
  glBufferData(GL_ARRAY_BUFFER, 4 * MAX_PARTICLES * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

  //bind and configure buffer for particle's scale
  glBindBuffer(GL_ARRAY_BUFFER, m_scaleBuffer);
  glBufferData(GL_ARRAY_BUFFER, 2 * MAX_PARTICLES * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

  //modify the rate at which attributes advance for each instance drawn
  glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
  glVertexAttribDivisor(1, 0); // particles uv : always reuse the same 4 vertices -> 0
  glVertexAttribDivisor(2, 1); // MVP matrix, one per quad
  glVertexAttribDivisor(6, 1); // color : one per quad -> 1
  glVertexAttribDivisor(7, 1); // scale : one per quad -> 1

  //unbind VAO and buffer
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void InstanceRenderer::Draw(const std::vector<Mtx4>& MVPs, const std::vector<Vec4>& colors, const std::vector<Vec2>& scales,
                            unsigned instanceCount, GLuint textureID)
{
  glBindVertexArray(m_VAO);
  //update the matrices buffer with the new matrices every frame 
  glBindBuffer(GL_ARRAY_BUFFER, m_matrixBuffer);
  //use buffer orphaning to improve performance
  glBufferData(GL_ARRAY_BUFFER, sizeof(Mtx4) * MAX_PARTICLES, NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Mtx4) * instanceCount, MVPs.data());

  //update the color buffer data with new colors
  glBindBuffer(GL_ARRAY_BUFFER, m_colorsBuffer);
  glBufferData(GL_ARRAY_BUFFER, 4 * MAX_PARTICLES * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * instanceCount * sizeof(GLfloat), colors.data());

  //update the scale buffer with new scales
  glBindBuffer(GL_ARRAY_BUFFER, m_scaleBuffer);
  glBufferData(GL_ARRAY_BUFFER, 2 * MAX_PARTICLES * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * instanceCount * sizeof(GLfloat), scales.data());


  //bind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);

  //draw all particles with a single call
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instanceCount);
  
  //unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

//destructor
InstanceRenderer::~InstanceRenderer()
{
  //deallocate openGL vertex array objects
  glDeleteVertexArrays(1, &m_VAO);

  //deallocate openGL buffers
  glDeleteBuffers(1, &m_VBO);
  glDeleteBuffers(1, &m_EBO);
  glDeleteBuffers(1, &m_matrixBuffer);
  glDeleteBuffers(1, &m_colorsBuffer);
  glDeleteBuffers(1, &m_scaleBuffer);
}
