/*=============================================================================

		GravityVector.h

=============================================================================*/
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
	
	/// Stops the vector from pushing on objects and sets it to be invisible.
	bool Stop();
	/// Starts the vector pushing on objects and makes it visible.
	bool Start(b2Vec2 newPosition, b2Vec2 newDirection);

	void SetPosition(b2Vec2 position);

	/// Forces the removal of the indicated player from the list of objects the vector is pushing on.
	bool RemovePlayer(GameObjectId characterId);

	// Required callback used in Box2d based raycasts
	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);

	/// Sets partical effects on or off.
	void SetParticalsVisible(bool active);


protected:
	b2Vec2					position_;
	b2Fixture*				gravitySensor_;

	/// List of bodies the vector is pushing on
	std::vector<b2Body*>	bodyList_;

	/// Amount of force applied with direction
	b2Vec2					forceApplied_;
	float					maxForce_;
	bool					active_;
	
	// Used to store which body is hit in raycasts
	int						listNumber_;
	/// Stores the point to be pushed on as a result of the raycast
	b2Vec2					forcePoint_;

};



#endif