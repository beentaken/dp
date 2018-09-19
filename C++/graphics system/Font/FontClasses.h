/******************************************************************************/
/*!
\file		FontClasses.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef FONT_H
#define FONT_H

#include <unordered_map>
#include "Graphics\ogl.h"
#include "Math\OMath.h"

//freetype headers
#include "ft2build.h"
#include "freetype\freetype.h"
#include "freetype\ftglyph.h"
#include "freetype\ftoutln.h"
#include "freetype\fttrigon.h"


//A loaded glyph
struct Character
{
  GLuint texture;   // ID handle of the glyph texture
  Vec2 size;    // Size of glyph
  Vec2 bearing;  // Offset from baseline to left/top of glyph
  Vec2 Advance;    // Horizontal offset to advance to next glyph
};

//Contains all loaded textures(glyphs)
class Font
{
public:
  std::vector<Character> m_characterList;
  void Init(const char* fontFilename);
};


#endif
