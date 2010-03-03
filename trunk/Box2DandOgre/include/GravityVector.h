#ifndef GRAVITYVECTOR_H
#define GRAVITYVECTOR_H

#include "GameObjectOgreBox2D.h"

 
class GravityVector : public GameObjectOgreBox2D, public b2RayCastCallback
{
public:
	GravityVector(Ogre::SceneManager* sceneManager, b2Vec2 center, b2Vec2 direction);
	~GravityVector();
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	bool Update(double timeSinceLastFrame);
	bool Stop();
	bool Start(b2Vec2 newPosition, b2Vec2 newDirection);
	void SetPosition(b2Vec2 position);
	bool RemovePlayer();
	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);


protected:
	b2Vec2					position_;
	b2Fixture*				gravitySensor_;
	std::vector<b2Body*>	bodyList_;
	b2Vec2					forceApplied_;
	float					maxForce_;
	bool					active_;
	int						listNumber_;
	b2Vec2					forcePoint_;

};



#endif