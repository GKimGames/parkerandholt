/*=============================================================================

  GameObject.cpp

  Author: Matt King

=============================================================================*/

#include "GameObject.h"

GameObjectId GameObject::staticObjectId_ = 1;
GameObjectId GameObject::parkerId_ = -1;
GameObjectId GameObject::holtId_ = -1;

GameObjectMap GameObject::objectList;
GameObjectNameMap GameObject::objectNameList;

//=============================================================================
//							GameObject::Constructor
//
/// GameObject represents the lowest form of object possible in the game.
/// An Object doesn't have to have a graphical representation  or a
/// physical representation. It is added to a vector of all Object's
/// when it is created. Objects also have a unique Id number. Objects
/// have a name that by default is just GameObject and then their Id
/// number, this name must be unique.
GameObject::GameObject(Ogre::String str)
{
	
	// Increase the staticObjectId value so each Object has its own
	// unique identifying number.
	staticObjectId_++;
	objectId_ = staticObjectId_;
	objectName_ = str;
	objectList.insert(std::make_pair(objectId_,this));

	initialized_ = false;
}

//=============================================================================
//							GameObject::Destructor
//
/// The destructor removes it from the objectList and objectNameList
GameObject::~GameObject()
{
	objectList.erase(objectList.find(objectId_));
}

//=============================================================================
//							Update
//
/// All Objects must be able to  update.
bool GameObject::Update(double timeSinceLastFrame)
{
	return true;
}

//=============================================================================
//							Initialize
//
/// Initialize the GameObject. This is usually used to load and create objects
/// within the object after some variables are setup.
bool GameObject::Initialize()
{
	std::pair<std::map<Ogre::String, GameObject*>::iterator,bool> ret;

	ret = objectNameList.insert(std::make_pair(objectName_,this));
	
	if(ret.second == false || objectName_.empty())
	{
		Ogre::String s = "ERROR: GameObject creation name conflict: ";
		s += objectName_;
		s += ". Now is: ";

		objectName_ += "_";
		objectName_ += Ogre::StringConverter::toString(objectId_);

		s += objectName_;

		DEBUG_LOG(s);
	}

	initialized_ = true;

	return initialized_;
}

//=============================================================================
//							HandleMessage
//
/// Implement this to have this Object respond to messages
/// By default this returns false to indicate this Object
/// did not handle the message that was sent to it. Return
/// true if the object was able to handle the message.
bool GameObject::HandleMessage(const KGBMessage message)
{ 
	return false;
}