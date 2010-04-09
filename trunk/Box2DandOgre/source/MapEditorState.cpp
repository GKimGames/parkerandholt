/*=============================================================================

		MapEditorState.cpp

		Author: Matt King

=============================================================================*/

#include "MapEditorState.h"
#include "MyGUI.h"
#include "GameObjectFactory.h"

using namespace Ogre;

//=============================================================================
//							MapEditorState
//
MapEditorState::MapEditorState()
{
	m_MoveSpeed		= 1.0;
	currentObject_	= 0;
}

//=============================================================================
//							enter
//
void MapEditorState::enter()
{

	GAMEFRAMEWORK->log_->logMessage("Entering MapEditorState...");

	// Load XML 
	TiXmlDocument configXML_( "..\\MapEditorSettings.xml");
	configXML_.LoadFile();
	TiXmlHandle hDoc(&configXML_);

	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = hDoc.FirstChildElement().Element();

	// The XML file didn't load, we can't do anything.
	if (!pElem) 
		return;

	hRoot = TiXmlHandle(pElem);
	
	TiXmlElement* cameraElement = hRoot.FirstChild( "Camera" ).FirstChild( "InitialPosition").Element();
	Vector3 camPos;
	double x,y,z = 0;

	if(cameraElement)
	{
		cameraElement->QueryDoubleAttribute("x", &x);
		cameraElement->QueryDoubleAttribute("y", &y);
		cameraElement->QueryDoubleAttribute("z", &z);
	}

	//gameObject_ = new GameObject();
	//gameObject_->SetName("MapEditorState");

	
	sceneManager_ = GAMEFRAMEWORK->root_->createSceneManager(ST_GENERIC, "MapEditorSceneMgr");
	sceneManager_->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));	

	camera_ = sceneManager_->createCamera("MapEditorCamera");
	camera_->setPosition(Vector3(x, y, z));
	camera_->lookAt(Vector3(0,0,0));
	camera_->setNearClipDistance(1);

	camera_->setAspectRatio(Real(GAMEFRAMEWORK->viewport_->getActualWidth()) / 
		Real(GAMEFRAMEWORK->viewport_->getActualHeight()));

	GAMEFRAMEWORK->viewport_->setCamera(camera_);
	GAMEFRAMEWORK->keyboard_->setEventCallback(this);
	GAMEFRAMEWORK->mouse_->setEventCallback(this);

	quit_ = false;

	setUnbufferedMode();

	createScene();

	dynamicLines_ = new DynamicLines();
	linesNodeScale_ = 1;

	Vector3 points[20000];
	for(int j = 0; j < 20000; j += 4)
	{
		points[j].x		= -10000.0;
		points[j+1].x	= 10000.0;
		points[j].y		= -1000.0 + double(j / 10.0);
		points[j+1].y	= -1000.0 + double(j / 10.0);
		

		points[j+2].x = 10000.0;
		points[j+3].x = -10000.0;
		points[j+2].y = -1000.0 + double((j + 2) / 10.0);
		points[j+3].y = -1000.0 + double((j + 2) / 10.0);
		
		points[j].z		= 0;
		points[j+1].z	= 0;
		points[j+2].z = 0;
		points[j+3].z = 0;
	}

	
	for(int i = 0; i < 20000; i++)
	{
		dynamicLines_->addPoint(points[i]);
	}

	for(int j = 0; j < 20000; j += 4)
	{
		points[j].x		= -1000.0 + double(j / 10.0);
		points[j+1].x	= -1000.0 + double(j / 10.0);
		points[j].y		= -10000;
		points[j+1].y	= 10000;
		

		points[j+2].x = -1000.0 + double((j + 2) / 10.0);
		points[j+3].x = -1000.0 + double((j + 2) / 10.0);
		points[j+2].y = 10000;
		points[j+3].y = -10000;
	
	}

	for(int i = 0; i < 20000; i++)
	{
		dynamicLines_->addPoint(points[i]);
	}

	dynamicLines_->update();
	linesNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode("lines");
	linesNode_->attachObject(dynamicLines_);


	myGUI_ = new MyGUI::Gui();
	myGUI_->initialise(GAMEFRAMEWORK->renderWindow_);
	
	MyGUI::ButtonPtr button = myGUI_->createWidget<MyGUI::Button>("Button", 10, 10, 300, 26, MyGUI::Align::Default, "Main");
	button->setCaption("exit");

}


//=============================================================================
//							pause
//
bool MapEditorState::pause()
{
	GAMEFRAMEWORK->log_->logMessage("Pausing MapEditorState...");

	this->pushAppState(findByName("InGameMenu"));

	return true;
}

//=============================================================================
//							resume
//
void MapEditorState::resume()
{
	GAMEFRAMEWORK->log_->logMessage("Resuming MapEditorState...");

	GAMEFRAMEWORK->viewport_->setCamera(camera_);

	quit_ = false;
	pause_ = false;
}


//=============================================================================
//							exit
//
void MapEditorState::exit()
{

	myGUI_->shutdown();
	delete myGUI_;

	delete world;

	GAMEFRAMEWORK->log_->logMessage("Leaving MapEditorState...");

	sceneManager_->destroyCamera(camera_);

	//GameObject::objectList.clear();


	if(sceneManager_)
	{
		GAMEFRAMEWORK->root_->destroySceneManager(sceneManager_);
	}

}


//=============================================================================
//							createPhysics
//
void MapEditorState::createPhysics()
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
	GAMEFRAMEWORK->SetWorld(world);
	world->SetContactListener(this);

	new Platform(sceneManager_, b2Vec2(-300.0f, 0.0f), b2Vec2(10.0f, 0.0f));
	new Platform(sceneManager_, b2Vec2(10.0f, 0.0f),   b2Vec2(20.0f, 3.0f));
	new Platform(sceneManager_, b2Vec2(25.0f, 5.0f),   b2Vec2(30.0f, 5.0f));
	new Platform(sceneManager_, b2Vec2(30.0f, 6.4f),   b2Vec2(35.0f, 6.4f));
	new Platform(sceneManager_, b2Vec2(22.0f, 7.5f),   b2Vec2(27.0f, 7.5f));

	// Create myCharacter
	//myCharacter_ = new Character(sceneManager_);
	//myCharacter_->Initialize();

	HoltBox* bb = new HoltBox(sceneManager_, b2Vec2(8.0f, 10.0f));
	bb->Initialize();

	new MovingPlatform(sceneManager_, b2Vec2(10.0f, 1.0f), b2Vec2(0.0f, 1.0f), b2Vec2(5.0f, 1.0f), b2Vec2(0.0f, 5.0f), 2);

	//new PressureSwitch(sceneManager_);


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
	world->Step(timeStep,10,10);
	//myPicking_ = new MousePicking(sceneManager_, world, camera_, pickingPlane);

	TiXmlDocument configXML_( "..\\MapEditorSettings.xml");
	configXML_.LoadFile();
	TiXmlHandle hDoc(&configXML_);

	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = hDoc.FirstChildElement().Element();

	// The XML file didn't load, we can't do anything.
	if (!pElem) 
		return;

	hRoot = TiXmlHandle(pElem);



	//TiXmlElement* ogreBox2DElement = hRoot.FirstChild( "Object" ).Element();
	//GAMEFRAMEWORK->gameObjectFactory->CreateGameObject(ogreBox2DElement);

}

//=============================================================================
//							CreateBox
//
void MapEditorState::CreateBox()
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
void MapEditorState::createScene()
{

	GAMEFRAMEWORK->viewport_->setBackgroundColour(Ogre::ColourValue(0,0,0));
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


//=============================================================================
//							keyPressed
//
bool MapEditorState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	myGUI_->injectKeyPress(keyEventRef);
	
	if(keyEventRef.key == OIS::KC_P)
	{
		pause_ = true;
	}


	if(GAMEFRAMEWORK->keyboard_->isKeyDown(OIS::KC_ESCAPE))
	{
		this->popAppState();
		return true;
	}

	

	GameFramework::getSingletonPtr()->KeyPressed(keyEventRef);

	return true;
}

//=============================================================================
//							keyReleased
//
bool MapEditorState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	GAMEFRAMEWORK->KeyReleased(keyEventRef);

	myGUI_->injectKeyPress(keyEventRef);
	

	if(keyEventRef.key == OIS::KC_EQUALS || keyEventRef.key == OIS::KC_ADD)
	{
		linesNodeScale_ = linesNodeScale_ << 1;
		linesNode_->setScale(linesNodeScale_,linesNodeScale_,linesNodeScale_);
	}

	if(keyEventRef.key == OIS::KC_MINUS || keyEventRef.key == OIS::KC_SUBTRACT)
	{
		if(linesNodeScale_ != 1)
		{
			linesNodeScale_ = linesNodeScale_ >> 1;
			linesNode_->setScale(linesNodeScale_,linesNodeScale_,linesNodeScale_);
		}
	}

	return true;
}


//=============================================================================
//							mouseMoved
//
bool MapEditorState::mouseMoved(const OIS::MouseEvent &evt)
{
	static int x = 0;
	static int y = 0;

	const OIS::MouseState ms = GameFramework::getSingletonPtr()->mouse_->getMouseState();

	myGUI_->injectMouseMove(evt);

	return true;
}

//=============================================================================
//							mousePressed
//
bool MapEditorState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{

	myGUI_->injectMousePress(evt,id);
	
	return true;
}


//=============================================================================
//							mouseReleased
//
bool MapEditorState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	myGUI_->injectMouseRelease(evt,id);

	return true;
}

//=============================================================================
//							moveCamera
//
void MapEditorState::moveCamera()
{
	if(GAMEFRAMEWORK->keyboard_->isKeyDown(OIS::KC_LSHIFT)) 
		camera_->moveRelative(m_TranslateVector);

	camera_->moveRelative(m_TranslateVector / 10);
}


//=============================================================================
//							getInput
//
void MapEditorState::getInput()
{

	if(GAMEFRAMEWORK->keyboard_->isKeyDown(OIS::KC_A))
	{
		m_TranslateVector.x = -m_MoveScale;
	}

	if(GAMEFRAMEWORK->keyboard_->isKeyDown(OIS::KC_D))
	{
		m_TranslateVector.x = m_MoveScale;
	}

	if(GAMEFRAMEWORK->keyboard_->isKeyDown(OIS::KC_W))
	{
		m_TranslateVector.z = -m_MoveScale;
	}

	if(GAMEFRAMEWORK->keyboard_->isKeyDown(OIS::KC_S))
	{
		m_TranslateVector.z = m_MoveScale;
	}

	if(GAMEFRAMEWORK->keyboard_->isKeyDown(OIS::KC_RETURN))
	{
		world->Step(0,10,10);
		world->ClearForces();
	
#if DEBUG_DRAW_ON
		if(debugDrawOn_)
		{
			debugDraw_->clear();
			world->DrawDebugData();
		}
#endif
	}
	

#if DEBUG_DRAW_ON
	// Turn Debug Draw off
	if(GAMEFRAMEWORK->keyboard_->isKeyDown(OIS::KC_Z))
	{
		debugDrawOn_ = false;
		debugDraw_->clear();
	}

	// Turn Debug Draw on
	if(GAMEFRAMEWORK->keyboard_->isKeyDown(OIS::KC_X))
	{
		debugDrawOn_ = true;
	}
#endif

}


//=============================================================================
//							update
//
/// Main Update looped for a level
bool MapEditorState::update(double timeSinceLastFrame)
{
	static double time = 0;

	if(pause_ == true)
	{
		pause();
		return false;
	}

	time += timeSinceLastFrame;

	m_MoveScale = 5 * m_MoveSpeed * timeSinceLastFrame;
	m_TranslateVector = Vector3::ZERO;

	getInput();
	moveCamera();

	if(debugDrawOn_)
	{
		debugDraw_->clear();
		world->DrawDebugData();
	}

	GameObject::UpdateObjectList(timeStep);	

	return true;
}

void MapEditorState::setBufferedMode()
{
}

void MapEditorState::setUnbufferedMode()
{
}


//=============================================================================
//							BeginContact
//
/// Called when two fixtures begin to touch.
/// This calls the BeginContact method of an GameObjectOgreBox2D if the user data 
/// for one the touching fixtures is there. The called BeginContact method passes
/// which fixture was of the GameObjectOgreBox2D and which one it collided with.
void MapEditorState::BeginContact(b2Contact* contact)
{

}

//=============================================================================
//							EndContact
//
/// Called when two fixtures cease to touch.
/// This calls the EndContact method of an GameObjectOgreBox2D if the user data 
/// for one the touching fixtures is there. The called EndContact method passes
/// which fixture was of the GameObjectOgreBox2D and which one it collided with.
void MapEditorState::EndContact(b2Contact* contact)
{
	
}
