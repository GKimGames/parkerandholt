/*=============================================================================

	HoltStateOnGround.h

	Author: Matt King

	State for Holt on the ground.

=============================================================================*/
#ifndef HOLT_STATE_ON_GROUND_H
#define HOLT_STATE_ON_GROUND_H

#include "FSMState.h"
#include "HoltState.h"

class CharacterHolt;
class HoltStateOnGround : public HoltState
{

public:

	friend class HoltStateInAir;

	HoltStateOnGround(CharacterHolt* holt, HoltStateMachine* stateMachine);

	~HoltStateOnGround(){}

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

	Ogre::Vector3 direction_;	

	bool isJumping_;

	bool blendingRun_;
	bool blendingIdle_;

	double jumpTimer_;

	bool moveLeftDown_;
	bool moveRightDown_;

	int feetContactCount_;

	//Ogre::Vector3 direction;
	/// This is an object that will be moving the character
	/// when the character is on it.
	b2Body* elevator_;

};

#endif
