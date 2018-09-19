#pragma once
#include <unordered_map>
#include "Utility\Singleton.h"
#include "Serialization.h"
#include <string>

enum class XMLContainerOptions : unsigned char
{
	NoOptions = 0x1,
	TempFile = 0x2,
	SaveFile = 0x4,
	CreateIfNotFound = 0x8,
};

using XMLCO = XMLContainerOptions;

XMLContainerOptions operator| (XMLContainerOptions lhs, XMLContainerOptions rhs);

XMLContainerOptions operator& (XMLContainerOptions lhs, XMLContainerOptions rhs);

struct XMLContainer
{

	Serialization::XMLDocument m_doc;
	std::string m_path;
	unsigned m_ref;
	XMLContainerOptions m_options;

	XMLContainer();

	~XMLContainer();

	bool Load(std::string const& path, std::string const& root = "", XMLContainerOptions options = XMLCO::NoOptions);

	void Save(bool save = false);

	Serialization::XMLNode GetRootNode();

	void IncreaseRef();

	unsigned DecreaseRef();

	bool CheckOption(XMLContainerOptions opt);

};

using XMLList = std::unordered_map<std::string, XMLContainer>;

class XMLManager : public Singleton<XMLManager>
{
	XMLList& m_xmls;

public:
	XMLManager();
	~XMLManager() = default;

	XMLContainer* Open(std::string const& path, std::string const& root = "", XMLContainerOptions options = XMLCO::NoOptions);

	// Decreases the reference counter of the XMLContainer and, if zero, will erase the XMLContainer.
	// To close the XMLContainer regardless of the reference counter, set bool to true, but you really shouldn't do this. (Unless the reference counter is screwed)
	void Close(XMLContainer* cont, bool close = false);
	void Close(std::string const & name);

	void Save(std::string const& name);
};

