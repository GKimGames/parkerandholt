/*=============================================================================

	Triangle.cpp

=============================================================================*/

#include "Triangle.h"

//=============================================================================
//								Constructor
//
/// Creates a permanent static box
Triangle::Triangle(Ogre::SceneManager* sceneManager, b2Vec2 center, double size)
{

	position_ = center;
	density_ =  10;
	gameObjectType_ = GOType_Triangle;

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

	temporary_ = false;

	CreateTriangle();
}

//=============================================================================
//								Constructor
//
/// Creates a box that exists for the set TimeToLive (TTL)
/// Box will not move, it is  a static object
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

	particleSystem_ = sceneManager_->createParticleSystem(entityName + "psystem", "PaH/fadingBox");
	particleSystem_->getEmitter(0)->setEnabled(false);
	sceneNode_->attachObject(particleSystem_);
	emitterSize_ = Ogre::Vector3(size * 1.5, size * 1.5, size * 1.5);

	TTL_ = TTL;
	temporary_ = true;
	Ogre::ParticleEmitter* emitter = particleSystem_->getEmitter(0);
	
	// Calculates the emmiter size adn emission rate based on the size of the box
	emitter->setParameter("height", Ogre::StringConverter::toString(emitterSize_.y));
	emitter->setParameter("width", Ogre::StringConverter::toString(emitterSize_.x));
	emitter->setParameter("depth", Ogre::StringConverter::toString(emitterSize_.z));
	emitter->setParameter("emission_rate", Ogre::StringConverter::toString((int)(size*size*12*200)));

	timeAlive_ = 0;

	CreateTriangle();
	
}

//=============================================================================
//								CreateTriangle
//
/// Creates the physics behind the object
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

//=============================================================================
//								Update
//
bool Triangle::Update(double timeSinceLastFrame)
{
	// If time is greater then .001 the placement is not active
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

	// If the box is temporary this will incriment its time alive
	if(temporary_)
	{
		timeAlive_ += timeSinceLastFrame;
		Ogre::ParticleEmitter* emitter = particleSystem_->getEmitter(0);

		// Disables the box if it has existed longer than its TTL
		if(timeAlive_ > TTL_)
		{
			particleSystem_->getEmitter(0)->setEnabled(false);
			SetInactive();
		}
		// If the box is only alive for another 2 seconds the partical effect is activated
		else if(TTL_ - timeAlive_ < 2.0)
		{
			this->SetBlendType(Ogre::SceneBlendType::SBT_REPLACE);
			SetTransparency(.5);//(2/TTL_-timeAlive);
			particleSystem_->getEmitter(0)->setEnabled(true);
		}

	}
	UpdateGraphics(timeSinceLastFrame);
	return true;
}

//=============================================================================
//								BeginContact
//
/// Called with objects begin to touch
// Used to check for bad placements
void Triangle::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(placementTest_)
	{
		if(contactFixture->GetBody() == body_ && !collidedFixture->IsSensor())
		{
			badPlacement_ = true;
			placementTest_ = false;
		}
	}
}


//=============================================================================
//								SetInactive
//
/// Disables graphics and physics and moves the box offscreen
void Triangle::SetInactive()
{
	entity_->setVisible(false);
	body_->SetActive(false);
	body_->SetTransform(b2Vec2(0, -100), 0);
}