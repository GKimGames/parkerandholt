/*=============================================================================

	ParkerStateLedgeGrab.cpp

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/

#include "ParkerStateLedgeGrab.h"

#include "Parker.h"
#include "Message.h"

#include "ParkerStateOnGround.h"
#include "ParkerStateInAir.h"

//=============================================================================
//								Constructor
//
ParkerStateLedgeGrab::ParkerStateLedgeGrab(
	CharacterParker* parker,
	FSMStateMachine<CharacterParker>* stateMachine):
	ParkerState(parker,stateMachine)
{
}


//=============================================================================
//								Enter
//
void ParkerStateLedgeGrab::Enter()
{
	b2Vec2 vp = driver_->GetBodyPosition();
	originalPosition_ = vp;
	ledgePosition_ = driver_->ledge_->GetBodyPosition();
	driver_->SetBodyPosition(b2Vec2(vp.x, ledgePosition_.y - 1.5));
	vp = driver_->GetBodyPosition();
	vp.y += 1.884 / 2.0;

	b2DistanceJointDef ddef;
	ddef.Initialize(driver_->ledge_->GetBody(),driver_->body_, ledgePosition_, vp);
	grabJoint_ = (b2DistanceJoint*)  driver_->world_->CreateJoint(&ddef);

	driver_->animationBlender_->Blend("climb", AnimationBlender::BlendThenAnimate, 0.3, false, 0.0);

	climbing_ = false;

}

//=============================================================================
//								Update
//
bool ParkerStateLedgeGrab::Update()
{

	Ogre::Vector3 direction;
	
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	// Set the position of the scene node to that of the Box2D body
	b2Vec2 v = driver_->body_->GetPosition();

	// Make the scene face the direction the body is moving
	if(ledgePosition_.x > v.x)
	{
		direction = Ogre::Vector3(0,0,1);
	}
	else
	{
		direction = Ogre::Vector3(0,0,-1);
	}

	driver_->sceneNode_->setDirection(direction,Ogre::Node::TS_WORLD);

	UpdateAnimation();

	Ogre::Bone* rightWrist = driver_->entity_->getSkeleton()->getBone("Right_Wrist");
	Ogre::Vector3 v2(ledgePosition_.x,ledgePosition_.y,0);

	rightWrist->setPosition(rightWrist->getParent()->_getDerivedOrientation().Inverse() * (v2 - rightWrist->getParent()->_getDerivedPosition()));
	driver_->sceneNode_->setPosition(v.x, v.y,0);

	if(driver_->debugDrawOn_)
	{
		driver_->UpdateDebugDraw();
	}

	if(climbing_)
	{
		Climb();
	}

	// We've successfully updated.
	return true;
}

//=============================================================================
//								HandleMessage
//
bool ParkerStateLedgeGrab::HandleMessage(const KGBMessage message)
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

				if(!climbing_ && !driver_->moveLeft_ && !driver_->moveRight_)
				{
					climbing_ = true;
					originalPosition_ = driver_->GetBodyPosition();
					driver_->world_->DestroyJoint(grabJoint_);
					grabJoint_ = 0;
					driver_->animationBlender_->targetTime_ = 1000;
				}
				else if(!climbing_)
				{
					if(grabJoint_)
					{
						driver_->world_->DestroyJoint(grabJoint_);
						grabJoint_ = 0;
					}
					stateMachine_->ChangeState(driver_->inAirState_);
				}

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

			case CHARACTER_MOVE_DOWN_PLUS:
			{
				stateMachine_->ChangeState(driver_->inAirState_);
				return true;
			}
		}
	}

	return false;
}

//=============================================================================
//								Exit
//
void ParkerStateLedgeGrab::Exit()
{
	if(grabJoint_)
	{
		driver_->world_->DestroyJoint(grabJoint_);
		grabJoint_ = 0;
	}
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
//								Climb
///
/// Getting a jump message while the character is grabbing a ledge will make
/// him climb. The time in the animation sets the position of the character
/// but he's being moved by setting his b2Body's position every frame.
/// He is moved toward the center of the LedgeGrabSensor.
void ParkerStateLedgeGrab::Climb()
{

	double timeLeft = driver_->animationBlender_->GetSource()->getTimePosition() / driver_->animationBlender_->GetSource()->getLength();

	b2Vec2 dest = ledgePosition_ - originalPosition_;
	driver_->SetBodyPosition(originalPosition_);

	// Different points in the animation will have Parker moving in different directions at different speeds.
	if(timeLeft < 0.8 && timeLeft > 0.1)
	{
		Ogre::Vector3 v(originalPosition_.x, originalPosition_.y + ((dest.y + 1.05) * timeLeft / 0.75),0);
		driver_->SetBodyPosition(b2Vec2(v.x,v.y));
		driver_->GetBody()->SetLinearVelocity(b2Vec2(0,0));
	}
	else if(timeLeft > 0.1 && timeLeft < 0.9)
	{
		Ogre::Vector3 v(originalPosition_.x + dest.x * (timeLeft + 0.1 - 0.9) * 5, originalPosition_.y + dest.y + 1.05,0);
		driver_->SetBodyPosition(b2Vec2(v.x,v.y));
		driver_->GetBody()->SetLinearVelocity(b2Vec2(0,0));
		Ogre::Skeleton* s = driver_->GetEntity()->getSkeleton();
	}
	else if(timeLeft > 0.9)
	{
		driver_->SetBodyPosition(b2Vec2(originalPosition_.x + dest.x, originalPosition_.y + dest.y + 1.05));
		driver_->GetBody()->SetLinearVelocity(b2Vec2(0,0));
		Ogre::Skeleton* s = driver_->GetEntity()->getSkeleton();
		climbing_ = false;

		driver_->animationBlender_->Blend("idle",AnimationBlender::BlendWhileAnimating, 0.1);
		stateMachine_->ChangeState(driver_->inAirState_);
	}


}

#include "XMLQuickVars.h"
//=============================================================================
//								PostSolve
//
/// We need to check if theres been a collission with Parker that has so much
/// force behind it that he will stop climbing.
void ParkerStateLedgeGrab::PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse)
{
	XMLQuickVars var("..\\Myvars.xml");
	float totalImpulse = abs(impulse->normalImpulses[0]);

	if(climbing_)
	{
		b2Body* b = collidedFixture->GetBody();

		if(b->GetType() != b2_staticBody)
		{
			if((totalImpulse > var.Double("ledge/maxForce") && totalImpulse < 500000) || (b->GetMass() * 9.8) > 470 )
			{
				if(grabJoint_)
				{
					driver_->world_->DestroyJoint(grabJoint_);
					grabJoint_ = 0;
				}

				stateMachine_->ChangeState(driver_->inAirState_);
			}
		}
	}
	
}

//=============================================================================
//								UpdateAnimation
void ParkerStateLedgeGrab::UpdateAnimation()
{
	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	driver_->animationBlender_->AddTime(timeSinceLastFrame);
}