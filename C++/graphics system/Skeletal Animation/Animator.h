/******************************************************************************/
/*!
\file		Animator.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef _ANIMATOR_H
#define _ANIMATOR_H

#include "AnimationClasses.h"
#include "Graphics\Shader.h"

struct AnimInstance
{
  float elapsed;
  float blendFactor;
  unsigned currAnimIndex;
  unsigned nextAnimIndex;
};


class Animator
{
public:
  friend class ModelRenderer;
  Animator();
  void Init(const AnimScene* scene);
  void Update(float dt);
  void SetActive(bool active);
  void SetBlendingTime(float blendTime);
  void SetAnimationSpeed(float aniSpeed);
  void PauseAnimation();
  void ResumeAnimation();
  void PlayAnimation(const std::string& animationName);
  void Reset();
  // void SetInitialAnimation(const std::string&);

  bool isActive() const;
  bool isBlending() const;
  std::string GetCurrentAnimationName() const;
  std::vector<std::string> GetAnimationNameList() const;

  //call update before this function to get current frame animation data
  AnimInstance GetAnimationInstance() const;

private:
  std::string m_currName;
  const AnimScene* m_animScene;
  float m_blendTime;
  float m_blendElapsed;
  float m_elapsed;
  unsigned m_currAnimIndex;
  unsigned m_nextAnimIndex;
  bool m_isBlending;
  bool m_isActive;
  bool m_isPaused;
  float m_animationSpeed;
};


#endif
