#include "MovingPlatform.h"

using namespace Ogre;

MovingPlatform::MovingPlatform(Ogre::SceneManager* sceneManager,b2Vec2 p1, b2Vec2 p2, b2Vec2 start, b2Vec2 end, double speed)
{
	gameObjectType_ = GOType_MovingPlatform;
	sceneManager_ = sceneManager;
	speed_ = -speed;

	startReachedMessage_ = NO_MESSAGE;
	endReachedMessage_ = NO_MESSAGE;

	disabled_ = false;

	if(p1.x > p2.x)
	{
		b2Vec2 holder = p2;
		p2 = p1;
		p1 = holder;
	}
	point1 = p1;
	point2 = p2;

	startVec_ = start;
	endVec_ = end;

	direction_ = startVec_ - endVec_;
	direction_.Normalize();

	Ogre::String planeName = "PlatformPlane";
	Ogre::String planeMeshName = "PlatformPlaneMesh";
	Ogre::String planeEntityName = "PlatformPlaneEntity";

	planeName += Ogre::StringConverter::toString(objectId_);
	planeMeshName += Ogre::StringConverter::toString(objectId_);
	planeEntityName += Ogre::StringConverter::toString(objectId_);

	plane = new MovablePlane(planeName);
	plane->d = 0; 
	plane->normal = Vector3(0.0, 1.0, 0.0);

	float lengthOfPlane =  sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	
	Ogre::MeshManager::getSingleton().createPlane(planeMeshName, 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*plane,
		lengthOfPlane,  // X Length
		5,				// Z Length
		5, 5,			// Segments x ,y
		true,
		1, 
		3 * lengthOfPlane / 10,		// Tile x
		1,							// Tile y
		Vector3::UNIT_Z);
	

	// Create background material
	MaterialPtr material = MaterialManager::getSingleton().create("Background", "General");
	material->getTechnique(0)->getPass(0)->createTextureUnitState("roadtexture.jpg");
	material->getTechnique(0)->getPass(0)->setLightingEnabled(true);
	//material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	//material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	// Temporary

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	Ogre::SceneNode* planeNode = sceneNode_->createChildSceneNode();
	
	Ogre::Entity* planeEnt = sceneManager_->createEntity(planeEntityName , planeMeshName);
	planeEnt->setMaterial(material);
	planeNode->attachObject(planeEnt);

	
	// Rotate the node to fit the points
	planeNode->roll(Ogre::Radian(atan2(p2.y - p1.y, p2.x - p1.x)));
	
	// Center the node on the midpoint of the two points
	planeNode->setPosition(Real(p1.x + p2.x) / 2.0, Real(p1.y + p2.y) / 2.0, 0.0);

	CreatePhysics();

	Initialize();

	parker_ = NULL;
	parkerCount_ = 0;
	
	unsigned int id = GameObject::GetHoltId();
	
	if(id != -1)
	{
		parker_ = (CharacterParker*) GameObject::GetObjectById(id);
	}
}

void MovingPlatform::CreatePhysics()
{
	// Create body and fixture
	b2BodyDef bd;
	bd.fixedRotation = true;

	bd.position = startVec_;
	bd.type = b2_dynamicBody;
	body_= world_->CreateBody(&bd);

	b2PolygonShape ed;
	ed.SetAsEdge(point1,point2);

	b2FixtureDef fd;
	fd.shape = &ed;

	body_->SetUserData(this);
	body_->CreateFixture(&fd);

	b2FixtureDef fd2;
	fd2.shape = &ed;
	bd.position = startVec_;
	bd.type = b2_kinematicBody; 
	bd.fixedRotation = true;
	kBody_ = world_->CreateBody(&bd);

	kBody_->CreateFixture(&fd2);
	kBody_->SetUserData(this);

	b2RevoluteJointDef jointDef;
	jointDef.Initialize(body_, kBody_, body_->GetWorldCenter());
	jointDef.collideConnected = true;
	world_->CreateJoint(&jointDef);
}

bool MovingPlatform::Update(double timeSinceLastFrame)
{
	if(disabled_ == false)
	{

		// Check if the position of the body is within velocity meters of the 
		// destination point. The destination could be either the endVec or
		// beginVec depending on the speed_. If the speed is negative the
		// platform is moving towards the endVec.
		b2Vec2 v = body_->GetPosition();
		b2Vec2 vel = direction_;
		vel *= speed_;

		if(speed_ < 0)
		{
			if(abs(v.x - endVec_.x) < abs(vel.x * timeSinceLastFrame) && abs(v.y - endVec_.y) < abs(vel.y * timeSinceLastFrame) )
			{
				speed_ *= -1;
				vel *= speed_;
			}

		}
		else
		{
			if(abs(v.x - startVec_.x) < abs(vel.x * timeSinceLastFrame) && abs(v.y - startVec_.y) < abs(vel.y * timeSinceLastFrame) )
			{
				speed_ *= -1;
				vel *= speed_;
			}
		}

		kBody_->SetLinearVelocity(vel);
		
		sceneNode_->setPosition(Ogre::Real(v.x),Ogre::Real(v.y),0);

		if(parker_ != NULL && parkerCount_ > 0)
		{
			b2Body* b = parker_->GetBody();
			b2Vec2 lv = b->GetLinearVelocity();
			b->SetLinearVelocity(b2Vec2(lv.x - direction_.x, lv.y));
		}
	}



	return true;

}

MovingPlatform::~MovingPlatform()
{
	delete plane;
}

