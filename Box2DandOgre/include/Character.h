/*=============================================================================
-------------------------------------------------------------------------------
  Character.h

  Author: Matt King

=============================================================================*/

#ifndef CHARACTER_H
#define CHARACTER_H

#define OGRE_LOG(t) GameFramework::getSingletonPtr()->log_->logMessage(t)

#define DEBUGGING 1

#if DEBUGGING
#define DLOG(X) OutputDebugString(X); OutputDebugString("\n") 
#else
#define DLOG(X) 
#endif


#include "AppState.hpp"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <Ogre.h>
#include <tinyxml.h>

#include "Box2D.h"

#include "AnimationBlender.h"

#include "GameObjectOgreBox2D.h"

#include "FSMStateMachine.h"

/// Base class for a character. Character extends GameObjectOgreBox2D
class Character :public GameObjectOgreBox2D
{
public:

	Character(Ogre::SceneManager* sceneManager, b2World* world);
	~Character();

	virtual void KeyPressed(const OIS::KeyEvent &keyEventRef);
	virtual void KeyReleased(const OIS::KeyEvent &keyEventRef);

	void MouseMoved(const OIS::MouseEvent &arg);
	void MousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 
	void MouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	bool Update(double timeSinceLastFrame);

	void GetInput(double timeSinceLastFrame);

	/// Called when two fixtures begin to touch.
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	/// Called when two fixtures cease to touch.
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	
	virtual bool UpdateGraphics(double timeSinceLastFrame){return true;};

	virtual void Jump();

	bool HandleMessage(const KGBMessage message)
	{

		if(message.messageType == CHARACTER_MOVE_LEFT)
		{
			MoveLeft();
		}

		if(message.messageType == CHARACTER_MOVE_RIGHT)
		{
			MoveRight();
		}

		return true;
	}

protected:
		 
	virtual void UpdateAnimation(double timeSinceLastFrame);
	virtual void InitVariables();
	virtual void EvaluateContacts(){;};

	/// Applies "friction" to the character if they are on a surface.
	virtual void ApplyWalkingFriction(double timeSinceLastFrame);

	virtual void MoveLeft();
	virtual void MoveRight();

	// There needs to be a finite state machine but I didn't want to have to 
	// template the character class so I left this here as a reminder.
	//FSMStateMachine<Character>*	stateMachine_;
  
	Ogre::String			meshName_;
	Ogre::AnimationState*	animationState_; // The current animation state of the object
	AnimationBlender*		animationBlender_;

	Ogre::SceneNode*	bodyNode_;

	b2PolygonDef*		bodyDef;

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
	int					thighSensorHitCount_;
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

	double				translateX;
	double				translateY;
	double				translateZ;	

	double				scaleX_;
	double				scaleY_;
	double				scaleZ_;

	double				rotateY;

	bool				justJumped;
	bool				justWallJumped_;

	int					canJump_;

};

#endif 