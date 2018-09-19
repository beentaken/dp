/******************************************************************************/
/*!
\file		DebugCollider.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef DEBUG_COLLIDER_H
#define DEBUG_COLLIDER_H

#include "Graphics\ogl.h"
#include "Math\OMath.h"

struct DebugCollider
{
  void Init();
  void Draw();
  void CleanUp();

  std::vector<Vec3> m_vertices;
  std::vector<unsigned> m_indices;

private:
  GLuint m_VAO, m_VBO, m_EBO;
};


#endif

