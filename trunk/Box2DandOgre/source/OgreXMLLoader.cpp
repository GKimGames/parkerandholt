#include "OgreXMLLoader.h"
#include "HelperFunctions.h"

//=============================================================================
//								GetVector3
//
OgreXMLLoaderResult OgreXMLLoader::GetVector3(TiXmlElement* element, const char* name, Ogre::Vector3* vector)
{

	OgreXMLLoaderResult result = OGREXML_OK;

	if(element == 0)
	{
		result = OGREXML_ELEMENT_IS_ZERO;
		Ogre::String s = "OgreXMLLoader::GetVector3: Failed to load Element: ";
		s += OgreXMLLoaderResultString[result];

		DEBUG_LOG(s);
	}

	if(name == 0)
	{
		result = OGREXML_NAME_IS_ZERO;
		Ogre::String s = "OgreXMLLoader::GetVector3: Failed to load Name: ";
		s += OgreXMLLoaderResultString[result];

		DEBUG_LOG(s);
	}

	if(result == OGREXML_OK)
	{

		const char* str1 = element->Attribute(name);
		
		if(str1 != 0)
		{
			Ogre::String str = Ogre::String(str1);
			Ogre::Vector3 v = Ogre::StringConverter::parseVector3(str);
			vector->x = v.x;
			vector->y = v.y;
			vector->z = v.z;
			
			if(v == Ogre::Vector3::ZERO)
			{
				result = OGREXML_PARSED_VECTOR3_ZERO;
				Ogre::String s = "OgreXMLLoader::GetVector3: Parsing Error - ";
				s += name;
				s += ".";
				s += OgreXMLLoaderResultString[result];

				DEBUG_LOG(s);
			}
		}	
		else
		{
			result = OGREXML_ATTRIBUTE_NOT_FOUND;

			Ogre::String s = "OgreXMLLoader::GetVector3: Attribute not found - Element: ";
			s += element->ValueStr();
			s += " - ";
			s += name;
			s += ". Error: ";
			s += OgreXMLLoaderResultString[result];

			
		}
	}

	return result;
}


//=============================================================================
//								GetVector2
//
OgreXMLLoaderResult OgreXMLLoader::GetVector2(TiXmlElement* element, const char* name, Ogre::Vector2* vector)
{
	OgreXMLLoaderResult result = OGREXML_OK;

	if(element == 0)
	{
		result = OGREXML_ELEMENT_IS_ZERO;
		Ogre::String s = "OgreXMLLoader::GetVector3: Failed to load Element: ";
		s += OgreXMLLoaderResultString[result];

		DEBUG_LOG(s);
	}

	if(name == 0)
	{
		result = OGREXML_NAME_IS_ZERO;
		Ogre::String s = "OgreXMLLoader::GetVector2: Failed to load Name: ";
		s += OgreXMLLoaderResultString[result];

		DEBUG_LOG(s);
	}

	if(result = OGREXML_OK)
	{

		const char* str1 = element->Attribute(name);
		
		if(str1 != 0)
		{
			Ogre::String str = Ogre::String(str1);
			Ogre::Vector2 v = Ogre::StringConverter::parseVector2(str);
			vector->x = v.x;
			vector->y = v.y;
			
			if(v == Ogre::Vector2::ZERO)
			{
				result = OGREXML_PARSED_VECTOR3_ZERO;
				Ogre::String s = "OgreXMLLoader::GetVector2: Parsing Error - ";
				s += name;
				s += ".";
				s += OgreXMLLoaderResultString[result];

				DEBUG_LOG(s);
			}
		}	
		else
		{
			result = OGREXML_ATTRIBUTE_NOT_FOUND;

			Ogre::String s = "OgreXMLLoader::GetVector2: Attribute not found - Element: ";
			s += element->ValueStr();
			s += " - ";
			s += name;
			s += ". Error: ";
			s += OgreXMLLoaderResultString[result];

			
		}
	}

	return result;
}