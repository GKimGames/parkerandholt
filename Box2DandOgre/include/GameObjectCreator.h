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
	CREATOR_NO_MESH,
	CREATOR_INITIALIZED_FAILED,
	CREATOR_NO_OGRE_OBJECT_ELEMENT,
	CREATOR_RESULT_COUNT
};

const static char* CreatorResultString[CREATOR_RESULT_COUNT] = 
{
	"Creator_OK",
	"Creator_Element_Is_Zero",
	"Creator_Object_Is_Zero",
	"Creator_No_Mesh",
	"Creator_Initialized_Failed",
	"Creator_No_Ogre_Object_Element"

};

class GameObjectCreator
{
	
public:

	GameObjectCreator(){}

	virtual ~GameObjectCreator(){}

	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		GameObject* object = new GameObject();
		CreatorResult result = LoadFromXML(element, object);

		if(result == CREATOR_OK)
		{
			return object;
		}

		// It didn't get created ok.

		delete object;
		
		return 0;		
	}

	virtual CreatorResult LoadFromXML(TiXmlElement* element, GameObject* gameObject)
	{
		CreatorResult result;
		if(element != 0 && gameObject != 0)
		{
			TiXmlElement* gameObjectElement = element->FirstChild( "GameObject" )->ToElement();
			if(gameObjectElement != 0)
			{
				std::string str = "GameObject_";
				str += Ogre::StringConverter::toString(gameObject->objectId_);
				// We make sure that getting the name attribute succeeds.
				if(element->QueryValueAttribute("name", &str) == TIXML_SUCCESS)
				{
					gameObject->objectName_ = str;
				}

				gameObject->Initialize();


				result =  CREATOR_OK;
			}
			else
			{
				result = CREATOR_ELEMENT_IS_ZERO;

				Ogre::String s = "GameObjectCreator: Failed to load - ";

				s += element->Value();
				s += ". Error ";
				s += CreatorResultString[result];
			}
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

			Ogre::String s = "GameObjectCreator: Failed to load. Error: ";;
			s += CreatorResultString[result];

			GAMEFRAMEWORK->log_->logMessage(s);
			DEBUG_LOG(s);

		}

		return result;

	}
};

#endif 