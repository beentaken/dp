#include "XMLManager.h"
#include "Utility\ResourceManager.h"

XMLContainerOptions operator| (XMLContainerOptions lhs, XMLContainerOptions rhs)
{
	return static_cast<XMLContainerOptions>(static_cast<unsigned char>(lhs) | static_cast<unsigned char>(rhs));
}

XMLContainerOptions operator& (XMLContainerOptions lhs, XMLContainerOptions rhs)
{
	return static_cast<XMLContainerOptions>(static_cast<unsigned char>(lhs) & static_cast<unsigned char>(rhs));
}

#pragma region XMLContainer

XMLContainer::XMLContainer() : m_path(""), m_ref(0) {}

XMLContainer::~XMLContainer()
{}

bool XMLContainer::Load(std::string const& path, std::string const& root, XMLContainerOptions options)
{
	IncreaseRef();
	m_path = path;
	m_options = options;

	if (CheckOption(XMLCO::TempFile))
	{
		if(!m_doc.child(root.c_str())) m_doc.append_child(root.c_str());
	}
	else
	{
		Serialization::XMLParseResult result = m_doc.load_file(m_path.c_str());

		if (result.status != pugi::status_ok)
		{
			// Error checking
			switch (result.status)
			{
				case pugi::status_file_not_found:
				{
					if (!CheckOption(XMLCO::CreateIfNotFound)) return false;
					break;
				}
			}
		}

		if (!m_doc.child(root.c_str()))  m_doc.append_child(root.c_str());
	}

	return true;
}

void XMLContainer::Save(bool save)
{
	if (save || static_cast<unsigned char>(m_options & XMLCO::SaveFile))  m_doc.save_file(m_path.c_str());
}

Serialization::XMLNode XMLContainer::GetRootNode()
{
	return m_doc.document_element();
}

void XMLContainer::IncreaseRef() { ++m_ref; }

unsigned XMLContainer::DecreaseRef() { return --m_ref; }

bool XMLContainer::CheckOption(XMLContainerOptions opt)
{
	return static_cast<unsigned char>(m_options & opt);
}

#pragma endregion

#pragma region XMLManager

XMLManager::XMLManager() : m_xmls(ResourceManager::GetInstance().GetXMLList()) {}

XMLContainer* XMLManager::Open(std::string const& path, std::string const& root, XMLContainerOptions options)
{
	return (m_xmls.find(path) == m_xmls.end() && !m_xmls[path].Load(path, root, options)) ? 
		nullptr : 
		&m_xmls[path];
}

void XMLManager::Close(XMLContainer* cont, bool close)
{
	if (cont && !cont->CheckOption(XMLCO::TempFile) && (close || cont->DecreaseRef() <= 0))
	{
		Close(cont->m_path);
		cont = nullptr;
	}

}

void XMLManager::Close(std::string const& name)
{
	if (m_xmls.find(name) != m_xmls.end())
	{
		m_xmls[name].Save();
		m_xmls.erase(name);
	}
}

void XMLManager::Save(std::string const & name)
{
	if (m_xmls.find(name) != m_xmls.end())
	{
		m_xmls[name].Save(true);
	}
}

#pragma endregion
