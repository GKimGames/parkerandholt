#include "LedgeSensor.h"

using namespace PaH;

LedgeSensor::LedgeSensor(b2Vec2 position)
{
	position_ = position;

	world_ = GameFramework::getSingletonPtr()->GetWorld();
	
	// Create body and fixture
	b2BodyDef bd;
	bd.position = position_;
	body_= world_->CreateBody(&bd);
	
	// This is the sensor
	b2CircleDef	circleDef;
	circleDef.radius = 0.38608f;
	circleDef.isSensor = true;
	
	body_->CreateFixture(&circleDef); 
	body_->SetUserData(this);

	gameEntityType_ = PaH::LedgeSensor;

}

LedgeSensor::~LedgeSensor()
{
	Entity::~Entity();
}