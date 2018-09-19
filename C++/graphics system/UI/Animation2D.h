/******************************************************************************/
/*!
\file		Animation2D.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef ANIMATION_2D_
#define ANIMATION_2D_

#include "2DAnimationLoader.h"

static const float DEFAULT_FRAME_TIME = 1.f / 24.f;

struct FrameData
{
  GLuint texture;
  Vec2 uv;
  Vec2 origin;          //the bottom left of the quad in texture coordinates
};

class Animator2D
{
public:
  Animator2D() : elapsed(0.f), frame_time(DEFAULT_FRAME_TIME), timer(frame_time), frameCount(0), currIndex(0), playing(true), reversed(false), play_once(false), pAnimation(nullptr)
  {}
  
  void SetAnimSpeed(float fps);
  void SetAnimation(const std::string& animName);
  void Update(float dt);

  void Play(bool reverse = false, bool playonce = false);
  void Pause();
  void Stop();
  FrameData GetCurrentFrame() const;

  void MoveFrames(int);
  void SetFrameNumber(int);

private:
  FrameData currFrame;
  AnimationContainer* pAnimation;
  float elapsed;
  float frame_time;
  float timer;
  unsigned frameCount;
  unsigned currIndex;
  bool  playing;
  bool  reversed;
  bool  play_once;
};



#endif

