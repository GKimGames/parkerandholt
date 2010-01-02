/*=============================================================================

  GameObjectOgreBox2D.h

  Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_OGRE_BOX2D_H
#define GAME_OBJECT_OGRE_BOX2D_H

#include <Box2D/Box2D.h>

#include "GameObjectOgre.h"


/// Structure to represent a contact point.
struct ContactPoint
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
	b2Contact* contact;
};

/// The default value of friction for all Box2D objects in the game.
const static double DEFAULT_FRICTION = 0.4;
const static signed short STATIC_MAP_GROUP = -10;

/// This extends GameObjectOgre and adds a Box2D component.
class GameObjectOgreBox2D  : public GameObjectOgre
{
public:

	/// Constructor grabs the reference from the OgreFramework of the b2World.
	/// The object is not initialized until Initialize is called and will only
	/// be initialized if the body doesn't equal 0.
	GameObjectOgreBox2D(b2Body* body = 0, Ogre::Entity* entity = 0)
	{
		// There is only ever one world at a time and all objects get the
		// current world from the GameFramework singleton.
		world_ = GameFramework::getSingletonPtr()->GetWorld();

		entity_ = entity;
		body_ = body;
	}


	virtual ~GameObjectOgreBox2D()
	{
		if(body_ != 0)
		{
			world_->DestroyBody(body_);
		}
	}

	virtual bool Initialize()
	{
		if(body_ != 0)
		{
			initialized_ = true;
		}

		return initialized_;
	}

	/// Called when two fixtures begin to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){}


	/// Called when two fixtures cease to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){}

	/// Returns the b2Body for this GameObjectOgreBox2D.
	virtual b2Body* GetBody()
	{
		return body_;
	}

	/// Returns the body's position.
	virtual b2Vec2 GetBodyPosition()
	{
		return body_->GetPosition();
	}

	void SetBodyPosition(b2Vec2 pos)
	{
		body_->SetTransform(pos, body_->GetAngle());
	}

	void SetBodyAngle(float32 angle)
	{
		body_->SetTransform(body_->GetPosition(), angle);
	}


	virtual bool Update(double timeSinceLastFrame)
	{
		
		UpdateGraphics(timeSinceLastFrame);

		return true;
	}


	/// Sets the scene node to be positioned the same as as the Box2D body
	/// this object has.
	virtual bool UpdateGraphics(double timeSinceLastFrame)
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
	};

	virtual void CreatePhysics(){}

protected:

	/// Box2D body for this Object
	b2Body* body_;

	/// Reference to the b2World
	b2World* world_;

	float prevAngle;


};

#endif
