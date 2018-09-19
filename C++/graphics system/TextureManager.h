/******************************************************************************/
/*!
\file		TextureManager.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef _TEXTUREMANAGER_H
#define _TEXTUREMANAGER_H
#pragma once

#include "Utility\Singleton.h"
#include "Graphics\ogl.h"


//brief: manager to store textures and prevent same textures from being loaded multiple times

struct Texture
{
  GLuint id;
  std::string type;
  std::string filename;
  //unsigned char* image_data;
  Texture(/*std::string file = {}, std::string t = {}, GLuint i = 0*/) : id(0), type(), filename() {}
};

using TextureList = std::unordered_map<std::string, Texture>;

class TextureManager : public Singleton<TextureManager>
{
public:
	enum class TextureType
	{
		Model,
		Standalone,
		SpriteSheet,
	};
	TextureManager();
  ~TextureManager() = default;

  Texture* GetModelTexture(const std::string& texName);
  Texture * GetStandaloneTexture(const std::string & texName);
  bool isModelTextureLoaded(const std::string& path) const;
  bool isStandaloneTextureLoaded(const std::string& path) const;
  Texture* LoadModelTexture(const std::string& path, const std::string & directory);
  Texture* LoadStandaloneTexture(const char* path, const std::string & directory);
  Texture* LoadSpriteSheetTexture(const char * file, const std::string & directory);
  void LoadTextures();
  void Terminate();
  void SetGamma(bool value);


  std::vector<std::string>& GetStandaloneTextureNames();
  TextureList& GetStandaloneTextures();
  std::string GetStandaloneTextureString();
  void UpdateStandaloneTextureString();

private:
  Texture* LoadTexture(const std::string& file, const std::string & directory, TextureType type);
  Texture* LoadTexture_SOIL(const std::string& file, const std::string & directory, TextureType type);
  TextureList& m_modelTextures;
  TextureList& m_standaloneTextures;
  TextureList& m_spriteSheetTextures;


  std::vector<std::string> m_standaloneTextureNames;
  std::string m_standaloneTextureStr;
  bool m_gammaCorrect = true;
};


#endif
