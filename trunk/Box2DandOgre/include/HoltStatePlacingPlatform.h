/*=============================================================================

	HoltStatePlacingPlatform.h

	State for Holt when placing platforms, boxes and gravity vector

=============================================================================*/
#ifndef HOLT_STATE_PLACING_PLATFORM_H
#define HOLT_STATE_PLACING_PLATFORM_H

#include "FSMState.h"
#include "HoltState.h"

#include "Holt.h"
#include "MousePicking.h"
#include "GravityVector.h"
#include "HoltBox.h"
#include "Platform.h"

class CharacterHolt;
class HoltStatePlacingPlatform : public HoltState
{

public:

	HoltStatePlacingPlatform(CharacterHolt* Holt, FSMStateMachine<CharacterHolt>* stateMachine);

	~HoltStatePlacingPlatform(){}

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

	GravityVector* GetGravityVector();

	/// Creates a holt box
	bool SpawnBox();

	/// Creates a platform
	bool SpawnPlatform();

	/// Updates the gravity vector, one is precreated to avoid time spent creating a new partical system
	bool SpawnGravityVector();

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

	// Start and end positions of when the mouse is clicked and released
	b2Vec2			startPosition_;
	b2Vec2			endPosition_;

	// Pointer for platform when mouse is being dragged
	Platform*		platform_;
	GravityVector*	gravityVector_;
	// Maximum lenght a platform can be
	float			maxLength_;

	EntityMaterialInstance*	trans_;
};

#endif