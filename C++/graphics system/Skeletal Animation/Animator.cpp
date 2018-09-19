/******************************************************************************/
/*!
\file		Animator.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "Animator.h"

Animator::Animator() : m_animScene(nullptr), m_currAnimIndex(0), m_nextAnimIndex(0), 
                       m_elapsed(0.f), m_isActive(false), m_isPaused(false), 
                       m_isBlending(false), m_blendTime(0.f), m_blendElapsed(0.f), m_animationSpeed(1.f)
{
  //default blendTime
  m_blendTime = 0.1f;
}

void Animator::Init(const AnimScene* scene)
{
  assert(scene != nullptr);
  m_animScene = scene;
  m_isActive = true;

  //check if there are animations
  //assert(m_animScene->GetAnimationData().size() > 0);
  
  //default animation
  m_currAnimIndex = 0;
}

void Animator::Update(float dt)
{
  //if paused return
  if (m_isPaused) 
    return;
  
  //if not blending, increase animation time
  if (!m_isBlending)
  {
    m_elapsed += m_animationSpeed * dt;
  }
  else
  {
    m_blendElapsed += dt;

    //check if enough time for blending has passed (90% of the blend time) 
    //dont reach the start of the next animation to play
    if (m_blendElapsed > 0.9f * m_blendTime)
    {
      m_blendElapsed = 0.f;
      m_isBlending = false;

      //change to next animation
      m_currAnimIndex = m_nextAnimIndex;
    }
  }
}

#define BLEND_TEST
#ifndef BLEND_TEST
void Animator::PlayAnimation(const std::string & animationName)
{
  assert(m_animScene != nullptr);
  unsigned result = -1;
  const std::vector<AnimationData>& allAnims = m_animScene->GetAnimationData();
  size_t n = allAnims.size();
  for (size_t i = 0; i < n; ++i)
  {
    if (allAnims[i].m_animationName == animationName)
      result = i;
  }
  assert(result != -1);
  m_currAnimIndex = result;

  //restart animation
  m_currName = animationName;
  m_elapsed = 0.f;
}

#else
void Animator::PlayAnimation(const std::string & animationName)
{
  assert(m_animScene != nullptr);

  if (m_currName != animationName)
    Reset();

  //find the index of the next animation to play
  unsigned next = -1;
  const std::vector<AnimationData>& allAnims = m_animScene->GetAnimationData();
  size_t n = allAnims.size();
  for (size_t i = 0; i < n; ++i)
  {
    if (allAnims[i].m_animationName == animationName)
      next = i;
  }
  assert(next != -1);
  m_nextAnimIndex = next;

  //activate blending
  m_isBlending = true;
  m_blendElapsed = 0.f;
  m_currName = animationName;
}
#endif

void Animator::Reset()
{
  m_elapsed = 0.f;
}

std::string Animator::GetCurrentAnimationName() const
{
  return m_currName;
}


std::vector<std::string> Animator::GetAnimationNameList() const
{
  const std::vector<AnimationData>& allAnims = m_animScene->GetAnimationData();
  std::vector<std::string> result;
  result.reserve(allAnims.size());
  for (auto& elem : allAnims)
  {
    result.push_back(elem.m_animationName);
  }
  return result;
}

void Animator::SetActive(bool active)
{
  m_isActive = active;
}

void Animator::SetBlendingTime(float blendTime)
{
  m_blendTime = blendTime;
}

void Animator::SetAnimationSpeed(float aniSpeed)
{
	m_animationSpeed = aniSpeed;
}

bool Animator::isActive() const
{
  return m_isActive;
}

bool Animator::isBlending() const
{
  return m_isBlending;
}

void Animator::PauseAnimation()
{
  m_isPaused = true;
}

void Animator::ResumeAnimation()
{
  m_isPaused = false;
}

AnimInstance Animator::GetAnimationInstance() const
{
  //if not blending blendfactor is 0
  float blendFactor = 0.f;

  if (m_isBlending)
  {
    blendFactor = m_blendElapsed/m_blendTime;
  }

  return AnimInstance{m_elapsed, blendFactor, m_currAnimIndex, m_nextAnimIndex};
}