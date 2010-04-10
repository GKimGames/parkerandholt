/*=============================================================================

	HoltStatePlacingStatic.h

	State for Holt when placing semi permanent boxes and deleting placeable objects

=============================================================================*/
#ifndef HOLT_STATE_PLACING_STATIC_H
#define HOLT_STATE_PLACING_STATIC_H

#include "FSMState.h"
#include "HoltState.h"

#include "Holt.h"
#include "MousePicking.h"
#include "GravityVector.h"
#include "Triangle.h"

class CharacterHolt;
class HoltStatePlacingStatic : public HoltState
{

public:

	HoltStatePlacingStatic(CharacterHolt* holt, FSMStateMachine<CharacterHolt>* stateMachine);

	~HoltStatePlacingStatic(){}

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

	/// Creates a temporary box at the mouse location
	bool SpawnTempBox();

	bool leftMouseDown_;
	
	

protected:

	void MoveLeft();
	void MoveRight();
	void Jump();
	void UpdateAnimation();

	bool blendingRun_;
	bool blendingIdle_;

	int				feetContactCount_;
	b2Body*			elevator_;

	EntityMaterialInstance*	trans_;
};

#endif