#include "CheckPoint.h"
#include "Parker.h"
#include "MessageDispatcher.h"


CheckPoint::CheckPoint(Ogre::SceneManager *sceneManager, b2Vec2 center, float width, float height)
{
	triggered_ = false;
	gameObjectType_ = GOType_CheckPoint;
	sceneManager_ = sceneManager;
	center.y += height /2.0;
	position_ = center;
	width_ = width;
	height_ = height;

	b2BodyDef bd;
	bd.position.Set(center.x, center.y);
	body_= world_->CreateBody(&bd);
	
	body_->SetUserData(this);
	
	b2PolygonShape bodyShapeDef;
	
	b2FixtureDef fd;
	fd.shape = &bodyShapeDef;

	fd.isSensor = true;

	b2PolygonShape checkPointSensor_Def;
	checkPointSensor_Def.SetAsBox(width / 2.0,height / 2.0);

	fd.shape = &checkPointSensor_Def;
	fd.isSensor = true;

	checkPointSensor_ = body_->CreateFixture(&fd);

	body_->SetUserData(this);

	Ogre::String name = "CheckPoint";
	name += Ogre::StringConverter::toString(objectId_);
	entity_ = sceneManager_->createEntity(name, "CheckPoint.mesh");

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	sceneNode_->attachObject(entity_);
	sceneNode_->setScale(width, height, width);
	entity_->setMaterial(Ogre::MaterialManager::getSingletonPtr()->getByName("CheckPoint/CheckPoint"));
	
	Initialize();
	
}


CheckPoint::~CheckPoint()
{	
	sceneManager_->destroyEntity(entity_);
}


//=============================================================================
//								BeginContact
//
void CheckPoint::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(!triggered_)
	{
		if(collidedFixture->IsSensor() == true)
		{
			GameObjectOgreBox2D* go = (GameObjectOgreBox2D*)collidedFixture->GetBody()->GetUserData();
			if(go->GetGameObjectType() == GOType_Character_Parker || go->GetGameObjectType() == GOType_Character_Holt)
			{
				CharacterParker* temp = (CharacterParker*)go;
				Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, temp->GetPlayerInfo()->GetId(), UPDATE_CHECKPOINT, position_);
				//triggered_ = true;
			}
		}
	}
}


//=============================================================================
//								EndContact
//
void CheckPoint::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{

}


//=============================================================================
//								Update
//
bool CheckPoint::Update(double timeSinceLastFrame)
{

	UpdateGraphics(timeSinceLastFrame);
	return true;
}
