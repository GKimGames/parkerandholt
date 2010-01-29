/*=============================================================================

			FSMState.h

			Author: Matt King

			Finite State Machine State

=============================================================================*/
#ifndef FSMSTATE_H
#define FSMSTATE_H

#include <Box2D\Box2D.h>

struct KGBMessage;
struct ContactPoint;
template <class T> class FSMStateMachine;

/// This represents a state in a state machine. It has Enter, Update, Exit
/// functions so that the state can be entered, left, and updated by the
/// state machine.
///
/// NOTE:
/// It would probably be a bit better if these classes were singletons
/// and you handed it the object you wanted to control for each method,
/// instead of the state storing a pointer to the object.
template <class T>
class FSMState
{

public:
	FSMState(T* driver, FSMStateMachine<T>* stateMachine):
	  driver_(driver), stateMachine_(stateMachine){}

	virtual ~FSMState(){}

	/// This is called when the state is entered.
	virtual void Enter()=0;

	/// Update.
	virtual bool Update()=0;

	/// This will execute when the state is exited.
	virtual void Exit()=0;

/*=============================================================================
	These methods really should be in a class that extends FSMState
	but for the sake of convenience they are in here.
=============================================================================*/
	/// Used to handle messages.
	virtual bool HandleMessage(const KGBMessage message) { return false; }

	/// Called when two fixtures begin to touch.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){}

	/// Called when two fixtures cease to touch.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){}

	

	
protected:

	/// Pointer to the object that drives this state. This would be the object
	/// that the State Machine would be in.
	T* driver_;

	/// Pointer to the state machine this state is in.
	FSMStateMachine<T>* stateMachine_;


};

#endif