/*=============================================================================

	HoltBox.cpp

	Author: Matt King

=============================================================================*/

#include "HoltBox.h"

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

	CreateBox2DBox();
	
}

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

	CreateBox2DBox();
}


bool HoltBox::CreateBox2DBox()
{
	if(initialized_ == true)
	{
		return true;
	}

	/*
	b2AABB aabb;
	aabb.lowerBound.Set(position_.x - boxWidth_, position_.y - boxWidth_);
	aabb.upperBound.Set(position_.x + boxWidth_, position_.y + boxWidth_);
	
	const int32 k_bufferSize = 10;
	
	b2Fixture* buffer[k_bufferSize];
	
	int32 count = world_->QueryIgnoreSensors(aabb, buffer, k_bufferSize);
	
	// There are shapes where we want to put our box so we can't 
	// put it there.
	if(count > 0)
	{
		return false;
	}
	*/

	b2BodyDef boxDef;
	boxDef.position = position_;
	boxDef.active = true;
	boxDef.type = b2_dynamicBody;
	body_ = world_->CreateBody(&boxDef);

	if(body_)
	{
		b2PolygonShape sd;
		sd.SetAsBox(boxWidth_, boxWidth_);

		body_->CreateFixture(&sd);

		b2FixtureDef fd;
		fd.shape = &sd;
		fd.density = density_;
		fd.friction = DEFAULT_FRICTION;

		body_->CreateFixture(&fd);

		body_->SetUserData(this);

		Initialize();

		return true;
	}


	

	return false;

	
}
