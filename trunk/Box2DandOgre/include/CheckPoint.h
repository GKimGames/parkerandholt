/*=============================================================================

		CheckPoint.h

		CheckPoint objects that character can return to on death

=============================================================================*/
#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include "GameObjectOgreBox2D.h"

class CheckPoint : public GameObjectOgreBox2D
{
public:

	/// CheckPoints have a defualt size of 2X4
	CheckPoint(Ogre::SceneManager* sceneManager, b2Vec2 center, float width = 2.0f, float height = 4.0f);
	~CheckPoint();

	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	
	bool Update(double timeSinceLastFrame);

protected:

	b2Vec2			position_;
	b2Fixture*		checkPointSensor_;
	float			width_;
	float			height_;
};



#endif