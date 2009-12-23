/*=============================================================================

	ParkerStateInAir.h

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/
#ifndef PARKER_STATE_IN_AIR_H
#define PARKER_STATE_IN_AIR_H

#include "FSMState.h"
#include "Parker.h"

class CharacterParker;
class ParkerStateInAir : public FSMState<CharacterParker>
{

public:

	ParkerStateInAir(CharacterParker* parker);

	~ParkerStateInAir(){}

	/// This is called when the state is entered.
	virtual void Enter();

	/// Update.
	virtual bool Update();

	/// Used to handle messages.
	virtual bool HandleMessage(const KGBMessage message);

	/// This will execute when the state is exited.
	virtual void Exit();

	/// Called when two fixtures begin to touch.
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	/// Called when two fixtures cease to touch.
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	
	void MoveLeft();
	void MoveRight();
	void Jump();

};

#endif
