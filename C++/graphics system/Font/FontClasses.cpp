/******************************************************************************/
/*!
\file		FontClasses.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "FontClasses.h"
#include "Core\Component.h"
#include "Components\FontComponent.h"
#include "Graphics\Window\GameWindow.h"

static const int DEFAULTSIZE = 48;
static const unsigned NUM_CHARACTERS = 128;

void Font::Init(const char* fontFilename)
{
  FT_Library lib;
  FT_Face face;

  // All functions return a value different than 0 whenever an error occurred
  if (FT_Init_FreeType(&lib))
    Assert("ERROR::FREETYPE: Could not init FreeType Library", false, ErrorLevel::SystemCrash);

  // Load font as face
  if (FT_New_Face(lib, fontFilename, 0, &face))
    Assert("ERROR::FREETYPE: Failed to load font", false, ErrorLevel::SystemCrash);

  // Set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, DEFAULTSIZE);

  // Disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  int count = 0;
  m_characterList.reserve(NUM_CHARACTERS);
  // Load first 128 characters of ASCII set
  for (unsigned char c = 0; c < NUM_CHARACTERS; ++c)
  {
    // Load character glyph 
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))    //Load_render - create bitmap
    {
      Assert("Failed to load glyph", false, ErrorLevel::SystemCrash);
    }
    FT_GlyphSlot& g = face->glyph;
    // Generate one texture for the entire font
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


    //glyph width, height, and bitmapbuffer contains the data to create the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

    //prevent certain artifacts when a character is not rendered exactly on pixel boundaries, 
    //clamp at edge, and enable linear interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Create character and insert into map
    Character character =
    {
      texture,
      Vec2((float)g->bitmap.width, (float)g->bitmap.rows),
      Vec2((float)g->bitmap_left, (float)g->bitmap_top),
      Vec2((float)g->advance.x, (float)g->advance.y)
    };
    m_characterList.push_back(character);
  }

  // Destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(lib);
}
