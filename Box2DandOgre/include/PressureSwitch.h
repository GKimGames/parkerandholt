/*=============================================================================

  Platform.h

  Author: Matt King

=============================================================================*/
#ifndef PRESSURE_SWITCH_H
#define PRESSURE_SWITCH_H

#include <OgreMeshManager.h>
#include <OgreMovablePlane.h>

#include "GameFramework.h"

#include "GameObjectSensor.h"

class PressureSwitch : public GameObjectSensor
{

public:

	PressureSwitch(Ogre::SceneManager* sceneManager,b2Vec2 position,
				   b2Body* body = 0, Ogre::Entity* entity = 0);

	/// Called when two fixtures begin to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);


	/// Called when two fixtures cease to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	~PressureSwitch(){};

	bool Update(double timeSinceLastFrame);


protected:

	double		minimumTime_;
	double		timeAccum_;
	double		endTime_;

	bool		hitBegun_;
	bool		hitEnd_;

	b2Body*		staticBody_;
	b2Fixture*	movingFixture_;
	b2Fixture*	hitFixture_;

	
	
};

#endif