/*=============================================================================

	  OgreApplication.cpp

	  Author: Matt King

=============================================================================*/

#include "OgreApplication.h"


//=============================================================================
//						Constructor
//
/// Does nothing
KGBOgreApplication::KGBOgreApplication()
{
	initialized_ = false;
}


//=============================================================================
//						Destructor
//
/// Delete the GameFramwork, KGBMessageDispatcher and AppStateManager.
KGBOgreApplication::~KGBOgreApplication()
{
	delete GameFramework::getSingletonPtr();
	delete KGBMessageDispatcher::getSingletonPtr();

	if(initialized_)
	{
		delete appStateManager_;
	}

}

//=============================================================================
//						Start
//
/// Start the Ogre Application.
/// Creates an AppStateManager, a GameFramework singleton and a 
/// KGBMessageDispatcher and creates the application states.
void KGBOgreApplication::Start()
{

	new GameFramework();
	new KGBMessageDispatcher();

	bool initialized = GameFramework::getSingletonPtr()->InitOgre("Parker And Holt", 0, 0);
	
	if(initialized)
	{
		GameFramework::getSingletonPtr()->log_->logMessage("Parker and Holt initialized");

		appStateManager_ = new AppStateManager();

		MenuState::create(appStateManager_, "MenuState");
		PhysicsState::create(appStateManager_, "PhysicsState");
		PhysicsState::create(appStateManager_, "PhysicsState2", "..\\LevelTwoTwo.xml");

		GAMEFRAMEWORK->appStateManager = appStateManager_;

		appStateManager_->start(appStateManager_->findByName("MenuState"));

		
	}
}