#pragma once
#include "Serialization.h"
#include "XMLManager.h"

class DataConverter
{
	Serialization::XMLNode m_data;
	XMLContainer* m_cont;
public:

	struct DataConverterItr
	{
		Serialization::XMLNodeItr m_itr;
		XMLContainer* m_cont;
	public:
		DataConverterItr();
		DataConverterItr(DataConverterItr const& itr);
		DataConverterItr(Serialization::XMLNodeItr itr, XMLContainer* cont);
		~DataConverterItr();
		bool operator==(DataConverterItr const& rhs);
		bool operator!=(DataConverterItr const& itr);

		DataConverter operator*();

		DataConverterItr operator++();

		DataConverterItr operator++(int);
	};

	DataConverter();
	DataConverter(Serialization::XMLNode node, XMLContainer* cont);
	DataConverter(DataConverter const& dCon);

	~DataConverter();

	bool Open(std::string const & path, std::string const& root = "", XMLContainerOptions options = XMLCO::NoOptions);

	template<typename T>
	void AddVar(std::string const& name, T const& var, Serialization::NodeAddOption opt = Serialization::NodeAddOption::Overwrite)
	{
		Serialization::AppendAttribute(m_data, name, std::to_string(var), opt);
	}

	template<>
	void AddVar(std::string const& name, std::string const& var, Serialization::NodeAddOption opt);

	template<typename T>
	void GetVar(std::string const& name, T& var)
	{
		Serialization::GetAttributeValue(m_data, name, var);
	}

	template<typename T>
	void EditVar(std::string const& name, T const& var)
	{
		Serialization::EditAttributeValue(m_data, name, var);
	}

	std::string GetNodeName();

	DataConverter AddScope(std::string const& name, Serialization::NodeAddOption opt = Serialization::NodeAddOption::Overwrite);

	DataConverter GetScope(std::string const& name) const;

	void RemoveScope(std::string const & name);

	DataConverter ExitScope() const;

	void SetChildMove(std::string const & name, DataConverter const & node);

	void SetChildCopy(std::string const & name, DataConverter const & node, Serialization::NodeAddOption opt = Serialization::NodeAddOption::Overwrite);

	using  DataConverterRange = std::pair<DataConverterItr, DataConverterItr>;

	DataConverterRange GetChildren() const;

	void ClearChildren();

	XMLContainer* GetXMLDocument();

	Serialization::XMLNode GetNode();

	void PrintRootTree();

	void PrintCurrentSubtree();
	void LogCurrentSubtree();
};

template<>
inline void DataConverter::AddVar(std::string const & name, std::string const & var, Serialization::NodeAddOption opt)
{
	Serialization::AppendAttribute(m_data, name, var, opt);
}
