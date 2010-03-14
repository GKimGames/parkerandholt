/*=============================================================================

	  CameraStateGoToPoint.cpp

	  Author: Matt King

=============================================================================*/

#include "CameraStateGoToPoint.h"
#include "GameCamera.h"
#include "GameFramework.h"

//=============================================================================
//					CameraStateGoToPoint::Constructor
//
CameraStateGoToPoint::CameraStateGoToPoint(GameCamera* camera, FSMStateMachine<GameCamera>* stateMachine):
CameraState(camera, stateMachine)
{

	target_ = Ogre::Vector3(-20,5,14);
	factor_ = 2;
	toleranceDistance_ = 0.01;
}


//=============================================================================
//							Update
//
bool CameraStateGoToPoint::Update()
{

	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();
	Ogre::Vector3 camPos = driver_->camera_->getPosition();

	Ogre::Vector3 diff(0,0,0);

	diff.x = camPos.x - target_.x;
	diff.y = camPos.y - target_.y;
	diff.z = camPos.z - target_.z; 
	diff *= (factor_ * timeSinceLastFrame);

	Ogre::Real distance = camPos.squaredDistance(target_);
	if(distance > toleranceDistance_)
	{
		camPos -= diff;
		driver_->camera_->setPosition(camPos);
	}
	else
	{
		return true;
	}

	return true;
}



//=============================================================================
//							InitializeDef
//
void CameraStateGoToPoint::InitializeDef(const CameraStateDef* cameraStateDef)
{
	CameraStateGoToPointDef* def = (CameraStateGoToPointDef*) cameraStateDef;

	factor_ = def->factor;
	target_ = def->target;
	toleranceDistance_ = def->toleranceDistance;

}



//=============================================================================
//								Enter
//
void CameraStateGoToPoint::Enter()
{

}



//=============================================================================
//								Exit
//
void CameraStateGoToPoint::Exit()
{

}

//=============================================================================
//							HandleMessage
//
bool CameraStateGoToPoint::HandleMessage(const KGBMessage message)
{

	return false;
}


//===========================================================================//
//																			 //
//						Start Getters and Setters							 //
//																			 //
//===========================================================================//


//=============================================================================
//							SetFactor
//
/// The percentage of distance between the camera and target point that the
/// camera moves each update.
void CameraStateGoToPoint::SetFactor(const Ogre::Real factor)
{
	factor_ = factor;
}

//=============================================================================
//							SetPoint
//
/// Set the point the camera will move towards
void CameraStateGoToPoint::SetPoint(const Ogre::Vector3 point)
{
	target_ = point;
}

//=============================================================================
//							SetToleranceDistance
//
/// Set how close a the camera neads to be to a point to be considered to 
/// have reached that point.
void CameraStateGoToPoint::SetToleranceDistance(const Ogre::Real tolerance)
{
	toleranceDistance_ = tolerance;
}