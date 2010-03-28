/*=============================================================================

		Platform.cpp

		Author: Matt King

=============================================================================*/

#include "Platform.h"

using namespace Ogre;

Platform::Platform(Ogre::SceneManager* sceneManager,b2Vec2 p1, b2Vec2 p2)
{
	sceneManager_ = sceneManager;

	point1.x = p1.x;
	point1.y = p1.y;
	point2.x = p2.x;
	point2.y = p2.y;
	
	Initialize();
}

Platform::Platform(Ogre::SceneManager* sceneManager,b2Vec2 p1, b2Vec2 p2, int temp)
{
	sceneManager_ = sceneManager;

	point1.x = p1.x;
	point1.y = p1.y;
	point2.x = p2.x;
	point2.y = p2.y;
	placementTest_ = false;
	badPlacement_ = false;
	
	InitializePlaceable();
}

Platform::Platform()
{
	// do nothing
}

bool Platform::Initialize()
{
	//initialized_ = GameObjectOgreBox2D::Initialize();

	if(initialized_)
	{
		gameObjectType_ = GOType_Platform;

		if(point1.x > point2.x)
		{
			b2Vec2 holder = point2;
			point2 = point1;
			point1 = holder;
		}

		Ogre::String planeName("PlatformPlane");
		Ogre::String planeMeshName("PlatformPlaneMesh");
		Ogre::String planeEntityName("PlatformPlaneEntity");

		planeName += Ogre::StringConverter::toString(objectId_);
		planeMeshName += Ogre::StringConverter::toString(objectId_);
		planeEntityName += Ogre::StringConverter::toString(objectId_);

		plane = new MovablePlane(planeName);
		plane->d = 0; 
		plane->normal = Vector3(0.0, 1.0, 0.0);

		float lengthOfPlane = sqrt((point2.x - point1.x) * (point2.x - point1.x) + (point2.y - point1.y) * (point2.y - point1.y));
		
		Ogre::MeshManager::getSingleton().createPlane(planeMeshName, 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			*plane,
			lengthOfPlane,  // X Length
			15,				// Z Length
			3 * lengthOfPlane / 15, 5,			// Segments x ,y
			true,
			1, 
			3 * lengthOfPlane / 15,		// Tile x
			1,							// Tile y
			Vector3::UNIT_Z);
		

		// Create background material
		MaterialPtr material = MaterialManager::getSingleton().create("Background", "General");
		material->getTechnique(0)->getPass(0)->createTextureUnitState("road.png");
		material->getTechnique(0)->getPass(0)->setLightingEnabled(true);
		//material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
		//material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
		// Temporary

		Ogre::Entity* planeEnt = sceneManager_->createEntity(planeEntityName , planeMeshName);
		planeEnt->setMaterial(material);

		Ogre::SceneNode* planeNode = sceneManager_->getRootSceneNode()->createChildSceneNode();
		planeNode->attachObject(planeEnt);
		
		// Rotate the node to fit the points
		planeNode->roll(Ogre::Radian(atan2(point2.y - point1.y, point2.x - point1.x)));
		
		// Center the node on the midpoint of the two points
		planeNode->setPosition(Real(point1.x + point2.x) / 2.0, Real(point1.y + point2.y) / 2.0, 0.0);

		// Create body and fixture
		b2BodyDef bd;
		bd.position.Set(0.0f, 0.0f);
		bd.type = b2_staticBody;
		body_= world_->CreateBody(&bd);

		b2PolygonShape ed;
		ed.SetAsEdge(point1,point2);

		b2FixtureDef fd;
		fd.shape = &ed;
		fd.friction = DEFAULT_FRICTION;
		fd.filter.groupIndex = STATIC_MAP_GROUP;

		body_->CreateFixture(&fd);
	}

	return initialized_;
}

bool Platform::Initialize(Ogre::String materialName)
{
	gameObjectType_ = GOType_Platform;

	if(point1.x > point2.x)
	{
		b2Vec2 holder = point2;
		point2 = point1;
		point1 = holder;
	}

	Ogre::String planeName = "PlatformPlane";
	Ogre::String planeMeshName = "PlatformPlaneMesh";
	Ogre::String planeEntityName = "PlatformPlaneEntity";

	planeName += Ogre::StringConverter::toString(objectId_);
	planeMeshName += Ogre::StringConverter::toString(objectId_);
	planeEntityName += Ogre::StringConverter::toString(objectId_);

	plane = new MovablePlane(planeName);
	plane->d = 0; 
	plane->normal = Vector3(0.0, 1.0, 0.0);

	float lengthOfPlane = sqrt((point2.x - point1.x) * (point2.x - point1.x) + (point2.y - point1.y) * (point2.y - point1.y));
	
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
	
	Ogre::Entity* planeEnt = sceneManager_->createEntity(planeEntityName , planeMeshName);
	planeEnt->setMaterial(MaterialManager::getSingleton().getByName(materialName));

	Ogre::SceneNode* planeNode = sceneManager_->getRootSceneNode()->createChildSceneNode();
	planeNode->attachObject(planeEnt);
	
	Ogre::Radian rollBy = Ogre::Radian(atan2(point2.y - point1.y, point2.x - point1.x));
	// Rotate the node to fit the points
	planeNode->roll(rollBy);
	
	// Center the node on the midpoint of the two points
	planeNode->setPosition(Real(point1.x + point2.x) / 2.0, Real(point1.y + point2.y) / 2.0, 0.0);

	// Create body and fixture
	b2BodyDef bd;
	bd.position.Set(0.0f, 0.0f);
	bd.type = b2_staticBody;
	body_= world_->CreateBody(&bd);

	b2PolygonShape ed;
	ed.SetAsEdge(point1,point2);

	b2FixtureDef fd;
	fd.shape = &ed;
	fd.friction = DEFAULT_FRICTION;
	fd.filter.groupIndex = STATIC_MAP_GROUP;

	body_->CreateFixture(&fd);

	return true;
}

Platform::~Platform()
{

}


bool Platform::InitializePlaceable()
{
	gameObjectType_ = GOType_Platform;

	if(point1.x > point2.x)
	{
		b2Vec2 holder = point2;
		point2 = point1;
		point1 = holder;
	}

	
	// Create body and fixture
	b2BodyDef bd;
	bd.position.Set((point1.x + point2.x) / 2.0, (point1.y + point2.y) / 2.0);
	bd.type = b2_dynamicBody;
	bd.fixedRotation = false;
	body_= world_->CreateBody(&bd);
	body_->SetUserData(this);

	float tempLength = sqrt((point2.x - point1.x) * (point2.x - point1.x) + (point2.y - point1.y) * (point2.y - point1.y));
	float platformHeight = 0.1f;

	b2PolygonShape bodyShapeDef;
	bodyShapeDef.SetAsBox(tempLength/2.0f, platformHeight/2.0f);
	b2FixtureDef fd;
	fd.shape = &bodyShapeDef;

	fd.density = 25;
	fd.isSensor = true;
	fd.friction = DEFAULT_FRICTION;
	//fd.filter.groupIndex = STATIC_MAP_GROUP;

	body_->CreateFixture(&fd);
	body_->SetTransform(body_->GetPosition(), atan2(point2.y - point1.y, point2.x - point1.x));
	
	Ogre::String entityName = "Platform";

	// Add the entity number to the HoltBox name to make it unique.
	entityName += Ogre::StringConverter::toString(objectId_);
	entity_ = sceneManager_->createEntity(entityName, "cube.1m.mesh");
	Ogre::AxisAlignedBox box = entity_->getBoundingBox();
	Ogre::Vector3 v3 = box.getSize();

	sceneNode_->attachObject(entity_);

	sceneNode_->scale(tempLength/v3.x,platformHeight / v3.y, 1.0);

	previousAngle_ = body_->GetAngle();
	initialized_ = true;
	return true;
}
bool Platform::SetGraphics(b2Vec2 position, float length, float angle, bool final)
{
		
		body_->SetTransform(body_->GetPosition(), -previousAngle_);
		body_->SetTransform(position, angle);

		sceneNode_->setScale(length, 0.1f, 1.0);

		previousAngle_ = angle;
 


		//b2PolygonShape bodyShapeDef;
		//bodyShapeDef.SetAsBox(2.0f, 2.0f); //body_->GetPosition(), previousAngle_);
		//b2FixtureDef fd;
		//fd.shape = &bodyShapeDef;

		//fd.density = 25;
		//fd.friction = DEFAULT_FRICTION;
		//body_->GetPosition().x -3;
		////fd.filter.groupIndex = STATIC_MAP_GROUP;

		//body_->CreateFixture(&fd);


	if(final)
	{
		body_->DestroyFixture(body_->GetFixtureList());
		b2PolygonShape bodyShapeDef;
		bodyShapeDef.SetAsBox(length/2, 0.05f); //body_->GetPosition(), previousAngle_);
		b2FixtureDef fd;
		fd.shape = &bodyShapeDef;
		fd.isSensor = true;
		fd.density = 25;
		fd.friction = DEFAULT_FRICTION;
		//fd.filter.groupIndex = STATIC_MAP_GROUP;

		body_->CreateFixture(&fd);
		
		placementTest_ = true;
		world_->Step(0.001, 1, 1);
	}

	return true;
}


void Platform::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(placementTest_)
	{
		if(contactFixture->GetBody() == body_  && !collidedFixture->IsSensor())
		{
			body_->GetFixtureList()->SetSensor(true);
			badPlacement_ = true;
			placementTest_ = false;
		}
	}
}