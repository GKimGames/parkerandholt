/*=============================================================================

	ParkerStateLedgeGrab.cpp

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/

#include "ParkerStateLedgeGrab.h"

#include "Parker.h"
#include "Message.h"


//=============================================================================
//								Constructor
//
ParkerStateLedgeGrab::ParkerStateLedgeGrab(
	CharacterParker* parker,
	FSMStateMachine<CharacterParker>* stateMachine):
	FSMState<CharacterParker>(parker,stateMachine)
{
}

//=============================================================================
//								Enter
//
void ParkerStateLedgeGrab::Enter()
{
	//b2RevoluteJointDef jdef;
	//jdef.bodyA = driver_->body_;
	//driver_->world_->CreateJoint();

}

//=============================================================================
//								Update
//
bool ParkerStateLedgeGrab::Update()
{
	if(driver_->moveLeft_)
	{
		MoveLeft();
	}
	if(driver_->moveRight_)
	{
		MoveRight();
	}
	if(driver_->jump_)
	{
		//Jump();
		;
	}
	static Ogre::Vector3 direction;
	
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	if(driver_->feetSensorHitCount_ > 0)
	{
		stateMachine_->ChangeState(driver_->onGroundState_);
	}
	else
	{
		// Set the position of the scene node to that of the Box2D body
		b2Vec2 v = driver_->body_->GetPosition();
		driver_->sceneNode_->setPosition(v.x, v.y,0);

		// Make the scene face the direction the body is moving
		if(driver_->body_->GetLinearVelocity().x > 0.1)
		{
			direction = Ogre::Vector3(0,0,1);
		}
		else if(driver_->body_->GetLinearVelocity().x < -0.1)
		{
			direction = Ogre::Vector3(0,0,-1);
		}

		driver_->sceneNode_->setDirection(direction,Ogre::Node::TS_WORLD);

		UpdateAnimation();

		if(driver_->debugDrawOn_)
		{
			driver_->UpdateDebugDraw();
		}

	}

	// We've successfully updated.
	return true;
}

//=============================================================================
//								HandleMessage
//
bool ParkerStateLedgeGrab::HandleMessage(const KGBMessage message)
{

	switch(message.messageType)
	{
		case CHARACTER_MOVE_LEFT_PLUS:
		{
			driver_->moveLeft_ = true;
			//MoveLeft();
			return true;
		}
		case CHARACTER_MOVE_RIGHT_PLUS:
		{
			driver_->moveRight_ = true;
			//MoveRight();
			return true;
		}
		case CHARACTER_JUMP_PLUS:
		{
			driver_->jump_ = true;
			//Jump();
			return true;
		}

		case CHARACTER_MOVE_LEFT_MINUS:
		{
			driver_->moveLeft_ = false;
			//MoveLeft();
			return true;
		}
		case CHARACTER_MOVE_RIGHT_MINUS:
		{
			driver_->moveRight_ = false;
			//MoveRight();
			return true;
		}
		case CHARACTER_JUMP_MINUS:
		{
			driver_->jump_ = false;
			//Jump();
			return true;
		}
	}

	return false;
}

//=============================================================================
//								Exit
//
void ParkerStateLedgeGrab::Exit()
{

}

//=============================================================================
//								BeginContact
//
/// Called when two fixtures begin to touch.
void ParkerStateLedgeGrab::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(!collidedFixture->IsSensor())
	{
		if(contactFixture == driver_->feetSensor_)
		{
			stateMachine_->ChangeState(driver_->onGroundState_);
			driver_->BeginContact(contact,contactFixture,collidedFixture);
		}
	}

}

//=============================================================================
//								EndContact
//
/// Called when two fixtures cease to touch.
void ParkerStateLedgeGrab::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{

}

//=============================================================================
//								MoveLeft
//
///
void ParkerStateLedgeGrab::MoveLeft()
{

	double timeSinceLastFrame = GameFramework::getSingletonPtr()->GetTimeSinceLastFrame();

	if(driver_->body_->GetLinearVelocity().x > -driver_->maximumAirVelocity_)
	{
		driver_->body_->ApplyForce(b2Vec2(-driver_->airForce_ * timeSinceLastFrame,0), driver_->body_->GetPosition());
	}

}

//=============================================================================
//								MoveRight
//
/// 
void ParkerStateLedgeGrab::MoveRight()
{
	double timeSinceLastFrame = GameFramework::getSingletonPtr()->GetTimeSinceLastFrame();

	if(driver_->body_->GetLinearVelocity().x < driver_->maximumAirVelocity_)
	{
		driver_->body_->ApplyForce(b2Vec2(driver_->airForce_ * timeSinceLastFrame,0), driver_->body_->GetPosition());
	}

}


//=============================================================================
//								Climb
///
/// Getting a jump message while the character is grabbing a ledge will make
/// him climb.
void ParkerStateLedgeGrab::Climb()
{

	double timeLeft = (driver_->animationState_->getLength() - driver_->animationState_->getTimePosition()) / driver_->animationState_->getLength();
	driver_->body_->ApplyForce(b2Vec2(0,driver_->jumpingAfterForce_ * timeLeft), driver_->body_->GetPosition());

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

//=============================================================================
//								UpdateAnimation
void ParkerStateLedgeGrab::UpdateAnimation()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	driver_->animationState_->addTime(timeSinceLastFrame);
}