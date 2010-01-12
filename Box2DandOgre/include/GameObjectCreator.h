/*=============================================================================

	GameObjectCreator.h

	Author: Matt King

=============================================================================*/

#ifndef GAMEOBJECT_CREATOR_H
#define GAMEOBJECT_CREATOR_H

#include "GameObject.h"
#include "Box2DXMLLoader.h"
#include "HelperFunctions.h"
#include "GameFramework.h"

enum CreatorResult
{
	CREATOR_OK,
	CREATOR_ELEMENT_IS_ZERO,
	CREATOR_OBJECT_IS_ZERO,
	CREATOR_RESULT_COUNT
};

const static char* CreatorResultString[CREATOR_RESULT_COUNT] = 
{
	"Creator_OK",
	"Creator_Element_Is_Zero",
	"Creator_Object_Is_Zero"
};

class GameObjectCreator
{
	
public:

	GameObjectCreator(){}

	virtual ~GameObjectCreator(){}

	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		GameObject* object = new GameObject();
		if(element != 0)
		{
			std::string str;
			element->QueryValueAttribute("name", &str);
			object->objectName_ = str;
		}
		else
		{
			Ogre::String s = "GameObjectCreator: Failed to load - Element is zero";
			GAMEFRAMEWORK->log_->logMessage(s);
			DEBUG_LOG(s);

		}


		return object;
	}

	virtual CreatorResult LoadFromXML(TiXmlElement* element, GameObject* gameObject)
	{
		CreatorResult result;
		if(element != 0 && gameObject != 0)
		{
			std::string str;
			element->QueryValueAttribute("name", &str);
			gameObject->objectName_ = str;

			result =  CREATOR_OK;
		}
		else
		{
			if(element == 0)
			{
				result = CREATOR_ELEMENT_IS_ZERO;

			}
			else if(gameObject == 0)
			{
				result = CREATOR_OBJECT_IS_ZERO;
			}

			Ogre::String s = "GameObjectCreator: Failed to load - ";

			s += element->Value();
			s += ". Error ";
			s += CreatorResultString[result];

			GAMEFRAMEWORK->log_->logMessage(s);
			DEBUG_LOG(s);

		}

		return result;

	}
};

#endif 