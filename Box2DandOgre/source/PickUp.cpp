/*=============================================================================

	PickUp.cpp

	Author: Greg King

=============================================================================*/

#include "PickUp.h"
#include "Parker.h"
#include <boost/any.hpp>
#include "MessageDispatcher.h"

//=============================================================================
//								Constructor
// 
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
	breakable_ = false;
	
	Initialize();
}

//=============================================================================
//								Constructor
//
// Creates picup with set breaking force, responds to hits from physical objects to break
PickUp::PickUp(Ogre::SceneManager* sceneManager, b2Vec2 center, float breakingForce)
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
	fd.friction = DEFAULT_FRICTION;
	
	itemSensor_ = body_->CreateFixture(&fd);

	body_->SetUserData(this);

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	Ogre::String name = "PickUp";
	name += Ogre::StringConverter::toString(objectId_);
	entity_ = sceneManager_->createEntity(name, "sphere.mesh");
	
	entity_->setMaterial(Ogre::MaterialManager::getSingletonPtr()->getByName("Materials/RedPickUp"));
	Ogre::SceneNode* meshNode = sceneNode_->createChildSceneNode();
	meshNode->attachObject(entity_);
	meshNode->scale(0.05/10,0.05/10,0.05/10);
	
	pickedUp_ = false;

	breakable_ = true;
	breakingForce_ = breakingForce;
	
	Initialize();
}

//=============================================================================
//								Destructor
//
PickUp::~PickUp()
{	
	sceneManager_->destroyEntity(entity_);
}

//=============================================================================
//								Update
//
bool PickUp::Update(double timeSinceLastFrame)
{
	UpdateGraphics(timeSinceLastFrame);

	if(pickedUp_)
	{
		sceneNode_->setVisible(false);
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
			if(go->GetGameObjectType() == GameObjectType::GOType_Character_Parker || go->GetGameObjectType() == GameObjectType::GOType_Character_Holt)
			{
				CharacterParker* temp = (CharacterParker*)go;
				Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, temp->GetPlayerInfo()->GetId(), ADD_ITEM, this);
				HasBeenPickedUp(true);
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

//=============================================================================
//								PostSolve
//
/// Checks if the body is hit hard enough to break it and if so messages Holt to add to inventory
void PickUp::PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse)
{
	if(breakable_)
	{
		if(contact->GetFixtureA()->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->GetFixtureA()->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableA;

			contactableA = dynamic_cast<GameObjectOgreBox2D*> (object);
			if(contactableA == this)
			{
				float totalImpulse = abs(impulse->normalImpulses[0]);
				if(totalImpulse > breakingForce_ && totalImpulse < 500000)
				{
					Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, CharacterParker::GetHoltId(), ADD_ITEM, this);
					HasBeenPickedUp(true);
				}			
			}
		}

		if(contact->GetFixtureB()->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->GetFixtureB()->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableB;

			contactableB = dynamic_cast<GameObjectOgreBox2D*> (object);
			if(contactableB == this)
			{
				float totalImpulse = abs(impulse->normalImpulses[0]);
				if(totalImpulse > breakingForce_ && totalImpulse < 500000)
				{
					Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, CharacterParker::GetHoltId(), ADD_ITEM, this);
					HasBeenPickedUp(true);
				}
			}
		}
	}
}


//=============================================================================
//								HasBeenPickedUp
//
/// Sets whether the object has been picked up or not, toggles visibility
void PickUp::HasBeenPickedUp(bool pickedUp)
{
	pickedUp_ = pickedUp;
	sceneNode_->setVisible(!pickedUp);
	body_->SetActive(!pickedUp);
}