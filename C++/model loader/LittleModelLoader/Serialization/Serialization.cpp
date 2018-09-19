#include "Serialization\Serialization.h"


namespace Serialization
{
#pragma region Value

	

#pragma endregion

#pragma region Node name

	/// <summary>
	/// <para>Gets the name of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode returning its name</param>
	std::string GetNodeName(XMLNode node)
	{
		return node.name();
	}

	/// <summary>
	/// <para>Edits the name of an XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode with its name changed</param>
	/// <param name='name'>The new name</param>
	bool SetNodeName(XMLNode node, std::string const& name)
	{
		bool result = CheckIfNodeExists(node, name);
		if(result) node.set_name(name.c_str());
		return result;
	}

#pragma endregion

#pragma region Parent-Child-Sibling

	/// <summary>
	/// <para>Gets a child node from an XML node.</para>
	/// </summary>
	/// <param name='parent'>XMLNode returning its child node</param>
	/// <param name='name'>The child name</param>
	XMLNode GetChild(XMLNode parent, std::string const& name)
	{
		return parent.child(name.c_str());
	}

	/// <summary>
	/// <para>Appends a child node to an XML node.</para>
	/// </summary>
	/// <param name='parent'>XMLNode getting a child node</param>
	/// <param name='name'>The child name</param>
	XMLNode AppendChild(XMLNode parent, std::string const& name, NodeAddOption opt)
	{
		return ((opt & Overwrite) && parent.child(name.c_str())) ? 
			parent.child(name.c_str()) : 
			parent.append_child(name.c_str());
	}

	/// <summary>
	/// <para>Sets a node to be the child of a XML node.</para>
	/// </summary>
	/// <param name='parent'>XMLNode getting a child node</param>
	/// <param name='child'>The child node</param>
	XMLNode SetChildMove(XMLNode parent, XMLNode child)
	{
		return parent.append_move(child);
	}

	/// <summary>
	/// <para>Sets a node to be the child of a XML node.</para>
	/// </summary>
	/// <param name='parent'>XMLNode getting a child node</param>
	/// <param name='child'>The child node</param>
	XMLNode SetChildCopy(XMLNode parent, XMLNode child, NodeAddOption opt)
	{
		return ((opt & Overwrite) && parent.child(child.name())) ? 
			parent.child(child.name()) : 
			parent.append_copy(child);
	}

	/// <summary>
	/// <para>Removes a child node from an XML node.</para>
	/// </summary>
	/// <param name='parent'>XMLNode losing a child node</param>
	/// <param name='name'>The child name</param>
	bool RemoveChild(XMLNode parent, std::string const& name)
	{
		bool result = CheckIfNodeExists(parent, name);
		if (result) parent.remove_child(name.c_str());
		return result;
	}

	/// <summary>
	/// <para>Gets the parent node of an XML node.</para>
	/// </summary>
	/// <param name='child'>XMLNode returning its parent node</param>
	XMLNode GetParent(XMLNode child)
	{
		return child.parent();
	}

	/// <summary>
	/// <para>Gets the previous sibling node of an XML node.</para>
	/// </summary>
	/// <param name='child'>XMLNode returning its previous sibling node</param>
	XMLNode GetPrevSibiling(XMLNode sibling)
	{
		return sibling.previous_sibling();
	}

	/// <summary>
	/// <para>Gets the next sibling node of an XML node.</para>
	/// </summary>
	/// <param name='child'>XMLNode returning its next sibling node</param>
	XMLNode GetNextSibiling(XMLNode sibling)
	{
		return sibling.next_sibling();
	}

#pragma endregion

#pragma region Attribute

	/// <summary>
	/// <para>Appends an attribute to the XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the new attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='attrValue'>The attribute value</param>
	XMLAttribute AppendAttribute(XMLNode node, std::string const& attrName, std::string const& attrValue, NodeAddOption opt)
	{
		XMLAttribute attr = ((opt & Overwrite) && node.attribute(attrName.c_str())) ? 
			node.attribute(attrName.c_str()) : 
			node.append_attribute(attrName.c_str());
		
		attr.set_value(attrValue.c_str());
		return attr;
	}

	bool CheckIfAttributeExists(XMLNode node, std::string const& attrName)
	{
		return node.attribute(attrName.c_str());
	}

	bool CheckIfNodeExists(XMLNode node, std::string const& childName)
	{
		return node.child(childName.c_str());
	}

  template<>
  void GetValue(XMLNode node, float & value)
  {
    value = std::stof(node.value());
  }

  template<>
  void GetValue(XMLNode node, int & value)
  {
    value = std::stoi(node.value());
  }

  template<>
  void GetValue(XMLNode node, double & value)
  {
    value = std::stod(node.value());
  }

  template<>
  void GetValue(XMLNode node, std::string & value)
  {
    value = node.value();
  }

  template<>
  void GetValue(XMLNode node, bool & value)
  {
    value = std::stoi(node.value());
  }

  /// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The float reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, float& value)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if(result)	value = node.attribute(attrName.c_str()).as_float();
		return result;
	}

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The double reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, double& value)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result)	value = node.attribute(attrName.c_str()).as_double();
		return result;
	}

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The int reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, int& value)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result)	value = node.attribute(attrName.c_str()).as_int();
		return result;
	}

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The string reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, std::string& value)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result)	value = node.attribute(attrName.c_str()).as_string();
		return result;
	}

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The bool reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, bool& value)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result)	value = node.attribute(attrName.c_str()).as_bool();
		return result;
	}

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The unsigned reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, unsigned int& value)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result)	value = node.attribute(attrName.c_str()).as_uint();
		return result;
	}

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The unsigned reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, char& value)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result)	value = node.attribute(attrName.c_str()).as_int();
		return result;
	}

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The unsigned reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, unsigned char& value)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result)	value = node.attribute(attrName.c_str()).as_uint();
		return result;
	}

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The long long reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, long long& value)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result)	value = node.attribute(attrName.c_str()).as_llong();
		return result;
	}

	/// <summary>
	/// <para>Gets the value of an attribute to a XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode getting the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='value'>The unsigned long long reference to store the value</param>
	template <>
	bool GetAttributeValue(XMLNode node, std::string const& attrName, unsigned long long& value)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result)	value = node.attribute(attrName.c_str()).as_ullong();
		return result;
	}

	/// <summary>
	/// <para>Edits an attribute in the XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode editng the attribute</param>
	/// <param name='attrName'>The attribute name</param>
	/// <param name='attrValue'>The new attribute value</param>
	bool EditAttributeValue(XMLNode node, std::string const& attrName, std::string const& attrValue)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result) node.attribute(attrName.c_str()).set_value(attrValue.c_str());
		return result;
	}

	/// <summary>
	/// <para>Edits an attribute in the XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode editng the attribute</param>
	/// <param name='attrName'>The old attribute name</param>
	/// <param name='attrValue'>The new value</param>
	bool EditAttributeName(XMLNode node, std::string const& oldAttrName, std::string const& newAttrName)
	{
		bool result = CheckIfAttributeExists(node, newAttrName);
		if (result) node.attribute(oldAttrName.c_str()).set_name(newAttrName.c_str());
		return result;
	}

	/// <summary>
	/// <para>Removes an attribute from the XML node.</para>
	/// </summary>
	/// <param name='node'>XMLNode losing an attribute</param>
	/// <param name='attrName'>The attribute name</param>
	bool RemoveAttribute(XMLNode node, std::string const& attrName)
	{
		bool result = CheckIfAttributeExists(node, attrName);
		if (result) node.remove_attribute(attrName.c_str());
		return result;
	}

#pragma endregion
}
