/*=============================================================================

		Door.h

		Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_DOOR_H
#define GAME_OBJECT_SENSOR_H

#include "GameObjectOgreBox2D.h"
#include "AppStateManager.hpp"
#include "AnimationBlender.h"


class Door : public GameObjectOgreBox2D
{

	friend class DoorCreator;

public:
	
	Door(Ogre::String name, bool isEntrance, b2Vec2 position):
	GameObjectOgreBox2D(name, 0, 0)
	{
		stateName_ = "PhysicsState";

		GameObject::Initialize();
		gameObjectType_ = GOType_Door;

		if(isEntrance)
		{
			entity_ = GAMEFRAMEWORK->sceneManager->createEntity(objectName_ + "_Door" , "start.mesh");
		}
		else
		{
			entity_ = GAMEFRAMEWORK->sceneManager->createEntity(objectName_ + "_Door" , "exit.mesh");
		}

		Ogre::AxisAlignedBox box = entity_->getBoundingBox();
		Ogre::Vector3 size = box.getSize();

		b2BodyDef bdef;
		bdef.position.Set(position.x, position.y + size.y / 2.0);

		b2PolygonShape boxShape;
		boxShape.SetAsBox(size.z / 2.0, size.y / 2.0);

		b2FixtureDef fd;
		fd.shape = &boxShape;
		fd.isSensor = true;

		body_ = world_->CreateBody(&bdef);
		body_->SetUserData(this);
		body_->CreateFixture(&fd);

		sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
		sceneNode_->attachObject(entity_);
		sceneNode_->yaw(Ogre::Degree(-90));
		sceneNode_->setPosition(position.x, position.y,-1);

		animationBlender_ = new AnimationBlender(entity_);

		animationBlender_->Initialize("open", false);

		isEntrance_ = isEntrance; 
		isOpening_ = false;
	}

	virtual ~Door(){}

	bool Update(double timeSinceLastFrame)
	{
		if(isOpening_ && animationBlender_->complete_ == false)
		{
			animationBlender_->AddTime(timeSinceLastFrame);
		}
		else if(isOpening_ && animationBlender_->complete_)
		{
			if(isEntrance_)
			{
				GAMEFRAMEWORK->appStateManager->pushAppState(stateName_);
			}
			else
			{
				GAMEFRAMEWORK->appStateManager->popStateAfterNextUpdate();
			}

			isOpening_ = false;
			animationBlender_->Initialize("open", false);
		}

		return true;
	}

	void OpenDoor()
	{
		animationBlender_->Blend("open", AnimationBlender::BlendSwitch, 0,false);
		isOpening_ = true;
	}

	//bool Initialize();

private:

	bool isOpening_;
	bool isEntrance_;

	Ogre::String stateName_;
};

#endif