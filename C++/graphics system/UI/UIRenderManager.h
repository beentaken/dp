/******************************************************************************/
/*!
\file		UIRenderManager.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef _2D_RENDER_MGR_H
#define _2D_RENDER_MGR_H

#include "Graphics\Shader.h"
#include "Utility\Singleton.h"
#include "Math\OMath.h"
#include "Utility\SharedQueue.h"
#include "Graphics\Font\FontManager.h"
#include <unordered_map>


//struct used by both FontComponent and UIRenderer
struct UIRenderData
{
  std::string text;
  std::string fontName;
  Mtx4 modelMatrix;
  Vec4 color;
  Vec2 position;
  Vec2 scale;
  Vec2 uv_origin;
  Vec2 uv_offset;
  float depth;
  float tintFactor;
  GLuint texture;
  bool centerAligned;
  bool isFont;
  bool is_animated;
};

//using TempUIRendererList = SharedQueue<UIRenderInfo>;
using TempUIRendererList = SharedQueue<UIRenderData>;
class UIRenderManager : public Singleton<UIRenderManager>
{
public:
  void Init();
  void Draw();
  void Terminate();
  void AddToDrawList(const UIRenderData& data);

  void SetActiveFont(const std::string& fontName);
  //add a text object to render from code
  void RegisterTextForRendering(const std::string & text, const Vec3& start, const Vec2 & scale, const Vec4 & color, const std::string& fontName, bool center);

private:
  //internal render implementations
  void Render2DObject(const UIRenderData& info);
  void RenderUIObject(GLuint texture, const Mtx4& model, const Vec4& tintColor, float tintFactor);
  void RenderString(const std::string & text, const Vec2& start, const Vec2 & scale, const Vec4 & color, const std::string& fontName, bool center);
  void RenderFont(const std::string & text, const Vec2& start, const Vec2 & scale, bool centeralign);

  //threadsafe container
  TempUIRendererList m_tempRenderList;
  //font container
  std::vector<UIRenderData> m_textObjects;

  //shared variables
  Shader* m_fontShader;
  Shader* m_uiShader;
  Mtx4 m_ortho;
  Vec2 m_scaleRatio;

  //for UI
  GLuint m_VAO;
  GLuint m_VBO;
  GLuint m_EBO;

  //for font
  std::string m_currentFontName;
  Font* m_currentFont;
  FontManager* m_fontMgr;
  GLuint m_fontVAO;
  GLuint m_fontVBO;
  unsigned m_font_registerCount;
};

#endif

