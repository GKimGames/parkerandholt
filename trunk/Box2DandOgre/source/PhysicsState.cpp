

#include "PhysicsState.h"

using namespace Ogre;

class LedgeSensor;

//=============================================================================
//							Constructor
//
PhysicsState::PhysicsState()
{
	m_MoveSpeed			= 0.1;
	m_RotateSpeed		= 0.3;
	m_pCurrentObject	= 0;
}

//=============================================================================
//							enter
//
void PhysicsState::enter()
{
	gameObject_ = new GameObject("PhysicsState");

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

	sceneManager_ = GAMEFRAMEWORK->root_->createSceneManager(ST_GENERIC, "PhysicsSceneMgr");
	GAMEFRAMEWORK->sceneManager = sceneManager_;

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

	CompositorManager::getSingleton().addCompositor(GAMEFRAMEWORK->viewport_, "B&W");
	CompositorManager::getSingleton().setCompositorEnabled(GAMEFRAMEWORK->viewport_, "B&W",true);
}


//=============================================================================
//							pause
//
bool PhysicsState::pause()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Pausing PhysicsState...");

	this->pushAppState(findByName("InGameMenu"));

	return true;
}

//=============================================================================
//							resume
//
void PhysicsState::resume()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Resuming PhysicsState...");

	GameFramework::getSingletonPtr()->viewport_->setCamera(camera_);

	m_bQuit = false;
	pause_ = false;
}


//=============================================================================
//							exit
//
void PhysicsState::exit()
{

	CompositorManager::getSingleton().setCompositorEnabled(GAMEFRAMEWORK->viewport_, "B&W",false);

	CompositorManager::getSingleton().removeAll();

	delete world;

	GameFramework::getSingletonPtr()->log_->logMessage("Leaving PhysicsState...");

	sceneManager_->destroyCamera(camera_);

	GameObject::objectList.clear();
	
	myGUI->shutdown();
	delete myGUI;

	if(sceneManager_)
	{
		GameFramework::getSingletonPtr()->root_->destroySceneManager(sceneManager_);
	}

}


//=============================================================================
//							createPhysics
//
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

	// Create myCharacter
	//myCharacter_ = new Character(sceneManager_);
	//myCharacter_->Initialize();
	
	

	for(int i = 0; i < 3;i++)
	{
		//HoltBox* bb = new HoltBox(sceneManager_, b2Vec2(8.0f, 10.0f),1,50);
		//bb->Initialize();
	}

	myMouse_ = new MousePicking(sceneManager_, camera_);
	parker_  = new CharacterParker(sceneManager_, myMouse_);
	parker_->Initialize();
	parker_->InitializeDebugDraw();

	new MovingPlatform(sceneManager_, b2Vec2(10.0f, 1.0f), b2Vec2(0.0f, 1.0f), b2Vec2(5.0f, 1.0f), b2Vec2(0.0f, 5.0f), 2);
	PressureSwitch* ps = new PressureSwitch(sceneManager_);

	ps->AddToMessageList(parker_->GetId(), CREATE_BOX);

	myKeyHandler_ = new KeyHandler(GameFramework::getSingletonPtr()->keyboard_);

	myKeyHandler_->AddKey(OIS::KC_RIGHT, std::make_pair(CHARACTER_MOVE_RIGHT_PLUS, CHARACTER_MOVE_RIGHT_MINUS));
	myKeyHandler_->AddKey(OIS::KC_LEFT, std::make_pair(CHARACTER_MOVE_LEFT_PLUS, CHARACTER_MOVE_LEFT_MINUS));
	myKeyHandler_->AddKey(OIS::KC_UP, std::make_pair(CHARACTER_JUMP_PLUS, CHARACTER_JUMP_MINUS));
	myKeyHandler_->AddKey(OIS::KC_1, CHARACTER_ENTER_PLATFORMSTATE);
	myKeyHandler_->AddKey(OIS::KC_2, CHARACTER_ENTER_GRAVITYSTATE);
	myKeyHandler_->AddKey(OIS::KC_Q, CHARACTER_EXIT_PLACINGSTATE);
	
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


	b2BodyDef bdef;
	bdef.position.Set(0, -1000);
	b2PolygonShape bodyShapeDef2;
	bodyShapeDef2.SetAsBox(3.54/2.0, 0.6/2.0);

	b2Body* b2 = world->CreateBody(&bdef);
	b2->CreateFixture(&bodyShapeDef2);

	{
		b2BodyDef bd;
		bd.position.Set(22, 3.5);
		bd.type = b2_dynamicBody;
		
		b2PolygonShape bodyShapeDef;
		bodyShapeDef.SetAsBox(3.54/2.0, 0.6/2.0);

		b2FixtureDef fd;
		fd.shape = &bodyShapeDef;
		fd.density = 50;
		fd.friction = DEFAULT_FRICTION;
		fd.restitution = .2;

		b2Body* b = world->CreateBody(&bd);
		b->CreateFixture(&fd);

		Ogre::Entity* e = sceneManager_->createEntity("Hey Steve", "LeverArm.mesh");

		GameObjectOgreBox2D* goob = new GameObjectOgreBox2D("Meow meow meow", b,e);
		goob->GetSceneNode()->attachObject(e);
		goob->GetSceneNode()->scale(0.1,0.1,0.1);
		goob->Initialize();

		b2RevoluteJointDef jointDef;
		jointDef.Initialize(b2, b, b->GetWorldCenter());
		//jointDef.maxMotorTorque = 10.0f;
		//jointDef.motorSpeed = 0.0f;
		//jointDef.enableMotor = true;

		world->CreateJoint(&jointDef);
	}
	
	{
		b2BodyDef bd;
		bd.position.Set(-5, 3.5);
		bd.type = b2_dynamicBody;
		
		b2PolygonShape bodyShapeDef;
		bodyShapeDef.SetAsBox(3.54, 0.6/2.0);

		b2FixtureDef fd;
		fd.shape = &bodyShapeDef;
		fd.density = 50;
		fd.friction = DEFAULT_FRICTION;
		fd.restitution = .2;

		b2Body* b = world->CreateBody(&bd);
		b->CreateFixture(&fd);

		Ogre::Entity* e = sceneManager_->createEntity("Hey Steve2", "LeverArm.mesh");

		GameObjectOgreBox2D* goob = new GameObjectOgreBox2D("Meow meow meow2", b,e);
		goob->GetSceneNode()->attachObject(e);
		goob->GetSceneNode()->scale(0.2,0.1,0.1);
		goob->Initialize();

		b2RevoluteJointDef jointDef;
		jointDef.Initialize(b2, b, b->GetWorldCenter());
		//jointDef.maxMotorTorque = 10.0f;
		//jointDef.motorSpeed = 0.0f;
		//jointDef.enableMotor = true;

		world->CreateJoint(&jointDef);
	}



}

//=============================================================================
//							CreateBox
//
void PhysicsState::CreateBox()
{
	HoltBox* bb = new HoltBox(sceneManager_, b2Vec2(-0.0f, 15.0f));

	if(!bb->CreateBox2DBox())
	{
		delete bb;
	}
}

//=============================================================================
//							createScene
//
void PhysicsState::createScene()
{

	GameFramework::getSingletonPtr()->viewport_->setBackgroundColour(Ogre::ColourValue(0,0,0));
	//sceneManager_->setSkyBox(true, "Examples/SpaceSkyBox");
	float mCurvature = 1;
	float mTiling = 15;
	//sceneManager_->setSkyDome(true, "Examples/CloudySky", mCurvature, mTiling);
	Ogre::Light* light = sceneManager_->createLight("Light");
	
	light->setType(Light::LT_SPOTLIGHT);
	light->setPosition(0,25,0);
	light->setDirection(0,-1,0);
	light->setSpotlightRange(Degree(35), Degree(50));

	Ogre::Light* light2 = sceneManager_->createLight("Light2");
	
	light2->setType(Light::LT_SPOTLIGHT);
	light2->setPosition(0,25,50);
	light2->setDirection(0,-1,-1);
	light2->setSpotlightRange(Degree(35), Degree(50));

	Ogre::Light* light3 = sceneManager_->createLight("Light3");
	
	light3->setType(Light::LT_SPOTLIGHT);
	light3->setPosition(28,25,0);
	light3->setDirection(0,-1,0);
	light3->setSpotlightRange(Degree(35), Degree(50));

	sceneManager_->setAmbientLight(ColourValue(0.09, 0.09, 0.09));
	sceneManager_->setAmbientLight(ColourValue(0.7, 0.7, 0.7));
	sceneManager_->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	myGUI = new MyGUI::Gui();
	myGUI->initialise(GAMEFRAMEWORK->renderWindow_);
	
	MyGUI::ButtonPtr button = myGUI->createWidget<MyGUI::Button>("Button", 10, 10, 300, 26, MyGUI::Align::Default, "Main");
	button->setCaption("exit");

	//button->eventMouseButtonClick = MyGUI::newDelegate(this, &CLASS_NAME::METHOD_NAME);
	//button->eventMouseButtonClick = MyGUI::newDelegate(this, &PhysicsState::MakeExit);

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
	
	
	// Create background rectangle covering the whole screen
	Rectangle2D* rect = new Rectangle2D(true);
	rect->setCorners(-1.0, 1.0, 1.0, -1.0);
	rect->setMaterial("Matt/Skyline");

	// Render the background before everything else
	rect->setRenderQueueGroup(RENDER_QUEUE_BACKGROUND);

	// Use infinite AAB to always stay visible
	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	rect->setBoundingBox(aabInf);

	// Attach background to the scene
	SceneNode* node = sceneManager_->getRootSceneNode()->createChildSceneNode("BackgroundNode");
	node->attachObject(rect);
	

	// Example of background scrolling
	//===================================================================

	
	Ogre::MovablePlane* plane = new MovablePlane("MyPlane");
	plane->d = 0; 
	plane->normal = Vector3(0.0, 1.0, 0.0);

	Ogre::MeshManager::getSingleton().createPlane("HeySteve", 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*plane,
		1000,			// X Length
		1000,			// Z Length
		5, 5,			// Segments x ,y
		true,
		1, 
		1,				// Tile x
		1,				// Tile y
		Vector3::UNIT_Z);

	Ogre::Entity* planeEnt = sceneManager_->createEntity("butts" , "HeySteve");
	planeEnt->setMaterial(MaterialManager::getSingleton().getByName("Matt/Black"));

	Ogre::SceneNode* planeNode = sceneManager_->getRootSceneNode()->createChildSceneNode();
	planeNode->attachObject(planeEnt);
	
	planeNode->setPosition(0.0,-0.01,500.0);

	Ogre::SceneNode* buildingNode = sceneManager_->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* entity = sceneManager_->createEntity("ABuilding", Ogre::SceneManager::PT_CUBE);
	entity->setMaterialName("Matt/Black");
	buildingNode->attachObject(entity);
	buildingNode->setPosition(0.0,-0.01,0);

	createPhysics();
	
}


//=============================================================================
//							keyPressed
//
bool PhysicsState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	myKeyHandler_->KeyPressed(keyEventRef);
	if(keyEventRef.key == OIS::KC_P)
	{
		pause_ = true;
	}


	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}


	GameFramework::getSingletonPtr()->KeyPressed(keyEventRef);


	return true;
}

//=============================================================================
//							keyReleased
//
bool PhysicsState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	GameFramework::getSingletonPtr()->KeyReleased(keyEventRef);
	myKeyHandler_->KeyReleased(keyEventRef);

#if DEBUG_DRAW_ON
	if(keyEventRef.key == OIS::KC_C)
	{
		debugDraw_->SetFlags(debugDraw_->GetFlags() ^ b2DebugDraw::e_shapeBit);
	}
#endif 

	if(keyEventRef.key == OIS::KC_G)
	{
		parker_->SetDebugDrawOn();
	}

	if(keyEventRef.key == OIS::KC_H)
	{
		parker_->SetDebugDrawOff();
	}

	return true;
}


//=============================================================================
//							mouseMoved
//
bool PhysicsState::mouseMoved(const OIS::MouseEvent &evt)
{
	Dispatch->DispatchMessageA(SEND_IMMEDIATELY, 0, myMouse_->GetId(), UPDATE_MOUSE, &evt);
	myGUI->injectMouseMove(evt);
	//myPicking_->MouseMoved(evt);

	if(m_bRMouseDown)
	{
		camera_->yaw(Degree(evt.state.X.rel * -0.1));
		camera_->pitch(Degree(evt.state.Y.rel * -0.1));
	}

	return true;
}

//=============================================================================
//							mousePressed
//
bool PhysicsState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	myGUI->injectMousePress(evt, id);

	//myPicking_->MousePressed(evt, id);

	if(id == OIS::MB_Left)
	{
		Dispatch->DispatchMessageA(SEND_IMMEDIATELY, 0, SEND_TO_ALL, LEFT_MOUSE_PLUS, NULL);
		m_bLMouseDown = true;
	} 
	else if(id == OIS::MB_Right)
	{
		m_bRMouseDown = true;
	} 
	else if(id == OIS::MB_Middle)
	{
		Dispatch->DispatchMessageA(SEND_IMMEDIATELY, 0, SEND_TO_ALL, CREATE_BOX, NULL);
	}

	return true;
}


//=============================================================================
//							mouseReleased
//
bool PhysicsState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	//myPicking_->MouseReleased(evt, id);
	
	myGUI->injectMouseRelease(evt, id);
	if(id == OIS::MB_Left)
	{
		Dispatch->DispatchMessageA(SEND_IMMEDIATELY, 0, SEND_TO_ALL, LEFT_MOUSE_MINUS, NULL);
		m_bLMouseDown = false;
	} 
	else if(id == OIS::MB_Right)
	{
		m_bRMouseDown = false;
	}

	return true;
}



//=============================================================================
//							moveCamera
//
void PhysicsState::moveCamera()
{
	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_LSHIFT)) 
		camera_->moveRelative(m_TranslateVector);

	camera_->moveRelative(m_TranslateVector / 10);

	
	Dispatch->DispatchMessageA(SEND_IMMEDIATELY, 0, myMouse_->GetId(), UPDATE_MOUSE, boost::any());
}


//=============================================================================
//							getInput
//
void PhysicsState::getInput()
{
	if(m_bChatMode == false)
	{

		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_A))
		{
			m_TranslateVector.x = -m_MoveScale;
		}
#if DEBUG_DRAW_ON
		// Turn Debug Draw off
		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_Z))
		{
			debugDrawOn_ = false;
		}

		// Turn Debug Draw on
		if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_X))
		{
			debugDrawOn_ = true;
		}
#endif

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

//=============================================================================
//							update
//
/// Main Update looped for a level
bool PhysicsState::update(double timeSinceLastFrame)
{
	static double time = 0;

	if(m_bQuit == true)
	{
		this->popAppState();
		return false;
	}

	if(pause_ == true)
	{
		pause();
		return false;
	}

	time += timeSinceLastFrame;

	// Update the variables that control how the camera moves
	m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
	m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

	m_TranslateVector = Vector3::ZERO;
	
	while(time >= timeStep)
	{
		// Get Input from keyboard, the keys that are pressed down during this update
		getInput();

		// Move the camera based upon input
		moveCamera();

		// Update the physical Box2D world
		world->Step(timeStep,10,10);
		world->ClearForces();

#if DEBUG_DRAW_ON
		debugDraw_->clear();
		if(debugDrawOn_)
		{	
			world->DrawDebugData();
		}
#endif

		time -= timeStep;

		// Process b2Contacts that happened in this world step.
		ProcessContacts();

		// Update all the game objects.
		GameObject::UpdateObjectList(timeStep);

		// Update the camera to look at Parker.
		camera_->setPosition(parker_->GetBodyPosition().x,parker_->GetBodyPosition().y + 3,camera_->getPosition().z);
		camera_->lookAt(parker_->GetBodyPosition().x,parker_->GetBodyPosition().y,0);
		
	}

	return true;

}

void PhysicsState::setBufferedMode()
{
}

void PhysicsState::setUnbufferedMode()
{
}

//=============================================================================
//							BeginContact
//
/// Called when two fixtures begin to touch.
/// This calls the BeginContact method of an GameObjectOgreBox2D if the user data 
/// for one the touching fixtures is there. The called BeginContact method passes
/// which fixture was of the GameObjectOgreBox2D and which one it collided with.
void PhysicsState::BeginContact(b2Contact* contact)
{


	ContactPoint* c = new ContactPoint();
	c->fixtureA = contact->GetFixtureA();
	c->fixtureB = contact->GetFixtureB();

	// Should probably be making a deep copy of contact.
	c->contact = contact;
	beginContactList_.push_back(c);
}

//=============================================================================
//							EndContact
//
/// Called when two fixtures cease to touch.
/// This calls the EndContact method of an GameObjectOgreBox2D if the user data 
/// for one the touching fixtures is there. The called EndContact method passes
/// which fixture was of the GameObjectOgreBox2D and which one it collided with.
void PhysicsState::EndContact(b2Contact* contact)
{
	
	ContactPoint* c = new ContactPoint();
	c->fixtureA = contact->GetFixtureA();
	c->fixtureB = contact->GetFixtureB();

	// Should probably be making a deep copy of contact.
	c->contact = contact;
	endContactList_.push_back(c);
}

//=============================================================================
//							ProcessContacts
//
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


//=============================================================================
//							SayGoodbye
//
/// Called when any joint is about to be destroyed due
/// to the destruction of one of its attached bodies.
void PhysicsState::SayGoodbye(b2Joint* joint)
{
}


//=============================================================================
//							SayGoodbye
//
/// Called when any fixture is about to be destroyed due
/// to the destruction of its parent body.
void PhysicsState::SayGoodbye(b2Fixture* fixture)
{
}