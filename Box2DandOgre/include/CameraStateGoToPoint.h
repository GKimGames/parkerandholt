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

	/// Initial position of the camera
	Ogre::Vector3 initialPosition;
	
	/// The Vector the camera wants to move to.
	Ogre::Vector3 target;

	/// How fast the camera gets there.
	Ogre::Real factor;

	/// The distance in which the camera needs to be within to be considered
	/// have reached the target.
	Ogre::Real toleranceDistance;

	KGBMessageType messageType;

	GameObjectId messageTarget;
	
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
	
	/// The Vector the camera wants to move to.
	Ogre::Vector3 target_;

	/// How fast the camera gets there.
	Ogre::Real factor_;

	/// The distance in which the camera needs to be within to be considered
	/// have reached the target.
	Ogre::Real toleranceDistance_;

	KGBMessageType messageType_;

	GameObjectId messageTarget_;

	/// The distance between the camera's position and the destination
	Ogre::Vector3 diff;

	/// Has the camera reached the destination?
	bool done_;
};

#endif 