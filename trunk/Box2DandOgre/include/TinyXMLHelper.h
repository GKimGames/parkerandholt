/*=============================================================================

		TinyXMLHelper.h

		Author: Matt King
		
		This is a small wrapper to allow easy access to attributes in 
		TinyXML elements.
=============================================================================*/
#ifndef TINYXML_HELPER_H
#define TINYXML_HELPER_H


#include <Ogre.h>
#include <Box2D/Box2D.h>

#include "Message.h"

class TiXmlElement;
class TiXmlHandle;
class TiXmlDocument;

class TinyXMLHelper
{
public:
	static TiXmlHandle* GetRootFromFile(Ogre::String fileName, TiXmlDocument* configXML_);
	static Ogre::String GetAttribute(TiXmlElement* element, const Ogre::String& name, const Ogre::String& defaultValue = "");
	static KGBMessageType GetMessage(TiXmlElement* element, const Ogre::String& name, KGBMessageType defaultValue = NO_MESSAGE);
	static Ogre::Real GetAttributeReal(TiXmlElement* element, const Ogre::String& name, Ogre::Real defaultValue = 0);
	static float GetAttributeFloat(TiXmlElement* element, const Ogre::String& name, float defaultValue = 0.0f);
	static bool GetAttributeBool(TiXmlElement* element, const Ogre::String& name, bool defaultValue = false);
	
	static Ogre::Vector2 GetAttributeVector2(TiXmlElement* element, const Ogre::String& name, Ogre::Vector2 defaultValue = Ogre::Vector2::ZERO);
	static Ogre::Vector3 GetAttributeVector3(TiXmlElement* element, const Ogre::String& name, Ogre::Vector3 defaultValue = Ogre::Vector3::ZERO);
	
	static b2Vec2 GetAttributeb2Vec2(TiXmlElement* element, const Ogre::String& name, b2Vec2 defaultValue = b2Vec2(0,0));
};


#endif