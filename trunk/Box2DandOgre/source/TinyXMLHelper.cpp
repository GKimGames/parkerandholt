#include "TinyXMLHelper.h"
#include "tinyxml.h"

using namespace Ogre;


//=============================================================================
//								GetAttribute
//
/// Returns a String from "element" from attribute "attrib"
String TinyXMLHelper::GetAttribute(TiXmlElement* element, const String& attrib, const String& defaultValue)
{
	if(element->Attribute(attrib.c_str()))
	{
		return element->Attribute(attrib.c_str());
	}
	else
	{
		return defaultValue;
	}
}


//=============================================================================
//								GetAttributeReal
//
/// Returns a Real from "element" from attribute "attrib"
Real TinyXMLHelper::GetAttributeReal(TiXmlElement* element, const String& attrib, Real defaultValue)
{
	if(element->Attribute(attrib.c_str()))
	{
		return StringConverter::parseReal(element->Attribute(attrib.c_str()));
	}
	else
	{
		return defaultValue;
	}
}



//=============================================================================
//								GetAttributeFloat
//
/// Returns a float from "element" from attribute "attrib"
float TinyXMLHelper::GetAttributeFloat(TiXmlElement* element, const String& attrib, float defaultValue)
{
	element->QueryFloatAttribute(attrib.c_str(), &defaultValue);

	return defaultValue;
}


//=============================================================================
//								GetAttributeBool
//
/// Returns a bool from "element" from attribute "attrib"
bool TinyXMLHelper::GetAttributeBool(TiXmlElement* element, const String& attrib, bool defaultValue)
{

	element->QueryBoolAttribute(attrib.c_str(), &defaultValue);

	return defaultValue;
}


//=============================================================================
//								GetAttributeVector2
//
/// Returns an Ogre::Vector2 from "element" from attribute "attrib"
Vector2 TinyXMLHelper::GetAttributeVector2(TiXmlElement* element, const String& name, Vector2 defaultValue)
{
	if(element->Attribute(name.c_str()))
	{
		return StringConverter::parseVector2(element->Attribute(name.c_str()));
	}
	else
	{
		return defaultValue;
	}
}

//=============================================================================
//								GetAttributeVector3
//
/// Returns an Ogre::Vector3 from "element" from attribute "name"
Vector3 TinyXMLHelper::GetAttributeVector3(TiXmlElement* element, const String& name, Vector3 defaultValue)
{
	if(element->Attribute(name.c_str()))
	{
		return StringConverter::parseVector3(element->Attribute(name.c_str()));
	}
	else
	{
		return defaultValue;
	}
}