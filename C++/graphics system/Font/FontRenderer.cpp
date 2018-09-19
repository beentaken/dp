/******************************************************************************/
/*!
\file		FontRenderer.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "FontRenderer.h"
#include "Graphics\GraphicsSystem.h"
#include "Components\Transform.h"
#include "Graphics\Window\GameWindow.h"
#include "Graphics\Font\FontClasses.h"

void FontRenderer::Init()
{
  m_fontMgr = &FontManager::GetInstance();
  GraphicsSystem& gsys = GraphicsSystem::GetInstance();
  m_fontShader = gsys.AddShader("font", "font.vert", "font.frag");
  m_scaleRatio = GameWindow::GetInstance().GetWindowToDisplayRatio();
  m_registerCount = 0;

  //ortho matrix
  GameWindow& gwin = GameWindow::GetInstance();
  float halfwidth = gwin.GetWidth() * 0.5f;
  float halfheight = gwin.GetHeight() * 0.5f;
  m_projection = Mtx4::glortho(-halfwidth, halfwidth, -halfheight, halfheight, -1.0f, 1.0f);

  //create gl buffers that will be updated every frame
  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  //set default font
  SetActiveFont("CooperBlackStd.otf");
}

void FontRenderer::Draw()
{
  //send projection matrix and tex color
  m_fontShader->SetMatrix4("projection", m_projection, true);
  
  //remove object from threadsafe container
  FontRenderInfo info;
  while (m_queue.Pop(info))
  {
    m_textObjects.push_back(info);
  }

  //render each object
  for (auto& elem : m_textObjects)
  {
    RenderString(elem.text, elem.position, elem.scale, elem.color, elem.fontName, elem.centerAligned);
  }

  //clear and reserve for next iteration
  m_textObjects.clear();
  m_textObjects.reserve(m_registerCount);
}

void FontRenderer::RenderString(const std::string & text, const Vec2& position, const Vec2 & scale, const Vec4 & color, const std::string& fontName, bool centered)
{
  //set font color
  m_fontShader->SetVec4("textColor", color);

  Vec2 pos = position;
  //magic number to make font smaller
  Vec2 newScale = 1.f / 20.f * scale;

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
    Render(std::string(tmp), pos, newScale, centered);
    while (tmp)
    {
      if (prevInitial)
      {
        pos.y -= h * (newScale.x * 1.2f);
      }
      Render(tmp, pos, newScale, centered);
      prevInitial = *tmp;
      tmp = strtok_s(NULL, "`", &context);
    }
    return;
  }
  else
    Render(text, pos, newScale, centered);
}

//center align or left align
void FontRenderer::Render(const std::string & text, const Vec2& startPos, const Vec2 & scale, bool centeralign)
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

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

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

void FontRenderer::Terminate()
{
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);
}

void FontRenderer::SetActiveFont(const std::string & fontName)
{
  m_currentFont = m_fontMgr->GetFont(fontName);
  m_currentFontName = fontName;
}

void FontRenderer::RegisterTextForRendering(const std::string & text, const Vec2 & start, const Vec2 & scale, const Vec4 & color, const std::string & fontName, bool center)
{
  //add to main container
  FontRenderInfo info;
  info.position = start;
  info.text = text;
  info.color = color;
  info.scale = scale;
  info.fontName = fontName;
  info.centerAligned = center;
  m_textObjects.push_back(info);
  ++m_registerCount;
}

void FontRenderer::AddToRenderList(const FontRenderInfo & info)
{
  //add to temp container for thread safety
  m_queue.Push(info);
}

