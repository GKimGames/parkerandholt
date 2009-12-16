/*=============================================================================

  EventListener.h

  Author: Matt King

=============================================================================*/

#ifndef GAMEEVENTLISTENER_H
#define GAMEEVENTLISTENER_H

#include "Event.h"

namespace PaH 
{
	/// GameEventListener can receive GameEvents and handle them.
	/// ProcessEvent must be implemented so that the GameEvent actually
	/// has something done with it.
	///
	/// Each GameEvent in the gameEventQueue_ must have Release() called.
	class GameEventListener
	{

	public:

		/// Reserves memory for gameEventQueue_.
		GameEventListener()
		{
			// Reserving memory for a possible 10 events at a time
			gameEventQueue_.reserve(10);
		};

		/// Adds a GameEvent to gameEventQueue_
		virtual void HandleEvent(GameEvent* gameEvent)
		{
			gameEventQueue_.push_back(gameEvent);
		};


		/// Calls ProcessEvent on each GameEvent in the gameEventQueue_.
		virtual void ProcessEventQueue()
		{				     
			std::vector<PaH::GameEvent*>::iterator i = gameEventQueue_.begin();

			for( ;i != gameEventQueue_.end(); i++)
			{
				ProcessEvent((*i));
			}

		};

		
	
	protected:

		/// Process an event in the gameEventQueue_.
		/// Each event must have Release() called on it so the event
		/// can be deleted.
		virtual void ProcessEvent(GameEvent* gameEvent)=0;
		
		/// Queue for the GameEvents.
		/// GameEvents are added to this vector by HandleEvent,
		/// the GameEvents are then evaluatedd by ProcessEventQueue
		std::vector<GameEvent*> gameEventQueue_;
	
	};
}

#endif