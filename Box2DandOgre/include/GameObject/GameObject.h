/*=============================================================================

		GameObject.h

		Author: Matt King

=============================================================================*/

#ifndef GameObject_H
#define GameObject_H

#include <Ogre.h>
#include <tinyxml.h>
#include <vector>

#include "HelperFunctions.h"

class GameObject;
typedef unsigned int GameObjectId;
typedef std::map<GameObjectId, GameObject*> GameObjectMap;
typedef std::map<Ogre::String, GameObject*> GameObjectNameMap;
struct KGBMessage;
struct KGBMessage;
/// This will identify what type of GameObject an Object is.
/// This is useful due to Box2D having all contact callbacks go through a single
/// world wide callback.
enum GameObjectType
{
	GOTYPE_GameObject,
	GOType_Character,						//< \brief Character Object
	GOType_Character_Parker,				//< \brief Character Parker Object
	GOType_Character_Parker_FEET_SENSOR,	//< \brief Character Parker Object
	GOType_Character_Holt,					//< \brief Character Holt Object
	CharacterFoot,
	GOType_HoltBox,
	GOType_Sensor,
	GOType_LedgeSensor,
	GOType_Platform,
	GOType_ElevatorBeginTypes,
	GOType_MovingPlatform,
	GOType_ElevatorEndTypes,
	GOType_FadingPlatform,
	GOType_Mouse,
	GOType_CheckPoint,
	GOType_Door,
	GOType_PlaceablePlatform,
	GOType_Triangle
};

class GameObjectDef
{
public:
	GameObjectType			gameObjectType_;	//< \brief GameObjectType of the object.
	Ogre::String			objectName_;		//< \brief A name for the object.
};
/// GameObject represents the lowest form of object possible in the game.
/// An Object doesn't have to have a graphical representation  or a
/// physical representation. It is added to a vector of all Object's
/// when it is created. Objects also have a unique Id number. Objects
/// have a name that by default is just GameObject and then their Id
/// number, this name must be unique.
class GameObject 
{
	friend class GameObjectCreator;
	
public:
	
/*=============================================================================
				Object methods
=============================================================================*/

	// Constructor adds this object to the objectList.
	// It also increases the static objectId and assigns this object an Id.
	GameObject(Ogre::String name = "GameObject" );

	// Constructor adds this object to the objectList.
	// It also increases the static objectId and assigns this object an Id.
	GameObject(GameObjectDef* objectDef);

	// The destructor removes it from the objectList and objectNameList
	virtual ~GameObject();

	// All Objects must be able to update.
	virtual bool Update(double timeSinceLastFrame);

	// Implement this to have this Object respond to messages.
	// By default this returns false to indicate this Object
	// did not handle the message that was sent to it. Return
	// true if the object was able to handle the message.
	virtual bool HandleMessage(const KGBMessage message);

	// Initialize the GameObject.
	virtual bool Initialize();

/*=============================================================================
				Getter / Setter methods
=============================================================================*/
	
	/// Get the GameObjectType
	GameObjectType GetGameObjectType() const
	{
		return gameObjectType_;
	};

	/// Returns this object's objectId_
	GameObjectId GetId() const
	{ 
		return objectId_;
	};

	/// Return initialized_.
	bool IsInitialized()
	{
		return initialized_;
	}

	/// Set the Objects Name
	void SetName(Ogre::String s)
	{
		objectName_ = s;
	}

	Ogre::String GetName()
	{
		return objectName_;
	}

/*=============================================================================
				PUBLIC static members
=============================================================================*/
	/// List of all Objects created. It uses their objectId.
	static GameObjectMap* objectList;

	/// List of all Objects created using their names as a key.
	static GameObjectNameMap* objectNameList;

	/// Update all the Objects.
	static void UpdateObjectList(double timeSinceLastFrame)
	{
		GameObjectMap::iterator it;

		for(it = objectList->begin(); it != objectList->end();)
		{
			if(!(*it).second->Update(timeSinceLastFrame))
			{

				objectNameList->erase(objectNameList->find((*it).second->objectName_));
				delete (*it).second;
				objectList->erase(it++);
			}
			else
			{
				++it;
			}
		}
	}

	/// Returns an Object from its objectId
	static GameObject* GetObjectById(GameObjectId objectId)
	{
		// Find the Object
		GameObjectMap::const_iterator object = objectList->find(objectId);

		if(object == objectList->end())
		{
			return 0;
		}

		return object->second;
	}

	static GameObjectMap* GetobjectList()
	{ 
		return objectList;
	}

	/// Returns an Object from its objectName
	static GameObject* GetObjectById(Ogre::String objectName)
	{
		// Find the Object
		GameObjectNameMap::const_iterator object = objectNameList->find(objectName);

		if(object == objectNameList->end())
		{
			return 0;
		}

		return object->second;
	}

	/// This is a convenience method to get Parker's Id
	static GameObjectId GetParkerId()
	{
		return parkerId_;
	}

	/// This is a convenience method to get Holt's Id.
	static GameObjectId GetHoltId()
	{
		return holtId_;
	}

protected:

	GameObjectType			gameObjectType_;	//< \brief GameObjectType of the object.

	GameObjectId			objectId_;			//< \brief Unique Id for the object.
	Ogre::String			objectName_;		//< \brief A name for the object.
	bool					initialized_;

	bool					destroyMe_;	//< \brief Set this to false to have the object deleted

/*=============================================================================
				PROTECTED static members.
=============================================================================*/

	static GameObjectId		parkerId_;			//< \brief these are stored here to make easy access to parker and holt
	static GameObjectId		holtId_;

private:

/*=============================================================================
				PRIVATE static members.
=============================================================================*/
	/// This is the static int that is increased every time an object is
	/// so that all objects can have a unique id.
	static GameObjectId		staticObjectId_;	

};

#endif
