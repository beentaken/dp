/******************************************************************************/
/*!
\file		BillboardManager.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef BILLBOARD_MGR_H
#define BILLBOARD_MGR_H

#include "Billboard.h"
#include "Graphics\Shader.h"
#include "Utility/Singleton.h"
#include "Math\OMath.h"
#include "Utility\SharedQueue.h"

struct BbRenderData
{
  Mtx4 modelMtx;
  Vec4 tintColor;
  Vec3 scale;
  Vec2 uv_origin;
  Vec2 uv_offset;
  float tintFactor;
  GLuint texture;
  bool is_animated;
};

using BillboardList = SharedQueue<BbRenderData>;

class BillboardManager : public Singleton<BillboardManager>
{
public:
  void Init();
  void InitGL();
  void Draw(const Mtx4& proj, const Mtx4& view);
  void Render(const BbRenderData& bb);
  void Terminate();

  void AddToDrawList(const BbRenderData& data);

private:
  Shader* m_billboardShader;
  BillboardList m_billboards;


  GLuint m_VAO;
  GLuint m_VBO;
  GLuint m_EBO;
};

#endif

