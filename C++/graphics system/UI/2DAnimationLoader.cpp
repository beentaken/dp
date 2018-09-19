/******************************************************************************/
/*!
\file		2DAnimationLoader.cpp
\project	Little Chef Story
\primary author Yeo Zhi Xiang

All content \@ 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "2DAnimationLoader.h"
#include "Serialization\DataConverter.h"
#include "Utility\ResourceManager.h"

static const unsigned MAX_ANIMATIONS = 1000;

FrameUVCoord AnimationContainer::NextFrameUVCoord(unsigned* index, bool reverse)
{
	//finds the next frame's uv coordinates of the animation from the container
	if (!reverse)
	{
		if (*index >= totalFrames)
			*index = 0;

		return uvList[(*index)++];
	}
	//if animation is being played in reverse, find the previous's frame uv coordinates
	else
	{
		if (*index < 0)
			*index = totalFrames - 1;

		return uvList[(*index)--];
	}
}

FrameUVCoord AnimationContainer::GetUVCoord(int index)
{
	return uvList[index];
}

AnimationLoader::AnimationLoader()
{
  animationList.reserve(MAX_ANIMATIONS);
  animationListStr = "";
}

AnimationLoader::~AnimationLoader()
{
}

AnimationContainer* AnimationLoader::GetAnimationData(const std::string & animationName)
{
  auto it = std::find_if(animationList.begin(), animationList.end(),
                                                  [&](const AnimationContainer& cont)
                                                {
                                                  return cont.animationName == animationName;
                                                }
                                                );
  Assert("Unable to find animation name", it != animationList.end());
  return &(*it);
}

AnimationContainer* AnimationLoader::CreateAnimationContainer(std::string const& animName, float texWidth, float texHeight, GLuint id)
{
	if (std::find(animationNames.nameList_.begin(), animationNames.nameList_.end(), animName) == animationNames.nameList_.end())
	{
		animationListStr += animName + '\0';
		animationList.emplace_back(AnimationContainer());
		animationNames.nameList_.emplace_back(animName);

		AnimationContainer* animCont = &animationList.back();
		animCont->texture = animationList.size() - 1;												//definitely wrong
		animCont->texWidth = texWidth;
		animCont->texHeight = texHeight;
    animCont->animationName = animName;
    animCont->texture = id;

		return animCont;
	}
	return nullptr;

}

AnimationContainer::~AnimationContainer()
{}

void AnimationLoader::LoadSpriteSheets()
{
	HANDLE hFind;
	WIN32_FIND_DATA data;

	std::string path(SPRITESHEET_DIR);
	std::vector<std::string> extensions{ ".dds"};

	path += "*.*";

	TextureManager& texMgr = TextureManager::GetInstance();

	hFind = FindFirstFile(path.c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			std::string filename(data.cFileName);

			if (filename[0] != '.')
			{
				size_t found = filename.find('.');
				if (found != std::string::npos)
				{
					auto beg = extensions.begin();

					while (beg != extensions.end())
					{
						size_t index = filename.find(*beg);
						if (index != std::string::npos)
						{
              Texture* loadedTex = texMgr.LoadSpriteSheetTexture(filename.c_str(), SPRITESHEET_DIR);
							ReadTexture(filename, loadedTex->id);
							break;
						}

						++beg;
					}
				}
			}
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

}

void AnimationLoader::ReadSingleImage(AnimationContainer* animCont, DataConverter& dc, float w, float h)
{
	++animCont->totalFrames;
	animCont->uvList.emplace_back(CutFrames(dc, w, h));
}

void AnimationLoader::ReadTexture(std::string const& filename, GLuint id)
{
	size_t pos = 0;
	const std::string delimiter = "%";
	std::string animNameTemp = "";
	std::string animName = "";
	float textureWidth, textureHeight;

	std::string xmlFile = filename.substr(0, filename.find_last_of('.')) + ".xml";

	DataConverter dc;
	AnimationContainer* animCont;

	if (dc.Open(SPRITESHEET_DIR + "/" + xmlFile))
	{
		dc.GetVar("width", textureWidth);
		dc.GetVar("height", textureHeight);

		auto range = dc.GetChildren();

		while (range.first != range.second)
		{
			auto& itemDc = *range.first++;

			//get animation name
			itemDc.GetVar("n", animNameTemp);
			animNameTemp = animNameTemp.substr(0, animNameTemp.find_first_of('%'));


			if (animNameTemp != animName)
			{
		      // Create new container for ths animation name
			  animCont = CreateAnimationContainer(animName = animNameTemp, textureWidth, textureHeight, id);
				
			}

			//add in every frame in the animation
			ReadSingleImage(animCont, itemDc, textureWidth, textureHeight);
		}
	}
}

FrameUVCoord AnimationLoader::CutFrames(DataConverter& dc, const float textureWidth, const float textureHeight)
{
	float x, y, w, h ;
	dc.GetVar("x", x);
	dc.GetVar("y", y);
	dc.GetVar("w", w);
	dc.GetVar("h", h);

  float inv_width = 1.f / textureWidth;
  float inv_height = 1.f / textureHeight;

  float y_pos = y + h;

	Vec2 bot_left(x * inv_width, y_pos * inv_height);

	return FrameUVCoord(bot_left, w * inv_width, h * inv_height);
}