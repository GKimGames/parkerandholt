/*=============================================================================

	FSMState.h

	Author: Matt King

	Finite State Machine State

=============================================================================*/
#ifndef FSMSTATE_H
#define FSMSTATE_H

#include "Message.h"
#include "GameObjectOgreBox2D.h"
#include "GameFramework.h"

template <class T>
class FSMState
{
public:


	FSMState(T* owner):owner_(owner){}

	virtual ~FSMState(){}

	/// This is called when the state is entered.
	virtual void Enter()=0;

	/// Update.
	virtual bool Update()=0;

	/// Used to handle messages.
	virtual bool HandleMessage(const KGBMessage message)=0;

	/// Called when two fixtures begin to touch.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)=0;

	/// Called when two fixtures cease to touch.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)=0;

	/// This will execute when the state is exited.
	virtual void Exit()=0;

protected:

	T* owner_;


};

#endif