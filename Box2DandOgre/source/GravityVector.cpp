/*=============================================================================

		GravityVector.cpp

		Author: Greg King

=============================================================================*/
#include "GravityVector.h"

//=============================================================================
//								GravityVector
//
GravityVector::GravityVector(Ogre::SceneManager *sceneManager, b2Vec2 center, b2Vec2 direction)
{
	active_ = false;
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
	Stop();
	
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
		bodyList_.push_back(collidedFixture->GetBody());
	}
}


//=============================================================================
//								EndContact
//
void GravityVector::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
  if(active_)
  {
	if(contactFixture == gravitySensor_)
	{
		for(int i = 0; i < bodyList_.size(); i++)
		{
			if(bodyList_[i] == collidedFixture->GetBody())
			{
				bodyList_.erase(bodyList_.begin()+i);
				break;
			}
		}
	}
  }
}


//=============================================================================
//								Update
//
bool GravityVector::Update(double timeSinceLastFrame)
{
  if(active_)
  {
	for(int i = 0; i < bodyList_.size(); i++)
	{
		bodyList_[i]->ApplyForce(forceApplied_, position_);
	}
	UpdateGraphics(timeSinceLastFrame);
  }
  else
  {
	  bodyList_.clear();
  }

	
	return true;
}


//=============================================================================
//								Stop
//
bool GravityVector::Stop()
{ 
	active_ = false;
	sceneNode_->setVisible(false);
	Ogre::ParticleSystem* tempPS;
	tempPS = (Ogre::ParticleSystem*)sceneNode_->getAttachedObject("Fireworks");
	return true;
}


//=============================================================================
//								Start
//
bool GravityVector::Start(b2Vec2 newPosition, b2Vec2 newDirection)
{
	position_ = newPosition;
	SetBodyPosition(position_);

	b2Vec2 tempMagnitude; 
	tempMagnitude.x = (newDirection.x - position_.x) / (abs(newDirection.x - position_.x) + abs(newDirection.y - position_.y));
	tempMagnitude.y = (newDirection.y - position_.y) / (abs(newDirection.x - position_.x) + abs(newDirection.y - position_.y));
	forceApplied_.x = maxForce_ * tempMagnitude.x;
	forceApplied_.y = maxForce_ * tempMagnitude.y;



	Ogre::ParticleSystem* tempPS;
	tempPS = (Ogre::ParticleSystem*)sceneNode_->getAttachedObject("Fireworks");
	tempMagnitude.Normalize();
	tempPS->getEmitter(0)->setDirection(Ogre::Vector3(tempMagnitude.x,tempMagnitude.y, 0));
	

	active_ = true;
	sceneNode_->setVisible(true);

	return true;
}