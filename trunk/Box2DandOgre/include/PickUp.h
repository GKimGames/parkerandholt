#ifndef PICKUP_H
#define PICKUP_H

#include "GameObjectOgreBox2D.h"

class PickUp : public GameObjectOgreBox2D
{
public:
	PickUp(Ogre::SceneManager* sceneManager, b2Vec2 center, std::string meshName);
	PickUp(Ogre::SceneManager* sceneManager, b2Vec2 center);
	~PickUp();
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	bool Update(double timeSinceLastFrame);

protected:
	b2Vec2					position_;
	b2Fixture*				itemSensor_;
	bool					pickedUp_;
	
};



#endif