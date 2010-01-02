/*=============================================================================

	GameObject.h

	Author: Matt King

=============================================================================*/

#ifndef GameObject_H
#define GameObject_H

#include <Ogre.h>
#include <tinyxml.h>
#include <vector>

#include "GameFramework.h"
#include "Message.h"


class GameObject;

typedef unsigned int GameObjectId;
typedef std::map<GameObjectId, GameObject*> GameObjectMap;

/// This will identify what type of GameObject an Object is.
/// This is useful due to Box2D having all contact callbacks go through a single
/// world wide callback.
enum GameObjectType
{
	GOType_Character,						//< \brief Character Object
	GOType_Character_Parker,				//< \brief Character Parker Object
	GOType_Character_Parker_FEET_SENSOR,	//< \brief Character Parker Object
	GOType_Character_Holt,					//< \brief Character Holt Object
	CharacterFoot,
	GOType_HoltBox,
	GOType_LedgeSensor,
	GOType_Platform,
	GOType_ElevatorBeginTypes,
	GOType_MovingPlatform,
	GOType_ElevatorEndTypes
};

/// Objects represents the base class for anything that is in the game.
///	It has a unique ID number and a name.
/// All Objects are added to the objectList and are updated using this
/// list.
/// All Objects also have a GameObjectType.
class GameObject 
{
	
public:

	/// List of all Objetcts created. It uses their objectId.
	static GameObjectMap objectList;

	/// Update all the Objects.
	static void UpdateObjectList(double timeSinceLastFrame)
	{
		GameObjectMap::iterator it;

		for (it = objectList.begin(); it != objectList.end(); it++)
		{
			(*it).second->Update(timeSinceLastFrame);		
		}
	}

	/// Returns an Object from its objectId
	static GameObject* GetObjectById(GameObjectId objectId)
	{
		// Find the Object
		GameObjectMap::const_iterator object = objectList.find(objectId);
		
		if(object == objectList.end())
		{
			return 0;
		}
		
		return object->second;
	}

	static GameObjectMap GetobjectList() { return objectList; }


	/// Constructor adds this object to the objectList.
	/// It also increases the static objectId and assigns this object an Id.
	GameObject();

	/// The destructor removes it from the objectList;
	virtual ~GameObject()
	{
		objectList.erase(objectList.find(this->objectId_));
	}

	/// All Objects must update.
	virtual bool Update(double timeSinceLastFrame){return true;}

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

	/// Implement this to have this Object respond to messages
	/// By default this returns false to indicate this Object
	/// did not handle the message that was sent to it. Return
	/// true if the object was able to handle the message.
	virtual bool HandleMessage(const KGBMessage message){ return false; };

	/// Initialize the GameObjectOgreBox2D.
	virtual bool Initialize()
	{
		initialized_ = true;

		return true;
	}

	/// Return initialized_.
	bool IsInitialized()
	{
		return initialized_;
	}

	void SetName(Ogre::String s)
	{
		objectName_ = s;
	}

	Ogre::String GetName()
	{
		return objectName_;
	}

	static GameObjectId GetParkerId()
	{
		return parkerId_;
	}

	static GameObjectId GetHoltId()
	{
		return holtId_;
	}



protected:

	GameObjectType			gameObjectType_;	//< \brief GameObjectType of the object.

	GameObjectId			objectId_;			//< \brief Unique Id for the object.
	Ogre::String			objectName_;		//< \brief A name for the object.
	bool					initialized_;

	static GameObjectId		parkerId_;			//< \brief these are stored here to make easy access to parker and holt
	static GameObjectId		holtId_;

private:

	/// This is the static int that is increased every time an object is
	/// so that all objects can have a unique id.
	static GameObjectId		staticObjectId_;	


};


#endif
