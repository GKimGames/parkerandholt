/*=============================================================================

	ParkerStateInAir.h

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/
#ifndef PARKER_STATE_IN_AIR_H
#define PARKER_STATE_IN_AIR_H

#include "FSMState.h"
#include "ParkerState.h"


class CharacterParker;

/// State for Parker in the Air.
class ParkerStateInAir : public ParkerState
{

public:

	ParkerStateInAir(CharacterParker* parker, FSMStateMachine<CharacterParker>* stateMachine);

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

protected:

	/// MoveLeft function.
	void MoveLeft();

	/// MoveRight function.
	void MoveRight();

	/// Jump function.
	void Jump();

	/// Update CharacterParker's AnimationBlender.
	void UpdateAnimation();

	Ogre::Vector3 direction_;

	double	jumpTimer_;
	bool	wallJumpedLeft_;
	bool	justWallJumped_;
	double	wallJumpTimer_;

	double  wallJumpBetweenTimer_;

	bool mySwitch;

};

#endif
