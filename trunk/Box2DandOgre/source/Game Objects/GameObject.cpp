/*=============================================================================

  GameObject.cpp

  Author: Matt King

=============================================================================*/

#include "GameObject.h"
#include "Message.h"

GameObjectId GameObject::staticObjectId_ = 1;
GameObjectId GameObject::parkerId_ = -1;
GameObjectId GameObject::holtId_ = -1;

GameObjectMap* GameObject::objectList = new GameObjectMap();
GameObjectNameMap* GameObject::objectNameList = new GameObjectNameMap();

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
	objectList->insert(std::make_pair(objectId_,this));

	// This has to be removed - just for testing
	gameObjectType_ = GOTYPE_GameObject;

	initialized_ = false;
	destroyMe_ = false;
}

//=============================================================================
//							Destructor
//
/// 
GameObject::~GameObject()
{
	
}

//=============================================================================
//							Update
//
/// All Objects must be able to  update.
bool GameObject::Update(double timeSinceLastFrame)
{
	if(destroyMe_)
	{
		return false;
	}

	return true;
}

//=============================================================================
//							Initialize
//
/// Initialize the GameObject. This makes sure the object has a unique name
/// and adds it to the list of GameObject names. This is usually used to load
/// and create objects within the object after some variables are setup.
/// All children of the class must call this Initialize function before 
/// their own. This pattern should extend through all children.
bool GameObject::Initialize()
{
	if(!initialized_)
	{
		std::pair<std::map<Ogre::String, GameObject*>::iterator,bool> ret;

		ret = objectNameList->insert(std::make_pair(objectName_,this));
		
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
		
		objectNameList->insert(std::make_pair(objectName_,this));

		initialized_ = true;
	}

	return initialized_;
}

//=============================================================================
//							HandleMessage
//
/// Implement this to have this Object respond to messages.
/// By default this returns false to indicate this Objec did not handle the
/// message that was sent to it. Return true if the object was able to 
/// handle the message. Each class that extends this class should call this
/// HandleMessage before their own in case this class can handle the 
/// message.
/// If you want this class to do something with a message and its children
/// to something with it as well just return false for that message.
bool GameObject::HandleMessage(const KGBMessage message)
{ 

	if(message.messageType == GO_DESTROY)
	{
		destroyMe_ = true;
	}

	return false;
}