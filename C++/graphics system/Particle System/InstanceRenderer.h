/******************************************************************************/
/*!
\File name: InstanceRenderer.h
\Project name: 
\Primary Author: Chan Yu Hong
All content © 2017-2018 DigiPen (SINGAPORE) Corporation, All rights reserved.
*/
/******************************************************************************/
#ifndef INSTANCE_RENDERER_H
#define INSTANCE_RENDERER_H

#include "Particles.h"
#include "Math\OMath.h"
#include "Graphics\ogl.h"

class InstanceRenderer
{
public:
  InstanceRenderer();
  ~InstanceRenderer();
  void Draw(const std::vector<Mtx4>& MVPs, const std::vector<Vec4>& colors, const std::vector<Vec2>& scales,
            unsigned instanceCount, GLuint textureID);

  //variables
  GLuint m_VAO;
  GLuint m_VBO;
  GLuint m_EBO;

  GLuint m_matrixBuffer;
  GLuint m_positionsBuffer;
  GLuint m_colorsBuffer;
  GLuint m_scaleBuffer;
};


#endif

