/*=============================================================================

		ParkerStateMachine.h

		Author: Matt King

		Finite State Machine

=============================================================================*/


#ifndef PARKERSTATEMACHINE_H
#define PARKERSTATEMACHINE_H

#include "Parker.h"
#include "ParkerState.h"

class CharacterParker;

/// Finite state machines have two states at a time, a global state and a
/// current state, both are updated every cycle if they exist. 
class ParkerStateMachine<CharacterParker> : FSMStateMachine<T>
{

public:


	ParkerStateMachine(CharacterParker* driver);

	/// Delete the current state and global state if they exist.
	virtual ~ParkerStateMachine(){};

	void PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse){};

	/// Update the FSM
	bool Update()
	{
		// If a global state exists update it.
		if(globalState_)
		{
			globalState_->Update();
		}

		// If a current state exists update it.
		if (currentState_)
		{
			return currentState_->Update();
		}

		return true;
	}

	/// Change current state to newState.
	/// This calls the exit method of the current state before calling the
	/// enter method of the new state.
	void ChangeState(ParkerState* newState)
	{
		previousState_ = currentState_;

		currentState_->Exit();

		currentState_ = newState;

		currentState_->Enter();
	}


	/// Change the current state back to the previous state. This calls
	/// ChangeState to change the state.
	void RevertToPreviousState()
	{
		ChangeState(previousState_);
	}



	/// Returns true if the current state's type is equal to the type of the
	/// class passed as a parameter. 
	bool IsInState(const ParkerState& state )const
	{
		return typeid(*currentState_) == typeid(state);
	}

	/// Send a message to the FSM
	bool HandleMessage(const KGBMessage message)
	{
		// If a global state exists hand it the message.
		if(globalState_)
		{
			globalState_->HandleMessage(message);
		}

		// If a current state exists hand it the message.
		if (currentState_)
		{
			return currentState_->HandleMessage(message);
		}

		return true;
	}


	/// Called when two fixtures begin to touch.
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		currentState_->BeginContact(contact,contactFixture, collidedFixture);
	}

	/// Called when two fixtures cease to touch.
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		currentState_->EndContact(contact,contactFixture, collidedFixture);
	}



	/*=============================================================================
	Getter / Setter methods
	=============================================================================*/

	void SetCurrentState(ParkerState*  state)  {  currentState_	 = state;}
	void SetGlobalState(ParkerState*   state)  {  globalState_	 = state;}
	void SetPreviousState(ParkerState* state)  {  previousState_   = state;}

	ParkerState*  GetCurrentState()  const { return currentState_;}
	ParkerState*  GetGlobalState()   const { return globalState_;}
	ParkerState*  GetPreviousState() const { return previousState_;}

protected:

	/// A pointer to the object that drives the state machine.
	T*				driver_;

	FSMState<T>*    currentState_;

	/// The previous state the machien was in.
	FSMState<T>*    previousState_;

	/// The global state is updated all of the time and regularly does not change.
	FSMState<T>*    globalState_;



};




#endif


