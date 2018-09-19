/******************************************************************************/
/*!
\file		UIRenderManager.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "Graphics\Window\GameWindow.h"
#include "UIRenderManager.h"
#include "Graphics\GraphicsSystem.h"
#include "Graphics\TextureManager.h"
#include "Graphics\Font\FontClasses.h"
#include "Components\Transform.h"

void UIRenderManager::Init()
{
  //create shaders
  m_fontMgr = &FontManager::GetInstance();
  GraphicsSystem& gsys = GraphicsSystem::GetInstance();
  m_uiShader = gsys.AddShader("ui", "sprite.vert", "sprite.frag");
  m_fontShader = gsys.AddShader("font", "font.vert", "font.frag");

  //get ortho matrix and scale ratio
  GameWindow& gwin = GameWindow::GetInstance();
  float halfwidth = gwin.GetWidth() * 0.5f;
  float halfheight = gwin.GetHeight() * 0.5f;
  m_ortho = Mtx4::glortho(-halfwidth, halfwidth, -halfheight, halfheight, -1.0f, 1.0f);
  m_scaleRatio = gwin.GetWindowToDisplayRatio();

  //init gl buffers for position
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

  //unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);


  //init gl buffers for font
  glGenVertexArrays(1, &m_fontVAO);
  glGenBuffers(1, &m_fontVBO);
  glBindVertexArray(m_fontVAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  //init font variables
  m_font_registerCount = 0;
  SetActiveFont("CooperBlackStd.otf");
}

struct UICompare
{
  // Sort by z position
  bool operator()(UIRenderData const & lhs, UIRenderData const & rhs) const
  {
    return (lhs.depth > rhs.depth);
  }
};

void UIRenderManager::AddToDrawList(const UIRenderData & data)
{
  m_tempRenderList.Push(data);
}

void UIRenderManager::Draw()
{
  //disable depth test for 2D rendering
  //glDepthMask(GL_FALSE);
  //glDisable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);

  //set projection matrix for font and ui
  m_uiShader->SetMatrix4("projection", m_ortho, true);
  m_fontShader->SetMatrix4("projection", m_ortho, true);

  //activate texture unit 0
  glActiveTexture(GL_TEXTURE0);

  //pop each transparent object from threadsafe queue and sort using set
  std::multiset<UIRenderData, UICompare> sorted;
  UIRenderData data;
  while (m_tempRenderList.Pop(data))
  {
	  sorted.emplace(data);
  }

  // After drawing opaque, draw all transparent objects from back to front
  for (auto & elem : sorted)
  {
    Render2DObject(elem);
  }

  glClear(GL_DEPTH_BUFFER_BIT);
  //enable depth test again
  //glEnable(GL_DEPTH_TEST);
  //glDepthMask(GL_TRUE);

  //clear and reserve for next iteration
  m_textObjects.clear();
  m_textObjects.reserve(m_font_registerCount);
}


void UIRenderManager::Render2DObject(const UIRenderData & data)
{
  //wrapper to decide if ui or font
  if (data.isFont)
  {
    m_fontShader->SetFloat("depth", data.depth);
    RenderString(data.text, data.position, data.scale, data.color, data.fontName, data.centerAligned);
  }
  else
  {
    Vec2 uv_origin = Vec2();
    Vec2 uv_offset = Vec2(1.f, 1.f);

    //for animated objects, change uv origin and offset
    if (data.is_animated)
    {
      uv_origin = data.uv_origin;
      uv_offset = data.uv_offset;
    }

    m_uiShader->SetVec2("uvOrigin", uv_origin);
    m_uiShader->SetVec2("uvOffset", uv_offset);

    m_uiShader->SetFloat("depth", data.depth);
    RenderUIObject(data.texture, data.modelMatrix, data.color, data.tintFactor);
  }
}

void UIRenderManager::RenderUIObject(GLuint texture, const Mtx4 & model, const Vec4 & color, float factor)
{
  glBindVertexArray(m_VAO);
  m_uiShader->SetMatrix4("model", model, true);
  m_uiShader->SetVec4("tintColor", color);
  m_uiShader->SetFloat("tintFactor", factor);
  glBindTexture(GL_TEXTURE_2D, texture);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

}

void UIRenderManager::RenderString(const std::string & text, const Vec2& position, const Vec2 & scale, const Vec4 & color, const std::string& fontName, bool centered)
{
  //set font color
  m_fontShader->SetVec4("textColor", color);

  Vec2 pos = position;
  //magic number to make font smaller
  Vec2 newScale = 1.f / 20.f * scale;                                               //used to be 20

  //check if font is same
  if (fontName != m_currentFontName)
    SetActiveFont(fontName);

  //use character A to get height to adjust for newline
  assert(m_currentFont != nullptr);
  char temp = 'A';
  float h = m_currentFont->m_characterList[temp].size.y;

  //repeatedly tokenize around "`" for newline
  if (text.find("`") != std::string::npos)
  {
    char* context = nullptr;
    char prevInitial = 0;
    char* tmp = strtok_s(const_cast<char*>(text.c_str()), "`", &context);
    RenderFont(std::string(tmp), pos, newScale, centered);
    while (tmp)
    {
      if (prevInitial)
      {
        pos.y -= h * (newScale.x * 1.2f);
      }
      RenderFont(tmp, pos, newScale, centered);
      prevInitial = *tmp;
      tmp = strtok_s(NULL, "`", &context);
    }
    return;
  }
  else
    RenderFont(text, pos, newScale, centered);
}

//center align or left align
void UIRenderManager::RenderFont(const std::string & text, const Vec2& startPos, const Vec2 & scale, bool centeralign)
{
  assert(m_currentFont != nullptr);
  Vec2 pos = startPos;

  //if text needs to be center aligned
  if (centeralign)
  {
    float maxwidth = 0.f;
    for (auto c = text.begin(); c != text.end(); c++)
    {
      Character ch = m_currentFont->m_characterList[*c];
      maxwidth += (ch.bearing.x * scale.x) + (ch.size.x * scale.x);
    }
    float halfTextwidth = maxwidth * 0.5f;
    pos.x -= halfTextwidth;
  }

  glBindVertexArray(m_fontVAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);

  // Iterate through all characters
  for (auto c = text.begin(); c != text.end(); c++)
  {
    Character ch = m_currentFont->m_characterList[*c];

    float xpos = pos.x + ch.bearing.x * scale.x;
    float ypos = pos.y + ch.bearing.y * scale.y;

    float w = ch.size.x * scale.x;
    float h = ch.size.y * scale.y;

    // Update VBO for each character  (CCW winding order)
    GLfloat vertices[6][4] =
    {
      { xpos,     ypos,           0.0, 0.0 },
      { xpos,     ypos - h,       0.0, 1.0 },
      { xpos + w, ypos - h,       1.0, 1.0 },

      { xpos,     ypos,           0.0, 0.0 },
      { xpos + w, ypos - h,       1.0, 1.0 },
      { xpos + w, ypos,           1.0, 0.0 },
    };

    //render after updating data
    glBindTexture(GL_TEXTURE_2D, ch.texture);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    pos.x += (ch.Advance.x / 64.f) * scale.x;
    pos.y += (ch.Advance.y / 64.f) * scale.y;
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void UIRenderManager::SetActiveFont(const std::string & fontName)
{
  m_currentFont = m_fontMgr->GetFont(fontName);
  m_currentFontName = fontName;
}

void UIRenderManager::RegisterTextForRendering(const std::string & text, const Vec3 & start, const Vec2 & scale, const Vec4 & color, const std::string & fontName, bool center)
{
  //add to main container
  UIRenderData data;
  data.position = Vec2(start.x, start.y);
  data.depth = start.z;
  data.text = text;
  data.color = color;
  data.scale = scale;
  data.fontName = fontName;
  data.centerAligned = center;
  m_textObjects.push_back(data);
  ++m_font_registerCount;
}

void UIRenderManager::Terminate()
{
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);

  glDeleteVertexArrays(1, &m_fontVAO);
  glDeleteBuffers(1, &m_fontVBO);
}
