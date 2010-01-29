/*=============================================================================

		TinyXMLHelper.h

		Author: Matt King
		
		This is a small wrapper to allow easy access to attributes in 
		TinyXML elements.
=============================================================================*/
#ifndef TINYXML_HELPER_H
#define TINYXML_HELPER_H


#include <Ogre.h>

class TiXmlElement;

class TinyXMLHelper
{
public:
	static Ogre::String GetAttribute(TiXmlElement* element, const Ogre::String& name, const Ogre::String& defaultValue = "");
	static Ogre::Real GetAttributeReal(TiXmlElement* element, const Ogre::String& name, Ogre::Real defaultValue = 0);
	static float GetAttributeFloat(TiXmlElement* element, const Ogre::String& name, float defaultValue = 0.0f);
	static bool GetAttributeBool(TiXmlElement* element, const Ogre::String& name, bool defaultValue = false);
	
	static Ogre::Vector2 GetAttributeVector2(TiXmlElement* element, const Ogre::String& name, Ogre::Vector2 defaultValue = Ogre::Vector2::ZERO);
	static Ogre::Vector3 GetAttributeVector3(TiXmlElement* element, const Ogre::String& name, Ogre::Vector3 defaultValue = Ogre::Vector3::ZERO);
};


#endif