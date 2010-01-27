/*=============================================================================

	ParkerStateInAir.cpp

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/
#include "ParkerStateInAir.h"

#include "Parker.h"
#include "Message.h"

//=============================================================================
//								Constructor
//
ParkerStateInAir::ParkerStateInAir(	
	CharacterParker* parker,
	FSMStateMachine<CharacterParker>* stateMachine):
	FSMState<CharacterParker>(parker,stateMachine)
{
}



//=============================================================================
//								Enter
//
void ParkerStateInAir::Enter()
{
	driver_->feetSensorHitCount_ = 0;
}



//=============================================================================
//								Update
//
bool ParkerStateInAir::Update()
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

	if(driver_->feetSensorHitCount_ > 0)
	{
		stateMachine_->ChangeState(driver_->onGroundState_);
	}
	else
	{
		// Set the position of the scene node to that of the Box2D body
		b2Vec2 v = driver_->body_->GetPosition();
		driver_->sceneNode_->setPosition(v.x, v.y,0);

		// Make the sceneNode face the direction the body is moving
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
bool ParkerStateInAir::HandleMessage(const KGBMessage message)
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

	double timeLeft = (driver_->animationState_->getLength() - driver_->animationState_->getTimePosition()) / driver_->animationState_->getLength();
	driver_->body_->ApplyForce(b2Vec2(0,driver_->jumpingAfterForce_ * timeLeft), driver_->body_->GetPosition());

	
	b2ContactEdge* contacts = driver_->body_->GetContactList();
	bool shinHitRight = false, torsoHitRight = false;
	bool shinHitLeft = false, torsoHitLeft = false;

	while(contacts != 0 && !shinHitRight && !torsoHitRight && !shinHitLeft && !torsoHitLeft)
	{
		if(contacts->contact->GetFixtureA() == driver_->shinSensorRight_)
		{
			shinHitRight = true;
		}
		else if (contacts->contact->GetFixtureA() == driver_->torsoSensorRight_)
		{
			torsoHitRight = true;
		}

		if(contacts->contact->GetFixtureB() == driver_->shinSensorRight_)
		{
			shinHitRight = true;
		}
		else if (contacts->contact->GetFixtureB() == driver_->torsoSensorRight_)
		{
			torsoHitRight = true;
		}


		if(contacts->contact->GetFixtureA() == driver_->shinSensorLeft_)
		{
			shinHitLeft = true;
		}
		else if (contacts->contact->GetFixtureA() == driver_->torsoSensorLeft_)
		{
			torsoHitLeft = true;
		}

		if(contacts->contact->GetFixtureB() == driver_->shinSensorLeft_)
		{
			shinHitLeft = true;
		}
		else if (contacts->contact->GetFixtureB() == driver_->torsoSensorLeft_)
		{
			torsoHitLeft = true;
		}

		contacts = contacts->next;

	}
	

	/*
	// Code to wall jump
	
	if(shinHitRight && torsoHitRight)
	{
		driver_->body_->SetLinearVelocity(b2Vec2(0,driver_->body_->GetLinearVelocity().y)); 
		driver_->body_->ApplyImpulse(b2Vec2(-driver_->jumpingForce_/2,driver_->jumpingForce_), driver_->body_->GetPosition());
		//justWallJumped_ = true;

		driver_->animationState_->setEnabled(false);
		driver_->animationState_ = driver_->entity_->getAnimationState("JumpNoHeight");
		driver_->animationState_->setTimePosition(Ogre::Real(0));
		driver_->animationState_->setLoop(false);
		driver_->animationState_->setEnabled(true);
	}
	else if(shinHitLeft && torsoHitLeft)
	{
		driver_->body_->SetLinearVelocity(b2Vec2(0,driver_->body_->GetLinearVelocity().y)); 
		driver_->body_->ApplyImpulse(b2Vec2(driver_->jumpingForce_/2,driver_->jumpingForce_), driver_->body_->GetPosition());
		//justWallJumped_ = true;

		driver_->animationState_->setEnabled(false);
		driver_->animationState_ = driver_->entity_->getAnimationState("JumpNoHeight");
		driver_->animationState_->setTimePosition(Ogre::Real(0));
		driver_->animationState_->setLoop(false);
		driver_->animationState_->setEnabled(true);
	}
	*/
	

}

//=============================================================================
//								UpdateAnimation
void ParkerStateInAir::UpdateAnimation()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	driver_->animationBlender_->AddTime(timeSinceLastFrame);
	//driver_->animationState_->addTime(timeSinceLastFrame);
}


