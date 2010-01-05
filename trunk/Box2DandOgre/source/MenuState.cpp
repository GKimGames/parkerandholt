#include "MenuState.hpp"


using namespace Ogre;


void MenuState::enter()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Entering MenuState...");
	
	m_pSceneMgr = GameFramework::getSingletonPtr()->root_->createSceneManager(ST_GENERIC, "MenuSceneMgr");
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 0.7, 0.7)); 

	m_pCamera = m_pSceneMgr->createCamera("MenuCam");
	m_pCamera->setPosition(Vector3(0, 25, -50));
	m_pCamera->lookAt(Vector3(0, 0, 0));
	m_pCamera->setNearClipDistance(1);

	m_pCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->viewport_->getActualWidth()) / 
							  Real(GameFramework::getSingletonPtr()->viewport_->getActualHeight()));
	
	GameFramework::getSingletonPtr()->viewport_->setCamera(m_pCamera);

	GameFramework::getSingletonPtr()->keyboard_->setEventCallback(this);
	GameFramework::getSingletonPtr()->mouse_->setEventCallback(this);

	betaGUI_ = new BetaGUI::GUI("MgOpen", 16);
	
	betaGUI_->createMousePointer(Vector2(32,32), "bgui.pointer");
	
	/*
	BetaGUI::Window* window = betaGUI_->createWindow(Vector4(100,100,300,100), "bgui.window", BetaGUI::RESIZE_AND_MOVE, "Parker and Holt");
	
	enterGameButton_ = window->createButton(Vector4(112,50,104,24), "bgui.button", "Game", BetaGUI::Callback(this));
	enterMapEditorButton_ = window->createButton(Vector4(4,50,104,24), "bgui.button", "Map Editor", BetaGUI::Callback(this));
	*/

	m_bQuit = false;
	
	createScene();
}



bool MenuState::pause()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Pausing MenuState...");

	return true;
}



void MenuState::resume()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Resuming MenuState...");
	GameFramework::getSingletonPtr()->viewport_->setCamera(m_pCamera);

	betaGUI_ = new BetaGUI::GUI("MgOpen", 16);
	
	betaGUI_->createMousePointer(Vector2(32,32), "bgui.pointer");

	m_bQuit = false;
}



void MenuState::createScene()
{
}



void MenuState::exit()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Leaving MenuState...");

	delete betaGUI_;

	m_pSceneMgr->destroyCamera(m_pCamera);
	if(m_pSceneMgr)
		GameFramework::getSingletonPtr()->root_->destroySceneManager(m_pSceneMgr);
}


bool MenuState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}

	GameFramework::getSingletonPtr()->KeyPressed(keyEventRef);

	return true;
}



bool MenuState::keyReleased(const OIS::KeyEvent &keyEventRef)
{		
	GameFramework::getSingletonPtr()->KeyReleased(keyEventRef);

	return true;
}



bool MenuState::mouseMoved(const OIS::MouseEvent &evt)
{

	static int x = 0;
	static int y = 0;

	const OIS::MouseState ms = GameFramework::getSingletonPtr()->mouse_->getMouseState();

	// Mouse injection must be absolute position than relative.
	x += ms.X.rel;
	y += ms.Y.rel;

	if (ms.buttons == 1)
		betaGUI_->injectMouse(x,y, true);  // LMB is down.
	else
		betaGUI_->injectMouse(x,y, false); // LMB is not down.

	return true;
}



bool MenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{

	return true;
}



bool MenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{

	return true;
}

void MenuState::getInput()
{
	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_RETURN))
	{
		delete betaGUI_;
		this->pushAppState(findByName("PhysicsState"));
		return;
	}

	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_RSHIFT))
	{
		delete betaGUI_;
		this->pushAppState(findByName("MapEditorState"));
		return;
	}
}

bool MenuState::update(double timeSinceLastFrame)
{
	if(m_bQuit == true)
	{
		this->popAppState();
		return false;
	}

	getInput();

	return true;
}

