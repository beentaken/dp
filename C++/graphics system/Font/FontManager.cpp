/******************************************************************************/
/*!
\file		FontManager.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "FontManager.h"
#include "Utility\ResourceManager.h"
#include "Core\Level.h"

static const std::string FONT_DIR = "Data/Font/";

//Initialise FontManager and load up shaders used
void FontManager::Init()
{
  //should load all fonts here
  //LoadFont("ROCKEB.ttf");
  //LoadFont("CooperBlackStd.otf");

  //std::vector<std::string> folderPaths{ XML_DIR, TEXTURES_DIR, MODELS_DIR, AUDIO_DIR };
  //std::map<std::string, std::string> pathToExtensions
  //{
	 // std::pair<std::string, std::string>(XML_DIR, ".xml"),
	 // std::pair<std::string, std::string>(TEXTURES_DIR, ".png"),
	 // std::pair<std::string, std::string>(AUDIO_DIR, ".wav"),
	 // std::pair<std::string, std::string>(MODELS_DIR, "Model"),
  //};

  //std::map<std::string, std::vector<std::string>> fileFormatMap;

  HANDLE hFind;
  WIN32_FIND_DATA data;

  std::string path(FONT_DIR);
  path += "*.*";
  std::vector<std::string> extensions
  {
	".otf",
	".ttf",
  };


  //for (auto const& pathR : folderPaths)
  //{
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
							  LoadFont(filename);
							  break;
						  }

						  ++beg;
					  }
				  }
			  }
		  } while (FindNextFile(hFind, &data));
		  FindClose(hFind);
	  }
  //}

}

//Shutdown the font manager
void FontManager::Terminate()
{
  m_fileNames.clear();
  m_fontList.clear();
}

//Font Loader given a .ttf file name
void FontManager::LoadFont(const std::string& fontFileName)
{
  //check if font has alreayd been loaded
  if (m_fontList.count(fontFileName) > 0) return;

  //else load
  std::string filepath = FONT_DIR + fontFileName;
  Font ft;
  ft.Init(filepath.c_str());
  m_fileNames.emplace_back(fontFileName);
  fontNamesStr += fontFileName;
  fontNamesStr += '\0';
  auto& result = m_fontList.emplace(std::piecewise_construct, std::forward_as_tuple(fontFileName), std::forward_as_tuple(ft));
  assert(result.second);

  return;
}

//Given the name of a font, find and return a pointer to the loaded font
Font * FontManager::GetFont(std::string fontname)
{
  auto itr = m_fontList.find(fontname);
  assert(itr != m_fontList.end());
  return &(itr->second);
}

std::vector<std::string> const & FontManager::GetFontNames() const
{
	return m_fileNames;
}

std::string FontManager::GetFontNamesStr() const
{
	return fontNamesStr;
}
