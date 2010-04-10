/*=============================================================================

		CameraState.h

		Author: Matt King

=============================================================================*/

#ifndef CAMERA_STATE_H
#define CAMERA_STATE_H

#include "FSMState.h"
#include "Message.h"



enum CameraStateType
{
	CAMERASTATE_GOTOPOINT,
	CAMERASTATE_WATCH,
	CAMERASTATETYPE_COUNT
};

class GameCamera;

/// Base class for a CameraStateDef. This allows for the camera state to setup
/// using a data class. Each extended class can then read that data class
/// in the way it needs to.
class CameraStateDef
{
public:
	/// The type is used so when a CameraState is passed into the Camera's
	/// FSM it knows which State will want to load this def.
	CameraStateType type;
	Ogre::Vector3 initialPosition;
};

/// A Camera State Controls how the camera moves around and if its following
/// some GameObject.
class CameraState : public FSMState<GameCamera>
{
public:
	
	CameraState(GameCamera* camera, FSMStateMachine<GameCamera>* stateMachine);

	/// Destructor does nothing.
	~CameraState(){}

	/// Initialize some CameraStateDefinition.
	virtual void InitializeDef(const CameraStateDef* cameraStateDef)=0;

	/// This is called when the state is entered.
	virtual void Enter()=0;

	/// Update.
	virtual bool Update()=0;

	/// This will execute when the state is exited.
	virtual void Exit()=0;

	/// Used to handle messages.
	virtual bool HandleMessage(const KGBMessage message)=0;

protected:

	Ogre::Vector3 initialPosition_;
};

#endif 