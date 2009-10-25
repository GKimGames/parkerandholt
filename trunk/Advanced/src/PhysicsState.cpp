//|||||||||||||||||||||||||||||||||||||||||||||||

#include "PhysicsState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;
double PhysicsState::timeStep = 1/60.0;

//|||||||||||||||||||||||||||||||||||||||||||||||

PhysicsState::PhysicsState()
{
	m_MoveSpeed			= 0.1;
	m_RotateSpeed		= 0.3;

	m_pCurrentObject	= 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::enter()
{
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

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering PhysicsState...");

	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "GameSceneMgr");
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));		

	m_pRSQ = m_pSceneMgr->createRayQuery(Ray());
	m_pRSQ->setQueryMask(OGRE_HEAD_MASK);

	m_pCamera = m_pSceneMgr->createCamera("GameCamera");
	m_pCamera->setPosition(Vector3(camPosition.x, camPosition.y, camPosition.z));
	m_pCamera->lookAt(Vector3(camLook.x, camLook.y, camLook.z));
	m_pCamera->setNearClipDistance(5);

	m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) / 
							  Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));
	
	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	OgreFramework::getSingletonPtr()->m_pKeyboard->setEventCallback(this);
	OgreFramework::getSingletonPtr()->m_pMouse->setEventCallback(this);
	
	OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(m_pSceneMgr);

	OgreFramework::getSingletonPtr()->m_pGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow"); 
    CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow"); 
	const OIS::MouseState state = OgreFramework::getSingletonPtr()->m_pMouse->getMouseState();
	CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition(); 
	CEGUI::System::getSingleton().injectMouseMove(state.X.abs-mousePos.d_x,state.Y.abs-mousePos.d_y);

	m_pMainWnd = CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME");
	m_pChatWnd = CEGUI::WindowManager::getSingleton().getWindow("ChatWnd");

	//OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(m_pMainWnd);

	CEGUI::PushButton* pExitButton = (CEGUI::PushButton*)m_pMainWnd->getChild("ExitButton_Game");
	pExitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PhysicsState::onExitButtonGame, this));
	
	m_bLMouseDown = m_bRMouseDown = false;
	m_bQuit = false;
	m_bChatMode = false;

	setUnbufferedMode();

	createScene();
#ifdef DEBUG_DRAW_ON
	debugDraw_ = new cDebugDraw(OgreFramework::getSingletonPtr()->m_pRoot,OgreFramework::getSingletonPtr()->m_pViewport->getTarget());
	debugDraw_->CopyCamera(m_pCamera);
#endif

}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PhysicsState::pause()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing PhysicsState...");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);
	OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(0);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::resume()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming PhysicsState...");

	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
	OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(m_pSceneMgr);

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME"));

	m_bQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::exit()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving PhysicsState...");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);
	OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(0);
	
	m_pSceneMgr->destroyCamera(m_pCamera);
	m_pSceneMgr->destroyQuery(m_pRSQ);

	if(m_pSceneMgr)
		OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);


}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::createPhysics()
{
	// Define the size of the world. Simulation will still work
	// if bodies reach the end of the world, but it will be slower.
	b2AABB worldAABB;
	worldAABB.lowerBound.Set(-100.0f, -100.0f);
	worldAABB.upperBound.Set(100.0f, 100.0f);

	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);

	// Do we want to let bodies sleep?
	bool doSleep = true;

	// Construct a world object, which will hold and simulate the rigid bodies.
	world = new b2World(worldAABB, gravity, doSleep);

}


//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::createScene()
{
	m_pSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	m_pSceneMgr->createLight("Light")->setPosition(75,75,75);

	// Create myCharacter
	myCharacter_ = new Character();

	MovablePlane *plane = new MovablePlane("Plane");
	plane->d = 0;
	plane->normal = Vector3(0,1.0,0.0);


	
	Ogre::MeshManager::getSingleton().createPlane("PlaneMesh", 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*plane, 60, 60, 1, 1, true, 1, 3, 3, Vector3::UNIT_Z);
	
	Entity *planeEnt = m_pSceneMgr->createEntity("PlaneEntity", "PlaneMesh");
	planeEnt->setMaterialName("Examples/GrassFloor");

	Ogre::SceneNode* mPlaneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	mPlaneNode->attachObject(planeEnt);
	mPlaneNode->scale(10, 10, 10);

	
	



}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PhysicsState::keyPressed(const OIS::KeyEvent &keyEventRef)
{


	if(m_bChatMode == true)
	{
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectKeyDown(keyEventRef.key);
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectChar(keyEventRef.text);
	}
	else
	{
		myCharacter_->KeyPressed(keyEventRef);
	}
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}


	if(m_bChatMode && OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RETURN) ||
		OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPADENTER))
	{
		CEGUI::Editbox* pChatInputBox = (CEGUI::Editbox*)m_pChatWnd->getChild("ChatInputBox");
		CEGUI::MultiLineEditbox *pChatContentBox = (CEGUI::MultiLineEditbox*)m_pChatWnd->getChild("ChatContentBox");
		pChatContentBox->setText(pChatContentBox->getText() + pChatInputBox->getText() + "\n");
		pChatInputBox->setText("");
		pChatContentBox->setCaratIndex(pChatContentBox->getText().size());
		pChatContentBox->ensureCaratIsVisible();
	}
	
	if(!m_bChatMode || (m_bChatMode && !OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_O)))
		OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);


	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PhysicsState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PhysicsState::mouseMoved(const OIS::MouseEvent &evt)
{
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseWheelChange(evt.state.Z.rel);
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
	
	if(m_bRMouseDown)
	{
		m_pCamera->yaw(Degree(evt.state.X.rel * -0.1));
		m_pCamera->pitch(Degree(evt.state.Y.rel * -0.1));
	}
	
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PhysicsState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
    {
        //onLeftPressed(evt);
        m_bLMouseDown = true;
		
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonDown(CEGUI::LeftButton);
    } 
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = true;
    } 
	
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PhysicsState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
    {
        m_bLMouseDown = false;

		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonUp(CEGUI::LeftButton);
    } 
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = false;
    }
	
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::onLeftPressed(const OIS::MouseEvent &evt)
{

}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PhysicsState::onExitButtonGame(const CEGUI::EventArgs &args)
{
	m_bQuit = true;
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::moveCamera()
{
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT)) 
		m_pCamera->moveRelative(m_TranslateVector);
	m_pCamera->moveRelative(m_TranslateVector / 10);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::getInput()
{
	if(m_bChatMode == false)
	{

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A))
		{
			m_TranslateVector.x = -m_MoveScale;
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D))
		{
			m_TranslateVector.x = m_MoveScale;
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
		{
			m_TranslateVector.z = -m_MoveScale;
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
		{
			m_TranslateVector.z = m_MoveScale;
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LEFT))
		{
			m_pCamera->yaw(m_RotScale);
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RIGHT))
		{
			m_pCamera->yaw(-m_RotScale);
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_UP))
		{
			m_pCamera->pitch(m_RotScale);
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_DOWN))
		{
			m_pCamera->pitch(-m_RotScale);
		}

	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::update(double timeSinceLastFrame)
{
	if(m_bQuit == true)
	{
		this->popAppState();
		return;
	}
	double dt = timeSinceLastFrame;

	if(dt > 0.2)
		dt = timeStep;

	m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
	m_RotScale  = m_RotateSpeed * timeSinceLastFrame;
		
	m_TranslateVector = Vector3::ZERO;

	getInput();
	moveCamera();

	while(dt >= timeStep)
	{
		dt = dt - timeStep;
	}
	myCharacter_->GetInput(timeSinceLastFrame);
	myCharacter_->Update(timeSinceLastFrame);
	
	

#ifdef DEBUG_DRAW_ON

#endif

	
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::setBufferedMode()
{
	CEGUI::Editbox* pModeCaption = (CEGUI::Editbox*)m_pMainWnd->getChild("ModeCaption");
	pModeCaption->setText("Buffered Input Mode");

	CEGUI::Editbox* pChatInputBox = (CEGUI::Editbox*)m_pChatWnd->getChild("ChatInputBox");
	pChatInputBox->setText("");
	pChatInputBox->activate();
	pChatInputBox->captureInput();

	CEGUI::MultiLineEditbox* pControlsPanel = (CEGUI::MultiLineEditbox*)m_pMainWnd->getChild("ControlsPanel");
	pControlsPanel->setText("[Tab] - To switch between input modes\n\nAll keys to write in the chat box.\n\nPress [Enter] or [Return] to send message.\n\n[Print] - Take screenshot\n\n[Esc] - Quit to main menu");
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::setUnbufferedMode()
{
	CEGUI::Editbox* pModeCaption = (CEGUI::Editbox*)m_pMainWnd->getChild("ModeCaption");
	pModeCaption->setText("Unuffered Input Mode");

	CEGUI::MultiLineEditbox* pControlsPanel = (CEGUI::MultiLineEditbox*)m_pMainWnd->getChild("ControlsPanel");
	pControlsPanel->setText("[Tab] - To switch between input modes\n\n[W] - Forward\n[S] - Backwards\n[A] - Left\n[D] - Right\n\nPress [Shift] to move faster\n\n[O] - Toggle Overlays\n[Print] - Take screenshot\n\n[Esc] - Quit to main menu");
}

//|||||||||||||||||||||||||||||||||||||||||||||||