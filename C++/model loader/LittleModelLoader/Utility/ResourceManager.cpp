#include "stdafx.h"
#include "ResourceManager.h"
#include "Graphics\TextureManager.h"

ResourceManager::ResourceManager()
{

}

ModelList & ResourceManager::GetModelList()
{
	return m_models;
}

TextureList & ResourceManager::GetModelTextureList()
{
	return m_modelTextures;
}

TextureList & ResourceManager::GetStandaloneTextureList()
{
	return m_standaloneTextures;
}

XMLList & ResourceManager::GetXMLList()
{
	return m_xmls;
}

void ResourceManager::LoadData()
{
	//std::vector<std::string> folderPaths{XML_DIR, TEXTURES_DIR, MODELS_DIR, AUDIO_DIR};
	//std::map<std::string, std::string> pathToExtensions
	//{
	//	std::pair<std::string, std::string>(XML_DIR, ".xml"),
	//	std::pair<std::string, std::string>(TEXTURES_DIR, ".png"),
	//	std::pair<std::string, std::string>(AUDIO_DIR, ".wav"),
	//	std::pair<std::string, std::string>(MODELS_DIR, "Model"),
	//};

	//std::map<std::string, std::vector<std::string>> fileFormatMap;

	//HANDLE hFind;
	//WIN32_FIND_DATA data;

	//std::string path(TEXTURES_DIR);

	//for(auto const& pathR : folderPaths)
	//{
	//	std::string path = pathR + "*.*";
	//	std::string extension = pathToExtensions[pathR];
	//	hFind = FindFirstFile(path.c_str(), &data);
	//	if (hFind != INVALID_HANDLE_VALUE) {
	//		do {
	//			std::string filename(data.cFileName);

	//			if (filename[0] != '.')
	//			{
	//				size_t found = filename.find('.');
	//				if (found != std::string::npos)
	//				{
	//						size_t index = filename.find(extension.c_str());
	//						if (index != std::string::npos)
	//						{
	//							fileFormatMap[extension].emplace_back(filename);
	//						}
	//				}
	//				else if (found == std::string::npos)
	//				{
	//					fileFormatMap[extension.c_str()].emplace_back(filename);
	//				}
	//			}
	//		} while (FindNextFile(hFind, &data));
	//		FindClose(hFind);
	//	}
	//}



}


