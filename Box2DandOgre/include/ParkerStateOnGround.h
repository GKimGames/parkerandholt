/*=============================================================================

	ParkerStateOnGround.h

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/
#ifndef PARKER_STATE_ON_GROUND_H
#define PARKER_STATE_ON_GROUND_H

#include "FSMState.h"
#include "ParkerState.h"

class CharacterParker;
class ParkerStateOnGround : public ParkerState
{

public:

	friend class ParkerStateInAir;

	ParkerStateOnGround(CharacterParker* parker, ParkerStateMachine* stateMachine);

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
	
	int GetFeetContactCount();
	

protected:

	void MoveLeft();
	void MoveRight();
	void Jump();
	void UpdateAnimation();

	

	bool isJumping_;

	bool blendingRun_;
	bool blendingIdle_;

	double jumpTimer_;

	bool moveLeftDown_;
	bool moveRightDown_;

	int feetContactCount_;

	/// This is an object that will be moving the character
	/// when the character is on it.
	b2Body* elevator_;

};

#endif