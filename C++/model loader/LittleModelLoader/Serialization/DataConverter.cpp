#include <iostream>
#include <sstream>
#include "DataConverter.h"

#pragma region DataCoverterItr

DataConverter::DataConverterItr::DataConverterItr() {}
DataConverter::DataConverterItr::DataConverterItr(DataConverterItr const& itr) : m_itr(itr.m_itr), m_cont(itr.m_cont) {}
DataConverter::DataConverterItr::DataConverterItr(Serialization::XMLNodeItr itr, XMLContainer* cont) : m_itr(itr), m_cont(cont) {}
DataConverter::DataConverterItr::~DataConverterItr() {}

bool DataConverter::DataConverterItr::operator==(DataConverter::DataConverterItr const& rhs)
{
	return m_itr == rhs.m_itr;
}

bool DataConverter::DataConverterItr::operator!=(DataConverterItr const & itr)
{
	return !operator==(itr);
}

DataConverter DataConverter::DataConverterItr::operator*()
{
	return DataConverter(*m_itr, m_cont);;
}

DataConverter::DataConverterItr DataConverter::DataConverterItr::operator++()
{
	++m_itr;
	return *this;
}

DataConverter::DataConverterItr DataConverter::DataConverterItr::operator++(int)
{
	DataConverterItr temp(*this);
	++m_itr;
	return temp;
	
}

#pragma endregion

#pragma region DataConverter

// Used when you are opening an XML file for the first time
// Will require calls to 
// - Open right after constructor
// - Close right before destructor
// if an XML file will be loaded to it
DataConverter::DataConverter() : m_data(), m_cont(nullptr) {}

// Used when traversing between scopes
// Will require call to Close before destructor
DataConverter::DataConverter(Serialization::XMLNode node, XMLContainer* cont) : m_data(node), m_cont(cont) 
{ 
	m_cont->IncreaseRef(); 
}
DataConverter::DataConverter(DataConverter const& dCon) : m_data(dCon.m_data), m_cont(dCon.m_cont) { m_cont->IncreaseRef(); }

// Call right after constructor
bool DataConverter::Open(std::string const & path, std::string const& root, XMLContainerOptions options)
{
	std::string tmpPath = (static_cast<unsigned char>(options & XMLCO::TempFile) || path.find(XML) != std::string::npos) ? path : path + XML;
	if (m_cont)
	{
		if (m_cont->m_path == tmpPath) 
			return true;
		else 
			XMLManager::GetInstance().Close(m_cont);
	}

	m_cont = XMLManager::GetInstance().Open(path, root, options);

	if (!m_cont)
	{
		XMLManager::GetInstance().Close(path);
		return false;
	}

	m_data = m_cont->GetRootNode();

	return true;
}

DataConverter::~DataConverter()
{
	if (m_cont) XMLManager::GetInstance().Close(m_cont);
}

std::string DataConverter::GetNodeName()
{
	return Serialization::GetNodeName(m_data);
}

DataConverter DataConverter::AddScope(std::string const& name, Serialization::NodeAddOption opt)
{
	return DataConverter(Serialization::AppendChild(m_data, name, opt), m_cont);
}

DataConverter DataConverter::GetScope(std::string const& name) const
{
	return DataConverter(Serialization::GetChild(m_data, name), m_cont);
}

void DataConverter::RemoveScope(std::string const& name)
{
	if (m_data.child(name.c_str())) Serialization::RemoveChild(m_data, name);
}

DataConverter DataConverter::ExitScope() const
{
	return DataConverter(Serialization::GetParent(m_data), m_cont);
}

// Moves node's position to be child of m_data (To be used within the same tree)
void DataConverter::SetChildMove(std::string const& name, DataConverter const& node)
{
	Serialization::SetNodeName(Serialization::SetChildMove(m_data, node.m_data), name);
}

// Copies over node from its original tree to be child of m_data (To be used between trees)
void DataConverter::SetChildCopy(std::string const& name, DataConverter const& node, Serialization::NodeAddOption opt)
{
	Serialization::SetNodeName(Serialization::SetChildCopy(m_data, node.m_data, opt), name);
}

DataConverter::DataConverterRange DataConverter::GetChildren() const
{
	return DataConverterRange(DataConverterItr(m_data.children().begin(), m_cont), DataConverterItr(m_data.children().end(), m_cont));
}

void DataConverter::ClearChildren()
{
	auto itr = m_data.children().begin();
	//m_data.children().begin
	while (itr != m_data.children().end())
	{
		m_data.remove_child(*itr++);
	}
}

XMLContainer * DataConverter::GetXMLDocument()
{
	return m_cont;
}

Serialization::XMLNode DataConverter::GetNode()
{
	return m_data;
}

void DataConverter::PrintRootTree()
{
	m_cont->m_doc.print(std::cout);
}

void DataConverter::PrintCurrentSubtree()
{
	m_data.print(std::cout);
}

void DataConverter::LogCurrentSubtree()
{
	std::stringstream ss;
	m_data.print(ss);
}

#pragma endregion
