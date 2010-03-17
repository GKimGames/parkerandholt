/*=============================================================================

		CameraStateGoToPoint.h

		Author: Matt King

=============================================================================*/

#ifndef CAMERA_STATE_GO_TO_POINT_H
#define CAMERA_STATE_GO_TO_POINT_H

#include "CameraState.h"
#include "MessageDispatcher.h"

class GameCamera;

class CameraStateGoToPointDef : public CameraStateDef
{
public:

	CameraStateGoToPointDef()
	{
		type = CAMERASTATE_GOTOPOINT;
	}

	Ogre::Vector3 initialPosition;
	
	Ogre::Vector3 target;

	Ogre::Real factor;

	Ogre::Real toleranceDistance;

	KGBMessageType messageType;
	

};

class CameraStateGoToPoint : public CameraState
{
public:
	
	CameraStateGoToPoint(GameCamera* camera, FSMStateMachine<GameCamera>* stateMachine);

	/// Destructor does nothing.
	~CameraStateGoToPoint(){}

	/// Initialized a CameraStateDef.
	virtual void InitializeDef(const CameraStateDef* cameraStateDef);


	/// Set the point the camera will move towards
	virtual void SetPoint(const Ogre::Vector3 point);

	/// The percentage of distance between the camera and target point that the
	/// camera moves each update.
	virtual void SetFactor(const Ogre::Real factor);

	/// Set how close a the camera neads to be to a point to be considered to 
	/// have reached that point.
	virtual void SetToleranceDistance(const Ogre::Real tolerance);

	/// This is called when the state is entered.
	virtual void Enter();

	/// Update.
	virtual bool Update();

	/// Used to handle messages.
	virtual bool HandleMessage(const KGBMessage message);

	/// This will execute when the state is exited.
	virtual void Exit();

protected:

	Ogre::Vector3 target_;

	Ogre::Real factor_;

	Ogre::Real toleranceDistance_;

	KGBMessageType messageType_;

	Ogre::Vector3 diff;

	bool done_;
};

#endif 