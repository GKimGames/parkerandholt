#include "Platform.h"

using namespace Ogre;

Platform::Platform(Ogre::SceneManager* sceneManager,b2Vec2 p1, b2Vec2 p2)
{
	gameObjectType_ = GOType_Platform;
	sceneManager_ = sceneManager;

	if(p1.x > p2.x)
	{
		b2Vec2 holder = p2;
		p2 = p1;
		p1 = holder;
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
	//material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	//material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	//material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	// Temporary

	Ogre::Entity* planeEnt = sceneManager_->createEntity(planeEntityName , planeMeshName);
	planeEnt->setMaterial(material);

	Ogre::SceneNode* planeNode = sceneManager_->getRootSceneNode()->createChildSceneNode();
	planeNode->attachObject(planeEnt);

	
	// Rotate the node to fit the points
	planeNode->roll(Ogre::Radian(atan2(p2.y - p1.y, p2.x - p1.x)));
	
	// Center the node on the midpoint of the two points
	planeNode->setPosition(Real(p1.x + p2.x) / 2.0, Real(p1.y + p2.y) / 2.0, 0.0);

	// Create body and fixture
	b2BodyDef bd;
	bd.position.Set(0.0f, 0.0f);
	body_= world_->CreateBody(&bd);

	b2EdgeDef ed;
	ed.vertex1 = p1;
	ed.vertex2 = p2;
	body_->CreateFixture(&ed);

}

Platform::~Platform()
{
	delete plane;
}

