/*=============================================================================

		TinyXMLHelper.cpp

		Author: Matt King

=============================================================================*/

#include "TinyXMLHelper.h"
#include "tinyxml.h"

using namespace Ogre;


//=============================================================================
//								GetRootFromFile
//
/// Returns a handle to the root node of a file or 0 if it can't find it.
TiXmlHandle* TinyXMLHelper::GetRootFromFile(Ogre::String fileName, TiXmlDocument* configXML_)
{
	configXML_ = new TiXmlDocument(fileName.c_str());
	configXML_->LoadFile();
	
	// If there was an error loading the file return 0.
	if(configXML_->Error())
	{
		return 0;
	}

	TiXmlHandle hDoc(configXML_);

	TiXmlElement* rootElement = hDoc.FirstChildElement().Element();

	// If we couldn't get an element we return 0.
	if (rootElement == 0) 
	{
		return 0;
	}


	return new TiXmlHandle(rootElement);
}

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
//								GetMessage
//
/// Returns a String from "element" from attribute "attrib"
KGBMessageType TinyXMLHelper::GetMessage(TiXmlElement* element, const Ogre::String& name, KGBMessageType defaultValue)
{
	return StringToMessage(GetAttribute(element, name, MessageToString(defaultValue)));
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

//=============================================================================
//								GetAttributeb2Vec2
//
/// Returns a b2Veec2 from "element" from attribute "name"
b2Vec2 TinyXMLHelper::GetAttributeb2Vec2(TiXmlElement* element, const Ogre::String& name, b2Vec2 defaultValue)
{
	if(element->Attribute(name.c_str()))
	{
		Ogre::Vector2 vector = StringConverter::parseVector2(element->Attribute(name.c_str()));
		return b2Vec2(vector.x,vector.y);
	}

	return defaultValue;
	
}