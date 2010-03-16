/*=============================================================================

		AppStateManager.h

		Author: spacegaier
		This was taken from the Advanced Ogre Framework by spacegaier.

		Updated by: Matt King

		+ Changed from struct and vector of AppStates to std::map.
		
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

	typedef std::map<Ogre::String, AppState*> AppMap;

	AppStateManager();
	~AppStateManager();

	void manageAppState(Ogre::String stateName, AppState* state);

	AppState* findByName(Ogre::String stateName);

	void start(AppState* state);
	void changeAppState(AppState* state);
	bool pushAppState(AppState* state);
	void popAppState(void);
	void popStateAfterNextUpdate();
	void shutdown(void);

protected:

	void init(AppState *state);

	AppMap							m_AppMap;

	std::vector<AppState*>			m_ActiveStateStack;
	bool							m_bShutdown;
	Ogre::GpuCommandBufferFlush		m_BufferFlush;

	bool							popState_;
};


#endif
