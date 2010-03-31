
/*=============================================================================

	HoltStateInAir.cpp

	State for Holt on the ground.

=============================================================================*/
#include "HoltStateInAir.h"
#include "HoltStateOnGround.h"
#include "ParkerStateOnGround.h"
#include "ParkerStateInAir.h"

#include "Holt.h"
#include "Message.h"
#include "TinyXMLHelper.h"
#include "XMLQuickVars.h"



//=============================================================================
//								Constructor
//
HoltStateInAir::HoltStateInAir(	
	CharacterHolt* holt,
	FSMStateMachine<CharacterHolt>* stateMachine):
	HoltState(holt,stateMachine)
{

	direction_ = Ogre::Vector3(0,0,1);
}



//=============================================================================
//								Enter
//
void HoltStateInAir::Enter()
{

	if(driver_->onGroundState_->isJumping_)
	{
		jumpTimer_ = 0.0;
	}

	if(driver_->onGroundState_->isJumping_)
	{
		driver_->animationBlender_->Blend("jump_idle", AnimationBlender::BlendThenAnimate, 1.0, true);
	}
	else
	{
		driver_->animationBlender_->Blend("jump_idle", AnimationBlender::BlendWhileAnimating, 0.3, true);
	}

	wallJumpedLeft_ = false;
	justWallJumped_ = false;
	wallJumpTimer_ = -1.0;
	wallJumpBetweenTimer_ = -1.0;
	mySwitch = true;
}



//=============================================================================
//								Update
//
bool HoltStateInAir::Update()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();
	
	
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

	if(jumpTimer_ != -1.0)
	{
		jumpTimer_ += timeSinceLastFrame;
	}

	if(driver_->feetSensorHit_ == true)
	{
		stateMachine_->ChangeState(driver_->onGroundState_);
	}
	else
	{

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
bool HoltStateInAir::HandleMessage(const KGBMessage message)
{
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
	}
  }
  return false;
}



//=============================================================================
//								Exit
//
void HoltStateInAir::Exit()
{

}



//=============================================================================
//								BeginContact
//
/// Called when two fixtures begin to touch.
void HoltStateInAir::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
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
void HoltStateInAir::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{

}

//=============================================================================
//								MoveLeft
//
///
void HoltStateInAir::MoveLeft()
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
void HoltStateInAir::MoveRight()
{
	double timeSinceLastFrame = GameFramework::getSingletonPtr()->GetTimeSinceLastFrame();

	if(driver_->body_->GetLinearVelocity().x < driver_->maximumAirVelocity_)
	{
		driver_->body_->ApplyForce(b2Vec2(driver_->airForce_ * timeSinceLastFrame,0), driver_->body_->GetPosition());
	}

}


//=============================================================================
//								Jump
///
/// Calling jump while the character is in the air will add slightly more
/// height to the jump.
void HoltStateInAir::Jump()
{
	if(jumpTimer_ > 0.5)
	{
		jumpTimer_ = -1.0;
	}
	else if(jumpTimer_ != -1.0)
	{
		b2Vec2 force(b2Vec2(0, (driver_->jumpingAfterForce_ * ((0.5*2) - jumpTimer_)) * GAMEFRAMEWORK->GetTimeSinceLastFrame()));
		driver_->body_->ApplyImpulse(force, driver_->body_->GetPosition());
	}
	double timeLeft = (driver_->animationState_->getLength() - driver_->animationState_->getTimePosition()) / driver_->animationState_->getLength();
	//driver_->body_->ApplyForce(b2Vec2(0,driver_->jumpingAfterForce_ * timeLeft), driver_->body_->GetPosition());	

}

//=============================================================================
//								UpdateAnimation
void HoltStateInAir::UpdateAnimation()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	driver_->animationBlender_->AddTime(timeSinceLastFrame);
	
}


