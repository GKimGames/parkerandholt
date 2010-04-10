/*=============================================================================

	GameObjectCreator.h

	Author: Matt King

=============================================================================*/

#ifndef GAMEOBJECT_CREATOR_H
#define GAMEOBJECT_CREATOR_H

#include "HelperFunctions.h"
#include "GameFramework.h"

#include "GameObjectFactory.h"
#include "TinyXMLHelper.h"
#include "Box2DXMLLoader.h"

#include "GameObject.h"

/// Enum for results when creating an object.
enum CreatorResult
{
	CREATOR_OK,
	CREATOR_ELEMENT_IS_ZERO,
	CREATOR_OBJECT_IS_ZERO,
	CREATOR_NO_MESH,
	CREATOR_INITIALIZED_FAILED,CREATOR_NO_OGRE_OBJECT_ELEMENT,
	CREATOR_RESULT_COUNT
};

/// Static char array for easy conversion of enum to string.
const static char* CreatorResultString[CREATOR_RESULT_COUNT] = 
{
	"Creator_OK",
	"Creator_Element_Is_Zero",
	"Creator_Object_Is_Zero",
	"Creator_No_Mesh",
	"Creator_Initialized_Failed",
	"Creator_No_Ogre_Object_Element"

};

/// The base class for all GameObject Creators. A Creator loads the settings
/// and returns an instance of a GameObject. Only one GameObjectCreator is
/// ever needed to be created per GameObjectFactory.
///
/// Each GameObjectCreator has two Methods, LoadFromXML(TiXmlElement* element)
/// and LoadFromXML(TiXmlElement* element, GameObject* gameObject). The second
/// method allows for easy reuse of code, so any class that extends this
/// can call the second method on the GameObject that extended class is
/// creating.
class GameObjectCreator
{
	
public:

	/// GameObjectCreator is passed an instance of the GameObjectFactory so it can
	/// use the speecific Ogre and Box2D variables associated with that
	/// GameObjectFactory.
	GameObjectCreator(GameObjectFactory* gameObjectFactory) : gameObjectFactory_(gameObjectFactory){}

	virtual ~GameObjectCreator(){}

	/// This function creates the GameObject and then tries to load an XML
	/// element passed to the function. The XML Element contains the 
	/// config for the object and all necessary variables to setup the
	/// instance of the class.
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

	/// Loads a set of variables and creates object necessary on a GameObject
	/// passed to it from the XML element.
	virtual CreatorResult LoadFromXML(TiXmlElement* element, GameObject* gameObject)
	{
		CreatorResult result;
		if(element != 0 && gameObject != 0)
		{
			TiXmlElement* gameObjectElement = element->FirstChild( "Object" )->ToElement();
			if(gameObjectElement != 0)
			{
				Ogre::String str = "GameObject_" + Ogre::StringConverter::toString(gameObject->objectId_);
				
				// Get the object name.
				gameObject->objectName_ = TinyXMLHelper::GetAttribute(gameObjectElement, "name", str);

				// Initialize the object.
				gameObject->Initialize();

				result = CREATOR_OK;
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

protected:

	GameObjectFactory* gameObjectFactory_;

};

#endif 