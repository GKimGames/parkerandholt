/*=============================================================================

	ParkerStateOnGround.h

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/
#ifndef PARKER_STATE_ON_GROUND_H
#define PARKER_STATE_ON_GROUND_H

#include "FSMState.h"
#include "Parker.h"

class CharacterParker;
class ParkerStateOnGround : public FSMState<CharacterParker>
{

public:

	ParkerStateOnGround(CharacterParker* parker);

	~ParkerStateOnGround(){}

	/// This is called when the state is entered.
	virtual void Enter();

	/// Update.
	virtual bool Update();

	/// Used to handle messages.
	virtual bool HandleMessage(const KGBMessage message);

	/// Called when two fixtures begin to touch.
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	/// Called when two fixtures cease to touch.
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	/// This will execute when the state is exited.
	virtual void Exit();
	
	void MoveLeft();
	void MoveRight();
	void Jump();

protected:

	int feetContactCount_;

};

#endif
