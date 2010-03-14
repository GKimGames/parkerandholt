/*=============================================================================

		GameCamera.cpp

		Author: Matt King

=============================================================================*/

#include "GameCamera.h"
#include "GameObject.h"
#include "GameObjectOgre.h"
#include "GameFramework.h"

#include "CameraStateGoToPoint.h"
#include "CameraStateWatch.h"


//=============================================================================
//							GameCamera::Constructor
//
/// The GameCamera takes a string for the name of the GameCamera object (as
/// well as the name of the camera) and an initial position.
///
/// The name of the camera will always be the same name of the GameObject
/// when created using this constructor.
GameCamera::GameCamera(Ogre::String name, Ogre::Vector3 position) :
GameObject(name)
{
	createdCamera_ = true;

	GameObject::Initialize();

	camera_ = GAMEFRAMEWORK->sceneManager->createCamera(objectName_);
	camera_->setPosition(position);


	stateMachine_ = new FSMStateMachine<GameCamera>(this);

	cameraStateGoToPoint = new CameraStateGoToPoint(this, stateMachine_);
	cameraStateWatch_ = new CameraStateWatch(this, stateMachine_);

	stateMachine_->SetCurrentState(cameraStateGoToPoint);
	stateMachine_->ChangeState(cameraStateGoToPoint);
}



//=============================================================================
//							GameCamera::Constructor
//
/// Pass an already created camera to GameCamera and an optional name for the
/// GameCamera object.
GameCamera::GameCamera(Ogre::Camera* camera, Ogre::String name) :
GameObject(name), camera_(camera)
{
	createdCamera_ = false;

	GameObject::Initialize();

	stateMachine_ = new FSMStateMachine<GameCamera>(this);

	cameraStateGoToPoint = new CameraStateGoToPoint(this, stateMachine_);
	cameraStateWatch_ = new CameraStateWatch(this, stateMachine_);

	stateMachine_->SetCurrentState(cameraStateGoToPoint);
	stateMachine_->ChangeState(cameraStateGoToPoint);
}

//=============================================================================
//							GameCamera::Destructor
//
GameCamera::~GameCamera()
{
	if(createdCamera_)
	{
		GAMEFRAMEWORK->sceneManager->destroyCamera(camera_);
	}
}


//=============================================================================
//							Update
//
bool GameCamera::Update(double timeSinceLastFrame)
{
	return stateMachine_->Update();
}

//=============================================================================
//							Update
//
void GameCamera::InitializeDef(CameraStateDef* cameraStateDef)
{
	// This would be better done with a map of Camera Types
	// and the camera states objects. Theres only a few
	// camera types though, so it is easier this way.

	if(cameraStateDef->type == CAMERASTATE_WATCH)
	{
		cameraStateWatch_->InitializeDef(cameraStateDef);
		stateMachine_->ChangeState(cameraStateWatch_);
	}
	else if(cameraStateDef->type == CAMERASTATE_GOTOPOINT)
	{
		cameraStateGoToPoint->InitializeDef(cameraStateDef);
		stateMachine_->ChangeState(cameraStateGoToPoint);
	}
}