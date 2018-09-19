/******************************************************************************/
/*!
\file		RenderTexture.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "stdafx.h"
#include "RenderTexture.h"
void RenderTexture::Init(int width, int height, const BufferProps& props)
{
  m_RBO = 0;
  m_texWidth = m_rbWidth = width;
  m_texHeight = m_rbHeight = height;
  m_props = props;

  //create texture (dont initialize data)
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_props.texFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_props.texWrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_props.texWrap);
  glTexImage2D(GL_TEXTURE_2D, 0, m_props.texInFormat, m_texWidth, m_texHeight, 0, m_props.texInFormat, m_props.texPixelType, NULL);
  glGenerateMipmap(GL_TEXTURE_2D);

  //bind fbo
  glGenFramebuffers(1, &m_FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  //attach texture
  glFramebufferTexture2D(GL_FRAMEBUFFER, m_props.texAttachment, GL_TEXTURE_2D, m_texture, 0);

  //check if reading or writing colors
  if (!m_props.readColor)
    glReadBuffer(GL_NONE);
  if (!m_props.writeColor)
    glDrawBuffer(GL_NONE);
    
  if (m_props.hasRB)
  {
    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, m_props.rbInFormat, m_rbWidth, m_rbHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, m_props.rbAttachment, GL_RENDERBUFFER, m_RBO);
  }

  //set as draw buffer
  glDrawBuffer(m_props.texAttachment);
  
  //check if complete
  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  Assert("Did not successfully create FBO in RenderBuffer constructor",
    Status == GL_FRAMEBUFFER_COMPLETE);

  //unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

RenderTexture::~RenderTexture()
{
  glDeleteFramebuffers(1, &m_FBO);
  glDeleteTextures(1, &m_texture);

  if(m_props.hasRB)
    glDeleteRenderbuffers(1, &m_RBO);
}

void RenderTexture::Bind()
{
  //save previous viewport
  //glGetIntegerv(GL_VIEWPORT, m_prev_viewport);

  //bind
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);


  //check if need to clear color or depth or stencil
  unsigned flags = 0;
  if (m_props.writeDepth)
    flags |= GL_DEPTH_BUFFER_BIT;
  if (m_props.writeColor)
    flags |= GL_COLOR_BUFFER_BIT;
  if (m_props.writeStencil)
    flags |= GL_STENCIL_BUFFER_BIT;

  //clear the buffers
  glClear(flags);

  glViewport(0, 0, m_texWidth, m_texHeight);

  if (!m_props.writeColor)
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
}

void RenderTexture::Unbind()
{
  if (!m_props.writeColor)
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  //unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //restore viewport
  //glViewport(m_prev_viewport[0], m_prev_viewport[1], (GLsizei)m_prev_viewport[2], (GLsizei)m_prev_viewport[3]);
}


GLuint RenderTexture::GetRBO() const
{
  return m_props.hasRB ? m_RBO : m_texture;
}

int RenderTexture::GetRenderBufferWidth() const
{
  return m_rbWidth;
}

int RenderTexture::GetRenderBufferHeight() const
{
  return m_rbHeight;
}

GLuint RenderTexture::GetTexture() const
{
  return m_texture;
}

int RenderTexture::GetTextureWidth() const
{
  return m_texWidth;
}

int RenderTexture::GetTextureHeight() const
{
  return m_texHeight;
}


