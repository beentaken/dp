/******************************************************************************/
/*!
\file		RenderTexture.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/

#ifndef _RENDER_TEXTURE_H
#define _RENDER_TEXTURE_H

#include "Graphics\ogl.h"

struct BufferProps
{ 
  BufferProps():
    texWrap(GL_REPEAT),
    texFilter(GL_LINEAR),
    texInFormat(GL_RGBA),
    texPixelType(GL_UNSIGNED_BYTE),
    texAttachment(GL_COLOR_ATTACHMENT0),
    rbAttachment(GL_DEPTH_STENCIL_ATTACHMENT),
    rbInFormat(GL_DEPTH24_STENCIL8),
    hasRB(true),
    readColor(true),
    writeColor(true),
    writeDepth(true),
    writeStencil(true)
  {}
  GLenum texWrap;
  GLenum texFilter;
  GLenum texInFormat;
  GLenum texPixelType;
  GLenum texAttachment;
  GLenum rbAttachment;
  GLenum rbInFormat;
  bool hasRB;          //is there a renderbuffer attachment?
  bool readColor;
  bool writeColor;
  bool writeDepth;
  bool writeStencil;
};


//only supports single texture single renderbuffer
class RenderTexture
{
public:
  RenderTexture() = default;
  void Init(int width, int height, const BufferProps& props = BufferProps());
  ~RenderTexture();

  void Bind();
  void Unbind();
  GLuint GetRBO() const;
  GLuint GetTexture() const;
  int GetRenderBufferWidth() const; 
  int GetRenderBufferHeight() const;
  int GetTextureWidth() const;
  int GetTextureHeight() const;

private:
  BufferProps m_props;
  //GLint m_prev_viewport[4];
  GLuint m_texture;
  GLuint m_RBO;
  GLuint m_FBO;
  int m_texWidth;
  int m_texHeight;
  int m_rbWidth;
  int m_rbHeight;
};



#endif

