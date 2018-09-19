/******************************************************************************/
/*!
\file		FontManager.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <unordered_map>
#include "Utility\Singleton.h"
#include "Graphics\Shader.h"
#include "Math\OMath.h"
#include "FontClasses.h"

class FontComponent;

//creates and store fonts
class FontManager : public Singleton<FontManager> 
{
public:
  void Init();
  void Terminate();
  //Font Loader given a .ttf file name
  void LoadFont(const std::string&);
  //Given the name of a font, find and return a pointer to the loaded font
  Font* GetFont(std::string);

  std::vector<std::string>const & GetFontNames() const;
  std::string GetFontNamesStr() const;

private:

	std::string fontNamesStr;
  //List of font(.ttf) file names
  std::vector <std::string> m_fileNames;
  //Map of loaded fonts and related names
  std::unordered_map <std::string, Font> m_fontList;
};

#endif
