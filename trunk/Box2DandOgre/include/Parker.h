/*=============================================================================

  Parker.h

  Author: Matt King

=============================================================================*/

#ifndef CHARACTER_PARKER_H
#define CHARACTER_PARKER_H

#include "Character.h"
#include "ParkerStateOnGround.h"
#include "ParkerStateInAir.h"


//class ParkerStateOnGround;
//class ParkerStateInAir;

/// Parkers class
class CharacterParker : public Character
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
		return stateMachine_->Update();
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

	FSMStateMachine<CharacterParker>*	stateMachine_;

	ParkerStateOnGround*	onGroundState_;
	ParkerStateInAir*		inAirState_;

	b2Fixture*			feetSensor_;
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