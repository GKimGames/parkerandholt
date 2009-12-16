/*=============================================================================
-------------------------------------------------------------------------------
  EventDispatcher.h

  Author: Matt King

=============================================================================*/

#ifndef GAMEEVENTDISPATCHER_H
#define GAMEEVENTDISPATCHER_H

#include "OgreSingleton.h"
#include <vector>

#include "EventListener.h"

#define REGISTEREVENTLISTENER(x) PaH::GameEventDispatcher::getSingletonPtr()->RegisterListener(x)
#define REMOVEREVENTLISTENER(x) PaH::GameEventDispatcher::getSingletonPtr()->RemoveListener(x)
#define DISPATCHEVENT(x) PaH::GameEventDispatcher::getSingletonPtr()->DispatchEvent(x)

namespace PaH 
{
	class GameEventDispatcher : public Ogre::Singleton<GameEventDispatcher>
	{
	public:
		GameEventDispatcher();
		~GameEventDispatcher();

		void DispatchEvent(GameEvent* gameEvent);
		void RegisterListener(GameEventListener* gameEventListener);
		void RemoveListener(GameEventListener* gameEventListener);	
	
	private:
		std::vector<GameEventListener*> gameEventListeners_;
		GameEventDispatcher( GameEventDispatcher&);
		GameEventDispatcher & operator= ( GameEventDispatcher&);
	};
}

#endif
