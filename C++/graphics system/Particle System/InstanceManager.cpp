/******************************************************************************/
/*!
\file		InstanceManager.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "InstanceManager.h"
#include "Graphics\GraphicsSystem.h"
#include "Graphics\TextureManager.h"

#define MAX_INFO_COUNT 1000     //max emitters

void InstanceManager::Init()
{
  m_instanceShader = GraphicsSystem::GetInstance().AddShader("instance", "instance.vert", "instance.frag");
}

void InstanceManager::Draw(const Mtx4& projection, const Mtx4& view)
{
  EmitterRenderInfo info;
  while (m_tempInstanceList.Pop(info))
  {
    m_instances.push_back(info);
  }

  //send matrices to shader
  m_instanceShader->SetMatrix4("projection", projection, true);
  m_instanceShader->SetMatrix4("view", view, true);

  //save previous blendfunc
  GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
  GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
  GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
  GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);

  //draw
  for (auto& elem : m_instances)
  {
    if (elem.instanceCount != 0)
    {
      m_instanceShader->SetFloat("tintFactor", elem.tintFactor);

      //set blending mode
      m_instanceShader->SetBool("customblend", elem.additiveBlend);

      if (info.additiveBlend)
      {
        m_instanceShader->SetFloat("blendratio", elem.blendRatio);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      }
      else
      {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
        
      m_renderer.Draw(*elem.pMatrices, *elem.pColors, *elem.pScales, elem.instanceCount, elem.texture);
    }
  }

  //restore blendfunc
  glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
  //enable depth test again
  glEnable(GL_DEPTH_TEST);

  //clear for next frame
  m_instances.clear();
  m_instances.reserve(MAX_INFO_COUNT);


}

void InstanceManager::AddInstance(const EmitterRenderInfo & inst)
{
  m_tempInstanceList.Push(inst);
}

