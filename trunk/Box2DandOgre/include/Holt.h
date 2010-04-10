/*=============================================================================

		Holt.h

=============================================================================*/

#ifndef CHARACTER_HOLT_H
#define CHARACTER_HOLT_H

#include "Parker.h"
#include "Character.h"

#include "FSMStateMachine.h"

#include "Box2DXMLLoader.h"
#include "TraumaMeter.h"
#include "PlayerInfo.h"

#include "LedgeSensor.h"
#include "ParkerState.h"
#include "GameCamera.h"

class MousePicking;
class Door;

class CharacterHolt: public Character
{
	friend class HoltStateOnGround;
	friend class HoltStateInAir;
	friend class HoltStateLedgeGrab;
	friend class HoltStatePlacingPlatform;
	friend class HoltStatePlacingStatic;

public:

	CharacterHolt(MousePicking* mousePicking);
	~CharacterHolt(){}
	bool Initialize();

	bool HandleMessage(const KGBMessage message);

	bool Update(double timeSinceLastFrame);

	/// Called when two fixtures begin to touch.
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	/// Called when two fixtures cease to touch.
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	
	void PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse);

	/// Returns the character to the last touched checkpoint
	void ReturnToCheckPoint();

	/// Sets if this character is the character being controled by the player
	void SetActive(bool active);

	// Returns this characters player information
	PlayerInfo* GetPlayerInfo();

	bool IsFootSensor(b2Fixture* fixture);

protected:

	void InitVariables();
	bool ReadXMLConfig();
	void CreatePhysics();
	void CreateGraphics();


	void UpdateAnimation(double timeSinceLastFrame);

	/// Applies "friction" to the character if they are on a surface.
	void ApplyWalkingFriction(double timeSinceLastFrame);
	b2Body* elevator_;

	FSMStateMachine<CharacterHolt>*	stateMachine_;

	HoltStateOnGround*				onGroundState_;
	HoltStateInAir*					inAirState_;
	HoltStatePlacingPlatform*		placingPlatform_;
	HoltStatePlacingStatic*			placingStatic_;

	b2Fixture*			feetSensor_;
	bool				feetSensorHit_;
	b2Fixture*			feetCircle_;

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

	MousePicking*		mousePicking_;

	double boundingBoxHeight_;
	double boundingBoxWidth_;

	b2Vec2 wallJumpForce_;

	double timeBetweenJump_;
	
	TraumaMeter*		traumaMeter_;
	PlayerInfo*			playerInfo_;

	GameObjectSensor*	ledge_;
	b2Fixture*			sensorLedge_;

	GameCamera*			gameCamera_;
	Ogre::Camera*		camera_;

	// Controls if the character is currently being controled by the players
	bool				active_;
	Door*				door_;

};

#endif