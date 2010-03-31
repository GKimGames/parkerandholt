/*=============================================================================

	HoltStateInAir.h

	State for Holt on the ground.

=============================================================================*/
#ifndef HOLT_STATE_IN_AIR_H
#define HOLT_STATE_IN_AIR_H

#include "FSMState.h"
#include "HoltState.h"


class CharacterHolt;
class HoltStateOnGround;

class HoltStateInAir : public HoltState
{

public:

	HoltStateInAir(CharacterHolt* holt, FSMStateMachine<CharacterHolt>* stateMachine);

	~HoltStateInAir(){}

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

	void MoveLeft();
	void MoveRight();
	void Jump();
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