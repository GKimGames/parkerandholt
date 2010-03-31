///*=============================================================================
//
//		HoltStatePlacingGravityVector.cpp
//
//		Author: Greg King
//
//=============================================================================*/
//#include "Holt.h" 
//
//#include "HoltStateOnGround.h"
//#include "HoltStateInAir.h"
//#include "HoltStatePlacingGravityVector.h"
//#include "HoltStatePlacingPlatform.h"
//
//
////=============================================================================
////								Constructor
////
//HoltStatePlacingGravityVector::HoltStatePlacingGravityVector(	
//	CharacterParker* parker,
//	ParkerStateMachine* stateMachine):
//	ParkerState(parker,stateMachine)
//{
//	feetContactCount_ = 0;
//	//gravityVector_ = new GravityVector(driver_->sceneManager_, b2Vec2(0,0), b2Vec2(1,1));
//	deleteVector_ = false;
//	createVector_ = false;
//}
//
////=============================================================================
////								Enter
////
//void HoltStatePlacingGravityVector::Enter()
//{
//	blendingRun_ = false;
//	blendingIdle_ = false;
//
//	//feetContactCount_ = driver_->feetSensorHitCount_;
//	feetContactCount_ = driver_->onGroundState_->GetFeetContactCount();
//	if(driver_->initialized_)
//	{
//		driver_->animationBlender_->Blend("run", AnimationBlender::BlendWhileAnimating, 0.2, true);
//	}
//
//	driver_->mousePicking_->boxSize_ = 0.2f;
//	driver_->mousePicking_->SetVisibility(true);
//	driver_->mousePicking_->boxSize_ = 0.2f;
//	
//	deleteVector_ = false;
//	createVector_ = false;
//}
//
////=============================================================================
////								Update
////
//bool HoltStatePlacingGravityVector::Update()
//{
//
//	if(driver_->moveLeft_)
//	{
//		MoveLeft();
//	}
//
//	if(driver_->moveRight_)
//	{
//		MoveRight();
//	}
//
//	if(driver_->jump_)
//	{
//		Jump();
//	}
//
//	static Ogre::Vector3 direction;
//
//	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();
//	
//	if(driver_->feetSensorHitCount_ == 0)
//	{
//		stateMachine_->ChangeState(driver_->inAirState_);
//	}
//	else
//	{
//
//		{
//			driver_->ApplyWalkingFriction(timeSinceLastFrame);
//		}
//
//
//		if(driver_->body_->GetLinearVelocity().x > 0.1)
//		{
//			direction = Ogre::Vector3(0,0,1);
//		}
//		else if(driver_->body_->GetLinearVelocity().x < -0.1)
//		{
//			direction = Ogre::Vector3(0,0,-1);
//		}
//
//		UpdateAnimation();
//
//		b2Vec2 v = driver_->body_->GetPosition();
//		driver_->sceneNode_->setPosition(Ogre::Real(v.x),Ogre::Real(v.y),0);
//		driver_->sceneNode_->setDirection(direction,Ogre::Node::TS_WORLD);
//
//		if(driver_->debugDrawOn_)
//		{
//			driver_->UpdateDebugDraw();
//		}
//	}
//
//	//end of testcode
//	if(deleteVector_)
//	{
//		gravityVector_->Stop();
//		//delete gravityVector_;
//		//gravityVector_ = 0;
//		deleteVector_ = false;
//	}
//
//	if(createVector_)
//	{
//		SpawnGravityVector();
//		//gravityVector_->Start(startPosition_, endPosition_);
//		createVector_ = false;
//	}
//
//	return true;
//}
//
////=============================================================================
////								HandleMessage
////
//bool HoltStatePlacingGravityVector::HandleMessage(const KGBMessage message)
//{
//	std::string* gameObject = 0;
//	
//	switch(message.messageType)
//	{
//		case CHARACTER_EXIT_PLACINGSTATE:
//		{
//			driver_->stateMachine_->ChangeState(driver_->onGroundState_);
//			return true;
//		}
//		case LEFT_MOUSE_PLUS:
//		{	
//			startPosition_.x = driver_->mousePicking_->GetPosition().x;
//			startPosition_.y = driver_->mousePicking_->GetPosition().y;
//
//			//old gravity vector is deleted outside of messagehandling to avoid changing the object list
//			//during the message handling for each frame
//			if(gravityVector_ != 0)
//			{
//				deleteVector_ = true;
//			}
//			return true;
//		}
//		case LEFT_MOUSE_MINUS:
//		{
//			endPosition_.x = driver_->mousePicking_->GetPosition().x;
//			endPosition_.y = driver_->mousePicking_->GetPosition().y;
//			createVector_ = true;
//			return true;
//		}
//		case CHARACTER_MOVE_LEFT_PLUS:
//		{
//			driver_->moveLeft_ = true;
//			return true;
//		}
//		case CHARACTER_MOVE_RIGHT_PLUS:
//		{
//			driver_->moveRight_ = true;
//			return true;
//		}
//		case CHARACTER_JUMP_PLUS:
//		{
//			driver_->jump_ = true;
//			return true;
//		}
//		case CHARACTER_MOVE_LEFT_MINUS:
//		{
//
//			driver_->moveLeft_ = false;
//			return true;
//		}
//		case CHARACTER_MOVE_RIGHT_MINUS:
//		{
//			driver_->moveRight_ = false;
//			return true;
//		}
//		case CHARACTER_JUMP_MINUS:
//		{
//			driver_->jump_ = false;
//			return true;
//		}
//		case CHARACTER_ENTER_PLATFORMSTATE:
//		{
//			//driver_->stateMachine_->ChangeState(driver_->placingPlatform_);
//			return true;
//		}
//	}
//
//	return false;
//}
//
////=============================================================================
//
////								Exit
////
//void HoltStatePlacingGravityVector::Exit()
//{
//	driver_->mousePicking_->SetVisibility(false);    
//	//driver_->elevator_ = NULL;
//}
//
////=============================================================================
////								BeginContact
////
///// Called when two fixtures begin to touch.
//void HoltStatePlacingGravityVector::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
//{
//	if(!collidedFixture->IsSensor())
//	{
//		if(contactFixture == driver_->feetSensor_)
//		{
//			driver_->feetSensorHitCount_;
//			feetContactCount_++;
//
//			if(collidedFixture->GetBody()->GetUserData())
//			{
//				GameObject* go = (GameObject*) collidedFixture->GetBody()->GetUserData();
//				int type = go->GetGameObjectType();
//
//				if(driver_->elevator_ == NULL)
//				{
//					driver_->elevator_ = collidedFixture->GetBody();
//				}
//
//			}
//		}
//
//		
//		
//	}
//}
//
////=============================================================================
////								EndContact
////
///// Called when two fixtures cease to touch.
//void HoltStatePlacingGravityVector::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
//{
//	if(!collidedFixture->IsSensor())
//	{
//		if(contactFixture == driver_->feetSensor_)
//		{
//			feetContactCount_--;
//
//			//if(feetContactCount_ == 0)
//			if(feetContactCount_ == 0)
//			{
//				stateMachine_->ChangeState(driver_->inAirState_);
//			}
//
//			if(elevator_ == collidedFixture->GetBody())
//			{
//				driver_->elevator_ = NULL;
//			}
//
//		}
//	}
//}
//
//
////=============================================================================
////								MoveLeft
////
/////
//void HoltStatePlacingGravityVector::MoveLeft()
//{
//	driver_->stateMachine_->ChangeState(driver_->onGroundState_);
//}
//
////=============================================================================
////								MoveRight
////
///// 
//void HoltStatePlacingGravityVector::MoveRight()
//{
//	driver_->stateMachine_->ChangeState(driver_->onGroundState_);
//}
//
//
////=============================================================================
////								Jump
/////
///// Jump!
//void HoltStatePlacingGravityVector::Jump()
//{
//	driver_->stateMachine_->ChangeState(driver_->onGroundState_);
//}
//
////=============================================================================
////								UpdateAnimation
////
//void HoltStatePlacingGravityVector::UpdateAnimation()
//{
//	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();
//
//	b2Vec2 lv = driver_->body_->GetLinearVelocity();
//	
//	if(driver_->elevator_ != NULL)
//	{
//		lv -= driver_->elevator_->GetLinearVelocity();
//	}
//
//	if(abs(lv.x)  < .1 )
//	{
//		if(blendingIdle_ == false)
//		{
//			blendingIdle_ = true;
//			blendingRun_ = false;
//			driver_->animationBlender_->Blend("idle", AnimationBlender::BlendWhileAnimating, 0.3, true);
//		}
//
//		driver_->animationBlender_->AddTime(timeSinceLastFrame);
//	}
//	else
//	{
//		double ratio = driver_->maximumRunningVelocity_ / lv.x;
//		
//		if(blendingRun_ == false)
//		{
//			blendingRun_ = true;
//			blendingIdle_ = false;
//			driver_->animationBlender_->Blend("run", AnimationBlender::BlendWhileAnimating, 0.3, true);
//		}
//
//		if(driver_->animationBlender_->timeleft_ == 0)
//		{
//			driver_->animationBlender_->AddTime(timeSinceLastFrame / abs(ratio));
//		}
//		else
//		{
//			driver_->animationBlender_->AddTime(timeSinceLastFrame);
//		}
//	}
//}
//
////=============================================================================
////								SpawnGravityVector
////
//bool HoltStatePlacingGravityVector::SpawnGravityVector()
//{
//	float tempLength = sqrt((startPosition_.x - endPosition_.x) * (startPosition_.x - endPosition_.x) + (startPosition_.y - endPosition_.y) * (startPosition_.y - endPosition_.y));
//	if(tempLength < .5)
//	{
//		return false;
//	}
//	else
//	{
//		gravityVector_->Start(startPosition_, endPosition_);
//	}
//	/*else if(gravityVector_ == 0)
//	{
//		gravityVector_->Start(startPosition_, endPosition_);
//		return true;
//	}
//	else if(gravityVector_ != 0)
//	{
//		deleteVector_ = true;
//	}*/
//
//
//	return false;
//}