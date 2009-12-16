

#include "PhysicsState.h"



using namespace Ogre;

class LedgeSensor;
PhysicsState::PhysicsState()
{
	m_MoveSpeed			= 0.1;
	m_RotateSpeed		= 0.3;
	m_pCurrentObject	= 0;
}


void PhysicsState::enter()
{

	GameFramework::getSingletonPtr()->log_->logMessage("Entering PhysicsState...");

	// Grabbing settings for the world and camera from a cfg file (ini)
	std::string settingsFile = "../Settings.cfg";
	TCHAR buffer[256];

	GetPrivateProfileStringA("Settings", "CameraX", "0", buffer, 256, settingsFile.c_str());
	camPosition.x = atof(buffer);

	GetPrivateProfileStringA("Settings", "CameraY", "5", buffer, 256, settingsFile.c_str());
	camPosition.y = atof(buffer);

	GetPrivateProfileStringA("Settings", "CameraZ", "20", buffer, 256, settingsFile.c_str());
	camPosition.z = atof(buffer);

	GetPrivateProfileStringA("Settings", "LookX", "0", buffer, 256, settingsFile.c_str());
	camLook.x = atof(buffer);

	GetPrivateProfileStringA("Settings", "LookY", "5", buffer, 256, settingsFile.c_str());
	camLook.y = atof(buffer);

	GetPrivateProfileStringA("Settings", "LookZ", "0", buffer, 256, settingsFile.c_str());
	camLook.z = atof(buffer);

	GetPrivateProfileStringA("Settings", "Gravity", "10", buffer, 256, settingsFile.c_str());
	gravity_ = atof(buffer);

	GetPrivateProfileStringA("Settings", "TimeStep", "0.016666666666667", buffer, 256, settingsFile.c_str());
	timeStep = atof(buffer);

	sceneManager_ = GameFramework::getSingletonPtr()->root_->createSceneManager(ST_GENERIC, "GameSceneMgr");
	sceneManager_->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));		

	camera_ = sceneManager_->createCamera("GameCamera");
	camera_->setPosition(Vector3(camPosition.x, camPosition.y, camPosition.z));
	camera_->lookAt(Vector3(camLook.x, camLook.y, camLook.z));
	camera_->setNearClipDistance(5);

	camera_->setAspectRatio(Real(GameFramework::getSingletonPtr()->viewport_->getActualWidth()) / 
		Real(GameFramework::getSingletonPtr()->viewport_->getActualHeight()));

	GameFramework::getSingletonPtr()->viewport_->setCamera(camera_);

	GameFramework::getSingletonPtr()->keyboard_->setEventCallback(this);
	GameFramework::getSingletonPtr()->mouse_->setEventCallback(this);

	m_bLMouseDown = m_bRMouseDown = false;
	m_bQuit = false;
	m_bChatMode = false;

	setUnbufferedMode();

	createScene();

}



bool PhysicsState::pause()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Pausing PhysicsState...");

	this->pushAppState(findByName("InGameMenu"));

	return true;
}


void PhysicsState::resume()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Resuming PhysicsState...");

	GameFramework::getSingletonPtr()->viewport_->setCamera(camera_);

	m_bQuit = false;
	pause_ = false;
}



void PhysicsState::exit()
{

	delete world;

	GameFramework::getSingletonPtr()->log_->logMessage("Leaving PhysicsState...");

	sceneManager_->destroyCamera(camera_);

	PaH::Entity::GetEntityList().clear();
	

	if(sceneManager_)
	{
		GameFramework::getSingletonPtr()->root_->destroySceneManager(sceneManager_);
	}

}



void PhysicsState::createPhysics()
{
	// Define the size of the world. Simulation will still work
	// if bodies reach the end of the world, but it will be slower.
	b2AABB worldAABB;
	worldAABB.lowerBound.Set(-500.0f, -500.0f);
	worldAABB.upperBound.Set(500.0f, 500.0f);

	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -gravity_);

	// Do we want to let bodies sleep?
	bool doSleep = true;

	// Construct a world object, which will hold and simulate the rigid bodies.
	world = new b2World(worldAABB, gravity, doSleep);
	GameFramework::getSingletonPtr()->SetWorld(world);
	world->SetContactListener(this);

	new Platform(sceneManager_, b2Vec2(-300.0f, 0.0f), b2Vec2(10.0f, 0.0f));
	new Platform(sceneManager_, b2Vec2(10.0f, 0.0f),   b2Vec2(20.0f, 3.0f));
	new Platform(sceneManager_, b2Vec2(25.0f, 5.0f),   b2Vec2(30.0f, 5.0f));
	new Platform(sceneManager_, b2Vec2(30.0f, 6.4f),   b2Vec2(35.0f, 6.4f));
	new Platform(sceneManager_, b2Vec2(22.0f, 7.5f),   b2Vec2(27.0f, 7.5f));


	// Create myCharacter
	myCharacter_ = new Character(sceneManager_, world);

	HoltBox* bb = new HoltBox(sceneManager_, b2Vec2(-5.0f, 10.0f));
	bb->Initialize();
	//bb->GetBody()->SetTimeScale(0.5f);

	LedgeSensor* ls = new LedgeSensor(b2Vec2(30.0f, 6.4f));

	ls->OnContact(boost::bind(&Character::Jump, myCharacter_));

	LedgeSensor* ls2 = new LedgeSensor(b2Vec2(-5.0f, 1.0f));

	ls2->OnContact(boost::bind(&PhysicsState::CreateBox, this));

	Dispatch->DispatchMessage(3000,0,myCharacter_->GetId(),1,NULL);

#if DEBUG_DRAW_ON
	debugDraw_ = new OgreB2DebugDraw(sceneManager_, "debugDraw", 0);
	debugDraw_->SetFlags(
		b2DebugDraw::e_obbBit
		| b2DebugDraw::e_shapeBit
		| b2DebugDraw::e_aabbBit 
		);

	world->SetDebugDraw(debugDraw_);
#endif

	//myPicking_ = new MousePicking(sceneManager_, world, camera_, pickingPlane);

}


void PhysicsState::CreateBox()
{
	HoltBox* bb = new HoltBox(sceneManager_, b2Vec2(-0.0f, 15.0f));
	bb->Initialize();
}


void PhysicsState::createScene()
{

	GameFramework::getSingletonPtr()->viewport_->setBackgroundColour(Ogre::ColourValue(0,0,0));
	//sceneManager_->setSkyBox(true, "Examples/SpaceSkyBox");
	float mCurvature = 1;
	float mTiling = 15;
	//sceneManager_->setSkyDome(true, "Examples/CloudySky", mCurvature, mTiling);
	sceneManager_->createLight("Light")->setPosition(75,75,75);

	
	//=============================================================
	// Create background
	// Create background material
	MaterialPtr material = MaterialManager::getSingleton().create("Background", "General");
	material->getTechnique(0)->getPass(0)->createTextureUnitState("rockwall.tga");
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	material->getTechnique(0)->getPass(0)->setLightingEnabled(false);

	// Create background rectangle covering the whole screen
	Rectangle2D* rect = new Rectangle2D(true);
	rect->setCorners(-1.0, 1.0, 1.0, -1.0);

	rect->setMaterial("Background");

	// Render the background before everything else
	rect->setRenderQueueGroup(RENDER_QUEUE_BACKGROUND);

	// Use infinite AAB to always stay visible
	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	rect->setBoundingBox(aabInf);

	// Attach background to the scene
	SceneNode* node = sceneManager_->getRootSceneNode()->createChildSceneNode("Background");
	node->attachObject(rect);

	// Example of background scrolling
	//material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setScrollAnimation(-0.25, 0.0);

	// Create background
	//============================================================
	

	createPhysics();

}



bool PhysicsState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(keyEventRef.key == OIS::KeyCode::KC_P)
	{
		pause_ = true;
	}

	if(m_bChatMode == true)
	{

	}
	else
	{
		myCharacter_->KeyPressed(keyEventRef);
	}

	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}

	if(!m_bChatMode || (m_bChatMode && !GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_O)))
		GameFramework::getSingletonPtr()->KeyPressed(keyEventRef);


	return true;
}


bool PhysicsState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	GameFramework::getSingletonPtr()->KeyReleased(keyEventRef);

	return true;
}



bool PhysicsState::mouseMoved(const OIS::MouseEvent &evt)
{

	//myPicking_->MouseMoved(evt);

	if(m_bRMouseDown)
	{
		camera_->yaw(Degree(evt.state.X.rel * -0.1));
		camera_->pitch(Degree(evt.state.Y.rel * -0.1));
	}

	return true;
}


bool PhysicsState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	//myPicking_->MousePressed(evt, id);

	if(id == OIS::MB_Left)
	{
		m_bLMouseDown = true;
	} 
	else if(id == OIS::MB_Right)
	{
		m_bRMouseDown = true;
	} 

	return true;
}



bool PhysicsState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	//myPicking_->MouseReleased(evt, id);

	if(id == OIS::MB_Left)
	{
		m_bLMouseDown = false;
	} 
	else if(id == OIS::MB_Right)
	{
		m_bRMouseDown = false;
	}

	return true;
}






void PhysicsState::moveCamera()
{
	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_LSHIFT)) 
		camera_->moveRelative(m_TranslateVector);

	camera_->moveRelative(m_TranslateVector / 10);
}



void PhysicsState::getInput()
{
	if(m_bChatMode == false)
	{

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_A))
		{
			m_TranslateVector.x = -m_MoveScale;
			world->SetDebugDraw(NULL);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_D))
		{
			m_TranslateVector.x = m_MoveScale;
			world->SetDebugDraw(debugDraw_);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_W))
		{
			m_TranslateVector.z = -m_MoveScale;
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_S))
		{
			m_TranslateVector.z = m_MoveScale;
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_LEFT))
		{
			camera_->yaw(m_RotScale);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_RIGHT))
		{
			camera_->yaw(-m_RotScale);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_UP))
		{
			camera_->pitch(m_RotScale);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_DOWN))
		{
			camera_->pitch(-m_RotScale);
		}


	}
}


/// Main Update looped for a level
void PhysicsState::update(double timeSinceLastFrame)
{
	static double time = 0;

	if(m_bQuit == true)
	{
		this->popAppState();
		return;
	}

	if(pause_ == true)
	{
		pause();
		return;
	}

	double dt = timeSinceLastFrame / 1000;

	time += dt;

	if(time > 0.2)
	{
		time = timeStep;
	}

	m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
	m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

	m_TranslateVector = Vector3::ZERO;

	getInput();
	moveCamera();

	while(time >= timeStep)
	{
		// Only compile the debug draw commands if DEBUG_DRAW_ON
#if DEBUG_DRAW_ON
		debugDraw_->clear();
#endif
		myCharacter_->GetInput(timeStep);
		//myCharacter_->Update(timeStep);	

		world->Step(timeStep,10,9);
		time -= timeStep;
		//b2Vec2 charPos = myCharacter_->GetBodyPosition();
		//camera_->setPosition(charPos.x,  camera_->getPosition().y, camera_->getPosition().z);
		//camera_->lookAt(charPos.x,charPos.y, 0);

		

	}

	PaH::Entity::UpdateEntityList(dt);

}

void PhysicsState::setBufferedMode()
{
}

void PhysicsState::setUnbufferedMode()
{
}

/// Called when two fixtures cease to touch.
/// This calls the BeginContact method of an EntityBox2DResponder if the user data 
/// for one the touching fixtures is there. The called BeginContact method passes
/// which fixture was of the EntityBox2DResponder and which one it collided with.
void PhysicsState::BeginContact(b2Contact* contact)
{
	// Check if fixtureA's body has some user data
	// If it does check if the Entity respondes to contacts
	if(contact->GetFixtureA()->GetBody()->GetUserData() != NULL)
	{
		PaH::Entity* entity = (PaH::Entity*) contact->GetFixtureA()->GetBody()->GetUserData();
		PaH::EntityBox2DResponder* contactableA;
		
		contactableA = dynamic_cast<PaH::EntityBox2DResponder*> (entity);

		if(contactableA)
		{
			contactableA->BeginContact(contact,contact->GetFixtureA(),contact->GetFixtureB());
		}
	}
	
	// Check if fixtureB's body has some user data
	// If it does check if the Entity respondes to contacts
	if(contact->GetFixtureB()->GetBody()->GetUserData() != NULL)
	{
	    PaH::Entity* entity = (PaH::Entity*) contact->GetFixtureB()->GetBody()->GetUserData();
		PaH::EntityBox2DResponder* contactableB;

		contactableB = dynamic_cast<PaH::EntityBox2DResponder*> (entity);

		if(contactableB)
		{
			contactableB->BeginContact(contact,contact->GetFixtureB(),contact->GetFixtureA());
		}
	}
}

/// Called when two fixtures cease to touch.
/// This calls the EndContact method of an EntityBox2DResponder if the user data 
/// for one the touching fixtures is there. The called EndContact method passes
/// which fixture was of the EntityBox2DResponder and which one it collided with.
void PhysicsState::EndContact(b2Contact* contact)
{

	// Check if fixtureA's body has some user data
	// If it does check if the Entity respondes to contacts
	if(contact->GetFixtureA()->GetBody()->GetUserData() != NULL)
	{
	    PaH::Entity* entity = (PaH::Entity*) contact->GetFixtureA()->GetBody()->GetUserData();
		PaH::EntityBox2DResponder* contactable;

		contactable = dynamic_cast<PaH::EntityBox2DResponder*> (entity);

		if(contactable)
		{
			contactable->EndContact(contact,contact->GetFixtureA(),contact->GetFixtureB());
		}
	}

	// Check if fixtureA's body has some user data
	// If it does check if the Entity respondes to contacts
	if(contact->GetFixtureB()->GetBody()->GetUserData() != NULL)
	{
		PaH::Entity* entity = (PaH::Entity*) contact->GetFixtureB()->GetBody()->GetUserData();
		PaH::EntityBox2DResponder* contactable;

		contactable = dynamic_cast<PaH::EntityBox2DResponder*> (entity);

		if(contactable)
		{
			contactable->EndContact(contact,contact->GetFixtureB(),contact->GetFixtureA());
		}
	}

}
