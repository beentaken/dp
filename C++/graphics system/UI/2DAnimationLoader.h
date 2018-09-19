/******************************************************************************/
/*!
\file		2DAnimationLoader.h
\project	Little Chef Story
\primary author Yeo Zhi Xiang

All content \@ 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef ANIMATION_LOADER_H
#define ANIMATION_LOADER_H

#include "Math\OMath.h"
#include <fstream>
#include <unordered_map>
#include "Utility\Singleton.h"
#include "Utility\NameToItemList.h"
#include "Graphics\ogl.h"

class DataConverter;

struct FrameUVCoord
{
	FrameUVCoord(Vec2 bl, float width, float height) :
		bot_left(bl), u_size(width), v_size(height)
	{
	}

	Vec2 bot_left;
  float u_size;
  float v_size;
};

struct AnimationContainer
{
  std::vector<FrameUVCoord> uvList;
  std::string animationName;
  GLuint texture;
  float texWidth;
  float texHeight;
  float frameRate = 0.f;
  unsigned totalFrames = 0;

  ~AnimationContainer();
  FrameUVCoord NextFrameUVCoord(unsigned* index, bool reverse = false);
  FrameUVCoord GetUVCoord(int index);
};

struct AnimationLoader : Singleton<AnimationLoader>
{
	//using AnimationList = std::list<AnimationContainer>;
  using AnimationList = std::vector<AnimationContainer>;
	AnimationList animationList;
	NameToItemList animationNames;
	std::string animationListStr;

  AnimationLoader();
	~AnimationLoader();
  AnimationContainer* GetAnimationData(const std::string& animationName);
	AnimationContainer* CreateAnimationContainer(std::string const&, float texWidth, float texHeight, GLuint id);
	void LoadSpriteSheets();
	void ReadSingleImage(AnimationContainer* animCont, DataConverter & dc, float w, float h);
	void ReadTexture(std::string const& filename, GLuint id);
	FrameUVCoord CutFrames(DataConverter& dc, const float textureWidth, const float textureHeight);
	void CutSingleSS(unsigned int texture, std::string const& animName, float textureWidth, const float textureHeight, const int row = 1, const int col = 1);
	void LoadSingleImage(unsigned int texture, std::string const& animName);
};

#endif