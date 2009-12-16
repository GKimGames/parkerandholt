/*=============================================================================
-------------------------------------------------------------------------------
  Event.h

  Author: Matt King

=============================================================================*/

#ifndef GAMEEVENT_H
#define GAMEEVENT_H

namespace PaH 
{
	/// This will identify what type of Game Event an event is.
	/// It has a void pointer to store some data about the event.
	enum GameEventType
	{
		Jump		//< \brief Jump event
	};


	/// A GameEvent has the GameEventType, an arg that is an integer and a void
	/// pointer. GameEvent has a reference count (refCount_) so that it can be
	/// deleted once all GameEventListeners are done with the event. The
	/// reference count is set by the EventDispatcher and it is equal to the
	/// amount of GameEventListeners.
	///
	/// Call Release() when done with a GameEvent.
	class GameEvent
	{
	public:

		/// GameEvent is an event in the game that is sent to all
		/// GameEventListeners.
		GameEvent(GameEventType gameEventType, int arg = 0, void* gameEventData = NULL)
		{
			gameEventType_ = gameEventType;
			arg_ = arg;
			gameEventData_ = gameEventData; 
		}

		void SetRefCount(int count)
		{
			refCount_ = count;
		}

		void Release()
		{
			refCount_--;
			if(refCount_ <= 0)
			{
				delete this;
			}
		}
		
		const GameEventType	GetType(){return gameEventType_;};
		int					GetArg(){return arg_;};
		void*				GetData(){return gameEventData_;};



	protected:

		 int refCount_;
		 GameEventType	gameEventType_;
		 int			arg_;
		 void*			gameEventData_;
	};
}

#endif