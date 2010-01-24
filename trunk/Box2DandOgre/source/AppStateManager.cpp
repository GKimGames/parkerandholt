//|||||||||||||||||||||||||||||||||||||||||||||||

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

	while(!m_States.empty())
	{
		m_States.pop_back();
	}
}



//=============================================================================
//							manageAppState
//
void AppStateManager::manageAppState(Ogre::String stateName, AppState* state)
{
	try
	{
		state_info new_state_info;
		new_state_info.name = stateName;
		new_state_info.state = state;
		m_States.push_back(new_state_info);		
	}
	catch(std::exception& e)
	{
		delete state;
		throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Error while trying to manage a new AppState\n" + Ogre::String(e.what()), "AppStateManager.cpp " + __LINE__);
	}
}



//=============================================================================
//							findByName
//
/// 
AppState* AppStateManager::findByName(Ogre::String stateName)
{
	std::vector<state_info>::iterator itr;

	for(itr=m_States.begin();itr!=m_States.end();itr++)
	{
		if(itr->name==stateName)
			return itr->state;
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

	//m_BufferFlush.start(2);

	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	t1.QuadPart = t2.QuadPart = 0;

	// get ticks per second
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
			
			while(timeAccum >= timeStep && m_bShutdown == false)
			{

				GAMEFRAMEWORK->keyboard_->capture();
				GAMEFRAMEWORK->mouse_->capture();

				if(m_ActiveStateStack.back()->update(timeStep) && m_bShutdown == false)
				{
						Dispatch->DispatchDelayedMessages();

						GAMEFRAMEWORK->UpdateOgre(timeStep);
						GAMEFRAMEWORK->root_->renderOneFrame();

						timeAccum -= timeStep;
					
				}

			}
			
			/*
			GAMEFRAMEWORK->SetTimeSinceLastFrame(timeSinceLastFrame);
			GAMEFRAMEWORK->keyboard_->capture();
			GAMEFRAMEWORK->mouse_->capture();

			m_ActiveStateStack.back()->update(timeSinceLastFrame);

			Dispatch->DispatchDelayedMessages();

			GAMEFRAMEWORK->UpdateOgre(timeSinceLastFrame);
			GAMEFRAMEWORK->root_->renderOneFrame();
			*/

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
//							popAppState
//
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
void AppStateManager::shutdown()
{
	m_bShutdown=true;
}

//=============================================================================
//							init
//
void AppStateManager::init(AppState* state)
{
	GameFramework::getSingletonPtr()->keyboard_->setEventCallback(state);
	GameFramework::getSingletonPtr()->mouse_->setEventCallback(state);

	GameFramework::getSingletonPtr()->renderWindow_->resetStatistics();
}
