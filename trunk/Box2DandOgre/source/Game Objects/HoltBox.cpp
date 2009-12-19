#include "HoltBox.h"


HoltBox::HoltBox(Ogre::SceneManager* sceneManager, b2Vec2 center)
{

	position_ = center;

	gameObjectType_ = GOType_HoltBox;

	sceneManager_ = sceneManager;
	Ogre::String entityName = "HoltBox";
	entityName += Ogre::StringConverter::toString(objectId_);

	entity_ = sceneManager_->createEntity(entityName, "cube.1m.mesh");

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	sceneNode_->attachObject(entity_);

	sceneNode_->setPosition(center.x, center.y, 0);
	
	boxWidth_ = 0.6;

	ModifyBoxWidth(0.1);

	Initialize();

	CreateBox2DBox();
	
}

bool HoltBox::CreateBox2DBox()
{
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

	b2BodyDef boxDef;
	boxDef.position = position_;
	body_ = world_->CreateBody(&boxDef);

	b2PolygonDef sd;
	sd.SetAsBox(boxWidth_, boxWidth_);
	sd.density = 10.0;
	body_->CreateFixture(&sd);
	body_->SetMassFromShapes();

	body_->SetUserData(this);

	Initialize();

	return true;
}
