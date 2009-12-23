/*=============================================================================

	ParkerStateOnGround.cpp

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/

#include "ParkerStateOnGround.h"

//=============================================================================
//								Constructor
//
ParkerStateOnGround::ParkerStateOnGround(CharacterParker* parker):
FSMState(parker)
{
}

//=============================================================================
//								Enter
//
void ParkerStateOnGround::Enter()
{

	feetContactCount_ = 0;
}

//=============================================================================
//								Update
//
bool ParkerStateOnGround::Update()
{
	static Ogre::Vector3 direction;
	
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame() / 1000;
	
	owner_->ApplyWalkingFriction(timeSinceLastFrame);

	b2Vec2 v = owner_->body_->GetPosition();
	owner_->sceneNode_->setPosition(v.x, v.y,0);

	if(owner_->body_->GetLinearVelocity().x > 0.1)
	{
		direction = Ogre::Vector3(0,0,1);
	}
	else if(owner_->body_->GetLinearVelocity().x < -0.1)
	{
		direction = Ogre::Vector3(0,0,-1);
	}

	owner_->sceneNode_->setDirection(direction,Ogre::Node::TS_WORLD);

	owner_->UpdateAnimation(timeSinceLastFrame);

	if(feetContactCount_ < 1)
	{
		//owner_->stateMachine_->ChangeState(owner_->inAirState_);
	}

	return true;
}

//=============================================================================
//								HandleMessage
//
bool ParkerStateOnGround::HandleMessage(const KGBMessage message)
{

	switch(message.messageType)
	{
		case CHARACTER_MOVE_LEFT:
			{
				MoveLeft();
				return true;
			}
		case CHARACTER_MOVE_RIGHT:
			{
				MoveRight();
				return true;
			}
		case CHARACTER_JUMP:
			{
				Jump();
				return true;
			}
	}

	return false;
}

//=============================================================================
//								Exit
//
void ParkerStateOnGround::Exit()
{

}

//=============================================================================
//								BeginContact
//
/// Called when two fixtures begin to touch.
void ParkerStateOnGround::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(!collidedFixture->IsSensor())
	{
		if(contactFixture == owner_->feetSensor_)
		{
			feetContactCount_++;
		}
	}
}

//=============================================================================
//								EndContact
//
/// Called when two fixtures cease to touch.
void ParkerStateOnGround::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(!collidedFixture->IsSensor())
	{
		if(contactFixture == owner_->feetSensor_)
		{

			feetContactCount_--;
			if(feetContactCount_ == 0)
			{
				owner_->stateMachine_->ChangeState(owner_->inAirState_);
			}
			
		}
	}
}


//=============================================================================
//								MoveLeft
//
///
void ParkerStateOnGround::MoveLeft()
{

	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame() / 1000;

	if(owner_->body_->GetLinearVelocity().x > -owner_->maximumRunningVelocity_)
	{
		owner_->body_->ApplyForce(b2Vec2(-owner_->runningForce_ * timeSinceLastFrame,0), owner_->body_->GetPosition());
	}

}

//=============================================================================
//								MoveRight
//
/// 
void ParkerStateOnGround::MoveRight()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame() / 1000;

	if(owner_->body_->GetLinearVelocity().x < owner_->maximumRunningVelocity_)
	{
		owner_->body_->ApplyForce(b2Vec2(owner_->runningForce_ * timeSinceLastFrame,0), owner_->body_->GetPosition());
	}

}


//=============================================================================
//								Jump
///
/// Calling jump while the character is in the air will add slightly more
/// height to the jump.
void ParkerStateOnGround::Jump()
{

	owner_->body_->ApplyImpulse(b2Vec2(0,owner_->jumpingForce_), owner_->body_->GetPosition());

	owner_->animationState_->setEnabled(false);
	owner_->animationState_ = owner_->entity_->getAnimationState("JumpNoHeight");
	owner_->animationState_->setLoop(false);
	owner_->animationState_->setEnabled(true);

}