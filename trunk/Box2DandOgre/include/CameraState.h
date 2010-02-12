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

class CameraStateDef
{
public:
	CameraStateType type;
	Ogre::Vector3 initialPosition;
};

class CameraState : public FSMState<GameCamera>
{
public:
	
	CameraState(GameCamera* camera, FSMStateMachine<GameCamera>* stateMachine);

	/// Destructor does nothing.
	~CameraState(){}

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