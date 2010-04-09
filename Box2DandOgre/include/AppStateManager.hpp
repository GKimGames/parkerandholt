/*=============================================================================

		AppStateManager.h

		Author: spacegaier
		This was taken from the Advanced Ogre Framework by spacegaier.

		Updated by Matt King

		+ Changed from struct and vector of AppStates to std::map.
		+ Comments
		+ popStateAfterNextUpdate
		
=============================================================================*/

#ifndef APP_STATE_MANAGER_HPP
#define APP_STATE_MANAGER_HPP

#include "AppState.hpp"
#include "OgreGpuCommandBufferFlush.h"
#include "MessageDispatcher.h"
#include <map>

class AppStateManager : public AppStateListener
{
public:

	/// Typedef for a Map of String and an AppState.
	typedef std::map<Ogre::String, AppState*> AppMap;

	AppStateManager();

	~AppStateManager();

	/// Adds a new string / state pair to the Manager
	void manageAppState(Ogre::String stateName, AppState* state);

	/// Returns an AppState given it's name. 
	AppState*	findByName(Ogre::String stateName);

	/// Changes to a different AppState. The current AppState has nothing done to
	/// it, but it will not be updated anymore as it is not the top AppState. The 
	/// AppState passed in is pushed onto the stack of AppStates.
	void		changeAppState(AppState *state);

	/// Pushes an AppState to the top of the stack and pauses the current AppState.
	bool		pushAppState(AppState* state);

	/// Pushes an AppState, given by the state's name, to the top of the stack and
	/// pauses the current AppState.
	bool		pushAppState(Ogre::String stateName);

	/// This pops an AppState off the top of the stack. It calls exit on the top
	/// AppState in the stack and then calls Resume on the new top AppState. If
	/// there are no more AppStates after popping, the application is shutdown.
	void		popAppState();

	/// ShutDown the application.
	void		shutdown();

	/// This is the main loop of the game.
	void		start(AppState* state);

	/// This sets it so that the current state will be popped after the current
	/// update loop has finished.
	void		popStateAfterNextUpdate();

protected:

	/// Initialize an AppState. This sets the GameFrameworks keyboard and mouse to
	/// be that of the passed AppState.
	void init(AppState *state);

	AppMap							m_AppMap;

	std::vector<AppState*>			m_ActiveStateStack;
	bool							m_bShutdown;

	bool							popState_;
	bool							pushState_;
	Ogre::String					pushToState_;
};


#endif
