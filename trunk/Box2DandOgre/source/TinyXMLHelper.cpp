/*=============================================================================

		TinyXMLHelper.cpp

		Author: Matt King

=============================================================================*/

#include "TinyXMLHelper.h"
#include "tinyxml.h"

using namespace Ogre;

void TinyXMLHelper::WalkDoc(TiXmlNode * pParent, std::map<Ogre::String, Ogre::String>* map, Ogre::String pName )
{
    if ( !pParent ) return;
    int t = pParent->Type();
    switch (t)
    {
    case TiXmlNode::ELEMENT:
		{
		
			TiXmlAttribute* attribute = ((TiXmlElement*)pParent)->FirstAttribute();

			while(attribute)
			{
				Ogre::String key = pName + attribute->NameTStr();
				Ogre::StringUtil::toLowerCase(key);
				Ogre::String val = attribute->ValueStr();
				map->insert(std::make_pair(key,val));

				attribute = attribute->Next();
			}

        break;
		}
    default:
        break;
    }

    TiXmlNode * pChild;

    for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
    {
		if(pName.compare("") != 0)
		{
			WalkDoc(pChild,map,pName + pChild->ValueTStr() + "/");
		}
		else
		{
			WalkDoc(pChild,map,pChild->ValueTStr() + "/");
		}
    }
}

//=============================================================================
//								GetMapFromFile
//
/// Returns a handle to the root node of a file or 0 if it can't find it.
bool TinyXMLHelper::GetMapFromFile(Ogre::String fileName, std::map<Ogre::String, Ogre::String>* map)
{
	TiXmlDocument* configXML = new TiXmlDocument(fileName.c_str());
	configXML->LoadFile();

	// If there was an error loading the file return 0.
	if(configXML->Error())
	{
		return false;
	}

	TiXmlHandle hDoc(configXML);

	TiXmlElement* rootElement = hDoc.FirstChildElement().Element();

	// If we couldn't get an element we return 0.
	if (rootElement == 0) 
	{
		return false;
	}

	WalkDoc(rootElement,map,"");

	delete configXML;

	return true;
}



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