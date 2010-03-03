/*=============================================================================

		Parker.h

		Author: Matt King

=============================================================================*/

#ifndef CHARACTER_PARKER_H
#define CHARACTER_PARKER_H

#include "Character.h"

#include "FSMStateMachine.h"

#include "Box2DXMLLoader.h"
#include "TraumaMeter.h"
#include "PlayerInfo.h"

#include "LedgeSensor.h"
#include "ParkerState.h"

class MousePicking;

/// Parkers class
class CharacterParker : public Character, public b2RayCastCallback
{
	friend class ParkerStateOnGround;
	friend class ParkerStateInAir;
	friend class ParkerStateLedgeGrab;
	friend class HoltStatePlacingPlatform;
	friend class HoltStatePlacingGravityVector;

public:

	CharacterParker(Ogre::SceneManager* sceneManager, MousePicking* mousePicking, PlayerInfo* info);

	~CharacterParker(){}

	bool HandleMessage(const KGBMessage message);

	bool Update(double timeSinceLastFrame);

	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
									const b2Vec2& normal, float32 fraction);

	/// Called when two fixtures begin to touch.
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	/// Called when two fixtures cease to touch.
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	
	void PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse);
	void ReturnToCheckPoint(b2Vec2);
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

	ParkerStateOnGround*			onGroundState_;
	ParkerStateInAir*				inAirState_;
	ParkerStateLedgeGrab*			ledgeGrabState_;
	HoltStatePlacingPlatform*		placingPlatform_;
	HoltStatePlacingGravityVector*	placingGravityVector_;

	b2Fixture*			feetSensor_;
	bool				feetSensorHit_;
	b2Fixture*			feetCircle_;

	b2Vec2				bodyVec1;
	b2Vec2				bodyVec2;
	b2Vec2				feetVec1;
	b2Vec2				feetVec2;

	int					rayCastId;

	// These sensors are used to tell where things are in front of Parker
	// Such as if something is only shin high, thigh high, or up to the torso.
	// Used to check if something above the character is within a grabbable reach.
	b2Fixture*			shinSensorRight_;
	bool				shinRightHit_;
	b2Fixture*			torsoSensorRight_;
	bool				torsoRightHit_;

	b2Fixture*			shinSensorLeft_;
	bool				shinLeftHit_;
	b2Fixture*			torsoSensorLeft_;
	bool				torsoLeftHit_;

	int					shinSensorHitCount_;
	int					thighSensorHitCount_;
	int					torsoSensorHitCount_;
	int					feetSensorHitCount_;
	MousePicking*		mousePicking_;

	double boundingBoxHeight_;
	double boundingBoxWidth_;

	b2Vec2 wallJumpForce_;

	double timeBetweenJump_;
	
	TraumaMeter*		traumaMeter_;
	PlayerInfo*			playerInfo_;

	GameObjectSensor*	ledge_;
	b2Fixture*			sensorLedge_;
};


#endif