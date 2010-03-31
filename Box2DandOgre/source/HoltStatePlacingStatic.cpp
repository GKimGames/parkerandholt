/*=============================================================================

		HoltStatePlacingPlatform.cpp

		Author: Greg King

=============================================================================*/

#include "HoltStatePlacingStatic.h"

#include "HoltStateOnGround.h"
#include "HoltStateInAir.h"
#include "HoltStatePlacingGravityVector.h"
#include "HoltStatePlacingPlatform.h"
#include "XMLQuickVars.h"

//=============================================================================
//								Constructor
//
HoltStatePlacingStatic::HoltStatePlacingStatic(	
	CharacterHolt* holt,
	FSMStateMachine<CharacterHolt>* stateMachine):
	HoltState(holt,stateMachine)
{
	feetContactCount_ = 0;
	incrimenter_ = 0;
	leftMouseDown_ = false;
}

//=============================================================================
//								Enter
//
void HoltStatePlacingStatic::Enter()
{
	
	blendingRun_ = false;
	blendingIdle_ = false;

	if(driver_->initialized_)
	{
		driver_->animationBlender_->Blend("run", AnimationBlender::BlendWhileAnimating, 0.2, true);
	}

	driver_->mousePicking_->boxSize_ = 0.5f;
	driver_->mousePicking_->UpdateMouseFromCamera();
	driver_->mousePicking_->SetVisibility(true);
}

//=============================================================================
//								Update
//
bool HoltStatePlacingStatic::Update()
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

	static Ogre::Vector3 direction;

	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();
	
	if(driver_->feetSensorHit_ == false)
	{
		stateMachine_->ChangeState(driver_->inAirState_);
	}
	else
	{

		//if(moveRightDown_ || moveLeftDown_)
		{
			driver_->ApplyWalkingFriction(timeSinceLastFrame);
		}

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
bool HoltStatePlacingStatic::HandleMessage(const KGBMessage message)
{
  std::string* gameObject = 0;
  if(driver_->active_)
  {
	switch(message.messageType)
	{
		case CHARACTER_EXIT_PLACINGSTATE:
		{
			driver_->stateMachine_->ChangeState(driver_->onGroundState_);
			return true;
		}
		case LEFT_MOUSE_PLUS:
		{	
			return true;
		}
		case LEFT_MOUSE_MINUS:
		{
			SpawnBox();
			return true;
		}
		case MIDDLE_MOUSE_MINUS:
		{
			driver_->mousePicking_->DeletePlaceables();
			return true;
		}
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
			driver_->stateMachine_->ChangeState(driver_->placingPlatform_);
			return true;
		}
		case RIGHT_MOUSE_PLUS:
		{
			return true;
		}

		case RIGHT_MOUSE_MINUS:
		{
			SpawnTempBox();
			return true;
		}
	}
  }

	return false;
}

//=============================================================================

//								Exit
//
void HoltStatePlacingStatic::Exit()
{
	driver_->mousePicking_->SetVisibility(false); 
	driver_->elevator_ = NULL;
}

//=============================================================================
//								BeginContact
//
/// Called when two fixtures begin to touch.
void HoltStatePlacingStatic::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(!collidedFixture->IsSensor())
	{
		if(contactFixture == driver_->feetSensor_)
		{
			feetContactCount_++;

			if(collidedFixture->GetBody()->GetUserData())
			{
				GameObject* go = (GameObject*) collidedFixture->GetBody()->GetUserData();
				int type = go->GetGameObjectType();

				if(driver_->elevator_ == NULL)
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
void HoltStatePlacingStatic::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(!collidedFixture->IsSensor())
	{
		if(contactFixture == driver_->feetSensor_)
		{
			feetContactCount_--;

			if(feetContactCount_ <= 0)
			{
				stateMachine_->ChangeState(driver_->inAirState_);
			}

			if(elevator_ == collidedFixture->GetBody())
			{
				driver_->elevator_ = NULL;
			}

		}
	}
}


//=============================================================================
//								MoveLeft
//
///
void HoltStatePlacingStatic::MoveLeft()
{
	driver_->stateMachine_->ChangeState(driver_->onGroundState_);
}

//=============================================================================
//								MoveRight
//
/// 
void HoltStatePlacingStatic::MoveRight()
{
	driver_->stateMachine_->ChangeState(driver_->onGroundState_);
}


//=============================================================================
//								Jump
///
/// Jump!
void HoltStatePlacingStatic::Jump()
{
	driver_->stateMachine_->ChangeState(driver_->onGroundState_);
}

//=============================================================================
//								UpdateAnimation
//
void HoltStatePlacingStatic::UpdateAnimation()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	b2Vec2 lv = driver_->body_->GetLinearVelocity();
	
	if(driver_->elevator_ != NULL)
	{
		lv -= driver_->elevator_->GetLinearVelocity();
	}

	if(abs(lv.x)  < .1 )
	{
		if(blendingIdle_ == false)
		{
			blendingIdle_ = true;
			blendingRun_ = false;
			driver_->animationBlender_->Blend("idle", AnimationBlender::BlendWhileAnimating, 0.3, true);
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
			driver_->animationBlender_->Blend("run", AnimationBlender::BlendWhileAnimating, 0.3, true);
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


bool HoltStatePlacingStatic::SpawnBox()
{
	new Triangle(driver_->sceneManager_, 
		b2Vec2((float32)driver_->mousePicking_->GetPosition().x, (float32)driver_->mousePicking_->GetPosition().y),
		driver_->mousePicking_->boxSize_/2);
	return true;
}

bool HoltStatePlacingStatic::SpawnTempBox()
{
	float timeToLive = 4;
	new Triangle(driver_->sceneManager_, 
		b2Vec2((float32)driver_->mousePicking_->GetPosition().x, (float32)driver_->mousePicking_->GetPosition().y),
		driver_->mousePicking_->boxSize_/2, timeToLive);
	return true;
}


