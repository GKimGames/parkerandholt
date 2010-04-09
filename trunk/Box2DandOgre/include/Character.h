/*=============================================================================

		Character.h

		Author: Matt King

=============================================================================*/

#ifndef CHARACTER_H
#define CHARACTER_H

#include "AppState.hpp"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <Ogre.h>
#include <tinyxml.h>
#include "GameObjectOgreBox2D.h"

#include "FSMStateMachine.h"

#include "OgreXMLLoader.h"


/// Base class for a character. Character extends GameObjectOgreBox2D
class Character :public GameObjectOgreBox2D
{
public:

	/// Sets gameObjectType_ to GOType_Character.
	Character();
	
	~Character();

	/// Update applies the walking friction to the character and updates the 
	/// models animation and direction.
	virtual bool Update(double timeSinceLastFrame)=0;

	/// Called when two fixtures begin to touch.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)=0;

	/// Called when two fixtures cease to touch.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)=0;

	virtual bool HandleMessage(const KGBMessage message)=0;

	virtual bool Initialize()=0;

protected:
		 
	virtual void UpdateAnimation(double timeSinceLastFrame)=0;
	
	/// Sets the default values for variables.
	virtual void InitVariables()=0;

	/// Read in variables values from XML.
	virtual bool ReadXMLConfig()=0;

	/// Create Box2D representation of the character.
	virtual void CreatePhysics()=0;

	/// Create the mesh and animation object for the character.
	virtual void CreateGraphics()=0;

	/// Applies "friction" to the character if they are on a surface.
	virtual void ApplyWalkingFriction(double timeSinceLastFrame)=0;

	// There needs to be a finite state machine but I didn't want to have to 
	// template the character class so I left this here as a reminder.
	//FSMStateMachine<Character>*	stateMachine_;
  
	Ogre::String			meshName_;
	Ogre::AnimationState*	animationState_; // The current animation state of the object

	Ogre::SceneNode*	bodyNode_;

	/// On the bottom of the character to tell if its on something
	b2Fixture*			feetSensor_;

	// These sensors are used to tell where things are in front of the character
	// Such as if something is only shin high, thigh high, or up to the torso.
	// Used to check if something above the character is within a grabbable reach.
	b2Fixture*			shinSensor_;
	b2Fixture*			thighSensor_;
	b2Fixture*			torsoSensor_;
	b2Fixture*			wallJumpSensor_;

	int					shinSensorHitCount_;
	int					torsoSensorHitCount_;

	double				mass_;

	double				capsuleHeight_;
	double				capsuleRadius_;

	bool				isRunning_;
	bool				isTouchingSurface_;

	double				runningForce_;
	double				walkingForce_;
	double				jumpingForce_;
	double				jumpingAfterForce_;
	double				airForce_;

	double				maximumWalkingVelocity_;
	double				maximumRunningVelocity_;
	double				maximumAirVelocity_;
						
	double				friction_;
	double				restitution_;
	double				linearDamping_;

	Ogre::Vector3		translate_;
	double				translateX;
	double				translateY;
	double				translateZ;	

	Ogre::Vector3		scale_;
	double				scaleX_;
	double				scaleY_;
	double				scaleZ_;

	double				rotateY;

	bool				justJumped;
	bool				justWallJumped_;
	bool				moveLeft_;
	bool				moveRight_;
	bool				jump_;

	int					canJump_;

};

#endif 