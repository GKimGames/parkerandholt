/*=============================================================================

	FSMStateMachine.h

	Author: Matt King

	Finite State Machine

=============================================================================*/

#ifndef FSMSTATEMACHINE_H
#define FSMSTATEMACHINE_H

#include <cassert>
#include <string>

#include "FSMState.h"


template <class T>
class FSMStateMachine
{

public:

	FSMStateMachine(T* owner):owner_(owner),
		currentState_(NULL),
		previousState_(NULL),
		globalState_(NULL)
	{}

	virtual ~FSMStateMachine(){}

	// Use these methods to initialize the FSM
	void SetCurrentState(FSMState<T>* s)  {  currentState_	= s;}
	void SetGlobalState(FSMState<T>* s)   {  globalState_	= s;}
	void SetPreviousState(FSMState<T>* s) {  previousState_	= s;}

	/// Update the FSM
	void Update() const
	{
		// If a global state exists update it.
		if(globalState_)
		{
			globalState_->Update(owner_);
		}

		if (currentState_)
		{
			currentState_->Update(owner_);
		}
	}

	/// Change current state to newState.
	/// This calls the exit method of the current state before calling the
	/// enter method of the new state.
	void ChangeState(FSMState<T>* newState)
	{
		assert(newState && 
			"<FSMStateMachine::ChangeState>: trying to change to NULL state");

		// Keep a record of the previous state
		previousState_ = currentState_;

		// Call the exit method of the existing state
		currentState_->Exit(owner_);

		// Change state to the new state
		currentState_ = newState;

		// Call the entry method of the new state
		currentState_->Enter(owner_);
	}

	/// Change the current state back to the previous state
	void RevertToPreviousState()
	{
		ChangeState(previousState_);
	}

	/// Returns true if the current state's type is equal to the type of the
	/// class passed as a parameter. 
	bool IsInState(const FSMState<T>& st)const
	{
		return typeid(*currentState_) == typeid(st);
	}

	FSMState<T>*  GetCurrentState()  const { return currentState_;}
	FSMState<T>*  GetGlobalState()   const { return globalState_;}
	FSMState<T>*  GetPreviousState() const { return previousState_;}

private:

	// A pointer to the agent that owns this instance
	T*          owner_;

	FSMState<T>*   currentState_;

	// A record of the last state the agent was in
	FSMState<T>*   previousState_;

	// This is called every time the FSM is updated
	FSMState<T>*   globalState_;
};




#endif


