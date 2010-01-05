/*=============================================================================

  Parker.h

  Author: Matt King

=============================================================================*/

#ifndef CHARACTER_PARKER_H
#define CHARACTER_PARKER_H

#include "Character.h"
#include "ParkerStateOnGround.h"
#include "ParkerStateInAir.h"
#include "PhysicsState.h"

#include "Box2DXMLLoader.h"

/// Parkers class
class CharacterParker : public Character, public b2RayCastCallback
{
	friend class ParkerStateOnGround;
	friend class ParkerStateInAir;

public:

	CharacterParker(Ogre::SceneManager* sceneManager);

	~CharacterParker(){}

	bool HandleMessage(const KGBMessage message)
	{ 
		return stateMachine_->HandleMessage(message); 
	}

	bool Update(double timeSinceLastFrame)
	{
		/*
		static b2Color color(1,1,0);
		bodyVec1 = body_->GetWorldPoint(b2Vec2(-boundingBoxWidth_/2,	0));
		bodyVec2 = body_->GetWorldPoint(b2Vec2(boundingBoxWidth_/2,	0));
		feetVec1 = body_->GetWorldPoint(b2Vec2(-boundingBoxWidth_/2,	-boundingBoxHeight_/2));
		feetVec2 = body_->GetWorldPoint(b2Vec2(boundingBoxWidth_/2,	-boundingBoxHeight_/2));

		world_->RayCast(this, bodyVec1, feetVec1);
		world_->RayCast(this, bodyVec2, feetVec2);

		if(GAMEFRAMEWORK->GetDebugDraw())
		{
			GAMEFRAMEWORK->GetDebugDraw()->DrawSegment(bodyVec1, feetVec1, color);
			GAMEFRAMEWORK->GetDebugDraw()->DrawSegment(bodyVec2, feetVec2, color);
		}
		*/
		return stateMachine_->Update();
	}

	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
									const b2Vec2& normal, float32 fraction)
	{
		/*
		static bool oneTwo = true;
		static b2Color color(1,0,234.0/255.0);
		
		if(oneTwo == false)
		{
			GAMEFRAMEWORK->GetDebugDraw()->DrawSegment(bodyVec1, point, color);
			oneTwo = true;
		}
		else
		{
			GAMEFRAMEWORK->GetDebugDraw()->DrawSegment(bodyVec2, point, color);
			oneTwo = false;
		}

		body_->ApplyForce(b2Vec2(0,abs(body_->GetMass() * world_->GetGravity().y * 2)), body_->GetPosition());
		body_->SetLinearVelocity(b2Vec2(0,0));
		*/
		return 0;

	}

	/// Called when two fixtures begin to touch.
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		if(collidedFixture->IsSensor() == false)
		{
			if(contactFixture == feetSensor_ )
			{
				feetSensorHitCount_++;
			}
		}

		stateMachine_->BeginContact(contact,contactFixture, collidedFixture);
	}

	/// Called when two fixtures cease to touch.
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		if(collidedFixture->IsSensor() == false)
		{
			if(contactFixture == feetSensor_ )
			{
				feetSensorHitCount_--;
			}
		}

		stateMachine_->EndContact(contact,contactFixture, collidedFixture);

	}


	bool Initialize();

protected:

	void InitVariables();
	bool ReadXMLConfig();
	void CreatePhysics();
	void CreateGraphics();

private:

	void UpdateAnimation(double timeSinceLastFrame);

	/// Applies "friction" to the character if they are on a surface.
	void ApplyWalkingFriction(double timeSinceLastFrame);
	b2Body* elevator_;

	FSMStateMachine<CharacterParker>*	stateMachine_;

	ParkerStateOnGround*	onGroundState_;
	ParkerStateInAir*		inAirState_;

	b2Fixture*			feetSensor_;

	b2Vec2				bodyVec1;
	b2Vec2				bodyVec2;
	b2Vec2				feetVec1;
	b2Vec2				feetVec2;

	// These sensors are used to tell where things are in front of Parker
	// Such as if something is only shin high, thigh high, or up to the torso.
	// Used to check if something above the character is within a grabbable reach.
	b2Fixture*			shinSensor_;
	b2Fixture*			thighSensor_;
	b2Fixture*			torsoSensor_;

	int					shinSensorHitCount_;
	int					thighSensorHitCount_;
	int					torsoSensorHitCount_;
	int					feetSensorHitCount_;

	double boundingBoxHeight_;
	double boundingBoxWidth_;
	double wallJumpForce_;

	double timeBetweenJump_;

};


#endif