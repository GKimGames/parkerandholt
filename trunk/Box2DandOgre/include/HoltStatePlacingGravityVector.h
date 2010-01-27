#ifndef HOLT_STATE_PLACING_GRAVITY_VECTOR_H
#define HOLT_STATE_PLACING_GRAVITY_VECTOR_H

#include "FSMState.h"
#include "Parker.h"
#include "MousePicking.h"
#include "Platform.h"
#include "GravityVector.h"

class CharacterParker;
class HoltStatePlacingGravityVector : public FSMState<CharacterParker>
{

public:

	HoltStatePlacingGravityVector(CharacterParker* parker, FSMStateMachine<CharacterParker>* stateMachine);

	~HoltStatePlacingGravityVector(){}

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

	bool SpawnBox();
	bool SpawnGravityVector();
	
	

protected:

	void MoveLeft();
	void MoveRight();
	void Jump();
	void UpdateAnimation();

	bool blendingRun_;
	bool blendingIdle_;

	int				feetContactCount_;
	b2Body*			elevator_;
	b2Vec2			startPosition_;
	b2Vec2			endPosition_;
	GravityVector*	gravityVector_;
	bool			deleteVector_;
	bool			createVector_;

};

#endif