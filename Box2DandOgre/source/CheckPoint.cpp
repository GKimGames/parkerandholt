#include "CheckPoint.h"
#include "Parker.h"
#include "MessageDispatcher.h"


CheckPoint::CheckPoint(Ogre::SceneManager *sceneManager, b2Vec2 center, float width, float height)
{
	triggered_ = false;
	gameObjectType_ = GOType_CheckPoint;
	sceneManager_ = sceneManager;
	position_ = center;
	width_ = width;
	height_ = height;

	b2BodyDef bd;
	bd.position.Set(center.x, center.y);
	bd.type = b2_staticBody;
	bd.fixedRotation = false;
	body_= world_->CreateBody(&bd);
	
	body_->SetUserData(this);
	
	b2PolygonShape bodyShapeDef;
	
	b2FixtureDef fd;
	fd.shape = &bodyShapeDef;

	fd.density = 5;
	fd.isSensor = true;
	fd.friction = DEFAULT_FRICTION;


	b2PolygonShape checkPointSensor_Def;
	checkPointSensor_Def.m_vertexCount = 4;
	checkPointSensor_Def.m_vertices[0].Set(-width_/2,  height_/2);
	checkPointSensor_Def.m_vertices[1].Set(-width_/2, -height_/2);
	checkPointSensor_Def.m_vertices[2].Set(width_/2,  -height_/2);
	checkPointSensor_Def.m_vertices[3].Set(width_/2,   height_/2);

	fd.shape = &checkPointSensor_Def;
	fd.isSensor = true;

	checkPointSensor_ = body_->CreateFixture(&fd);

	body_->SetUserData(this);


	Ogre::String name = "CheckPoint";
	name += Ogre::StringConverter::toString(objectId_);
	entity_ = sceneManager_->createEntity(name, "CheckPoint.mesh");


	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	sceneNode_->attachObject(entity_);
	entity_->getParentSceneNode()->setScale(width, height, 0.1);
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
			if(go->GetGameObjectType() == GameObjectType::GOType_Character_Parker)
			{
				CharacterParker* temp = (CharacterParker*)go;
				Dispatch->DispatchMessageA(SEND_IMMEDIATELY, 0, temp->GetPlayerInfo()->GetId(), UPDATE_CHECKPOINT, position_);
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
