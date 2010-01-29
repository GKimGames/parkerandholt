/*=============================================================================

		  LedgeSensor.h

		  Author: Matt King

=============================================================================*/

#ifndef LEDGESENSOR_H
#define LEDGESENSOR_H

#include "GameObjectSensor.h"

class LedgeSensor : public GameObjectSensor
{
public:

	LedgeSensor();
	~LedgeSensor(){};

	bool Update(double timeSinceLastFrame){ return true;};

	/// Called when two fixtures begin to touch.
	/// Sends out the signal OnTouch to its subscribers
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

protected:

};


#endif