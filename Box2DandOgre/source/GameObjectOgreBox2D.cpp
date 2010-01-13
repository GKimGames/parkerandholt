/*=============================================================================

GameObjectOgreBox2D.cpp

Author: Matt King

=============================================================================*/

#include "GameObjectOgreBox2D.h"




/// Constructor grabs the reference from the OgreFramework of the b2World.
/// The object is not initialized until Initialize is called and will only
/// be initialized if the body doesn't equal 0.
GameObjectOgreBox2D::GameObjectOgreBox2D(Ogre::String name, b2Body* body, Ogre::Entity* entity)
: GameObjectOgre(name)
{
	// There is only ever one world at a time and all objects get the
	// current world from the GameFramework singleton.
	world_ = GAMEFRAMEWORK->GetWorld();
	sceneManager_ = GAMEFRAMEWORK->sceneManager;
	entity_ = entity;
	body_ = body;
	
	debugSceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	
	debugDrawInitialized_ = false;
	debugDrawOn_ = false;
	previousAngle_ = 0;
	wasAwake_ = false;
}


GameObjectOgreBox2D::~GameObjectOgreBox2D()
{
	if(body_ != 0)
	{
		world_->DestroyBody(body_);
	}
}

bool GameObjectOgreBox2D::Initialize()
{
	if(body_ != 0)
	{
		initialized_ = true;
	}
	return initialized_;
}

bool GameObjectOgreBox2D::InitializeDebugDraw(Ogre::ColourValue color, Ogre::String materialName)
{
	
	if(body_ != 0)
	{
		
		debugMaterialName_ = materialName;
		debugColor_ = color;

		debugDrawInitialized_ = true;
		debugDrawOn_ = true;

		Ogre::String manName = "debug_";
		manName += objectName_;

		manualObject_ = sceneManager_->createManualObject(manName);
		RedrawDebug();

		debugSceneNode_->attachObject(manualObject_);
	}

	return debugDrawInitialized_;
}

bool GameObjectOgreBox2D::Update(double timeSinceLastFrame)
{
	UpdateGraphics(timeSinceLastFrame);
	
	if(debugDrawOn_)
	{
		UpdateDebugDraw();
	}

	return true;
}

void GameObjectOgreBox2D::UpdateDebugDraw()
{
	
	// Since rotating a scenenode actually rotates it by an amount
	// and not to a specific value we must store the previous angle
	// that it was rotated by and rotate back by that and then rotate
	// to our new angle.
	if(debugDrawInitialized_)
	{

		if(body_->IsAwake() == true && wasAwake_ == false)
		{
			for(int i = 0; i < manualObject_->getNumSections(); ++i)
			{
				manualObject_->setMaterialName(i,debugMaterialName_);
			}
		}
		else if(body_->IsAwake() == false && wasAwake_ == true)
		{
			for(int i = 0; i < manualObject_->getNumSections(); ++i)
			{
				manualObject_->setMaterialName(i,"debugDraw/Sleeping");
			}
		}
		
		b2Vec2 v = body_->GetPosition();		

		debugSceneNode_->setPosition(Ogre::Real(v.x),Ogre::Real(v.y),0);
		debugSceneNode_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(-previousAngle_)); 
		debugSceneNode_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(body_->GetAngle()));

		previousAngle_ = body_->GetAngle();
		wasAwake_ = body_->IsAwake();
	}
	
}
/// Sets the scene node to be positioned the same as as the Box2D body
/// this object has.
bool GameObjectOgreBox2D::UpdateGraphics(double timeSinceLastFrame)
{
	// Since rotating a scenenode actually rotates it by an amount
	// and not to a specific value we must store the previous angle
	// that it was rotated by and rotate back by that and then rotate
	// to our new angle.

	
	if(initialized_)
	{
		b2Vec2 v = body_->GetPosition();
		sceneNode_->setPosition(Ogre::Real(v.x),Ogre::Real(v.y),0);
		sceneNode_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(-prevAngle)); 
		sceneNode_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(body_->GetAngle()));

		prevAngle = body_->GetAngle();

	}

	return true;
}

void GameObjectOgreBox2D::RedrawDebug()
{
	if(body_ != 0)
	{
		manualObject_->clear();
		DrawBody(manualObject_, body_, debugMaterialName_, debugColor_);
	}
}