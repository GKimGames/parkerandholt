/*=============================================================================

		Platform.cpp

		Author: Matt King

=============================================================================*/

#include "Platform.h"

using namespace Ogre;

//=============================================================================
//								Constructor
//
Platform::Platform(Ogre::SceneManager* sceneManager,b2Vec2 p1, b2Vec2 p2)
{
	sceneManager_ = sceneManager;

	point1.x = p1.x;
	point1.y = p1.y;
	point2.x = p2.x;
	point2.y = p2.y;
	
	Initialize();
}

//=============================================================================
//								Constructor
//
/// Constructor for a placeable platform
Platform::Platform(Ogre::SceneManager* sceneManager,b2Vec2 p1, b2Vec2 p2, bool placeable)
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


//=============================================================================
//								Initialize
//
bool Platform::Initialize()
{
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

//=============================================================================
//								EndContact
//
/// Initialize for a specific material
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

//=============================================================================
//								Destructor
//
Platform::~Platform()
{

}

//=============================================================================
//								Initialize
//
/// Initialize a placeable, starts the object as a sensor
bool Platform::InitializePlaceable()
{
	gameObjectType_ = GOType_PlaceablePlatform;

	if(point1.x > point2.x)
	{
		b2Vec2 holder = point2;
		point2 = point1;
		point1 = holder;
	}
	maxLength_ = 8.0;
	
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

//=============================================================================
//								SetGraphics
//
// Used to update the graphics based on mouse position
// Only use on placeables. Also limits the length of a placeable
bool Platform::SetGraphics(b2Vec2 position, float length, float angle, bool final)
{
	// Moves the body to move the graphics
	body_->SetTransform(body_->GetPosition(), -previousAngle_);
	body_->SetTransform(position, angle);

	sceneNode_->setScale(length, 0.1f, 1.0);

	previousAngle_ = angle;
	maxLength_ = 8.0;

	// If this is the final position it deletes the currently fixture and creates a new one
	// which is used as the placeable and set to be an active body, not a sensor
	if(final)
	{
		body_->DestroyFixture(body_->GetFixtureList());
		b2PolygonShape bodyShapeDef;
		if(length > maxLength_)
		{
			sceneNode_->setScale(maxLength_, 0.1f, 1.0);
			bodyShapeDef.SetAsBox(maxLength_/2, 0.05f);
		}
		else
		{
			sceneNode_->setScale(length, 0.1f, 1.0);
			bodyShapeDef.SetAsBox(length/2, 0.05f);
		}
		
		b2FixtureDef fd;
		fd.shape = &bodyShapeDef;
		fd.isSensor = true;
		fd.density = 25;
		fd.friction = DEFAULT_FRICTION;

		body_->CreateFixture(&fd);
		
		// Bool will start the placement test next update
		placementTest_ = true;
		world_->Step(0.001, 1, 1);
	}

	return true;
}

//=============================================================================
//								BeginContact
//
/// Used to perform the placement test. If the test fails the position conflicts with another object
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

//=============================================================================
//								SetInactive
//
/// Sets the entity invisible and the body inactive and moved off screen
void Platform::SetInactive()
{
	entity_->setVisible(false);
	body_->SetActive(false);
	body_->SetTransform(b2Vec2(0, -100), 0);
}

//=============================================================================
//								Update
//
bool Platform::Update(double timeSinceLastFrame)
{ 
	// Placement check fram uses this timestep
	if(timeSinceLastFrame > .001)
	{
		// Ends the test, no more small timesteps
		placementTest_ = false;
		
		if(badPlacement_)
		{
			SetInactive();
		}
		if(!badPlacement_)
		{
			body_->GetFixtureList()->SetSensor(false);
		}
	}
	UpdateGraphics(timeSinceLastFrame);
	if (badPlacement_)
	{
		body_->SetActive(false);
	}
		return true;
};