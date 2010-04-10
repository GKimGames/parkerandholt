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


	//create partical effects for the GravityVector
	Ogre::String name;
	name = "FireWorks";
	name += Ogre::StringConverter::toString(objectId_);

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	Ogre::ParticleSystem* ps = sceneManager_->createParticleSystem(name, "Examples/MattAureola");
	sceneNode_->attachObject(ps);
	tempMagnitude.Normalize();
	ps->getEmitter(0)->setDirection(Ogre::Vector3(tempMagnitude.x,tempMagnitude.y, 0));

	name = "GravMesh";
	name += Ogre::StringConverter::toString(objectId_);
	entity_ = sceneManager_->createEntity(name, "sphere.mesh");

	entity_->setMaterial(Ogre::MaterialManager::getSingletonPtr()->getByName("TransparentBox/Transparent125"));
	Ogre::SceneNode* meshNode = sceneNode_->createChildSceneNode();
	meshNode->attachObject(entity_);
	meshNode->scale(0.05,0.05,0.05);

	forcePoint_ = b2Vec2(0,0);


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
	Ogre::String name;
	name = "FireWorks";
	name += Ogre::StringConverter::toString(objectId_);
	sceneManager_->destroyParticleSystem(name);
}


//=============================================================================
//								BeginContact
//
// Previously used to check if something was in the vector, is innaccurate
void GravityVector::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	//if(contactFixture == gravitySensor_ && collidedFixture->GetBody()->GetUserData() != 0)
	//{
	//	bodyList_.push_back(collidedFixture->GetBody());
	//}
	//if(collidedFixture == gravitySensor_)
	//{
	//	bodyList_.push_back(collidedFixture->GetBody());
	//}
}


//=============================================================================
//								EndContact
//
// Previously used to check if something was leaving the vector, is innaccurate
void GravityVector::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	//if(active_)
	//{
	//	if(contactFixture == gravitySensor_)
	//	{
	//		for(int i = 0; i < bodyList_.size(); i++)
	//		{
	//			if(bodyList_[i] == collidedFixture->GetBody())
	//			{
	//				bodyList_.erase(bodyList_.begin()+i);
	//				break;
	//			}
	//		}
	//	}
	//}
}


//=============================================================================
//								Update
//
// If active the vector checks all contacts with itself to see if they are actually touching
//if so it raycasts to their center to get a position to push on, if the cast fails it pushes based on its own center
bool GravityVector::Update(double timeSinceLastFrame)
{
	bodyList_.clear();

	if(active_)
	{	
		b2ContactEdge* contacts = body_->GetContactList();
		while(contacts)
		{
			b2Fixture* A = contacts->contact->GetFixtureA();
			b2Fixture* B = contacts->contact->GetFixtureB();

			GameObject* goA = (GameObject*) A->GetBody()->GetUserData();
			GameObject* goB = (GameObject*) B->GetBody()->GetUserData();

			GameObject* collidedObject;
			b2Fixture*	collidedFixture;
			b2Fixture*	myFixture;

			if(goA == this)
			{
				collidedObject =	goB;
				collidedFixture =	B;
				myFixture =			A;
			}
			else
			{
				collidedObject =	goA;
				collidedFixture =	A;
				myFixture =			B;
			}
			
			bodyList_.push_back(collidedFixture->GetBody());

			contacts = contacts->next;
		}
	}

	if(active_)
	{	
		for(int i = 0; i < bodyList_.size(); i++)
		{
			listNumber_ = i;
			world_->RayCast(this, position_, bodyList_[0]->GetPosition());
			bodyList_[i]->ApplyForce(forceApplied_, forcePoint_);
		}
		
		UpdateGraphics(timeSinceLastFrame);
	}

	return true;
}


//=============================================================================
//								Stop
//
/// Stops the vector, sets it invisible and clears its list of bodies to push on
bool GravityVector::Stop()
{ 
	active_ = false;
	sceneNode_->setVisible(false);
	Ogre::ParticleSystem* tempPS;
	Ogre::String name;
	name = "FireWorks";
	name += Ogre::StringConverter::toString(objectId_);
	tempPS = (Ogre::ParticleSystem*)sceneNode_->getAttachedObject(name);
	bodyList_.clear();

	return true;
}


//=============================================================================
//								Start
//
/// Sets the vector visible, calculates the force direction and reactivates the partical effect
bool GravityVector::Start(b2Vec2 newPosition, b2Vec2 newDirection)
{
	position_ = newPosition;
	SetBodyPosition(position_);

	b2Vec2 tempMagnitude; 
	tempMagnitude.x = (newDirection.x - position_.x) / (abs(newDirection.x - position_.x) + abs(newDirection.y - position_.y));
	tempMagnitude.y = (newDirection.y - position_.y) / (abs(newDirection.x - position_.x) + abs(newDirection.y - position_.y));
	forceApplied_.x = maxForce_ * tempMagnitude.x;
	forceApplied_.y = maxForce_ * tempMagnitude.y;

	Ogre::String name;
	name = "FireWorks";
	name += Ogre::StringConverter::toString(objectId_);
	Ogre::ParticleSystem* tempPS;
	tempPS = (Ogre::ParticleSystem*)sceneNode_->getAttachedObject(name);
	tempMagnitude.Normalize();
	tempPS->getEmitter(0)->setDirection(Ogre::Vector3(tempMagnitude.x,tempMagnitude.y, 0));


	active_ = true;
	sceneNode_->setVisible(true);

	return true;
}


//=============================================================================
//								SetPosition
//
void GravityVector::SetPosition(b2Vec2 position)
{
	position_ = position;
	SetBodyPosition(position_);
	sceneNode_->setPosition(position_.x, position_.y, 0);
}


//=============================================================================
//								RemovePlayer
//
bool GravityVector::RemovePlayer(GameObjectId characterId)
{
	for(int i = 0; i < bodyList_.size(); i++)
	{
		if(bodyList_[i]->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) bodyList_[i]->GetUserData();
			if(object->GetId() == characterId)
			{
				bodyList_.erase(bodyList_.begin()+i);
				return true;
			}
		}

	}
	return false;
}

//=============================================================================
//								ReportFixture
//
float32 GravityVector::ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction)
{
	//returns fisrt contact point with the object being pushed, if no point is found it takes a mid point
	if(fixture->GetBody() == bodyList_[listNumber_])
	{
		forcePoint_ = point;
		return 0.0f;
	}
	b2Vec2 defaultPoint;
	defaultPoint.x = position_.x + (position_.x - bodyList_[listNumber_]->GetPosition().x)/2;
	defaultPoint.y = position_.y + (position_.y - bodyList_[listNumber_]->GetPosition().y)/2;

	forcePoint_ = defaultPoint;
	return 1.0f;

}

//=============================================================================
//								SetParticalsVisible
//
void GravityVector::SetParticalsVisible(bool visible)
{
	Ogre::String name;
	name = "FireWorks";
	name += Ogre::StringConverter::toString(objectId_);
	sceneManager_->getParticleSystem(name)->setVisible(visible);
}
