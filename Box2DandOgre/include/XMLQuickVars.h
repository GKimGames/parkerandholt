/*=============================================================================

	XMLQuickVars.h

	Author: Matt King
	
=============================================================================*/

#ifndef XML_QUICK_VARS_H
#define XML_QUICK_VARS_H

#include <map>
#include "TinyXMLHelper.h"

class XMLQuickVars
{
public:

	XMLQuickVars(Ogre::String fileName)
	{
		TinyXMLHelper::GetMapFromFile(fileName, &map);
	}

	float Float(Ogre::String var)
	{
		Ogre::String value = Get(var);
		return (float) Ogre::StringConverter::parseReal(value);
	}

	double Double(Ogre::String var)
	{
		Ogre::String value = Get(var);
		return (double) Ogre::StringConverter::parseReal(value);
	}

	int Int(Ogre::String var)
	{
		Ogre::String value = Get(var);
		return (double) Ogre::StringConverter::parseInt(value);
	}

	bool Bool(Ogre::String var)
	{
		Ogre::String value = Get(var);
		return Ogre::StringConverter::parseBool(value);
	}

	b2Vec2 B2Vec2(Ogre::String var)
	{
		Ogre::String value = Get(var);
		Ogre::Vector2 v(Ogre::StringConverter::parseVector2(value));
		return b2Vec2(v.x,v.y); 
	}

	Ogre::Vector2 Vector2(Ogre::String var)
	{
		Ogre::String value = Get(var);
		return Ogre::StringConverter::parseVector2(value);
	}

	Ogre::Vector3 Vector3(Ogre::String var)
	{
		Ogre::String value = Get(var);
		return Ogre::StringConverter::parseVector3(value);
	}

	Ogre::String String(Ogre::String var)
	{
		return Get(var);
	}

	//bool Var(Ogre::String var, Ogre::String* value)

protected:

	Ogre::String Get(Ogre::String var)
	{
		Ogre::String s("");
		Ogre::StringUtil::toLowerCase(var);
		it = map.find(var);
		
		if(it != map.end())
		{
			s = it->second;
		}

		return s;
	}

	std::map<Ogre::String, Ogre::String>::iterator it;
	std::map<Ogre::String, Ogre::String> map;
};

#endif