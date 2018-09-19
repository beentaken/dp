/******************************************************************************/
/*!
\file		TextureManager.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "TextureManager.h"
#include "Utility\ResourceManager.h"
#include "Utility\Logging.h"
#include "Texture Loading\DDSLoader.h"

Texture* TextureManager::LoadModelTexture(const std::string& file, const std::string & directory)
{
	return LoadTexture(file, directory, TextureType::Model);
}

Texture * TextureManager::LoadStandaloneTexture(const char * file, const std::string & directory)
{
	return LoadTexture(file, directory, TextureType::Standalone);
}

Texture * TextureManager::LoadSpriteSheetTexture(const char * file, const std::string & directory)
{
	return LoadTexture(file, directory, TextureType::SpriteSheet);
}

void TextureManager::LoadTextures()
{
  static GraphicsSystem & gs = GraphicsSystem::GetInstance();
	HANDLE hFind;
	WIN32_FIND_DATA data;

	std::string path(TEXTURES_DIR);
	std::vector<std::string> extensions{".png", ".dds"};

	path += "*.*";

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
							LoadStandaloneTexture(filename.c_str(), TEXTURES_DIR);
              gs.UpdateLoadingScreen();
              break;
						}

						++beg;
					}
				}
			}
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

	UpdateStandaloneTextureString();
}

Texture * TextureManager::LoadTexture(const std::string& file, const std::string & directory, TextureType type)
{
  Texture * tptr = nullptr;
  // Add texture to map and return a ptr for Model class to modify
  switch (type)
  {
    case TextureType::Model:
    {
      auto result = m_modelTextures.emplace(std::piecewise_construct,
        std::forward_as_tuple(file),
        std::forward_as_tuple());
      tptr = &result.first->second;
    }
    break;
    case TextureType::Standalone:
    {
      m_standaloneTextureNames.emplace_back(file);
      auto result = m_standaloneTextures.emplace(std::piecewise_construct,
          std::forward_as_tuple(file),
          std::forward_as_tuple());
      tptr = &result.first->second;
    }
    break;
    case TextureType::SpriteSheet:
    {
      auto result = m_spriteSheetTextures.emplace(std::piecewise_construct,
          std::forward_as_tuple(file),
          std::forward_as_tuple());
      tptr = &result.first->second;
  }
  }
  std::string filepath = directory + '/' + file;
  if (tptr->id == 0)
  {
    tptr->id = LoadDDSTexture(filepath);
    tptr->filename = file;
  }
  return tptr;
}


bool TextureManager::isModelTextureLoaded(const std::string& path) const
{
  return m_modelTextures.count(path) == 1;
}

bool TextureManager::isStandaloneTextureLoaded(const std::string& path) const
{
  return m_standaloneTextures.count(path) == 1;
}

TextureManager::TextureManager() : m_modelTextures(ResourceManager::GetInstance().GetModelTextureList())
, m_standaloneTextures(ResourceManager::GetInstance().GetStandaloneTextureList()), m_spriteSheetTextures(ResourceManager::GetInstance().GetSpriteSheetTextureList())
 {}

Texture * TextureManager::GetModelTexture(const std::string& texName)
{
  TextureList::iterator it = m_modelTextures.find(texName);
  if (it != m_modelTextures.end())
  {
    return &(it->second);
  }
  else return nullptr;
}

Texture * TextureManager::GetStandaloneTexture(const std::string& texName)
{
	TextureList::iterator it = m_standaloneTextures.find(texName);
	if (it != m_standaloneTextures.end())
	{
		return &(it->second);
	}
	else return nullptr;
}

void TextureManager::Terminate()
{
  //free all textures when graphics system terminates
  for (auto& elem : m_modelTextures)
  {
    //SOIL_free_image_data(elem.second.image_data);
    glDeleteTextures(1, &elem.second.id);
    
  }
  m_modelTextures.clear();

  for (auto& elem : m_standaloneTextures)
  {
	  //SOIL_free_image_data(elem.second.image_data);
    glDeleteTextures(1, &elem.second.id);
  }
  m_standaloneTextures.clear();
}

void TextureManager::SetGamma(bool value)
{
  m_gammaCorrect = value;
}

std::vector<std::string>& TextureManager::GetStandaloneTextureNames()
{
	return m_standaloneTextureNames;
}

TextureList & TextureManager::GetStandaloneTextures()
{
	return m_standaloneTextures;
}

std::string TextureManager::GetStandaloneTextureString()
{
	return m_standaloneTextureStr;
}

void TextureManager::UpdateStandaloneTextureString()
{
	m_standaloneTextureStr = "";

	for (auto& elem : m_standaloneTextureNames)
	{
		m_standaloneTextureStr += elem;
		m_standaloneTextureStr += '\0';
	}
}
