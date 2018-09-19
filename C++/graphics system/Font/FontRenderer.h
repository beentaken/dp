/******************************************************************************/
/*!
\file		FontRenderer.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

//merged into UI Render manager so font and UI can be sorted by depth
#include "Utility\Singleton.h"
#include "Utility\SharedQueue.h"
#include "FontManager.h"
#include "Graphics\Shader.h"

struct FontRenderInfo
{
  std::string text;
  std::string fontName;
  Vec4 color;
  Vec2 position;
  Vec2 scale;
  bool centerAligned;
};

using TextObjectQueue = SharedQueue<FontRenderInfo>;

class FontRenderer : public Singleton<FontRenderer>
{
public:
  void Init();
  void Draw();
  void Terminate();
  void SetActiveFont(const std::string& fontName);

  //add a text object to render from code
  void RegisterTextForRendering(const std::string & text, const Vec2& start, const Vec2 & scale, const Vec4 & color, const std::string& fontName, bool center);
  //uses threading to register text in FontComponent
  void AddToRenderList(const FontRenderInfo& info);

private:
  //internal render implementations
  void RenderString(const std::string & text, const Vec2& start, const Vec2 & scale, const Vec4 & color, const std::string& fontName, bool center);
  void Render(const std::string & text, const Vec2& start, const Vec2 & scale, bool centeralign);

  //variables
  std::vector<FontRenderInfo> m_textObjects;
  TextObjectQueue m_queue;
  std::string m_currentFontName;
  Font* m_currentFont;
  FontManager* m_fontMgr;
  Shader* m_fontShader;
  Mtx4 m_projection;
  Vec2 m_scaleRatio;
  GLuint m_VAO;
  GLuint m_VBO;
  unsigned m_registerCount;
};
#endif

