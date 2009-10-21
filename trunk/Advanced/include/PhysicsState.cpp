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
	std::string settingsFile = "Z:/OgreSDK/bin/release/Settings.cfg";
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

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(m_pMainWnd);

	CEGUI::PushButton* pExitButton = (CEGUI::PushButton*)m_pMainWnd->getChild("ExitButton_Game");
	pExitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PhysicsState::onExitButtonGame, this));
	
	m_bLMouseDown = m_bRMouseDown = false;
	m_bQuit = false;
	m_bChatMode = false;

	setUnbufferedMode();

	createScene();

	mTimeController = NxOgre::TimeController::getSingleton();

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

	mWorld->destroyWorld(true);

}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PhysicsState::createScene()
{

	m_pSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	m_pSceneMgr->createLight("Light")->setPosition(75,75,75);

	mWorld = NxOgre::World::createWorld();

	NxOgre::SceneDescription sceneDesc;
	sceneDesc.mGravity = NxOgre::Vec3(0, -4.9f, 0);
	sceneDesc.mName = "PhysicsStateScene";

	mScene = mWorld->createScene(sceneDesc);

	mScene->getMaterial(0)->setStaticFriction(0.5);
	mScene->getMaterial(0)->setDynamicFriction(0.5);
	mScene->getMaterial(0)->setRestitution(0.1);

	mRenderSystem = new OGRE3DRenderSystem(mScene);

	OGRE3DBody* mCube;
	mCube = mRenderSystem->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(0, 20, 0), "cube.1m.mesh");

	NxOgre::Enums::BodyFlags_FreezePositionZ;

	NxOgre::RigidBodyDescription d;
	d.mBodyFlags = NxOgre::Enums::BodyFlags_FreezePositionZ 
				 | NxOgre::Enums::BodyFlags_FreezeRotationX 
				 | NxOgre::Enums::BodyFlags_FreezeRotationY;

	NxOgre::RigidBodyDescription capsuleDesc;
	capsuleDesc.mBodyFlags = NxOgre::Enums::BodyFlags_FreezePositionZ
				 | NxOgre::Enums::BodyFlags_FreezeRotation;


	mCubeTwo = mRenderSystem->createBody(new NxOgre::Sphere(.5), NxOgre::Vec3(20, 35, 0), "sphere.mesh");
	Ogre::Vector3 sphereSize = mCubeTwo->getEntity()->getBoundingBox().getSize();
	mCubeTwo->getEntity()->getParentSceneNode()->scale(1.0 / sphereSize.x, 1.0 / sphereSize.y, 1.0 / sphereSize.z);
	
	capsule = mRenderSystem->createBody(new NxOgre::Box(10, 10, 10), NxOgre::Vec3(0, 20, 0), "cube.1m.mesh");
	//capsule = mRenderSystem->createBody(new NxOgre::Sphere(5),NxOgre::Vec3(20, 39, 0),"sphere.mesh", capsuleDesc);
	//capsule->getEntity()->getParentSceneNode()->scale(10.0 / sphereSize.x, 10.0 / sphereSize.y, 10.0 / sphereSize.z);
	capsule->getEntity()->getParentSceneNode()->scale(10.0, 10.0, 10.0);
	mScene->createSceneGeometry(new NxOgre::PlaneGeometry(0, NxOgre::Vec3(0, 1, 0)), Matrix44_Identity);

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

	m_pSceneMgr->getEntity("PlaneEntity")->setQueryFlags(CUBE_MASK);
	mCube->getEntity()->setQueryFlags(CUBE_MASK);


	/*// Original code
	Ogre::DotSceneLoader* pDotSceneLoader = new Ogre::DotSceneLoader();
	pDotSceneLoader->parseDotScene("CubeScene.xml", "General", m_pSceneMgr, m_pSceneMgr->getRootSceneNode());

	m_pSceneMgr->getEntity("Cube01")->setQueryFlags(CUBE_MASK);
	m_pSceneMgr->getEntity("Cube02")->setQueryFlags(CUBE_MASK);
	m_pSceneMgr->getEntity("Cube03")->setQueryFlags(CUBE_MASK);

	m_pOgreHeadEntity = m_pSceneMgr->createEntity("Cube", "ogrehead.mesh");
	m_pOgreHeadEntity->setQueryFlags(OGRE_HEAD_MASK);
	m_pOgreHeadNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("CubeNode");
	m_pOgreHeadNode->attachObject(m_pOgreHeadEntity);
	m_pOgreHeadNode->setPosition(Vector3(0, 0, -25));

	m_pOgreHeadMat = m_pOgreHeadEntity->getSubEntity(1)->getMaterial();
	m_pOgreHeadMatHigh = m_pOgreHeadMat->clone("OgreHeadMatHigh");
	m_pOgreHeadMatHigh->getTechnique(0)->getPass(0)->setAmbient(1, 0, 0);
	m_pOgreHeadMatHigh->getTechnique(0)->getPass(0)->setDiffuse(1, 0, 0, 0);
	*/
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PhysicsState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(m_bChatMode == true)
	{
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectKeyDown(keyEventRef.key);
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectChar(keyEventRef.text);
	}
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_TAB))
	{
		m_bChatMode = !m_bChatMode;
		
		if(m_bChatMode)
			setBufferedMode();
		else
			setUnbufferedMode();
		
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
	if(m_pCurrentObject)
	{
		m_pCurrentObject->showBoundingBox(false);
		m_pCurrentEntity->getSubEntity(1)->setMaterial(m_pOgreHeadMat);
	}
		
	CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
	Ogre::Ray mouseRay = m_pCamera->getCameraToViewportRay(mousePos.d_x/float(evt.state.width), mousePos.d_y/float(evt.state.height));
	m_pRSQ->setRay(mouseRay);
	m_pRSQ->setSortByDistance(true);

	Ogre::RaySceneQueryResult &result = m_pRSQ->execute();
	Ogre::RaySceneQueryResult::iterator itr;

    for(itr = result.begin(); itr != result.end(); itr++)
    {
        if(itr->movable)
        {
			OgreFramework::getSingletonPtr()->m_pLog->logMessage("MovableName: " + itr->movable->getName());
			m_pCurrentObject = m_pSceneMgr->getEntity(itr->movable->getName())->getParentSceneNode();
			OgreFramework::getSingletonPtr()->m_pLog->logMessage("ObjName " + m_pCurrentObject->getName());
			m_pCurrentObject->showBoundingBox(true);
			m_pCurrentEntity = m_pSceneMgr->getEntity(itr->movable->getName());
			m_pCurrentEntity->getSubEntity(1)->setMaterial(m_pOgreHeadMatHigh);
            break;
        }			
    }
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

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPAD4))
		{
			capsule->addForce(NxOgre::Vec3(-8, 0, 0));
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPAD6))
		{
			//mCubeTwo->addTorque(NxOgre::Vec3(100,0,0));
			capsule->addForce(NxOgre::Vec3(8, 0, 0));
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

	m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
	m_RotScale  = m_RotateSpeed * timeSinceLastFrame;
		
	m_TranslateVector = Vector3::ZERO;

	getInput();
	moveCamera();


	double dt = timeSinceLastFrame;

	if(dt > 0.2)
		dt = timeStep;

	while(dt >= timeStep)
	{
		mTimeController->advance();
		dt = dt - timeStep;
	}
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