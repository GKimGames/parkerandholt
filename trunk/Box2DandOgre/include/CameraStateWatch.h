/*=============================================================================

		CameraStateWatch.h

		Author: Matt King

=============================================================================*/

#ifndef CAMERA_STATE_WATCH_H
#define CAMERA_STATE_WATCH_H

#include "CameraState.h"
#include "GameObjectOgre.h"

class GameCamera;

class CameraStateWatchDef : public CameraStateDef
{
public:

	CameraStateWatchDef()
	{
		type = CAMERASTATE_WATCH;
	}

	GameObjectOgre* targetObject;

};

/// This class follows a GameObjectOgre around.
class CameraStateWatch : public CameraState
{
public:
	
	CameraStateWatch(GameCamera* camera, FSMStateMachine<GameCamera>* stateMachine);

	/// Destructor does nothing.
	~CameraStateWatch(){}

	virtual void InitializeDef(const CameraStateDef* cameraStateDef);

	/// This is called when the state is entered.
	virtual void Enter();

	/// Update.
	virtual bool Update();

	/// Used to handle messages.
	virtual bool HandleMessage(const KGBMessage message);

	/// This will execute when the state is exited.
	virtual void Exit();

protected:

	/// The target object the camera will watch.
	GameObjectOgre* targetObject_;

};

#endif 