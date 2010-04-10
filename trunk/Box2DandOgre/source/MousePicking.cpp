/*=============================================================================

		MousePicking.cpp

		Author: Greg King

=============================================================================*/

#include "MousePicking.h"
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <boost/any.hpp>
#include "HoltBox.h"
#include "Platform.h"
#include "Triangle.h"

#include "Message.h"
#include "MessageDispatcher.h"

//=============================================================================
//								Constructor
//
/// Creates teh graphic for the mouse and establishes its camera and sceneManager
MousePicking::MousePicking(Ogre::SceneManager* sceneManager, Ogre::Camera* camera)
{
	boxSize_ = 1;
	boxMinSize_ = 0.5;
	boxMaxSize_ = 2.0;
	boxSizeIncrement_ = 0.25;
	sceneManager_ = sceneManager;
	camera_ = camera;
	pickingPlane_ = new Ogre::Plane(Ogre::Vector3::UNIT_Z, 0);
	gameObjectType_ = GOType_Mouse;

	RSQ_ = sceneManager_->createRayQuery(Ogre::Ray());

	entity_ = sceneManager_->createEntity("mouse", "TransparentBox.mesh");

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	sceneNode_->attachObject(entity_);
	entity_->getParentSceneNode()->setScale(1,1,1);

	deleteCheck_ = false;

	entity_->setVisible(false);


	b2BodyDef def;
	def.position = b2Vec2(0, 0);
	def.type = b2BodyType::b2_staticBody;
	def.fixedRotation = false;
	body_ = world_->CreateBody(&def);
	body_->SetUserData(this);

	b2PolygonShape sd;
	sd.SetAsBox(.25, .25);

	b2FixtureDef fd;
	fd.shape = &sd;
	fd.isSensor = true;

	body_->CreateFixture(&fd);

	Initialize();

	body_->GetFixtureList()->SetSensor(true);
	body_->SetActive(false);

	
}

//=============================================================================
//								UpdateMouse
//
/// Updates the mouse position based ont he mouseEvent and Holt's position
void MousePicking::UpdateMouse(const OIS::MouseEvent &arg, b2Vec2 holtPosition)
{
	// Is the maximum distance the mouse can be from Holt, creates a radius
	float maxDistance = 10;

	// Positions for the raycast to be performed to a plane at z = 0
	rayPositionX_ = arg.state.X.abs/float(arg.state.width);
	rayPositionY_ = arg.state.Y.abs/float(arg.state.height);

	Ogre::Ray mouseRay = camera_->getCameraToViewportRay(  rayPositionX_, rayPositionY_ );
	RSQ_ = sceneManager_->createRayQuery(Ogre::Ray());
	RSQ_->setRay(mouseRay);
	
	// Store the results of the raycast
	std::pair<bool, Ogre::Real> hit = mouseRay.intersects(*pickingPlane_);
	
	// If the cast hits the plane store its position
	if(hit.first)
	{
		position_ = Ogre::Vector3(mouseRay.getPoint(hit.second).x, mouseRay.getPoint(hit.second).y, 0);
		entity_->getParentSceneNode()->setPosition(position_);
	}

	// Calculate the distance between Holt and the mouse
	float tempDistance = sqrt((position_.x - holtPosition.x) * (position_.x - holtPosition.x)
							 + (position_.y - holtPosition.y) * (position_.y - holtPosition.y));

	// If the distance is too great set the mouse to be at the maximum distance possible in the direction the mouse is
	if(tempDistance > maxDistance)
	{
		Ogre::Vector3 tempPosition;
		tempPosition.x = holtPosition.x + ((position_.x - holtPosition.x)/tempDistance) * maxDistance;
		tempPosition.y = holtPosition.y + ((position_.y - holtPosition.y)/tempDistance) * maxDistance;
		tempPosition.z = 0;

		position_ = tempPosition;
	}

	// Mouse movment in the Z direction is mouseWheel scrolling
	// used to incriment the box size
	if(arg.state.Z.rel > 0)
	{
		if(boxSize_ > boxMinSize_)
		{
			boxSize_ -= boxSizeIncrement_;
		}
	}
	if(arg.state.Z.rel < 0)
	{
		if(boxSize_ < boxMaxSize_)
		{
			boxSize_ += boxSizeIncrement_;
		}
	}

	// Update the size of the mouse cursor
	entity_->getParentSceneNode()->setScale( boxSize_, boxSize_, boxSize_);
}

//=============================================================================
//								GetPosition
//
/// Returns the current mouse position
const Ogre::Vector3 MousePicking::GetPosition()
{
	return position_;
}

//=============================================================================
//								HandleMessage
//
bool MousePicking::HandleMessage(const KGBMessage message)
{
	return false;
}

//=============================================================================
//								SetVisibility
//
/// Controls if the cursor is visible
void MousePicking::SetVisibility(bool visible)
{
	sceneNode_->setVisible(visible);
}

//=============================================================================
//								DeletePlaceables
//
/// enables delete check and the body and steps the world a very small amount to get new collisions
void MousePicking::DeletePlaceables()
{
	deleteCheck_ = true;
	body_->SetActive(true);
	world_->Step(0.001, 1, 1);
}


//=============================================================================
//								BeginContact
//
/// Called when bodies begin to touch
void MousePicking::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	// If delete check is on checks for contacts with the mouse cursor, sets them inactive if there is a contact with a placeable type
	if(deleteCheck_)
	{
		if(contactFixture->GetBody() == body_ && collidedFixture->GetBody()->GetUserData() != 0)
		{
			GameObjectOgreBox2D* temp = (GameObjectOgreBox2D*)collidedFixture->GetBody()->GetUserData();
			if(temp->GetGameObjectType() == GOType_PlaceablePlatform)
			{
				Platform* temp2 = (Platform*)temp;
				temp2->SetInactive();
			}
			else if(temp->GetGameObjectType() == GOType_HoltBox)
			{
				HoltBox* temp2 = (HoltBox*)temp;
				temp2->SetInactive();
			}
			else if(temp->GetGameObjectType() == GOType_Triangle)
			{
				Triangle* temp2 = (Triangle*)temp;
				temp2->SetInactive();
			}
		}
		deleteCheck_ = false;
	}
}

//=============================================================================
//								Update
//
bool MousePicking::Update(double timeSinceLastFrame)
{
	// This small timestep is only used for delete checks, disables the check if its found
	if(timeSinceLastFrame > .001)
	{
		deleteCheck_ = false;
		body_->SetActive(false);
	}

	SetBodyPosition(b2Vec2(position_.x, position_.y));
	UpdateGraphics(timeSinceLastFrame);
	
	return true;
}