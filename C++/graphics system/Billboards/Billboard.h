/******************************************************************************/
/*!
\file		Billboard.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "Math\OMath.h"
#include "Graphics\ogl.h"

class Billboard
{
public:
  Billboard();
  ~Billboard();
  void Draw();

  //temporary hack to use without a component
  Vec3 position;
  Vec3 scale;
  float alpha;
  bool active;

  //variables
  GLuint m_VAO;
  GLuint m_VBO;
  GLuint m_EBO;
  GLuint m_texture;
};

#endif