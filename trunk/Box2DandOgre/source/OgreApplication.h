/*=============================================================================

		  OgreApplication.h

		  Author: Matt King

=============================================================================*/

#ifndef KGB_OGRE_APPLICATION_H
#define KGB_OGRE_APPLICATION_H

#include "AppStateManager.hpp"

#include "MenuState.hpp"
#include "GameState.hpp"
#include "PhysicsState.h"
#include "MessageDispatcher.h"


/// KGBOgreApplication
/// This class is created by the main method.
/// When Start is called a new AppStateManager, a GameFramework singleton and
/// a KGBMessageDispatcher is created.
class KGBOgreApplication
{
public:

	/// Does nothing
	KGBOgreApplication();

	/// Delete GameFramework and 
	~KGBOgreApplication();

	/// Start the Ogre Application.
	/// Creates an AppStateManager, a GameFramework singleton and a 
	/// KGBMessageDispatcher and creates the application states.
	void Start();

private:

	AppStateManager*	appStateManager_;

	bool				initialized_;

};



#endif 

//|||||||||||||||||||||||||||||||||||||||||||||||