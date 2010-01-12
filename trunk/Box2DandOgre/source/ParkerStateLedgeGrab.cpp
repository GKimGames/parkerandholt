

/*=============================================================================

	ParkerStateLedgeGrab.cpp

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/

#include "ParkerStateLedgeGrab.h"

//=============================================================================
//								Constructor
//
ParkerStateLedgeGrab::ParkerStateLedgeGrab(CharacterParker* parker):
FSMState(parker)
{
}

//=============================================================================
//								Enter
//
void ParkerStateLedgeGrab::Enter()
{
	//b2RevoluteJointDef jdef;
	//jdef.bodyA = owner_->body_;
	//owner_->world_->CreateJoint();

}

//=============================================================================
//								Update
//
bool ParkerStateLedgeGrab::Update()
{
	static Ogre::Vector3 direction;
	
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	if(owner_->feetSensorHitCount_ > 0)
	{
		owner_->stateMachine_->ChangeState(owner_->onGroundState_);
	}
	else
	{
		// Set the position of the scene node to that of the Box2D body
		b2Vec2 v = owner_->body_->GetPosition();
		owner_->sceneNode_->setPosition(v.x, v.y,0);

		// Make the scene face the direction the body is moving
		if(owner_->body_->GetLinearVelocity().x > 0.1)
		{
			direction = Ogre::Vector3(0,0,1);
		}
		else if(owner_->body_->GetLinearVelocity().x < -0.1)
		{
			direction = Ogre::Vector3(0,0,-1);
		}

		owner_->sceneNode_->setDirection(direction,Ogre::Node::TS_WORLD);

		UpdateAnimation();
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

	if(owner_->body_->GetLinearVelocity().x > -owner_->maximumAirVelocity_)
	{
		owner_->body_->ApplyForce(b2Vec2(-owner_->airForce_ * timeSinceLastFrame,0), owner_->body_->GetPosition());
	}

}

//=============================================================================
//								MoveRight
//
/// 
void ParkerStateLedgeGrab::MoveRight()
{
	double timeSinceLastFrame = GameFramework::getSingletonPtr()->GetTimeSinceLastFrame();

	if(owner_->body_->GetLinearVelocity().x < owner_->maximumAirVelocity_)
	{
		owner_->body_->ApplyForce(b2Vec2(owner_->airForce_ * timeSinceLastFrame,0), owner_->body_->GetPosition());
	}

}


//=============================================================================
//								Climb
///
/// Getting a jump message while the character is grabbing a ledge will make
/// him climb.
void ParkerStateLedgeGrab::Climb()
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

//=============================================================================
//								UpdateAnimation
void ParkerStateLedgeGrab::UpdateAnimation()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	owner_->animationState_->addTime(timeSinceLastFrame);
}