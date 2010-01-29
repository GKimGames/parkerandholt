#include "GameObjectOgre.h"

//=============================================================================
//							GameObjectOgre::Constructor
//
GameObjectOgre::GameObjectOgre(Ogre::String name):
GameObject(name)
{
	sceneManager_ = GAMEFRAMEWORK->sceneManager;
	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
}


//=============================================================================
//							GameObjectOgre::Constructor
//
GameObjectOgre::~GameObjectOgre()
{
	sceneManager_->destroySceneNode(sceneNode_);
}

//=============================================================================
//							Initialize
//
bool GameObjectOgre::Initialize()
{
	initialized_ = true;
	return initialized_;
}

//=============================================================================
//							Initialize
//
bool GameObjectOgre::Initialize(Ogre::String meshName)
{
	
	Ogre::String entityName = objectName_;
	entityName += "_Entity_";
	entityName += Ogre::StringConverter::toString(objectId_);

	entity_ = sceneManager_->createEntity(entityName, meshName);
	sceneNode_->attachObject(entity_);
	initialized_ = true;

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