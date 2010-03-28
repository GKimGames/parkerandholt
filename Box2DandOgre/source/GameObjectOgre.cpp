/*=============================================================================

	  GameObjectOgre.cpp

	  Author: Matt King

=============================================================================*/

#include "GameObjectOgre.h"
#include "GameFramework.h"
#include "MessageDispatcher.h"

//=============================================================================
//							GameObjectOgre::Constructor
//
GameObjectOgre::GameObjectOgre(Ogre::String name):
GameObject(name)
{
	sceneManager_ = GAMEFRAMEWORK->sceneManager;
	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	transparency_ = 0;
	particleSystem_ = 0;
}


//=============================================================================
//							GameObjectOgre::Destructor
//
GameObjectOgre::~GameObjectOgre()
{

	if(sceneNode_)
	{
		sceneManager_->destroySceneNode(sceneNode_);
	}
}

//=============================================================================
//							Initialize
//
bool GameObjectOgre::Initialize()
{
	initialized_ = GameObject::Initialize();
	return initialized_;
}

//=============================================================================
//							Initialize
//
/// Initialize the object with a mesh.
bool GameObjectOgre::Initialize(Ogre::String meshName)
{
	objectName_ = meshName;
	initialized_ = GameObject::Initialize();

	if(initialized_)
	{
		Ogre::String entityName = objectName_;
		entityName += "_Entity_";
		entityName += Ogre::StringConverter::toString(objectId_);

		entity_ = sceneManager_->createEntity(entityName, meshName);
		sceneNode_->attachObject(entity_);
		initialized_ = true;
	}

	/*transparency_ = new EntityMaterialInstance(entity_);*/
	return initialized_;
}

//=============================================================================
//							Update
//
/// Update calls UpdataGraphics
bool GameObjectOgre::Update(double timeSinceLastFrame)
{
	if(GameObject::Update(timeSinceLastFrame))
	{
		UpdateGraphics(timeSinceLastFrame);
			
		return true;
	}
	else
	{
		return false;
	}
}

//=============================================================================
//							UpdateGraphics
//
/// 
bool GameObjectOgre::UpdateGraphics(double timeSinceLastFrame)
{
	return true;
}


//=============================================================================
//							HandleMessage
//
/// 
bool GameObjectOgre::HandleMessage(const KGBMessage message)
{
	if(GameObject::HandleMessage(message))
	{
		return true;
	}
	else
	{
		if(message.messageType == GOO_SET_TRANSPARENCY)
		{
			if(message.messageData.empty() == false)
			{	
				Ogre::String typeName(message.messageData.type().name());
				SetTransparency(boost::any_cast<double>(message.messageData));
			}

			return true;
		}
		else if(message.messageType == SET_POSITION)
		{
			if(message.messageData.empty() == false)
			{	
				Ogre::String typeName(message.messageData.type().name());
				sceneNode_->setPosition(boost::any_cast<Ogre::Vector3>(message.messageData));
			}
			return false;
		}
	}

	return false;
}


void GameObjectOgre::SetTransparency(Ogre::Real alpha)
	{
		if(transparency_ == 0)
		{
			transparency_ = new EntityMaterialInstance(entity_);
		}
		transparency_->setTransparency(alpha);
	}

void GameObjectOgre::SetBlendType(Ogre::SceneBlendType type)
{
	if(transparency_ == 0)
	{
		transparency_ = new EntityMaterialInstance(entity_);
	}

	transparency_->setSceneBlending(type);
}