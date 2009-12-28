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
	isJumping_ = false;
	moveLeftDown_ = false;
	moveRightDown_ = false;
	feetContactCount_ = 0;
}

//=============================================================================
//								Enter
//
void ParkerStateOnGround::Enter()
{

	isJumping_ = false;
	moveLeftDown_ = false;
	moveRightDown_ = false;
	
	feetContactCount_ = 0;
	jumpTimer_ = owner_->timeBetweenJump_;

	if(owner_->initialized_)
	{
		owner_->animationState_->setEnabled(false);
		owner_->animationState_ = owner_->entity_->getAnimationState("Walk");
		owner_->animationState_->setLoop(true);
		owner_->animationState_->setEnabled(true);
	}
}

//=============================================================================
//								Update
//
bool ParkerStateOnGround::Update()
{
	static Ogre::Vector3 direction;
	
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();
	
	if(owner_->feetSensorHitCount_ == 0)
	{
		owner_->stateMachine_->ChangeState(owner_->inAirState_);
	}
	else
	{

		jumpTimer_ -= timeSinceLastFrame;

		if(jumpTimer_ < 0.001)
		{
			jumpTimer_ = 0;
			isJumping_ = false;
		}

		//if(moveRightDown_ || moveLeftDown_)
		{
			owner_->ApplyWalkingFriction(timeSinceLastFrame);
		}

		moveRightDown_ = false;
		moveLeftDown_ = false;
		

		if(owner_->body_->GetLinearVelocity().x > 0.1)
		{
			direction = Ogre::Vector3(0,0,1);
		}
		else if(owner_->body_->GetLinearVelocity().x < -0.1)
		{
			direction = Ogre::Vector3(0,0,-1);
		}

		UpdateAnimation();

		b2Vec2 v = owner_->body_->GetPosition();
		owner_->sceneNode_->setPosition(Ogre::Real(v.x),Ogre::Real(v.y),0);
		owner_->sceneNode_->setDirection(direction,Ogre::Node::TS_WORLD);
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
	owner_->elevator_ = NULL;
	owner_->feetSensorHitCount_ = 0;                                                                              
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

			if(collidedFixture->GetBody()->GetUserData())
			{
				GameObject* go = (GameObject*) collidedFixture->GetBody()->GetUserData();
				int type = go->GetGameObjectType();

				if(owner_->elevator_ == NULL)
				{
					owner_->elevator_ = collidedFixture->GetBody();
				}

			}
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

			if(elevator_ == collidedFixture->GetBody())
			{
				owner_->elevator_ = NULL;
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

	if(isJumping_ == false)
	{
		moveLeftDown_ = true;
		double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

		if(owner_->body_->GetLinearVelocity().x > -owner_->maximumRunningVelocity_)
		{
			owner_->body_->ApplyForce(b2Vec2(-owner_->runningForce_ * timeSinceLastFrame,0), owner_->body_->GetPosition());
		}
	}

}

//=============================================================================
//								MoveRight
//
/// 
void ParkerStateOnGround::MoveRight()
{
	if(isJumping_ == false)
	{
		moveRightDown_ = true;
		double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();
		
		if(owner_->body_->GetLinearVelocity().x < owner_->maximumRunningVelocity_)
		{
			owner_->body_->ApplyForce(b2Vec2(owner_->runningForce_ * timeSinceLastFrame,0), owner_->body_->GetPosition());
		}
	}

}


//=============================================================================
//								Jump
///
/// Jump!
void ParkerStateOnGround::Jump()
{

	if(jumpTimer_ > 0)
	{
	
	}
	else
	{

		owner_->body_->ApplyImpulse(b2Vec2(0,owner_->jumpingForce_), owner_->body_->GetPosition());

		owner_->animationState_->setEnabled(false);
		owner_->animationState_ = owner_->entity_->getAnimationState("JumpNoHeight");
		owner_->animationState_->setTimePosition(0);
		owner_->animationState_->setLoop(false);
		owner_->animationState_->setEnabled(true);

		isJumping_ = true;

		jumpTimer_ = owner_->timeBetweenJump_;
	}

}

//=============================================================================
//								UpdateAnimation
//
void ParkerStateOnGround::UpdateAnimation()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	b2Vec2 lv = owner_->body_->GetLinearVelocity();
	
	if(owner_->elevator_ != NULL)
	{
		lv -= owner_->elevator_->GetLinearVelocity();
	}

	if(abs(lv.x)  < .1)
	{
		owner_->animationState_->setTimePosition(0.8);
	}
	else
	{
		double ratio = owner_->maximumRunningVelocity_ / lv.x;

		owner_->animationState_->addTime(timeSinceLastFrame / abs(ratio));
	}
}