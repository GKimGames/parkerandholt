/*=============================================================================

Switch.h

Author: Matt King

=============================================================================*/
#ifndef SWITCH_H
#define SWITCH_H

#include "GameFramework.h"

#include "GameObjectSensor.h"

class Switch : public GameObjectSensor
{

public:


	Switch(Ogre::SceneManager* sceneManager,
		KGBMessageType messageOn = GAME_SENSOR_ON,
		KGBMessageType messageOff = GAME_SENSOR_OFF,
		b2Body* body = 0, Ogre::Entity* entity = 0)
	{
		
	}

	/// Called when two fixtures begin to touch.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		// If we're ignoring sensors and the collided fixture is a sensor we 
		// won't want to respond to this contact.
		if(ignoreSensors_ && collidedFixture->IsSensor())
		{
			// do nothing
		}
		else
		{
			GameObject* object = (GameObject*) collidedFixture->GetUserData();

			if(object != 0)
			{
				if(object->GetId() == GameObject::GetHoltId())
				{
					holtHitCount_++;
				}

				if(object->GetId() == GameObject::GetParkerId()
				{
					parkerHitCount_++;
				}
			}
		}

	}

	/// Called when two fixtures cease to touch.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture,												       b2Fixture* collidedFixture)
	{
		// If we're ignoring sensors and the collided fixture is a sensor we 
		// won't want to respond to this contact.
		if(ignoreSensors_ && collidedFixture->IsSensor())
		{
			// do nothing
		}
		else
		{
			GameObject* object = (GameObject*) collidedFixture->GetUserData();

			if(object != 0)
			{
				if(object->GetId() == GameObject::GetHoltId())
				{
					holtHitCount_--;
				}

				if(object->GetId() == GameObject::GetParkerId()
				{
					parkerHitCount_--;
				}
			}
		}
	}

protected:

	int parkerHitCount_;
	int holtHitCount_;
};

#endif