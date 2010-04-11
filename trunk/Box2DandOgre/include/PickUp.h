#ifndef PICKUP_H
#define PICKUP_H

#include "GameObjectOgreBox2D.h"

class PickUp : public GameObjectOgreBox2D
{
public:
	/// Creates a normal pickup
	PickUp(Ogre::SceneManager* sceneManager, b2Vec2 center);

	/// Creates a pickup that breaks at the entered force
	PickUp(Ogre::SceneManager* sceneManager, b2Vec2 center, float breakingForce);
	
	virtual ~PickUp();

	bool Initialize();

	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	void PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse);

	bool Update(double timeSinceLastFrame);
	

protected:
	b2Vec2			position_;
	b2Fixture*		itemSensor_;
	bool			pickedUp_;
	bool			breakable_;
	float			breakingForce_;
	
};



#endif