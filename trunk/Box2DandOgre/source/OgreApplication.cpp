/*=============================================================================

  OgreApplication.cpp

  Author: Matt King

=============================================================================*/

#include "OgreApplication.h"



KGBOgreApplication::KGBOgreApplication()
{
}



KGBOgreApplication::~KGBOgreApplication()
{
	delete GameFramework::getSingletonPtr();
	delete PaH::GameEventDispatcher::getSingletonPtr();

	if(appStateManager_)
	{
		delete appStateManager_;
	}

}


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

		appStateManager_->start(appStateManager_->findByName("MenuState"));
	}
}