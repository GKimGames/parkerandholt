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

	m_bQuit = false;
}



void MenuState::createScene()
{

	GameFramework::getSingletonPtr()->viewport_->setBackgroundColour(Ogre::ColourValue(0,0,0));

	GameObjectFactory gof;
	gof.AddObjectCreators();
	gof.sceneManager = sceneManager_;
	gof.LoadFile("..\\MainMenu.xml");

	/*
	Ogre::StringVector sv = Ogre::ResourceGroupManager::getSingletonPtr()->getResourceGroups();
	

	for(int i = 0; i < sv.size(); i++)
	{
		Ogre::FileInfoListPtr resources = Ogre::ResourceGroupManager::getSingletonPtr()->listResourceFileInfo(sv[i]);
		Ogre::StringVectorPtr sv2 = Ogre::ResourceGroupManager::getSingletonPtr()->findResourceNames(sv[i], Ogre::String("*.mesh"));
		
		Ogre::StringVector::iterator it2 = sv2->begin();
		for(it2; it2 != (*sv2).end(); it2++)
		{
			Ogre::String sv3 = (*it2);
		}
		
		Ogre::FileInfoList::iterator it;
		it = resources->begin();
		
		for(it; it != resources->end(); it++)
		{
			Ogre::FileInfo fi = (*it);
		}
	}
	*/
	

}



void MenuState::exit()
{
	GAMEFRAMEWORK->log_->logMessage("Leaving MenuState...");

	sceneManager_->destroyCamera(m_pCamera);
	if(sceneManager_)
		GAMEFRAMEWORK->root_->destroySceneManager(sceneManager_);
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
		this->pushAppState(findByName("PhysicsState"));
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

