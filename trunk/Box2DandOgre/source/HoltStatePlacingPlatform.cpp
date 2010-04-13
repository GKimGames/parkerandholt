/*=============================================================================

		HoltStatePlacingPlatform.cpp

		Author: Greg King

=============================================================================*/

#include "HoltStatePlacingPlatform.h"

#include "HoltStateOnGround.h"
#include "HoltStateInAir.h"
#include "HoltStatePlacingGravityVector.h"
#include "HoltStatePlacingStatic.h"
#include "XMLQuickVars.h"

//=============================================================================
//								Constructor
//
HoltStatePlacingPlatform::HoltStatePlacingPlatform(	
	CharacterHolt* holt,
	FSMStateMachine<CharacterHolt>* stateMachine):
	HoltState(holt,stateMachine)
{
	feetContactCount_ = 0;
	platform_ = 0;
	gravityVector_ = new GravityVector(driver_->sceneManager_, b2Vec2(0,0), b2Vec2(1,1));
	leftMouseDown_ = false;
	maxLength_ = 8.0;
}

//=============================================================================
//								Enter
//
void HoltStatePlacingPlatform::Enter()
{
	
	blendingRun_ = false;
	blendingIdle_ = false;

	if(driver_->initialized_)
	{
		driver_->animationBlender_->Blend("run", AnimationBlender::BlendWhileAnimating, 0.2, true);
	}

	// Sets the mouse visible and sets the current box size
	driver_->mousePicking_->boxSize_ = 0.5f;
	driver_->mousePicking_->SetVisibility(true);
}

//=============================================================================
//								Update
//
bool HoltStatePlacingPlatform::Update()
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

	// Takes the character out of placing state if they are moving a significant amount
	if(driver_->GetBody()->GetLinearVelocity().LengthSquared() > 1)
	{
		stateMachine_->ChangeState(driver_->inAirState_);
	}	
	
	if(driver_->feetSensorHit_ == false)
	{
		stateMachine_->ChangeState(driver_->inAirState_);
	}
	else
	{

		driver_->ApplyWalkingFriction(timeSinceLastFrame);

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


	// If the left mouse is held down the current position is updated
	if(leftMouseDown_)
	{
		endPosition_.x = driver_->mousePicking_->GetPosition().x;
		endPosition_.y = driver_->mousePicking_->GetPosition().y;

		float tempLength = sqrt((startPosition_.x - endPosition_.x) * (startPosition_.x - endPosition_.x)
							 + (startPosition_.y - endPosition_.y) * (startPosition_.y - endPosition_.y));
		
		// Checks distance been click and release to see if it is within a valid length for a platform
		if(tempLength > 0.5 && tempLength < 8.0)
		{
			driver_->mousePicking_->SetVisibility(false);
			if(platform_ == 0)
			{
				SpawnPlatform();
			}
			b2Vec2 tempPosition(startPosition_.x - (startPosition_.x - endPosition_.x)/2,
								startPosition_.y - (startPosition_.y - endPosition_.y)/2);
			float angle = atan2((endPosition_.y - startPosition_.y)/2,(endPosition_.x - startPosition_.x)/2);
			platform_->SetGraphics(tempPosition, tempLength, angle, false);
		}
		// If the platform is too long it is limited by the maximum length and drawn at the angle between click and release
		else if(tempLength >= 8.0)
		{
			if(platform_ == 0)
			{
				SpawnPlatform();
			}

			b2Vec2 tempPosition(startPosition_.x - ((startPosition_.x - endPosition_.x)/2),
								startPosition_.y - ((startPosition_.y - endPosition_.y)/2));

			float angle = atan2((endPosition_.y - startPosition_.y)/2,(endPosition_.x - startPosition_.x)/2);
			
			tempPosition.x = startPosition_.x - ((startPosition_.x - endPosition_.x)/tempLength) * maxLength_/2;
			tempPosition.y = startPosition_.y - ((startPosition_.y - endPosition_.y)/tempLength) * maxLength_/2;
			
			platform_->SetGraphics(tempPosition, maxLength_, angle, false);
		}
	}


	return true;
}

//=============================================================================
//								HandleMessage
//
bool HoltStatePlacingPlatform::HandleMessage(const KGBMessage message)
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
			startPosition_.x = driver_->mousePicking_->GetPosition().x;
			startPosition_.y = driver_->mousePicking_->GetPosition().y;
			endPosition_.x = driver_->mousePicking_->GetPosition().x;
			endPosition_.y = driver_->mousePicking_->GetPosition().y;
			leftMouseDown_ = true;
			return true;
		}
		case LEFT_MOUSE_MINUS:
		{
			endPosition_.x = driver_->mousePicking_->GetPosition().x;
			endPosition_.y = driver_->mousePicking_->GetPosition().y;
			
			float tempLength = sqrt((startPosition_.x - endPosition_.x) * (startPosition_.x - endPosition_.x)
								 + (startPosition_.y - endPosition_.y) * (startPosition_.y - endPosition_.y));
			
			// Platform is only created if the distance between click and release is a valid length
			if(platform_ == 0)
			{
				leftMouseDown_ = false;
			}
			else if(tempLength < .5)
			{
				leftMouseDown_ = false;
			}
			else
			{
				platform_->GetBody()->SetActive(true);
				leftMouseDown_ = false;
				trans_->setTransparency(0.0);

				platform_->SetGraphics(platform_->GetBody()->GetPosition(), tempLength, platform_->GetBody()->GetAngle(), true);
				platform_ = 0;
			}

			driver_->mousePicking_->SetVisibility(true);

			return true;
		}
		case MIDDLE_MOUSE_MINUS:
		{
			SpawnBox();
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
		case CHARACTER_ENTER_GRAVITYSTATE:
		{
			driver_->stateMachine_->ChangeState(driver_->placingStatic_);
			return true;
		}
		case RIGHT_MOUSE_PLUS:
		{
			startPosition_.x = driver_->mousePicking_->GetPosition().x;
			startPosition_.y = driver_->mousePicking_->GetPosition().y;
			gravityVector_->Stop();
			
			gravityVector_->SetPosition(startPosition_);

			gravityVector_->GetEntity()->setVisible(true);
			gravityVector_->SetParticalsVisible(false);
			return true;
		}

		case RIGHT_MOUSE_MINUS:
		{
			endPosition_.x = driver_->mousePicking_->GetPosition().x;
			endPosition_.y = driver_->mousePicking_->GetPosition().y;
			SpawnGravityVector();
			return true;
		}
	}
  }

	return false;
}

//=============================================================================

//								Exit
//
void HoltStatePlacingPlatform::Exit()
{
	driver_->mousePicking_->SetVisibility(false); 
	driver_->elevator_ = NULL;
}

//=============================================================================
//								BeginContact
//
/// Called when two fixtures begin to touch.
void HoltStatePlacingPlatform::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
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
void HoltStatePlacingPlatform::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
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
void HoltStatePlacingPlatform::MoveLeft()
{
	driver_->stateMachine_->ChangeState(driver_->onGroundState_);
}

//=============================================================================
//								MoveRight
//
/// 
void HoltStatePlacingPlatform::MoveRight()
{
	driver_->stateMachine_->ChangeState(driver_->onGroundState_);
}


//=============================================================================
//								Jump
///
void HoltStatePlacingPlatform::Jump()
{
	driver_->stateMachine_->ChangeState(driver_->onGroundState_);
}

//=============================================================================
//								UpdateAnimation
//
void HoltStatePlacingPlatform::UpdateAnimation()
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

//=============================================================================
//								SpawnBox
//
/// Creates a box at the location with the same size as the mouse cursor
bool HoltStatePlacingPlatform::SpawnBox()
{
	XMLQuickVars var("..\\Myvars.xml");
	new HoltBox(driver_->sceneManager_, 
		b2Vec2((float32)driver_->mousePicking_->GetPosition().x, (float32)driver_->mousePicking_->GetPosition().y),
		driver_->mousePicking_->boxSize_/2, var.Double("HoltBox/boxDensity"));
	return true;
}

//=============================================================================
//								SpawnPlatform
//
/// Creates a platform "shadow" based on distance between clicks, is a guide for placing platforms
bool HoltStatePlacingPlatform::SpawnPlatform()
{
	float tempLength = sqrt((startPosition_.x - endPosition_.x) * (startPosition_.x - endPosition_.x)
						+ (startPosition_.y - endPosition_.y) * (startPosition_.y - endPosition_.y));

	platform_ = new Platform(driver_->sceneManager_, b2Vec2(startPosition_.x, startPosition_.y), b2Vec2(endPosition_.x, endPosition_.y), 1);
	platform_->GetBody()->SetActive(false);
	trans_ = new EntityMaterialInstance(platform_->GetEntity());
	trans_->setSceneBlending(Ogre::SceneBlendType::SBT_ADD);
	trans_->setTransparency(0.5);
	return true;
}

//=============================================================================
//								SpawnGravityVector
//
bool HoltStatePlacingPlatform::SpawnGravityVector()
{
	float tempLength = sqrt((startPosition_.x - endPosition_.x) * (startPosition_.x - endPosition_.x) + (startPosition_.y - endPosition_.y) * (startPosition_.y - endPosition_.y));
	if(tempLength < .5)
	{
		gravityVector_->Stop();
		return false;
	}
	else
	{
		gravityVector_->Start(startPosition_, endPosition_);
	}

	return false;
}

//=============================================================================
//								GetGravityVector
//
GravityVector* HoltStatePlacingPlatform::GetGravityVector()
{
	return gravityVector_;
}