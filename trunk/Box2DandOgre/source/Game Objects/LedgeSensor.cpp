#include "LedgeSensor.h"


LedgeSensor::LedgeSensor(b2Vec2 position)
{
	position_ = position;

	world_ = GameFramework::getSingletonPtr()->GetWorld();
	
	// Create body and fixture
	b2BodyDef bd;
	bd.position = position_;
	bd.type = b2_staticBody;
	body_= world_->CreateBody(&bd);
	
	// This is the sensor
	b2CircleShape	circleDef;
	circleDef.m_radius = 0.38608f;

	b2FixtureDef fd;
	fd.shape = &circleDef;
	fd.isSensor = true;
	
	body_->CreateFixture(&fd); 

	body_->SetUserData(this);

	gameObjectType_ = GOType_LedgeSensor;

}
