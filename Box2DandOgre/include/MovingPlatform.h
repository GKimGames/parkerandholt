/*=============================================================================

  MovingPlatform.h

  Author: Matt King

=============================================================================*/
#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H

#include <OgreMeshManager.h>
#include <OgreMovablePlane.h>

#include "GameFramework.h"

#include "GameObjectOgreBox2D.h"
#include "Parker.h"

class MovingPlatform : public GameObjectOgreBox2D
{

public:

	MovingPlatform(Ogre::SceneManager* sceneManager,b2Vec2 p1, b2Vec2 p2, b2Vec2 start, b2Vec2 end, double speed);
	MovingPlatform(Ogre::SceneManager* sceneManager, Ogre::Entity entity, b2Vec2 start, b2Vec2 end, double speed);
	
	~MovingPlatform();

	bool Update(double timeSinceLastFrame);

	/// Called when two fixtures begin to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		if(collidedFixture->GetUserData() != 0)
		{
			GameObject* go = (GameObject*) collidedFixture->GetBody()->GetUserData();
			if(go->GetGameObjectType() == GOType_Character_Parker)
			{
				parkerCount_++;
			}
		}
		
	}


	/// Called when two fixtures cease to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		if(collidedFixture->GetUserData() != 0)
		{
			GameObject* go = (GameObject*) collidedFixture->GetBody()->GetUserData();
			if(go->GetGameObjectType() == GOType_Character_Parker)
			{
				parkerCount_--;
			}
		}
	}

	void Disable() { disabled_ = true; }
	void Enable() { disabled_ = false; }
	const bool IsDisabled() { return disabled_; }

	virtual void CreatePhysics();

protected:

	KGBMessageType startReachedMessage_;
	KGBMessageType endReachedMessage_;

	bool disabled_;
	double speed_;

	b2Body* kBody_;
	Ogre::MovablePlane* plane;

	b2Vec2 startVec_;
	b2Vec2 endVec_;

	b2Vec2 point1;
	b2Vec2 point2;

	CharacterParker* parker_;
	int parkerCount_;

	b2Vec2 direction_;

	
};

#endif