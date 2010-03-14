/*=============================================================================

	LedgeSensor.cpp

	Author: Matt King

=============================================================================*/

#include "LedgeSensor.h"
#include "GameFramework.h"

LedgeSensor::LedgeSensor() : GameObjectSensor(NO_MESSAGE)
{
	world_ = GameFramework::getSingletonPtr()->GetWorld();
	
	// Create body and fixture
	b2BodyDef bd;
	bd.type = b2_staticBody;
	body_= world_->CreateBody(&bd);
	
	// This is the sensor
	b2CircleShape circleDef;
	circleDef.m_radius = 0.2f;

	b2FixtureDef fd;
	fd.shape = &circleDef;
	fd.isSensor = true;
	
	body_->CreateFixture(&fd); 

	body_->SetUserData(this);

	gameObjectType_ = GOType_Sensor;

}

//=============================================================================
//							BeginContact
//
/// Called when two fixtures begin to touch.
/// Sends out the signal OnTouch to its subscribers
void LedgeSensor::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(ignoreSensors_ && collidedFixture->IsSensor())
	{
		return;
	}
}
