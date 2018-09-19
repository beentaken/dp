/******************************************************************************/
/*!
\file		PontRenderer.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef POINT_RENDERER_H
#define POINT_RENDERER_H

#include "Utility\Singleton.h"
#include "Math\OMath.h"
#include "Graphics\ogl.h"
#include "Graphics\Shader.h"

class PointRenderer : public Singleton<PointRenderer>
{
public:
  void Init();
  void Draw(const Mtx4& vp);
  void Terminate();
  bool hasData() const;
  void AddPointToRender(const Vec3& point);
  void AddLineToRender(const Vec3& start, const Vec3& end);
private:
  std::vector<Vec3> m_points;
  std::vector<Vec3> m_line_points;
  Shader* m_pointShader;
  GLuint m_VAO;
  GLuint m_VBO;
};


#endif

