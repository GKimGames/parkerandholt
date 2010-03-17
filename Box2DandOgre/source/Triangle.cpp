/*=============================================================================

	Triangle.cpp

	Author: Greg King

=============================================================================*/

#include "Triangle.h"

Triangle::Triangle(Ogre::SceneManager* sceneManager, b2Vec2 center, double size)
{

	position_ = center;
	density_ =  10;
	gameObjectType_ = GOType_HoltBox;

	sceneManager_ = sceneManager;
	Ogre::String entityName = "Triangle";

	// Add the entity number to the HoltBox name to make it unique.
	entityName += Ogre::StringConverter::toString(objectId_);

	entity_ = sceneManager_->createEntity(entityName, "cube.1m.mesh");
	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	sceneNode_->attachObject(entity_);
	sceneNode_->setPosition(center.x, center.y, 0);
	
	triangleWidth_ = size;

	ModifyWidth(-0.1);

	placementTest_ = false;
	badPlacement_ = false;

	CreateTriangle();
	
}

Triangle::Triangle(Ogre::SceneManager *sceneManager, b2Vec2 center, double size, float TTL)
{
	
	position_ = center;
	density_ =  10;
	gameObjectType_ = GOType_HoltBox;

	sceneManager_ = sceneManager;
	Ogre::String entityName = "Triangle";

	// Add the entity number to the HoltBox name to make it unique.
	entityName += Ogre::StringConverter::toString(objectId_);

	entity_ = sceneManager_->createEntity(entityName, "cube.1m.mesh");
	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	sceneNode_->attachObject(entity_);
	sceneNode_->setPosition(center.x, center.y, 0);
	
	triangleWidth_ = size;

	ModifyWidth(-0.1);

	placementTest_ = false;
	badPlacement_ = false;

	TTL_ = TTL;
	temporary_ = true;
	timeAlive_ = 0;

	CreateTriangle();
	
}


bool Triangle::CreateTriangle()
{
	if(initialized_ == true)
	{
		return true;
	}

	b2BodyDef def;
	def.position = position_;
	def.type = b2BodyType::b2_staticBody;
	def.fixedRotation = false;
	body_ = world_->CreateBody(&def);
	body_->SetUserData(this);

	if(body_)
	{
		b2PolygonShape sd;
		sd.SetAsBox(triangleWidth_, triangleWidth_);

		b2FixtureDef fd;
		fd.shape = &sd;
		fd.density = density_;
		fd.isSensor = true;
		fd.friction = DEFAULT_FRICTION;

		body_->CreateFixture(&fd);

		Initialize();

		placementTest_ = true;
		world_->Step(0.001, 1, 1);
		body_->GetFixtureList()->SetSensor(true);

		return true;
	}

	return false;	
}

bool Triangle::Update(double timeSinceLastFrame)
{
	if(timeSinceLastFrame > .001)
	{
		placementTest_ = false;
		if(badPlacement_)
		{
			body_->SetActive(false);
		}
		if(!badPlacement_)
		{
			body_->GetFixtureList()->SetSensor(false);
		}
	}
	if(temporary_)
	{
		timeAlive_ += timeSinceLastFrame;
		if(timeAlive_ > TTL_)
		{
			entity_->setVisible(false);
			body_->SetActive(false);
		}
	}
	UpdateGraphics(timeSinceLastFrame);
	return true;
}

void Triangle::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(placementTest_)
	{
		if(contactFixture->GetBody() == body_ && !collidedFixture->IsSensor())
		{
			//body_->GetFixtureList()->SetSensor(true);
			badPlacement_ = true;
			placementTest_ = false;
		}
	}
}