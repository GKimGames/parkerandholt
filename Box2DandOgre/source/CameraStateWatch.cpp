/*=============================================================================

	  CameraStateWatch.cpp

	  Author: Matt King

=============================================================================*/

#include "CameraStateWatch.h"
#include "GameCamera.h"
#include "GameFramework.h"

//=============================================================================
//					CameraStateWatch::Constructor
//
CameraStateWatch::CameraStateWatch(GameCamera* camera, FSMStateMachine<GameCamera>* stateMachine):
CameraState(camera, stateMachine)
{
	targetObject_ = 0;
}


//=============================================================================
//							Update
//
bool CameraStateWatch::Update()
{

	double timeSinceLastFrame = GAMEFRAMEWORK->GetTimeSinceLastFrame();

	if(targetObject_)
	{
		Ogre::Vector3 target = targetObject_->GetPosition();
		driver_->camera_->setPosition(target.x, target.y + initialPosition_.y, initialPosition_.z);
		driver_->camera_->lookAt(target);
	}

	return true;
}



//=============================================================================
//							InitializeDef
//
void CameraStateWatch::InitializeDef(const CameraStateDef* cameraStateDef)
{
	CameraStateWatchDef* def = (CameraStateWatchDef*) cameraStateDef;

	targetObject_ = def->targetObject;
	initialPosition_ = def->initialPosition;

}



//=============================================================================
//								Enter
//
void CameraStateWatch::Enter()
{

}



//=============================================================================
//								Exit
//
void CameraStateWatch::Exit()
{

}

//=============================================================================
//							HandleMessage
//
bool CameraStateWatch::HandleMessage(const KGBMessage message)
{

	return false;
}


//===========================================================================//
//																			 //
//						Start Getters and Setters							 //
//																			 //
//===========================================================================//