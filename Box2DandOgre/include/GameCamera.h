/*=============================================================================

		GameCamera.h

		Author: Matt King

=============================================================================*/

#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#include "Ogre.h"

#include "FSMStateMachine.h"

#include "CameraState.h"

class GameObjectOgre;
class GameObject;

class GameCamera : public GameObject
{
	friend class CameraStateGoToPoint;
	friend class CameraStateWatch;

public:

	/// The GameCamera takes a string for the name of the GameCamera object (as
	/// well as the name of the camera) and an initial position.
	///
	/// The name of the camera will always be the same name of the GameObject
	/// when created using this constructor.
	GameCamera(Ogre::String name = "GameCamera", Ogre::Vector3 position = Ogre::Vector3::ZERO);
	
	/// Pass an already created camera to GameCamera and an optional name for the
	/// GameCamera object.
	GameCamera(Ogre::Camera* camera, Ogre::String name = "GameCamera");

	/// Destroy the camera if we created it.
	~GameCamera();

	/// Update the camera.
	bool Update(double timeSinceLastFrame);

	void InitializeDef(CameraStateDef* cameraStateDef);

//===========================================================================//
//																			 //
//						Start Getters and Setters							 //
//																			 //
//===========================================================================//

	/// Returns the Ogre Camera pointer.
	Ogre::Camera* GetOgreCamera();

protected:

	FSMStateMachine<GameCamera>* stateMachine_;
	Ogre::Camera* camera_;

	CameraStateGoToPoint* cameraStateGoToPoint;
	CameraStateWatch* cameraStateWatch_;
	bool createdCamera_;
	

};

#endif