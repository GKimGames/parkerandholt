/*=============================================================================

  LedgeSensor.h

  Author: Matt King

=============================================================================*/

#ifndef PAH_LEDGESENSOR_H
#define PAH_LEDGESENSOR_H

#include "GameObjectOgreBox2D.h"
#include "boost\signals.hpp"
#include "boost\bind.hpp"

class LedgeSensor : public GameObjectOgreBox2D
{
public:

	typedef boost::signal<void()> OnTouch;

	LedgeSensor(b2Vec2 position);
	~LedgeSensor(){};

	bool Update(double timeSinceLastFrame){ return true;};

	/// Called when two fixtures begin to touch.
	/// Sends out the signal OnTouch to its subscribers
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		if(collidedFixture->IsSensor() == false)
		{
			onTouch();
		}
	}

	void OnContact(const OnTouch::slot_type& slot)
	{
		onTouch.connect(slot);
	};

protected:

	b2Vec2 position_;

	OnTouch onTouch;

};


#endif