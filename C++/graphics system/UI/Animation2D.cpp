/******************************************************************************/
/*!
\file		Animation2D.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "Animation2D.h"
#include "2DAnimationLoader.h"

void Animator2D::SetAnimSpeed(float fps)
{
  // Assert("Animator2D: fps is zero", fps != 0.f);
  frame_time = (fps == 0.f) ? 0.f : 1.f / fps;
  timer = frame_time;
}

void Animator2D::SetAnimation(const std::string & animName)
{
  pAnimation = AnimationLoader::GetInstance().GetAnimationData(animName);

  //reset all animation timers
  Stop();
}

void Animator2D::Update(float dt)
{
  if (!playing) return;

  elapsed += dt;

  //if only playing once, break once a cycle is completed
  if (play_once && frameCount == pAnimation->totalFrames - 1)
  {
    Stop();
    return;
  }

  //if timer greater than one frame time, get next frame uv
  if (timer != 0.f && elapsed > timer)
  {
    if (play_once) ++frameCount;

    FrameUVCoord result = pAnimation->NextFrameUVCoord(&currIndex, reversed);
    currFrame.origin = result.bot_left;
    currFrame.uv = Vec2(result.u_size, result.v_size);
    elapsed = 0.f;
  }
}

void Animator2D::Stop()
{
  //reset all to default
  currIndex = 0;
  frameCount = 0;
  playing = false;
  play_once = false;
  elapsed = 0.f;
  timer = frame_time;                                           
  reversed = false;

  //get uv of first frame
  FrameUVCoord result = pAnimation->GetUVCoord(0);
  currFrame.origin = result.bot_left;
  currFrame.uv = Vec2(result.u_size, result.v_size);
  currFrame.texture = pAnimation->texture;
}

void Animator2D::Play(bool reverse, bool playonce)
{
  playing = true;
  reversed = reverse;
  play_once = playonce;
}

void Animator2D::Pause()
{
  playing = false;
}

FrameData Animator2D::GetCurrentFrame() const
{
  return currFrame;
}

void Animator2D::MoveFrames(int magnitude)
{
  if (pAnimation != nullptr)
  {
    int tmpval = (currIndex + magnitude) % pAnimation->totalFrames;
    currIndex = (tmpval < 0) ? tmpval + pAnimation->totalFrames : tmpval;
    FrameUVCoord & result = pAnimation->uvList[currIndex];
    currFrame.origin = result.bot_left;
    currFrame.uv = Vec2(result.u_size, result.v_size);
  }
} 

void Animator2D::SetFrameNumber(int frameNo)
{
	if (pAnimation != nullptr)
	{
		if (frameNo < 0)
			currIndex = 0;
		else if (frameNo >= (int)pAnimation->totalFrames)
			currIndex = pAnimation->totalFrames - 1;
		else
			currIndex = frameNo;

		FrameUVCoord & result = pAnimation->uvList[currIndex];
		currFrame.origin = result.bot_left;
		currFrame.uv = Vec2(result.u_size, result.v_size);
	}
}
