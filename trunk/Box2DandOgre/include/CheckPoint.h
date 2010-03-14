#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include "GameObjectOgreBox2D.h"

class CheckPoint : public GameObjectOgreBox2D
{
public:

	CheckPoint(Ogre::SceneManager* sceneManager, b2Vec2 center, float width, float height);
	~CheckPoint();
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	bool Update(double timeSinceLastFrame);

protected:

	b2Vec2			position_;
	b2Fixture*		checkPointSensor_;
	float			width_;
	float			height_;
	bool			triggered_;
};



#endif