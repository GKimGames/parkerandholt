/*=============================================================================

	HoltBox.cpp

	Author: Matt King

=============================================================================*/

#include "HoltBox.h"
#include "MessageDispatcher.h"

//=============================================================================
//								Constructor
//
/// Default size
HoltBox::HoltBox(Ogre::SceneManager* sceneManager, b2Vec2 center)
{

	position_ = center;
	density_ =  10;
	gameObjectType_ = GOType_HoltBox;

	sceneManager_ = sceneManager;
	Ogre::String entityName = "HoltBox";

	// Add the entity number to the HoltBox name to make it unique.
	entityName += Ogre::StringConverter::toString(objectId_);

	entity_ = sceneManager_->createEntity(entityName, "cube.1m.mesh");

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	sceneNode_->attachObject(entity_);
	sceneNode_->setPosition(center.x, center.y, 0);
	
	boxWidth_ = 0.6;

	ModifyBoxWidth(-0.1);

	placementTest_ = false;
	badPlacement_ = false;

	CreateBox2DBox();
	
}

//=============================================================================
//								Constructo
//
/// Creates box with specified attributes, used for Holt's placeable
HoltBox::HoltBox(Ogre::SceneManager* sceneManager, b2Vec2 center, double size, double density)
{
	position_ = center;
	density_ = density;
	boxWidth_ = size;
	
	gameObjectType_ = GOType_HoltBox;


	sceneManager_ = sceneManager;
	Ogre::String entityName = "HoltBox";

	// Add the entity number to the HoltBox name to make it unique.
	entityName += Ogre::StringConverter::toString(objectId_);

	entity_ = sceneManager_->createEntity(entityName, "cube.1m.mesh");

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	sceneNode_->attachObject(entity_);

	sceneNode_->setPosition(center.x, center.y, 0);

	ModifyBoxWidth(0.0);

	placementTest_ = false;
	badPlacement_ = false;

	CreateBox2DBox();
}

//=============================================================================
//								CreateBox2DBox
//
/// Creates the physical box
bool HoltBox::CreateBox2DBox()
{
	if(initialized_ == true)
	{
		return true;
	}


	b2BodyDef boxDef;
	boxDef.position = position_;
	boxDef.type = b2_dynamicBody;
	boxDef.fixedRotation = false;
	body_ = world_->CreateBody(&boxDef);
	body_->SetUserData(this);

	if(body_)
	{
		b2PolygonShape sd;
		sd.SetAsBox(boxWidth_, boxWidth_);

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
// used mainly to determine if the position is intersecting another physical object
bool HoltBox::Update(double timeSinceLastFrame)
{
	if(timeSinceLastFrame > .001)
	{
		placementTest_ = false;
		if(badPlacement_)
		{
			SetInactive();
		}
		if(!badPlacement_)
		{
			body_->GetFixtureList()->SetSensor(false);
		}
	}
	UpdateGraphics(timeSinceLastFrame);
	return true;
}

//=============================================================================
//								BeginContact
//
// Checks to see if the placement test is active, if so checks for contacts
void HoltBox::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
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
/// Sets the graphics and physics inactive and moves the box off screen
// Used in place of deletion as deleting can break the GameObject list
void HoltBox::SetInactive()
{
	entity_->setVisible(false);
	body_->SetActive(false);
	body_->SetTransform(b2Vec2(0, -100), 0);
}
