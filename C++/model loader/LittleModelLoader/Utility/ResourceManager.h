#pragma once
#include "Serialization\XMLManager.h"

static std::string const XML = ".xml";

static std::string const EXTENSION = XML;

static std::string const ASSETS_DIR = "Assets/";
static std::string const MODELS_DIR = ASSETS_DIR + "Models/";
static std::string const TEXTURES_DIR = ASSETS_DIR + "Textures/";
static std::string const XML_DIR = ASSETS_DIR + "XML/";
static std::string const AUDIO_DIR = ASSETS_DIR + "Audio/";

static std::string const DATA_DIR = "Data/";
static std::string const ARCHETYPES_DIR = DATA_DIR + "Archetypes/";
static std::string const EDITOR_DATA_DIR = DATA_DIR + "EditorData/";

static std::string const CONFIG_DIR = "Config/";

static std::string const ARCHETYPES = "Archetypes";

static std::string const LEVELS_DIR = DATA_DIR + "Levels/";
static std::string const SCENES_DIR = DATA_DIR + "Scenes/";


#define GetDirPath(directory, filename) directory + filename
#define GetXMLFile(directory, filename) GetDirPath(directory, filename) + EXTENSION
#define GetFilePath(directory, filename, extension) GetDirPath(directory, filename) + extension
#define GetScenePath(level, scene) level + "_" + scene


class ResourceManager : public Singleton<ResourceManager>
{
	XMLList m_xmls;

public:
	ResourceManager();
	~ResourceManager() = default;

	XMLList& GetXMLList();

	void LoadData();
};

