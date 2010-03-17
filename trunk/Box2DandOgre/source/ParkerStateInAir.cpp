
/*=============================================================================

	ParkerStateInAir.cpp

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/
#include "ParkerStateInAir.h"
#include "ParkerStateOnGround.h"

#include "Parker.h"
#include "Message.h"
#include "TinyXMLHelper.h"
#include "XMLQuickVars.h"

//=============================================================================
//								Constructor
//
ParkerStateInAir::ParkerStateInAir(	
	CharacterParker* parker,
	FSMStateMachine<CharacterParker>* stateMachine):
	ParkerState(parker,stateMachine)
{
	XMLQuickVars vars("..\\Myvars.xml");

	float f = vars.Float("steve2");
	Ogre::String s = vars.String("ben");
	b2Vec2 v = vars.B2Vec2("vecty");
	f = f + 5;

}



//=============================================================================
//								Enter
//
void ParkerStateInAir::Enter()
{
	driver_->feetSensorHitCount_ = 0;
	if(driver_->onGroundState_->isJumping_)
	{
		jumpTimer_ = 0.0;
	}
	if(driver_->stateMachine_->GetPreviousState() == (ParkerState*) driver_->ledgeGrabState_)
	{
		driver_->animationBlender_->Blend("jump_idle", AnimationBlender::BlendThenAnimate, 0.3, true);
	}
	else if(driver_->onGroundState_->isJumping_)
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
}



//=============================================================================
//								Update
//
bool ParkerStateInAir::Update()
{
	static Ogre::Vector3 direction;
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
		// Set the position of the scene node to that of the Box2D body
		b2Vec2 v = driver_->body_->GetPosition();
		driver_->sceneNode_->setPosition(v.x, v.y,0);

		// Make the sceneNode face the direction the body is moving
		if(driver_->body_->GetLinearVelocity().x > 0.3)
		{
			direction = Ogre::Vector3(0,0,1);
		}
		else if(driver_->body_->GetLinearVelocity().x < -0.3)
		{
			direction = Ogre::Vector3(0,0,-1);
		}

		driver_->sceneNode_->setDirection(direction,Ogre::Node::TS_WORLD);
		static bool mySwitch = true;
		if(justWallJumped_)
		{
			wallJumpTimer_ += GAMEFRAMEWORK->GetTimeSinceLastFrame();
			
			if(wallJumpedLeft_ && mySwitch)
			{
				driver_->sceneNode_->setDirection(Ogre::Vector3(0,0,-1),Ogre::Node::TS_WORLD);
			}
			else if (mySwitch)
			{	
				driver_->sceneNode_->setDirection(Ogre::Vector3(0,0,1),Ogre::Node::TS_WORLD);
			}

			if(wallJumpTimer_ > 0.2 && mySwitch)
			{
				mySwitch = false;
				driver_->body_->SetLinearVelocity(b2Vec2(0,driver_->body_->GetLinearVelocity().y)); 

				if(wallJumpedLeft_)
				{
					driver_->body_->ApplyImpulse(driver_->wallJumpForce_, driver_->body_->GetPosition());
				}
				else
				{
					driver_->body_->ApplyImpulse(b2Vec2(-driver_->wallJumpForce_.x,driver_->wallJumpForce_.y ), driver_->body_->GetPosition());
				}
			}

			if(wallJumpTimer_ > 0.5)
			{
				wallJumpTimer_ = -1.0;
				mySwitch = true;
				justWallJumped_ = false;
			}
		}

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
bool ParkerStateInAir::HandleMessage(const KGBMessage message)
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
		case CHARACTER_GRAB_LEDGE:
		{
			if(driver_->ledge_)
			{
				stateMachine_->ChangeState((FSMState<CharacterParker>*) driver_->ledgeGrabState_);
			}
		}
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
void ParkerStateInAir::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{

}

//=============================================================================
//								MoveLeft
//
///
void ParkerStateInAir::MoveLeft()
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
void ParkerStateInAir::MoveRight()
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
void ParkerStateInAir::Jump()
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

	
	
	// Code to wall jump
	if(justWallJumped_ == false && wallJumpTimer_ == -1)
	{
		if(driver_->shinRightHit_ && driver_->torsoRightHit_)
		{
			if(driver_->moveLeft_)
			{
				driver_->animationBlender_->Blend("clip7", AnimationBlender::BlendSwitch, 0.1, false);
				driver_->animationBlender_->Blend("jump_idle", AnimationBlender::BlendWhileAnimating, 0.7, false);


				wallJumpTimer_ = 0.0;
				justWallJumped_ = true;
				wallJumpedLeft_ = false;
			}
		}
		else if(driver_->shinLeftHit_ && driver_->torsoLeftHit_)
		{
			if(driver_->moveRight_)
			{
				driver_->animationBlender_->Blend("clip7", AnimationBlender::BlendSwitch, 0.1, false);
				driver_->animationBlender_->Blend("jump_idle", AnimationBlender::BlendWhileAnimating, 0.7, false);
				

				wallJumpTimer_ = 0.0;
				justWallJumped_ = true;
				wallJumpedLeft_ = true;
			}
		}
	}



	
	

}

//=============================================================================
//								UpdateAnimation
void ParkerStateInAir::UpdateAnimation()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	driver_->animationBlender_->AddTime(timeSinceLastFrame);
	
}


