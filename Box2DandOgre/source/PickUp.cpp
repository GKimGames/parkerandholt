#include "PickUp.h"
#include <boost/any.hpp>
#include "MessageDispatcher.h"

PickUp::PickUp(Ogre::SceneManager *sceneManager, b2Vec2 center)
{
	sceneManager_ = sceneManager;
	position_ = center;
	
	b2BodyDef bd;
	bd.position.Set(center.x, center.y);
	bd.type = b2_staticBody;
	bd.fixedRotation = false;
	body_= world_->CreateBody(&bd);

	b2CircleShape bodyShapeDef;
	bodyShapeDef.m_radius = 0.5f;
	
	b2FixtureDef fd;
	fd.shape = &bodyShapeDef;

	fd.density = 5;
	fd.isSensor = true;
	fd.friction = DEFAULT_FRICTION;
	
	itemSensor_ = body_->CreateFixture(&fd);

	body_->SetUserData(this);

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	Ogre::String name = "PickUp";
	name += Ogre::StringConverter::toString(objectId_);
	entity_ = sceneManager_->createEntity(name, "sphere.mesh");
	
	entity_->setMaterial(Ogre::MaterialManager::getSingletonPtr()->getByName("TransparentBox/Transparent125"));
	Ogre::SceneNode* meshNode = sceneNode_->createChildSceneNode();
	meshNode->attachObject(entity_);
	meshNode->scale(0.05/10,0.05/10,0.05/10);
	
	pickedUp_ = false;
	
	Initialize();
}

PickUp::PickUp(Ogre::SceneManager *sceneManager, b2Vec2 center, std::string meshName)
{
	sceneManager_ = sceneManager;
	position_ = center;
}

PickUp::~PickUp()
{	
	sceneManager_->destroyEntity(entity_);
}

bool PickUp::Update(double timeSinceLastFrame)
{
	UpdateGraphics(timeSinceLastFrame);

	if(pickedUp_)
	{
		sceneNode_->setVisible(false);
		return false;
	}

	return true;
}


//=============================================================================
//								BeginContact
//
void PickUp::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(!pickedUp_)
	{
		if(collidedFixture->IsSensor() == true)
		{
			GameObjectOgreBox2D* go = (GameObjectOgreBox2D*)collidedFixture->GetBody()->GetUserData();
			if(go->GetGameObjectType() == GameObjectType::GOType_Character_Parker)
			{
				Dispatch->DispatchMessageA(SEND_IMMEDIATELY, 0, SEND_TO_ALL, ADD_ITEM, 1);
				pickedUp_ = true;
			}
		}
	}
}


//=============================================================================
//								EndContact
//
void PickUp::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{

}