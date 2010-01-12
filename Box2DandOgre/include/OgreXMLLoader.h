/*=============================================================================

  OgreXMLLoader.h

  Author: Matt King

=============================================================================*/
#ifndef OGRE_XML_LOADER_H
#define OGRE_XML_LOADER_H

#include <map>

#include <tinyxml.h>
#include "Ogre.h"

// Ogre::Entity

enum OgreXMLLoaderResult
{
	OGREXML_OK,
	OGREXML_ELEMENT_IS_ZERO,
	OGREXML_NAME_IS_ZERO,
	OGREXML_ATTRIBUTE_NOT_FOUND,
	OGREXML_PARSED_VECTOR3_ZERO,
	OGREXML_RESULT_COUNT
};

const static char* OgreXMLLoaderResultString[OGREXML_RESULT_COUNT] = 
{
	"OgreXML_OK",
	"OgreXML_Element_Is_Zero",
	"OgreXML_Name_Is_Zero",
	"OgreXML_Attribute_Not_Found",
	"OgreXML_Parsed_Vector3_Zero"
};

class OgreXMLLoader
{

public:


	// These methods can be called anywhere
	static OgreXMLLoaderResult GetVector2(TiXmlElement* element, const char* name, Ogre::Vector2* vector);
	static OgreXMLLoaderResult GetVector3(TiXmlElement* element, const char* name, Ogre::Vector3* vector);

protected:


	
};

#endif

