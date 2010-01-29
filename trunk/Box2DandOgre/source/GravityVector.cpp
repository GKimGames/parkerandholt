#include "GravityVector.h"

//=============================================================================
//								GravityVector
//
GravityVector::GravityVector(Ogre::SceneManager *sceneManager, b2Vec2 center, b2Vec2 direction)
{
	maxForce_ = 300;
	sceneManager_ = sceneManager;
	position_ = center;

	b2BodyDef bd;
	bd.position.Set(center.x, center.y);
	bd.type = b2_staticBody;
	bd.fixedRotation = false;
	body_= world_->CreateBody(&bd);

	b2CircleShape bodyShapeDef;
	bodyShapeDef.m_radius = 1.5f;
	
	b2FixtureDef fd;
	fd.shape = &bodyShapeDef;

	fd.density = 5;
	fd.isSensor = true;
	fd.friction = DEFAULT_FRICTION;

	gravitySensor_ = body_->CreateFixture(&fd);
	body_->SetUserData(this);

	b2Vec2 tempMagnitude; 
	tempMagnitude.x = (direction.x - center.x) / (abs(direction.x - center.x) + abs(direction.y - center.y));
	tempMagnitude.y = (direction.y - center.y) / (abs(direction.x - center.x) + abs(direction.y - center.y));
	forceApplied_.x = maxForce_ * tempMagnitude.x;
	forceApplied_.y = maxForce_ * tempMagnitude.y;

	
	//sceneNode_ = sceneManager_->getRootSceneNode()->createChild();

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	Ogre::ParticleSystem* ps = sceneManager_->createParticleSystem("Fireworks", "Examples/MattAureola");
	sceneNode_->attachObject(ps);
	tempMagnitude.Normalize();
	ps->getEmitter(0)->setDirection(Ogre::Vector3(tempMagnitude.x,tempMagnitude.y, 0));

	entity_ = sceneManager_->createEntity("GravMesh", "sphere.mesh");
	
	entity_->setMaterial(Ogre::MaterialManager::getSingletonPtr()->getByName("TransparentBox/Transparent125"));
	Ogre::SceneNode* meshNode = sceneNode_->createChildSceneNode();
	meshNode->attachObject(entity_);
	meshNode->scale(0.05,0.05,0.05);



	// Rotate the node to fit the points
	//sceneNode_->roll(Ogre::Radian(atan2(direction.y, direction.x)));
	sceneNode_->scale(0.3,0.3,0.3);
	Initialize();
	
}

//=============================================================================
//								GravityVector
//
GravityVector::~GravityVector()
{	
	sceneManager_->destroyEntity(entity_);
	sceneManager_->destroyParticleSystem("Fireworks");
}


//=============================================================================
//								BeginContact
//
void GravityVector::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(contactFixture == gravitySensor_)
	{
		//collidedFixture->GetBody()->ApplyForce(b2Vec2(0.0f, 2000.0f), position_);
		bodyList_.push_back(collidedFixture->GetBody());
	}
}


//=============================================================================
//								EndContact
//
void GravityVector::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(contactFixture == gravitySensor_)
	{
		int temp;
		//collidedFixture->GetBody()->ApplyForce(b2Vec2(0.0f, 2000.0f), position_);
		for(int i = 0; i < bodyList_.size(); i++)
		{
			if(bodyList_[i] == collidedFixture->GetBody())
			{
				temp = i;
			}
		}
		bodyList_.erase(bodyList_.begin()+temp);

	}
}


//=============================================================================
//								Update
//
bool GravityVector::Update(double timeSinceLastFrame)
{
	for(int i = 0; i < bodyList_.size(); i++)
	{
		bodyList_[i]->ApplyForce(forceApplied_, position_);
	}

	UpdateGraphics(timeSinceLastFrame);
	return true;
}