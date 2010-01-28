#ifndef GRAVITYVECTOR_H
#define GRAVITYVECTOR_H

#include "GameObjectOgreBox2D.h"


class GravityVector : public GameObjectOgreBox2D
{
public:
	GravityVector(Ogre::SceneManager* sceneManager, b2Vec2 center, b2Vec2 direction);
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	bool Update(double timeSinceLastFrame);

protected:
	b2Vec2					position_;
	b2Fixture*				gravitySensor_;
	std::vector<b2Body*>	bodyList_;
	b2Vec2					forceApplied_;
	float					maxForce_;

};



#endif