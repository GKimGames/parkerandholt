/*=============================================================================

	  GameObjectOgre.cpp

	  Author: Matt King

=============================================================================*/

#include "GameObjectOgre.h"

//=============================================================================
//							GameObjectOgre::Constructor
//
GameObjectOgre::GameObjectOgre(Ogre::String name):
GameObject(name)
{
	sceneManager_ = GAMEFRAMEWORK->sceneManager;
	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	transparency_ = 0;
}


//=============================================================================
//							GameObjectOgre::Destructor
//
GameObjectOgre::~GameObjectOgre()
{
	// go through all attached entities
	/*
	Ogre::SceneNode::ObjectIterator Iter = sceneNode_->getAttachedObjectIterator();
	while(Iter.hasMoreElements())
	{
		Ogre::MovableObject* mobj = Iter.getNext();
	}
	*/
	sceneManager_->destroySceneNode(sceneNode_);
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

	Ogre::String entityName = objectName_;
	entityName += "_Entity_";
	entityName += Ogre::StringConverter::toString(objectId_);

	entity_ = sceneManager_->createEntity(entityName, meshName);
	sceneNode_->attachObject(entity_);
	initialized_ = true;

	/*transparency_ = new EntityMaterialInstance(entity_);*/
	return initialized_;
}

//=============================================================================
//							Update
//
/// Update calls UpdataGraphics
bool GameObjectOgre::Update(double timeSinceLastFrame)
{
	UpdateGraphics(timeSinceLastFrame);
	return true;
}

//=============================================================================
//							UpdateGraphics
//
/// 
bool GameObjectOgre::UpdateGraphics(double timeSinceLastFrame)
{
	return true;
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