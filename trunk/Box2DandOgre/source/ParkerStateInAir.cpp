/*=============================================================================

	ParkerStateInAir.cpp

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/

#include "ParkerStateInAir.h"

//=============================================================================
//								Constructor
//
ParkerStateInAir::ParkerStateInAir(CharacterParker* parker):
FSMState(parker)
{
}

//=============================================================================
//								Enter
//
void ParkerStateInAir::Enter()
{

}

//=============================================================================
//								Update
//
bool ParkerStateInAir::Update()
{
	static Ogre::Vector3 direction;
	
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame() / 1000;

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

	/*
	b2XForm xform;
	xform.SetIdentity();
	b2AABB aabb;

	b2World* world = GAMEFRAMEWORK->GetWorld();
	owner_->feetSensor_->GetShape()->ComputeAABB(&aabb, xform);

	if(world->Query(aabb,NULL,0))
	{
		owner_->stateMachine_->ChangeState(owner_->onGroundState_);
	}
	*/

	return true;
}

//=============================================================================
//								HandleMessage
//
bool ParkerStateInAir::HandleMessage(const KGBMessage message)
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
void ParkerStateInAir::Exit()
{

}

//=============================================================================
//								BeginContact
//
/// Called when two fixtures begin to touch.
void ParkerStateInAir::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(!collidedFixture->IsSensor())
	{
		if(contactFixture == owner_->feetSensor_)
		{
			owner_->stateMachine_->ChangeState(owner_->onGroundState_);
			owner_->BeginContact(contact,contactFixture,collidedFixture);
		}
	}

}

//=============================================================================
//								EndContact
//
/// Called when two fixtures cease to touch.
void ParkerStateInAir::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{

}

//=============================================================================
//								MoveLeft
//
///
void ParkerStateInAir::MoveLeft()
{

	double timeSinceLastFrame = GameFramework::getSingletonPtr()->GetTimeSinceLastFrame() / 1000;

	if(owner_->body_->GetLinearVelocity().x > -owner_->maximumAirVelocity_)
	{
		owner_->body_->ApplyForce(b2Vec2(-owner_->airForce_ * timeSinceLastFrame,0), owner_->body_->GetPosition());
	}

}

//=============================================================================
//								MoveRight
//
/// 
void ParkerStateInAir::MoveRight()
{
	double timeSinceLastFrame = GameFramework::getSingletonPtr()->GetTimeSinceLastFrame() / 1000;

	if(owner_->body_->GetLinearVelocity().x < owner_->maximumAirVelocity_)
	{
		owner_->body_->ApplyForce(b2Vec2(owner_->airForce_ * timeSinceLastFrame,0), owner_->body_->GetPosition());
	}

}


//=============================================================================
//								Jump
///
/// Calling jump while the character is in the air will add slightly more
/// height to the jump.
void ParkerStateInAir::Jump()
{

	double timeLeft = (owner_->animationState_->getLength() - owner_->animationState_->getTimePosition()) / owner_->animationState_->getLength();
	owner_->body_->ApplyForce(b2Vec2(0,owner_->jumpingAfterForce_ * timeLeft), owner_->body_->GetPosition());


	// Code to wall jump
	/*
	if(torsoSensorHitCount_ > 0 && justWallJumped_ == false)
	{
		body_->SetLinearVelocity(b2Vec2(0,body_->GetLinearVelocity().y)); 
		body_->ApplyImpulse(b2Vec2(-jumpingForce_/2,jumpingForce_), body_->GetPosition());
		justWallJumped_ = true;

		animationState_->setEnabled(false);
		animationState_ = entity_->getAnimationState("JumpNoHeight");
		animationState_->setTimePosition(Ogre::Real(0));
		animationState_->setLoop(false);
		animationState_->setEnabled(true);
	}
	*/

}