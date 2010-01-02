

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
	gameObject_ = new GameObject();
	gameObject_->SetName("PhysicsState");

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

	GameObject::objectList.clear();
	

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
	//b2Vec2 gravity(0.0f, -20);

	// Do we want to let bodies sleep?
	bool doSleep = true;

	// Construct a world object, which will hold and simulate the rigid bodies.
	//world = new b2World(worldAABB, gravity, doSleep);
	world = new b2World(gravity,doSleep);
	GameFramework::getSingletonPtr()->SetWorld(world);
	world->SetContactListener(this);

	new Platform(sceneManager_, b2Vec2(-300.0f, 0.0f), b2Vec2(10.0f, 0.0f));
	new Platform(sceneManager_, b2Vec2(10.0f, 0.0f),   b2Vec2(20.0f, 3.0f));
	new Platform(sceneManager_, b2Vec2(25.0f, 5.0f),   b2Vec2(30.0f, 5.0f));
	new Platform(sceneManager_, b2Vec2(30.0f, 6.4f),   b2Vec2(35.0f, 6.4f));
	new Platform(sceneManager_, b2Vec2(22.0f, 7.5f),   b2Vec2(27.0f, 7.5f));




	// Create myCharacter
	//myCharacter_ = new Character(sceneManager_);
	//myCharacter_->Initialize();
	
	parker_  = new CharacterParker(sceneManager_);
	parker_->Initialize();

	HoltBox* bb = new HoltBox(sceneManager_, b2Vec2(8.0f, 10.0f));
	bb->Initialize();

	new MovingPlatform(sceneManager_, b2Vec2(10.0f, 1.0f), b2Vec2(0.0f, 1.0f), b2Vec2(5.0f, 1.0f), b2Vec2(0.0f, 5.0f), 2);

	new PressureSwitch(sceneManager_);


#if DEBUG_DRAW_ON
	debugDraw_ = new OgreB2DebugDraw(sceneManager_, "debugDraw", 0);
	debugDraw_->SetFlags(
		b2DebugDraw::e_jointBit	
		| b2DebugDraw::e_shapeBit
		//| b2DebugDraw::e_aabbBit 
		);

	world->SetDebugDraw(debugDraw_);
	GAMEFRAMEWORK->SetDebugDraw(debugDraw_);
	debugDrawOn_ = true;

#endif

	//myPicking_ = new MousePicking(sceneManager_, world, camera_, pickingPlane);

}


void PhysicsState::CreateBox()
{
	HoltBox* bb = new HoltBox(sceneManager_, b2Vec2(-0.0f, 15.0f));

	if(!bb->CreateBox2DBox())
	{
		delete bb;
	}
}


void PhysicsState::createScene()
{

	GameFramework::getSingletonPtr()->viewport_->setBackgroundColour(Ogre::ColourValue(0,0,0));
	//sceneManager_->setSkyBox(true, "Examples/SpaceSkyBox");
	float mCurvature = 1;
	float mTiling = 15;
	//sceneManager_->setSkyDome(true, "Examples/CloudySky", mCurvature, mTiling);
	sceneManager_->createLight("Light")->setPosition(0,75,0);

	/*
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
	*/

	createPhysics();
	
}



bool PhysicsState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(keyEventRef.key == OIS::KC_P)
	{
		pause_ = true;
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
		}

		// Turn Debug Draw off
		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_Z))
		{
#if DEBUG_DRAW_ON
			debugDrawOn_ = false;
#endif
		}

		// Turn Debug Draw on
		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_X))
		{
#if DEBUG_DRAW_ON
			debugDrawOn_ = true;
#endif
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_D))
		{
			m_TranslateVector.x = m_MoveScale;
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
			Dispatch->DispatchMessageA(SEND_IMMEDIATELY, gameObject_->GetId(), parker_->GetId(), CHARACTER_MOVE_LEFT, NULL);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_RIGHT))
		{
			Dispatch->DispatchMessageA(SEND_IMMEDIATELY, gameObject_->GetId(), parker_->GetId(), CHARACTER_MOVE_RIGHT, NULL);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_UP))
		{
			Dispatch->DispatchMessageA(SEND_IMMEDIATELY, gameObject_->GetId(), parker_->GetId(), CHARACTER_JUMP, NULL);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_DOWN))
		{
			camera_->pitch(-m_RotScale);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_Y))
		{
			parker_->SetBodyPosition(b2Vec2(0, 10));
		}

		/*
		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_NUMPAD4))
		{
			Dispatch->DispatchMessageA(SEND_IMMEDIATELY, 0, myCharacter_->GetId(), CHARACTER_MOVE_LEFT, NULL);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_NUMPAD6))
		{
			Dispatch->DispatchMessageA(SEND_IMMEDIATELY, 0, myCharacter_->GetId(), CHARACTER_MOVE_RIGHT, NULL);
		}

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_NUMPAD8))
		{
			Dispatch->DispatchMessageA(SEND_IMMEDIATELY, 0, myCharacter_->GetId(), CHARACTER_JUMP, NULL);
		}
		*/
		
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

	time += timeSinceLastFrame;

	m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
	m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

	m_TranslateVector = Vector3::ZERO;
	
	while(time >= timeStep)
	{
		getInput();
		moveCamera();

		// Only compile the debug draw commands if DEBUG_DRAW_ON
#if DEBUG_DRAW_ON
		debugDraw_->clear();
#endif

		world->Step(timeStep,10,10);
		world->ClearForces();

#if DEBUG_DRAW_ON
		if(debugDrawOn_)
		{
			world->DrawDebugData();
		}
#endif

		time -= timeStep;

		// Process b2Contacts that happened in this world step.
		ProcessContacts();

		GameObject::UpdateObjectList(timeStep);

		camera_->setPosition(parker_->GetBodyPosition().x,parker_->GetBodyPosition().y + 7,camera_->getPosition().z);
		camera_->lookAt(parker_->GetBodyPosition().x,parker_->GetBodyPosition().y,0);
		
	}

	

}

void PhysicsState::setBufferedMode()
{
}

void PhysicsState::setUnbufferedMode()
{
}

/// Called when two fixtures begin to touch.
/// This calls the BeginContact method of an GameObjectOgreBox2D if the user data 
/// for one the touching fixtures is there. The called BeginContact method passes
/// which fixture was of the GameObjectOgreBox2D and which one it collided with.
void PhysicsState::BeginContact(b2Contact* contact)
{


	ContactPoint* c = new ContactPoint();
	c->fixtureA = contact->GetFixtureA();
	c->fixtureB = contact->GetFixtureB();
	c->contact = contact;
	beginContactList_.push_back(c);
}

/// Called when two fixtures cease to touch.
/// This calls the EndContact method of an GameObjectOgreBox2D if the user data 
/// for one the touching fixtures is there. The called EndContact method passes
/// which fixture was of the GameObjectOgreBox2D and which one it collided with.
void PhysicsState::EndContact(b2Contact* contact)
{
	
	ContactPoint* c = new ContactPoint();
	c->fixtureA = contact->GetFixtureA();
	c->fixtureB = contact->GetFixtureB();
	c->contact = contact;
	endContactList_.push_back(c);
}

void PhysicsState::ProcessContacts()
{

	ContactPoint* contact; 
	ContactList::iterator iter;

	// Process the EndContact list of b2Contacts
	for(iter = beginContactList_.begin(); iter != beginContactList_.end(); iter++)
	{
		contact = (*iter);

		// Check if fixtureA's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->fixtureA->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->fixtureA->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableA;

			contactableA = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableA)
			{
				contactableA->BeginContact(contact,contact->fixtureA,contact->fixtureB);
			}
		}

		// Check if fixtureB's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->fixtureB->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->fixtureB->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableB;

			contactableB = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableB)
			{
				contactableB->BeginContact(contact,contact->fixtureB,contact->fixtureA);
			}
		}
	}

	// Process the EndContact list of b2Contacts
	for(iter = endContactList_.begin(); iter != endContactList_.end(); iter++)
	{
		contact = (*iter);

		// Check if fixtureA's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->fixtureA->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->fixtureA->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableA;

			contactableA = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableA)
			{
				contactableA->EndContact(contact,contact->fixtureA,contact->fixtureB);
			}
		}

		// Check if fixtureA's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->fixtureB->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->fixtureB->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableB;

			contactableB = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableB)
			{
				contactableB->EndContact(contact,contact->fixtureB,contact->fixtureA);
			}
		}
	}

	endContactList_.clear();
	beginContactList_.clear();
}
