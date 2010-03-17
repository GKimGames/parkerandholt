/*=============================================================================

	ParkerStateOnGround.cpp

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/

#include "ParkerStateOnGround.h"

#include "ParkerStateInAir.h"
#include "HoltStatePlacingGravityVector.h"
#include "HoltStatePlacingPlatform.h"
#include "HoltStatePlacingStatic.h"
#include "Door.h"

#include "Parker.h"
#include "Message.h"


//=============================================================================
//								Constructor
//
ParkerStateOnGround::ParkerStateOnGround(	
	CharacterParker* parker,
	ParkerStateMachine* stateMachine):
	ParkerState(parker,stateMachine)
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

	blendingRun_ = false;
	blendingIdle_ = false;
	isJumping_ = false;
	moveLeftDown_ = false;
	moveRightDown_ = false;

	jumpTimer_ = driver_->timeBetweenJump_;

	if(driver_->stateMachine_->GetPreviousState() == driver_->inAirState_)
	{
		feetContactCount_ = 0;
	}

}



//=============================================================================
//								Update
//
bool ParkerStateOnGround::Update()
{
	if(driver_->active_)
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
			Jump();
		}
	}

	static Ogre::Vector3 direction;
	
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();
	
	//if(driver_->feetSensorHitCount_ == 0)
	if(driver_->feetSensorHit_ == false)
	{
		stateMachine_->ChangeState(driver_->inAirState_);
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
			driver_->ApplyWalkingFriction(timeSinceLastFrame);
		}

		moveRightDown_ = false;
		moveLeftDown_ = false;
		

		if(driver_->body_->GetLinearVelocity().x > 0.1)
		{
			direction = Ogre::Vector3(0,0,1);
		}
		else if(driver_->body_->GetLinearVelocity().x < -0.1)
		{
			direction = Ogre::Vector3(0,0,-1);
		}

		UpdateAnimation();

		b2Vec2 v = driver_->body_->GetPosition();
		driver_->sceneNode_->setPosition(Ogre::Real(v.x),Ogre::Real(v.y),0);
		driver_->sceneNode_->setDirection(direction,Ogre::Node::TS_WORLD);

		if(driver_->debugDrawOn_)
		{
			driver_->UpdateDebugDraw();
		}
	}

	return true;
}



//=============================================================================
//								HandleMessage
//
bool ParkerStateOnGround::HandleMessage(const KGBMessage message)
{
  std::string* gameObject = 0;
  if(driver_->active_)
  {
	switch(message.messageType)
	{
		case CHARACTER_MOVE_LEFT_PLUS:
		{
			driver_->moveLeft_ = true;
			return true;
		}
		case CHARACTER_MOVE_RIGHT_PLUS:
		{
			driver_->moveRight_ = true;
			return true;
		}
		case CHARACTER_JUMP_PLUS:
		{
			driver_->jump_ = true;
			return true;
		}
		case CHARACTER_MOVE_LEFT_MINUS:
		{

			driver_->moveLeft_ = false;
			return true;
		}
		case CHARACTER_MOVE_RIGHT_MINUS:
		{
			driver_->moveRight_ = false;
			return true;
		}
		case CHARACTER_JUMP_MINUS:
		{
			driver_->jump_ = false;
			return true;
		}
		case CHARACTER_ENTER_PLATFORMSTATE:
		{
			if(driver_->GetId() == driver_->GetHoltId())
			{
				driver_->stateMachine_->ChangeState(driver_->placingPlatform_);
			}
			return true;
		}
		case CHARACTER_ENTER_STATICSTATE:
		{
			if(driver_->GetId() == driver_->GetHoltId())
			{
				//driver_->stateMachine_->ChangeState((FSMState<CharacterParker> *)driver_->placingStatic_);
				driver_->stateMachine_->ChangeState(driver_->placingStatic_);
			}
			return true;
		}
		case CHARACTER_ENTER_GRAVITYSTATE:
		{
			driver_->stateMachine_->ChangeState(driver_->placingGravityVector_);
			return true;
		}
		case CHARACTER_OPEN_DOOR:
		{
			driver_->door_->OpenDoor();

			return true;
		}
	}
  }
  return false;
}



//=============================================================================
//								Exit
//
void ParkerStateOnGround::Exit()
{
	driver_->elevator_ = 0;                                                             
}



//=============================================================================
//								BeginContact
//
/// Called when two fixtures begin to touch.
void ParkerStateOnGround::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(!collidedFixture->IsSensor())
	{
		if(contactFixture == driver_->feetSensor_)
		{
			if(collidedFixture != driver_->feetCircle_)
				feetContactCount_++;

			if(collidedFixture->GetBody()->GetUserData())
			{
				GameObject* go = (GameObject*) collidedFixture->GetBody()->GetUserData();
				int type = go->GetGameObjectType();

				if(driver_->elevator_ == 0)
				{
					driver_->elevator_ = collidedFixture->GetBody();
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
		if(contactFixture == driver_->feetSensor_)
		{
			feetContactCount_--;

			if(feetContactCount_ == 0)
			{
				stateMachine_->ChangeState(driver_->inAirState_);
			}

			if(elevator_ == collidedFixture->GetBody())
			{
				driver_->elevator_ = 0;
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

		if(driver_->body_->GetLinearVelocity().x > -driver_->maximumRunningVelocity_)
		{
			driver_->body_->ApplyForce(b2Vec2(-driver_->runningForce_ * timeSinceLastFrame,0), driver_->body_->GetPosition());
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
		
		if(driver_->body_->GetLinearVelocity().x < driver_->maximumRunningVelocity_)
		{
			driver_->body_->ApplyForce(b2Vec2(driver_->runningForce_ * timeSinceLastFrame,0), driver_->body_->GetPosition());
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
		b2Vec2 force(b2Vec2(0, (driver_->jumpingAfterForce_)));
		driver_->body_->ApplyForce(force, driver_->body_->GetPosition());
	}
	else
	{

		driver_->body_->ApplyImpulse(b2Vec2(0,driver_->jumpingForce_/5), driver_->body_->GetPosition());

		driver_->animationBlender_->Blend("jump", AnimationBlender::BlendWhileAnimating, 0.2, false, 0.6);
		//driver_->animationBlender_->GetTarget()->setTimePosition(0.3);

		isJumping_ = true;

		jumpTimer_ = driver_->timeBetweenJump_;
	}

}



//=============================================================================
//								UpdateAnimation
//
void ParkerStateOnGround::UpdateAnimation()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	if(isJumping_ == false)
	{
		b2Vec2 lv = driver_->body_->GetLinearVelocity();
	
		if(driver_->elevator_ != 0)
		{
			lv -= driver_->elevator_->GetLinearVelocity();
		}

		if(abs(lv.x)  < .1)
		{
			if(blendingIdle_ == false)
			{
				blendingIdle_ = true;
				blendingRun_ = false;
				driver_->animationBlender_->Blend("idle", AnimationBlender::BlendWhileAnimating, 0.2, true);
			}

			driver_->animationBlender_->AddTime(timeSinceLastFrame);
		}
		else
		{
			double ratio = driver_->maximumRunningVelocity_ / lv.x;

			if(blendingRun_ == false)
			{
				blendingRun_ = true;
				blendingIdle_ = false;
				driver_->animationBlender_->Blend("run", AnimationBlender::BlendWhileAnimating, 0.2, true);
			}

			if(driver_->animationBlender_->timeleft_ == 0)
			{
				driver_->animationBlender_->AddTime(timeSinceLastFrame / abs(ratio));
			}
			else
			{
				driver_->animationBlender_->AddTime(timeSinceLastFrame);
			}
		}
	}
}

int ParkerStateOnGround::GetFeetContactCount()
{
	return feetContactCount_;
}