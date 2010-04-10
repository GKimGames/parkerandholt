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
#include "GameCamera.h"

class MousePicking;
class Door;


/// Parkers class, extends Character. 
class CharacterParker : public Character
{
	// For convenience and efficiency the FSM States are friend classes so
	// they can change CharacterParker easily.
	friend class ParkerStateOnGround;
	friend class ParkerStateInAir;
	friend class ParkerStateLedgeGrab;

public:

	CharacterParker();

	~CharacterParker(){}

	/// Handles messages. Some messages are handled inside this class, others
	/// are delegated to the Finite State Machine.
	virtual bool HandleMessage(const KGBMessage message);

	virtual bool Update(double timeSinceLastFrame);

	/// Called when two fixtures begin to touch.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	/// Called when two fixtures cease to touch.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	
	virtual void PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse);
	
	/// Returns character to the currently active checkpoint by first panning the 
	/// camera to the checkpoint
	virtual void ReturnToCheckPoint();
	
	/// Initialize calls InitVariables, ReadXMLConfig, CreateGraphics,
	/// CreatePhysics, and sets up the AnimationBlender.
	virtual bool Initialize();
	
	/// Sets the character to active character, the character the player is
	/// controlling
	virtual void SetActive(bool active);
	
	/// Returns the PlayerInfo.
	virtual PlayerInfo* GetPlayerInfo();

	/// Returns if the fixture passed to it is the Characters foot sensor
	/// fixture.
	virtual bool IsFootSensor(b2Fixture* fixture);

protected:

	/// Initialize some class variables' default values.
	void InitVariables();

	/// Reads the Character's XML config and sets values.
	bool ReadXMLConfig();

	/// Create the Box2D representation of Parker.
	void CreatePhysics();

	/// Create the OGRE stuff for Parker.
	void CreateGraphics();

	/// Updates the Character's AnimationBlender.
	void UpdateAnimation(double timeSinceLastFrame);

	/// Applies "friction" to the character if they are on a surface.
	void ApplyWalkingFriction(double timeSinceLastFrame);
	b2Body* elevator_;

	FSMStateMachine<CharacterParker>*	stateMachine_;

	ParkerStateOnGround*			onGroundState_;
	ParkerStateInAir*				inAirState_;
	ParkerStateLedgeGrab*			ledgeGrabState_;

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