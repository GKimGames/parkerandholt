/*=============================================================================

  OgreApplication.h

  Author: Matt King

=============================================================================*/

#ifndef KGB_OGRE_APPLICATION_H
#define KGB_OGRE_APPLICATION_H

#include "AdvancedOgreFramework.hpp"
#include "AppStateManager.hpp"

#include "MenuState.hpp"
#include "GameState.hpp"
#include "PhysicsState.h"
#include "EventDispatcher.h"
#include "MessageDispatcher.h"



class KGBOgreApplication
{
public:
	KGBOgreApplication();
	~KGBOgreApplication();

	void Start();

private:
	AppStateManager*	appStateManager_;
};



#endif 

//|||||||||||||||||||||||||||||||||||||||||||||||