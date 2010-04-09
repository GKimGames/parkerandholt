/*=============================================================================

		MenuState.cpp

		Author: Matt King

=============================================================================*/
#include "MenuState.hpp"
#include "GameObjectFactory.h"

using namespace Ogre;


void MenuState::enter()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Entering MenuState...");
	
	gameObjectList_ = new GameObjectMap();
	gameObjectNameList_ = new GameObjectNameMap();
	GameObject::objectList = gameObjectList_;
	GameObject::objectNameList = gameObjectNameList_;

	sceneManager_ = GAMEFRAMEWORK->root_->createSceneManager(ST_GENERIC, "MenuSceneManager");
	GAMEFRAMEWORK->sceneManager = sceneManager_;

	sceneManager_->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7)); 

	m_pCamera = sceneManager_->createCamera("MenuCam");
	m_pCamera->setPosition(Vector3(0, 0, 18));
	m_pCamera->lookAt(Vector3(0, 0, 0));
	m_pCamera->setNearClipDistance(1);

	m_pCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->viewport_->getActualWidth()) / 
							  Real(GameFramework::getSingletonPtr()->viewport_->getActualHeight()));
	
	GameFramework::getSingletonPtr()->viewport_->setCamera(m_pCamera);

	GameFramework::getSingletonPtr()->keyboard_->setEventCallback(this);
	GameFramework::getSingletonPtr()->mouse_->setEventCallback(this);

	quit_ = false;
	
	createScene();
}



bool MenuState::pause()
{
	GameFramework::getSingletonPtr()->log_->logMessage("Pausing MenuState...");

	return true;
}



void MenuState::resume()
{
	AppState::resume();

	GameFramework::getSingletonPtr()->log_->logMessage("Resuming MenuState...");
	GameFramework::getSingletonPtr()->viewport_->setCamera(m_pCamera);

	quit_ = false;
}



void MenuState::createScene()
{

	GameFramework::getSingletonPtr()->viewport_->setBackgroundColour(Ogre::ColourValue(0,0,0));

	GameObjectFactory gof;
	gof.AddObjectCreators();
	gof.sceneManager = sceneManager_;
	gof.LoadFile("..\\MainMenu.xml");

}



void MenuState::exit()
{
	GAMEFRAMEWORK->log_->logMessage("Leaving MenuState...");

	this->deleteObjects();

	sceneManager_->destroyCamera(m_pCamera);
	if(sceneManager_)
		GAMEFRAMEWORK->root_->destroySceneManager(sceneManager_);
}


bool MenuState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_ESCAPE))
	{
		quit_ = true;
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
		this->pushAppState(findByName("PhysicsState2"));
		return;
	}

}

bool MenuState::update(double timeSinceLastFrame)
{
	if(quit_ == true)
	{
		this->popAppState();
		return false;
	}

	getInput();

	return true;
}

