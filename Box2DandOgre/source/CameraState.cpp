/*=============================================================================

	  CameraState.cpp

	  Author: Matt King

=============================================================================*/

#include "CameraState.h"
#include "GameCamera.h"

//=============================================================================
//					CameraState::Constructor
//
CameraState::CameraState(GameCamera* camera, FSMStateMachine<GameCamera>* stateMachine):
FSMState(camera, stateMachine)
{
}


//===========================================================================//
//																			 //
//						Start Getters and Setters							 //
//																			 //
//===========================================================================//