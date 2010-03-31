/*=============================================================================

		PhysicsState.cpp

		Author: Matt King

=============================================================================*/

#include "PhysicsState.h"
#include "GameObjectFactory.h"
#include "TinyXMLHelper.h"
#include "CameraStateWatch.h"
#include "CameraStateGoToPoint.h"
#include "Teleporter.h"
#include "Parker.h"
#include "Holt.h"

#include "FadingPlatform.h"

#include "KGBTimer.h"
using namespace Ogre;

class LedgeSensor;

//=============================================================================
//							Constructor
//
PhysicsState::PhysicsState(Ogre::String levelName)
{
	levelName_ = levelName;
	m_MoveSpeed			= 0.1;
	m_RotateSpeed		= 0.3;
	m_pCurrentObject	= 0;
}

//=============================================================================
//							enter
//
void PhysicsState::enter()
{

	gameObjectList_ = new GameObjectMap();
	gameObjectNameList_ = new GameObjectNameMap();
	GameObject::objectList = gameObjectList_;
	GameObject::objectNameList = gameObjectNameList_;

	testing_ = .5;

	gameObject_ = new GameObject("PhysicsState");

	GameFramework::getSingletonPtr()->log_->logMessage("Entering PhysicsState...");
	
	TiXmlDocument* settingsDoc = 0;
	TiXmlHandle* root = TinyXMLHelper::GetRootFromFile("../Settings.xml", settingsDoc);
	
	TiXmlHandle cameraHandle = root->FirstChildElement("Camera");
	camPosition = TinyXMLHelper::GetAttributeVector3(cameraHandle.ToElement(), "position");
	Ogre::Vector3 camLook = TinyXMLHelper::GetAttributeVector3(cameraHandle.ToElement(), "lookAt");

	TiXmlHandle box2DHandle = root->FirstChildElement("Box2D");
	gravity_ = TinyXMLHelper::GetAttributeb2Vec2(box2DHandle.FirstChildElement("Gravity").ToElement(), "direction");
	timeStep = TinyXMLHelper::GetAttributeFloat(box2DHandle.FirstChildElement("Timestep").ToElement(),  "timestep");
	
	delete root;
	delete settingsDoc;

	sceneManager_ = GAMEFRAMEWORK->root_->createSceneManager(ST_GENERIC, "PhysicsSceneMgr" + levelName_);
	GAMEFRAMEWORK->sceneManager = sceneManager_;

	//myGUI = new MyGUI::Gui();
	//myGUI->initialise(GAMEFRAMEWORK->renderWindow_);
	//myGUI->setSceneManager(sceneManager_);
	//MyGUI::LayoutManager::getInstance().load("Console.layout");
	/*
	MyGUI::ButtonPtr button = myGUI->createWidget<MyGUI::Button>("Button", 10, 10, 300, 26, MyGUI::Align::Default, "Main");
	button->setCaption("exit");
	*/

	sceneManager_->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));		

	camera_ = sceneManager_->createCamera("PhysicsGameCamera" + levelName_);
	camera_->setPosition(camPosition);
	camera_->lookAt(camLook);
	camera_->setNearClipDistance(1);
	
	camera_->setAspectRatio(Real(GAMEFRAMEWORK->viewport_->getActualWidth()) / 
		Real(GAMEFRAMEWORK->viewport_->getActualHeight()));

	GAMEFRAMEWORK->camera_ = camera_;

	GAMEFRAMEWORK->viewport_->setCamera(camera_);

	GAMEFRAMEWORK->keyboard_->setEventCallback(this);
	GAMEFRAMEWORK->mouse_->setEventCallback(this);

	m_bLMouseDown = m_bRMouseDown = false;
	m_bQuit = false;
	m_bChatMode = false;

	setUnbufferedMode();

	createScene();

	CompositorManager::getSingleton().addCompositor(GAMEFRAMEWORK->viewport_, "B&W");
	//CompositorManager::getSingleton().setCompositorEnabled(GAMEFRAMEWORK->viewport_, "B&W",true);

	time_ = minutes_ = seconds_ = 0;

	// Drawing the debug overlay, it shows FPS stats.
	timeLeftOverlay_ = OverlayManager::getSingleton().getByName("PaH/Timer");
	timeLeftOverlay_->show();

	pickupOverlay_ = OverlayManager::getSingleton().getByName("PaH/Pickups");
	pickupOverlay_->show();
	
}


//=============================================================================
//						UpdateOverlay
//
void PhysicsState::UpdateOverlay()
{

	time_ += GAMEFRAMEWORK->GetTimeSinceLastFrame();

	minutes_ = time_ / 60;
	seconds_ = (int)(time_) % 60;

	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("PaH/Time"); 
	Ogre::String strSeconds;
	Ogre::String strMinutes;

	if(minutes_ < 10)
	{
		strMinutes +="0" + StringConverter::toString(minutes_);
	}
	else
	{
		strMinutes += StringConverter::toString(minutes_);
	}

	if(seconds_ < 10)
	{
		strSeconds +="0" + StringConverter::toString(seconds_);
	}
	else
	{
		strSeconds += StringConverter::toString(seconds_);
	}

	guiCurr->setCaption(strMinutes+":"+strSeconds); 
	guiCurr->setColour(Ogre::ColourValue(seconds_ / 60.0,1,1,1));
}

//=============================================================================
//							pause
//
bool PhysicsState::pause()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Pausing PhysicsState...");

	//this->pushAppState(findByName("InGameMenu"));

	return true;
}

//=============================================================================
//							resume
//
void PhysicsState::resume()
{
	
	GameFramework::getSingletonPtr()->log_->logMessage("Resuming PhysicsState...");
	GameFramework::getSingletonPtr()->viewport_->setCamera(camera_);
	GAMEFRAMEWORK->sceneManager = sceneManager_;
	GAMEFRAMEWORK->world_ = world;
	GAMEFRAMEWORK->gameCamera_ = gameCamera_;

	AppState::resume();

	m_bQuit = false;
	pause_ = false;
}


//=============================================================================
//							exit
//
void PhysicsState::exit()
{

	CompositorManager::getSingleton().setCompositorEnabled(GAMEFRAMEWORK->viewport_, "B&W",false);
	CompositorManager::getSingleton().removeCompositorChain(GAMEFRAMEWORK->viewport_);

	GameFramework::getSingletonPtr()->log_->logMessage("Leaving PhysicsState...");

	sceneManager_->destroyCamera(camera_);
	
	//myGUI->shutdown();
	//delete myGUI;

	this->deleteObjects();

	delete world;

	if(sceneManager_)
	{
		GameFramework::getSingletonPtr()->root_->destroySceneManager(sceneManager_);
	}

	timeLeftOverlay_->hide();
	pickupOverlay_->hide();

	endContactList_.clear();
	beginContactList_.clear();

}


//=============================================================================
//							createPhysics
//
void PhysicsState::createPhysics()
{
	// Define the gravity vector.
	b2Vec2 gravity(gravity_);

	// Do we want to let bodies sleep?
	bool doSleep = true;

	// Construct a world object, which will hold and simulate the rigid bodies.
	world = new b2World(gravity,doSleep);
	GameFramework::getSingletonPtr()->SetWorld(world);
	world->SetContactListener(this);


	GameObjectFactory* gof = new GameObjectFactory();
	gof->AddObjectCreators();
	gof->sceneManager = sceneManager_;
	gof->LoadFile(levelName_);

	TiXmlDocument* configXML_ = 0;
	TiXmlHandle* handle = TinyXMLHelper::GetRootFromFile("..\\LevelTwo.xml",configXML_);
	
	TiXmlElement* element = handle->FirstChildElement("LevelInfo").ToElement();
	
	delete handle;
	delete configXML_;
	
	holtInfo_ = new PlayerInfo();
	parkerInfo_ = new PlayerInfo();

	myMouse_ = new MousePicking(sceneManager_, camera_);

	parker_  = new CharacterParker(sceneManager_, myMouse_);
	parker_->Initialize();
	parker_->InitializeDebugDraw();
	parker_->SetBodyPosition(b2Vec2(0, 1.3));


	active_ = parker_;

	holt_ = new CharacterHolt(sceneManager_, myMouse_);
	holt_->Initialize();
	holt_->GetBody()->SetTransform(b2Vec2(4, 1), 0);

	gameCamera_ = new GameCamera(camera_);
	GAMEFRAMEWORK->gameCamera_ = gameCamera_;
	
	CameraStateWatchDef def;
	def.initialPosition = camPosition;
	def.targetObject = parker_;

	gameCamera_->InitializeDef(&def);

	Dispatch->DispatchMessage(SEND_IMMEDIATELY, parker_->GetId(), parker_->GetId(), SET_POSITION, Ogre::Vector3(3,1,0));


	//new MovingPlatform(sceneManager_, b2Vec2(10.0f, 1.0f), b2Vec2(0.0f, 1.0f), b2Vec2(5.0f, 1.0f), b2Vec2(0.0f, 5.0f), 2);
	PressureSwitch* ps = new PressureSwitch(sceneManager_);

	ps->AddToMessageList(parker_->GetId(), CREATE_BOX);

	myKeyHandler_ = new KeyHandler(GameFramework::getSingletonPtr()->keyboard_);

	myKeyHandler_->AddKey(OIS::KC_RIGHT, CHARACTER_MOVE_RIGHT_PLUS, CHARACTER_MOVE_RIGHT_MINUS);
	myKeyHandler_->AddKey(OIS::KC_LEFT, CHARACTER_MOVE_LEFT_PLUS, CHARACTER_MOVE_LEFT_MINUS);
	myKeyHandler_->AddKey(OIS::KC_UP, CHARACTER_JUMP_PLUS, CHARACTER_JUMP_MINUS);
	myKeyHandler_->AddKey(OIS::KC_DOWN, CHARACTER_MOVE_DOWN_PLUS, CHARACTER_MOVE_DOWN_MINUS);
	myKeyHandler_->AddKey(OIS::KC_D, CHARACTER_MOVE_RIGHT_PLUS, CHARACTER_MOVE_RIGHT_MINUS);
	myKeyHandler_->AddKey(OIS::KC_A, CHARACTER_MOVE_LEFT_PLUS, CHARACTER_MOVE_LEFT_MINUS);
	myKeyHandler_->AddKey(OIS::KC_W, CHARACTER_JUMP_PLUS, CHARACTER_JUMP_MINUS);
	myKeyHandler_->AddKey(OIS::KC_S, CHARACTER_MOVE_DOWN_PLUS, CHARACTER_MOVE_DOWN_MINUS);
	myKeyHandler_->AddKey(OIS::KC_1, CHARACTER_ENTER_PLATFORMSTATE);
	myKeyHandler_->AddKey(OIS::KC_2, CHARACTER_ENTER_GRAVITYSTATE);
	myKeyHandler_->AddKey(OIS::KC_Q, CHARACTER_EXIT_PLACINGSTATE);
	myKeyHandler_->AddKey(OIS::KC_F, CHARACTER_GRAB_LEDGE);
	myKeyHandler_->AddKey(OIS::KC_RETURN, CHARACTER_OPEN_DOOR);
	
	//Teleporter* t1 = new Teleporter("Steeeeeeve");
	//Teleporter* t2 = new Teleporter("Steeeeve");

	//t1->SetPartner(t2);
	//t2->SetPartner(t1);

	
	b2BodyDef bdef;
	bdef.position.Set(12, -1);
	bdef.angle = 3.14159/2.0;

	b2PolygonShape teleShape;
	teleShape.SetAsBox(1.1, 0.15, b2Vec2(0.125, 0), (3.14159 / 2.0));

	b2PolygonShape squareBack;
	squareBack.SetAsBox(1.3, 0.1, b2Vec2(-0.1, 0), (3.14159 / 2.0));
	
	b2FixtureDef fdBack;
	fdBack.shape = &squareBack;

	b2FixtureDef fd;
	fd.shape = &teleShape;
	fd.isSensor = true;

	//b2Body* b = world->CreateBody(&bdef);
	//b->SetUserData(t1);
	//b->CreateFixture(&fd);
	//b->CreateFixture(&fdBack);
	//t1->SetBody(b);

	//bdef.position.Set(-4, 4);
	//bdef.angle = 3.14159 / 4.0;
	//b = world->CreateBody(&bdef);
	//b->SetUserData(t2);
	//b->CreateFixture(&fd);
	//b->CreateFixture(&fdBack);
	//t2->SetBody(b);

	/*
	LedgeSensor* ls = new LedgeSensor();
	ls->SetBodyPosition(b2Vec2(0,5));
	*/

#if DEBUG_DRAW_ON
	debugDraw_ = new OgreB2DebugDraw(sceneManager_, "debugDraw", 0);
	debugDraw_->SetFlags(
		b2DebugDraw::e_jointBit	
		| b2DebugDraw::e_shapeBit
		//| b2DebugDraw::e_aabbBit 
		);

	world->SetDebugDraw(debugDraw_);
	GAMEFRAMEWORK->SetDebugDraw(debugDraw_);
	debugDrawOn_ = false;

#endif

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

	sceneManager_->getRootSceneNode()->setPosition(0,0,0);
	GAMEFRAMEWORK->viewport_->setBackgroundColour(Ogre::ColourValue(0.231, 0.631, 1.0));
	//sceneManager_->setSkyBox(true, "Examples/SpaceSkyBox");
	//sceneManager_->setSkyBox(true, "Examples/WhiteSkyBox", 10000);
	float mCurvature = 45;
	float mTiling = 5;
	//sceneManager_->setSkyDome(true, "Examples/WhiteSkyBox", mCurvature, mTiling);
	//sceneManager_->setSkyDome(true, "Examples/CloudySky", 20, 5);

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
	//sceneManager_->setAmbientLight(ColourValue(0.7, 0.7, 0.7));
	sceneManager_->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);

	//button->eventMouseButtonClick = MyGUI::newDelegate(this, &CLASS_NAME::METHOD_NAME);
	//button->eventMouseButtonClick = MyGUI::newDelegate(this, &PhysicsState::MakeExit);

	createPhysics();
}


//=============================================================================
//							keyPressed
//
bool PhysicsState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	static bool myFlip = true;
	myKeyHandler_->KeyPressed(keyEventRef);

	if(keyEventRef.key == OIS::KC_9)
	{
		if(active_ != parker_)
		{
			myMouse_->SetVisibility(false);
			active_ = parker_;
			holt_->SetActive(false);
			parker_->SetActive(true);
			CameraStateWatchDef def;
			def.initialPosition = camPosition;
			def.targetObject = parker_;
			gameCamera_->InitializeDef(&def);
		}
	}

	if(keyEventRef.key == OIS::KC_0)
	{
		if(active_ != holt_)
		{
			active_ = holt_;
			parker_->SetActive(false);
			holt_->SetActive(true);
			CameraStateWatchDef def;
			def.initialPosition = camPosition;
			def.targetObject = holt_;
			gameCamera_->InitializeDef(&def);
		}
	}


	if(keyEventRef.key == OIS::KC_P)
	{
		//pause_ = true;
	}

	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}
	
	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_LBRACKET))
	{
		CompositorManager::getSingleton().setCompositorEnabled(GAMEFRAMEWORK->viewport_, "B&W",false);
	}

	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_RBRACKET))
	{
		CompositorManager::getSingleton().setCompositorEnabled(GAMEFRAMEWORK->viewport_, "B&W",true);
	}
	

	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_R))
	{
		CameraStateGoToPointDef def;
		def.target = Ogre::Vector3(50,0,100);
		def.toleranceDistance = 5.0;
		def.factor = 2.5;
		def.initialPosition = camera_->getPosition();
		def.messageType = KGBMessageType::NO_MESSAGE;
		def.messageTarget = SEND_TO_ALL;
		gameCamera_->InitializeDef(&def);

	}

	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_T))
	{
		CameraStateWatchDef def;
		def.initialPosition = camPosition;
		def.targetObject = active_;
		gameCamera_->InitializeDef(&def);
	}

	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_E))
	{
		CameraStateWatchDef def;
		def.initialPosition = Ogre::Vector3(0,8,50);
		def.targetObject = active_;
		gameCamera_->InitializeDef(&def);
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

	if(keyEventRef.key == OIS::KC_Y)
	{
		active_->ReturnToCheckPoint();
	}

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
	static double angle = 0;
	Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, myMouse_->GetId(), UPDATE_MOUSE, &evt);
	//myGUI->injectMouseMove(evt);

	return true;
}

//=============================================================================
//							mousePressed
//
bool PhysicsState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	//myGUI->injectMousePress(evt, id);

	if(id == OIS::MB_Left)
	{
		Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, SEND_TO_ALL, LEFT_MOUSE_PLUS, NULL);
		m_bLMouseDown = true;
	} 
	else if(id == OIS::MB_Right)
	{
		Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, SEND_TO_ALL, RIGHT_MOUSE_PLUS, NULL);
		m_bRMouseDown = true;
	} 
	else if(id == OIS::MB_Middle)
	{
		Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, SEND_TO_ALL, MIDDLE_MOUSE_PLUS, NULL);
	}

	return true;
}


//=============================================================================
//							mouseReleased
//
bool PhysicsState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	
	//myGUI->injectMouseRelease(evt, id);
	if(id == OIS::MB_Left)
	{
		Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, SEND_TO_ALL, LEFT_MOUSE_MINUS, NULL);
		m_bLMouseDown = false;
	} 
	else if(id == OIS::MB_Right)
	{
		Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, SEND_TO_ALL, RIGHT_MOUSE_MINUS, NULL);
		m_bRMouseDown = false;
	}
	else if(id == OIS::MB_Middle)
	{
		Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, SEND_TO_ALL, MIDDLE_MOUSE_MINUS, NULL);
	}

	return true;
}



//=============================================================================
//							moveCamera
//

void PhysicsState::moveCamera()
{
	myMouse_->UpdateMouseFromCamera();
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

		/*if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_Y))
		{
			active_->ReturnToCheckPoint(active_->GetPlayerInfo()->GetCheckPoint());
		}*/

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

		ProcessPreSolveData();

		// Process b2Contacts that happened in this world step.
		ProcessContacts();

		ProcessPostSolveData();

		world->Step(0,1,1);
		// Update all the game objects.
		GameObject::UpdateObjectList(timeStep);
		

		moveCamera();
		UpdateOverlay();
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
//							PreSolve
//
void PhysicsState::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	/*
	PreSolveData* psd = new PreSolveData();

	contact->SetSensor
	psd->contact = contact;
	psd->oldManifold = new b2Manifold((*oldManifold));

	preSolveList_.push_back(psd);
	*/

	// Check if fixtureA's body has some user data
	// If it does check if the Object responds to contacts
	if(contact->GetFixtureA()->GetBody()->GetUserData() != NULL)
	{
		GameObject* object = (GameObject*) contact->GetFixtureA()->GetBody()->GetUserData();
		GameObjectOgreBox2D* contactableA;

		contactableA = dynamic_cast<GameObjectOgreBox2D*> (object);

		if(contactableA)
		{
			contactableA->PreSolve(contact,contact->GetFixtureA(),contact->GetFixtureB(),oldManifold);
		}
	}

	// Check if fixtureB's body has some user data
	// If it does check if the Object responds to contacts
	if(contact->GetFixtureB()->GetBody()->GetUserData() != NULL)
	{
		GameObject* object = (GameObject*) contact->GetFixtureB()->GetBody()->GetUserData();
		GameObjectOgreBox2D* contactableB;

		contactableB = dynamic_cast<GameObjectOgreBox2D*> (object);

		if(contactableB)
		{
			contactableB->PreSolve(contact,contact->GetFixtureB(),contact->GetFixtureA(),oldManifold);
		}
	}
}


//=============================================================================
//							PostSolve
//
void PhysicsState::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	PostSolveData* d = new PostSolveData();

	d->contact = contact;
	d->impulse = new b2ContactImpulse((*impulse));

	postSolveList_.push_back(d);
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
//						ProcessPreSolveData
//
void PhysicsState::ProcessPreSolveData()
{
	PreSolveData* psData;
	PreSolveList::iterator iter;

	// Process the PreSolve data
	for(iter = preSolveList_.begin(); iter != preSolveList_.end(); iter++)
	{
		psData = (*iter);
		b2Contact* contact = psData->contact;

		// Check if fixtureA's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->GetFixtureA()->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->GetFixtureA()->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableA;

			contactableA = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableA)
			{
				contactableA->PreSolve(contact,contact->GetFixtureA(),contact->GetFixtureB(),psData->oldManifold);
			}
		}

		// Check if fixtureB's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->GetFixtureB()->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->GetFixtureB()->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableB;

			contactableB = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableB)
			{
				contactableB->PreSolve(contact,contact->GetFixtureB(),contact->GetFixtureA(),psData->oldManifold);
			}
		}
	}

	for(iter = preSolveList_.begin(); iter != preSolveList_.end(); iter++)
	{
		delete (*iter);
	}

	preSolveList_.clear();
}

//=============================================================================
//						ProcessPostSolveData
//
void PhysicsState::ProcessPostSolveData()
{
	PostSolveData* psData;
	PostSolveList::iterator iter;

	// Process the PostSolve data
	for(iter = postSolveList_.begin(); iter != postSolveList_.end(); iter++)
	{
		psData = (*iter);
		b2Contact* contact = psData->contact;

		// Check if fixtureA's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->GetFixtureA()->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->GetFixtureA()->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableA;

			contactableA = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableA)
			{
				contactableA->PostSolve(contact,contact->GetFixtureA(),contact->GetFixtureB(),psData->impulse);
			}
		}

		// Check if fixtureB's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->GetFixtureB()->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->GetFixtureB()->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableB;

			contactableB = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableB)
			{
				contactableB->PostSolve(contact,contact->GetFixtureB(),contact->GetFixtureA(),psData->impulse);
			}
		}
	}

	for(iter = postSolveList_.begin(); iter != postSolveList_.end(); iter++)
	{
		delete (*iter);
	}

	postSolveList_.clear();
}

//=============================================================================
//							ProcessContacts
//
void PhysicsState::ProcessContacts()
{

	{
		ContactPoint* contact; 
		ContactList::iterator iter;

		// Process the BeginContact list of b2Contacts
		for(iter = beginContactList_.begin(); iter != beginContactList_.end(); iter++)
		{
			contact = (*iter);

			// Check if fixtureA's body has some user data
			// If it does check if the Object responds to contacts
			if(contact->fixtureA->GetBody()->GetUserData() != 0)
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
			if(contact->fixtureB->GetBody()->GetUserData() != 0)
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
	}

	{
		ContactPoint* contact; 
		ContactList::iterator iter;

		// Process the EndContact list of b2Contacts
		for(iter = endContactList_.begin(); iter != endContactList_.end(); iter++)
		{
			contact = (*iter);

			// Check if fixtureA's body has some user data
			// If it does check if the Object responds to contacts
			if(contact->fixtureA->GetBody()->GetUserData() != 0)
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
			if(contact->fixtureB->GetBody()->GetUserData() != 0)
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
	}

	ContactList::iterator iter;

	for(iter = endContactList_.begin(); iter != endContactList_.end(); iter++)
	{
		delete (*iter);
	}

	for(iter = beginContactList_.begin(); iter != beginContactList_.end(); iter++)
	{
		delete (*iter);
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