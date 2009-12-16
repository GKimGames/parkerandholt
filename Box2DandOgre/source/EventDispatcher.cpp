/*=============================================================================
-------------------------------------------------------------------------------
  EventDispatcher.cpp

  Author: Matt King

=============================================================================*/

#include "EventDispatcher.h"

using namespace PaH;

template<> GameEventDispatcher* Ogre::Singleton<GameEventDispatcher>::ms_Singleton = 0;

GameEventDispatcher::GameEventDispatcher()
{
	// Start with enough room for 5 event listeners
	gameEventListeners_.reserve(5);
}

GameEventDispatcher::~GameEventDispatcher()
{
	gameEventListeners_.clear();
}

void GameEventDispatcher::DispatchEvent(GameEvent* gameEvent)
{
	gameEvent->SetRefCount(gameEventListeners_.size());

	std::vector<GameEventListener*>::iterator i = gameEventListeners_.begin();
	for( ;i != gameEventListeners_.end(); i++)
	{
		
		(*i)->HandleEvent(gameEvent);
	}
}

void GameEventDispatcher::RegisterListener(GameEventListener* gameEventListener)
{
	gameEventListeners_.push_back(gameEventListener);
}

void GameEventDispatcher::RemoveListener(GameEventListener* gameEventListener)
{
	std::vector<GameEventListener*>::iterator iter;
	
	while (true)
	{
		iter = find(gameEventListeners_.begin(),gameEventListeners_.end(), gameEventListener);
		
		if (iter==gameEventListeners_.end())
		{
			break;
		}

		gameEventListeners_.erase(iter);
	}
}
