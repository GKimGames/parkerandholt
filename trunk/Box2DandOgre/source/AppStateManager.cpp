/*=============================================================================

		AppStateManager.h

		Author: spacegaier
		This was taken from the Advanced Ogre Framework by spacegaier.

		Updated by Matt King

		+ Changed from struct and vector of AppStates to std::map.
		+ Comments
		+ popStateAfterNextUpdate
		
=============================================================================*/

#include "AppStateManager.hpp"

#include <OgreWindowEventUtilities.h>
#include <stdio.h>
#include "windows.h"



//=============================================================================
//							AppStateManager
//
AppStateManager::AppStateManager()
{
	m_bShutdown = false;
	popState_ = false;
	pushState_ = false;
	pushToState_ = "";

}



//=============================================================================
//							AppStateManager
//
AppStateManager::~AppStateManager()
{
	while(!m_ActiveStateStack.empty()) 
	{
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

}



//=============================================================================
//							manageAppState
//
/// Adds a new string / state pair to the Manager
void AppStateManager::manageAppState(Ogre::String stateName, AppState* state)
{
	m_AppMap[stateName] = state;
}



//=============================================================================
//							findByName
//
/// Returns an AppState given it's name.
AppState* AppStateManager::findByName(Ogre::String stateName)
{
	AppMap::iterator iter = m_AppMap.find(stateName);

	if(iter != m_AppMap.end())
	{
		return iter->second;
	}

	return 0;
}



//=============================================================================
//							start
//
/// This is the main loop of the game.
void AppStateManager::start(AppState* state)
{
	changeAppState(state);
	
	static double timeSinceLastFrame = 1;
	static double startTime = 0;
	static double timeAccum = 0;
	static double currentTime = 0;

	// This is a way to get a more accurate time between frames.
	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	t1.QuadPart = t2.QuadPart = 0;

	// Get ticks per second
	QueryPerformanceFrequency(&frequency);
	
	double timeStep = 1.0 / 60.0;

	GAMEFRAMEWORK->SetTimeSinceLastFrame(timeStep);

	while(!m_bShutdown) 
	{
		if(GameFramework::getSingletonPtr()->renderWindow_->isClosed())
		{
			m_bShutdown = true;
		}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		Ogre::WindowEventUtilities::messagePump();
#endif	
		if(GameFramework::getSingletonPtr()->renderWindow_->isActive())
		{

			QueryPerformanceCounter(&t2);

			// Get the processor time.
			LONGLONG qpart = (t2.QuadPart - t1.QuadPart);
			// There is some weird math error here, I don't know what it is but
			// I had to multiply by 10000000 or it gave wrong results.
			timeSinceLastFrame = LONGLONG( (qpart * 10000000) / frequency.QuadPart );
			timeSinceLastFrame /= 10000000.0;

			QueryPerformanceCounter(&t1);

			// Prevent large timesteps.
			if(timeSinceLastFrame > .0250)
			{
				timeSinceLastFrame = .0250;
			}
			
			timeAccum += timeSinceLastFrame;

			GAMEFRAMEWORK->SetRealTimeSinceLastFrame(timeSinceLastFrame);
			
			// If the timeAccum is corrupted and somehow is below -0.1
			// we will set it back to zero.
			if(timeAccum < -0.1)
			{
				timeAccum = 0;
			}
			
			// Update the top AppState, DispatchMessages and make Ogre render.
			while(timeAccum >= timeStep && m_bShutdown == false)
			{

				GAMEFRAMEWORK->keyboard_->capture();
				GAMEFRAMEWORK->mouse_->capture();

				if(m_bShutdown == false)
				{
						Dispatch->DispatchDelayedMessages();

						m_ActiveStateStack.back()->update(timeStep);

						GAMEFRAMEWORK->UpdateOgre(timeStep);
						GAMEFRAMEWORK->root_->renderOneFrame();

						timeAccum -= timeStep;
					
				}

			}
			
			if(popState_)
			{
				popState_ = false;
				popAppState();
			}

			if(pushState_)
			{
				pushAppState( findByName(pushToState_) );
				pushState_ = false;
			}

			// Sleep for 5ms, we can't possibly need to update this fast.
			Sleep(5);
		}
		else
		{
			Sleep(1000);
		}
	}

	GameFramework::getSingletonPtr()->log_->logMessage("Main loop quit");
	GameFramework::getSingletonPtr()->log_->logMessage("Shutdown OGRE...");
}

//=============================================================================
//							changeAppState
//
/// Changes to a different AppState. The current AppState has nothing done to
/// it, but it will not be updated anymore as it is notthe top AppState. The 
/// AppState passed in is pushed onto the stack of AppStates.
void AppStateManager::changeAppState(AppState* state)
{
	if(!m_ActiveStateStack.empty()) 
	{
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	m_ActiveStateStack.push_back(state);
	init(state);
	m_ActiveStateStack.back()->enter();
}



//=============================================================================
//							pushAppState
//
/// Pushes an AppState to the top of the stack and pauses the current AppState.
bool AppStateManager::pushAppState(AppState* state)
{
	if(!m_ActiveStateStack.empty()) 
	{
		if(!m_ActiveStateStack.back()->pause())
			return false;
	}

	m_ActiveStateStack.push_back(state);
	init(state);
	m_ActiveStateStack.back()->enter();

	return true;
}

//=============================================================================
//							pushAppState
//
/// Pushes an AppState, given by the state's name, to the top of the stack and
/// pauses the current AppState.
bool AppStateManager::pushAppState(Ogre::String stateName)
{
	pushState_ = true;
	pushToState_ = stateName;

	return true;
}

//=============================================================================
//						popStateAfterNextUpdate
//
/// This sets it so that the current state will be popped after the current
/// update loop has finished.
void AppStateManager::popStateAfterNextUpdate()
{
	popState_ = true;
}

//=============================================================================
//							popAppState
//
/// This pops an AppState off the top of the stack. It calls exit on the top
/// AppState in the stack and then calls Resume on the new top AppState. If
/// there are no more AppStates after popping the application is shutdown.
void AppStateManager::popAppState(void)
{
	if(!m_ActiveStateStack.empty()) 
	{
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	if(!m_ActiveStateStack.empty()) 
	{
		init(m_ActiveStateStack.back());
		m_ActiveStateStack.back()->resume();
	}
    else
	{
		shutdown();
	}
}

//=============================================================================
//							shutdown
//
/// ShutDown the application.
void AppStateManager::shutdown()
{
	m_bShutdown = true;
}

//=============================================================================
//							init
//
/// Initialize an AppState. This sets the GameFrameworks keyboard and mouse to
/// be that of the passed AppState.
void AppStateManager::init(AppState* state)
{
	GameFramework::getSingletonPtr()->keyboard_->setEventCallback(state);
	GameFramework::getSingletonPtr()->mouse_->setEventCallback(state);

	GameFramework::getSingletonPtr()->renderWindow_->resetStatistics();
}
