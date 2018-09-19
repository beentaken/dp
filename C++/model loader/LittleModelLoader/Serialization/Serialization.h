
#pragma once
#include "pugi\pugixml.hpp"

//////////
//////////
//////////	XMLDocument is just XMLNode with ability to save and load
//////////
//////////
//////////	How to load an XML file to project
//////////
//////////	- Create an XMLDocument object
//////////	- Call LoadXML with XMLDocument object and file location string
//////////	- Loaded
//////////
//////////
//////////	How to save an XML file
//////////	
//////////	- Call SaveXML with XMLDocument object and file location string
//////////	- Saved
//////////
//////////
//////////	XML file structure
//////////
//////////		<Parent Name Attribute name="Attribute value" Attribute name="Attribute value" ...>			Parent
//////////			<Name Attribute name="Attribute value" Attribute name="Attribute value" ... />				Previous sibling
//////////			<Name Attribute name="Attribute value" Attribute name="Attribute value" ...>				Main
//////////				<Child Name Attribute name="Attribute value" Attribute name="Attribute value" />			Child
//////////			</Name>
//////////			<Name Attribute name="Attribute value" Attribute name="Attribute value" ... />				Next sibling
//////////			<Name>Value</Name>
//////////		</Parent Name>
//////////	
//////////	

namespace Serialization
{
	enum NodeAddOption
	{
		NoOptions = 0,
		Overwrite,
		AddNew,

	};

	using XMLNode = pugi::xml_node;
	using XMLNodeItr = pugi::xml_node_iterator;
	using XMLNamedNodeItr = pugi::xml_named_node_iterator;
	using XMLAttribute = pugi::xml_attribute;
	using XMLAttributeItr = pugi::xml_attribute_iterator;
	using XMLDocument = pugi::xml_document;
	using XMLParseResult = pugi::xml_parse_result;

	bool CheckIfNodeExists(XMLNode node, std::string const& name);

	bool CheckIfAttributeExists(XMLNode node, std::string const& attrName);


#pragma region Value

	/// <summary>
	/// <para>Function template getting the value of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode returning its value</param>
	/// <param name='value'>The value containing the returned value</param>
	template <typename T>
	void GetValue(XMLNode node, T& value)
	{
	}

	/// <summary>
	/// <para>Specialized function template getting the float value of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode returning its value</param>
	/// <param name='value'>The value containing the returned value</param>
	template <>
  void GetValue(XMLNode node, float& value);


	/// <summary>
	/// <para>Specialized function template getting the int value of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode returning its value</param>
	/// <param name='value'>The value containing the returned value</param>
	template <>
  void GetValue(XMLNode node, int& value);

	/// <summary>
	/// <para>Specialized function template getting the double value of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode returning its value</param>
	/// <param name='value'>The value containing the returned value</param>
	template <>
  void GetValue(XMLNode node, double& value);

	/// <summary>
	/// <para>Specialized function template getting the string value of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode returning its value</param>
	/// <param name='value'>The value containing the returned value</param>
	template <>
  void GetValue(XMLNode node, std::string& value);

	/// <summary>
	/// <para>Specialized function template getting the bool value of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode returning its value</param>
	/// <param name='value'>The value containing the returned value</param>
	template <>
  void GetValue(XMLNode node, bool& value);

	/// <summary>
	/// <para>Function template setting the value of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode changing its value</param>
	/// <param name='value'>The new value</param>
	template <typename T>
	void SetValue(XMLNode node, T const& value)
	{
		node.set_value(std::to_string(value).c_str());
	}

#pragma endregion

#pragma region Node name

	/// <summary>
	/// <para>Gets the name of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode returning its name</param>
	std::string GetNodeName(XMLNode node);

	/// <summary>
	/// <para>Edits the name of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode with its name changed</param>
	/// <param name='name'>The new name</param>
	bool SetNodeName(XMLNode node, std::string const& name);

#pragma endregion

#pragma region Parent-Child-Sibling

	/// <summary>
	/// <para>Gets a child node from an XML node.</para>
	/// </summary>
	/// <param name='parent'>XMLNode returning its child node</param>
	/// <param name='name'>The child name</param>
	XMLNode GetChild(XMLNode parent, std::string const& name);

	/// <summary>
	/// <para>Appends a child node to an XML node.</para>
	/// </summary>
	/// <param name='parent'>XMLNode getting a child node</param>
	/// <param name='name'>The child name</param>
	XMLNode AppendChild(XMLNode parent, std::string const& name, NodeAddOption opt = Overwrite);

	/// <summary>
	/// <para>Sets a node to be the child of a XML node. (Within tree only)</para>
	/// </summary>
	/// <param name='parent'>XMLNode getting a child node</param>
	/// <param name='child'>The child node</param>
	XMLNode SetChildMove(XMLNode parent, XMLNode child);

	/// <summary>
	/// <para>Sets a node to be the child of a XML node. (Between trees only)</para>
	/// </summary>
	/// <param name='parent'>XMLNode getting a child node</param>
	/// <param name='child'>The child node</param>
	XMLNode SetChildCopy(XMLNode parent, XMLNode child, NodeAddOption opt = Overwrite);

	/// <summary>
	/// <para>Removes a child node from an XML node.</para>
	/// </summary>
	/// <param name='parent'>XMLNode losing a child node</param>
	/// <param name='name'>The child name</param>
	bool RemoveChild(XMLNode parent, std::string const& name);

	/// <summary>
	/// <para>Gets the parent node of an XML node.</para>
	/// </summary>
	/// <param name='child'>XMLNode returning its parent node</param>
	XMLNode GetParent(XMLNode child);

	/// <summary>
	/// <para>Gets the previous sibling node of an XML node.</para>
	/// </summary>
	/// <param name='child'>XMLNode returning its previous sibling node</param>
	XMLNode GetPrevSibiling(XMLNode sibling);

	/// <summary>
	/// <para>Gets the next sibling node of an XML node.</para>
	/// </summary>
	/// <param name='child'>XMLNode returning its next sibling node</param>
	XMLNode GetNextSibiling(XMLNode sibling);

#pragma endregion

#pragma region Attribute

	/// <summary>
	/// <para>Appends an attribute to the XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the new attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='attrValue'>The attribute value</param>
	XMLAttribute AppendAttribute(XMLNode node, std::string const& attrName, std::string const& attrValue, NodeAddOption opt = Overwrite);

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The T reference to store the value</param>
	template <typename T>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, T& value)
	{
		Assert("Cannot take in variable of this type", false);

    //yuhong added(must return boolean)
    return false;
	}

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The float reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, float& value);

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The double reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, double& value);

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The int reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, int& value);

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The string reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, std::string& value);

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The bool reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, bool& value);

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The unsigned reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, unsigned int& value);

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The unsigned reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, char& value);

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The unsigned reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, unsigned char& value);

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The long long reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, long long& value);

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The unsigned long long reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, unsigned long long& value);

	/// <summary>
	/// <para>Edits an attribute in the XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode editng the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='attrValue'>The new attribute value</param>
	bool EditAttributeValue(XMLNode node, std::string const& attrName, std::string const& attrValue);

	/// <summary>
	/// <para>Edits an attribute in the XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode editng the attribute</param>
	/// <param name='attrName'>The old attribute name</param>
	/// <param name='attrValue'>The new value</param>
	bool EditAttributeName(XMLNode node, std::string const& oldAttrName, std::string const& newAttrName);

	/// <summary>
	/// <para>Removes an attribute from the XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode losing an attribute</param>
	/// <param name='attrName'>The attribute name</param>
	bool RemoveAttribute(XMLNode node, std::string const& attrName);

#pragma endregion

}
