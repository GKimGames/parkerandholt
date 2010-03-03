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

#include "Message.h"
#include "MessageDispatcher.h"

MousePicking::MousePicking(Ogre::SceneManager* sceneManager, Ogre::Camera* camera)
{
	boxSize_ = 1;
	boxMinSize_ = 0.5;
	boxMaxSize_ = 2.0;
	boxSizeIncrement_ = 0.25;
	sceneManager_ = sceneManager;
	m_pCamera_ = camera;
	pickingPlane_ = new Ogre::Plane(Ogre::Vector3::UNIT_Z, 0);
	gameObjectType_ = GOType_Mouse;

	m_pRSQ_ = sceneManager_->createRayQuery(Ogre::Ray());

	entity_ = sceneManager_->createEntity("mouse", "TransparentBox.mesh");

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
	sceneNode_->attachObject(entity_);
	entity_->getParentSceneNode()->setScale(1,1,1);

	//sceneNode_->setVisible(false);
	box_[0] = 0;
	box_[1] = 0;
	box_[2] = 0;
	incrementer_ = 0;
}


void MousePicking::MouseMoved(const OIS::MouseEvent &arg)
{
	sceneNode_->setVisible(true);

	rayPositionX_ = arg.state.X.abs/float(arg.state.width);
	rayPositionY_ = arg.state.Y.abs/float(arg.state.height);

	Ogre::Ray mouseRay = m_pCamera_->getCameraToViewportRay(  rayPositionX_, rayPositionY_ );

	m_pRSQ_ = sceneManager_->createRayQuery(Ogre::Ray());
	m_pRSQ_->setRay(mouseRay);
	
	std::pair<bool, Ogre::Real> hit = mouseRay.intersects(*pickingPlane_);
	
	if(hit.first)
	{
		position_ = Ogre::Vector3(mouseRay.getPoint(hit.second).x, mouseRay.getPoint(hit.second).y, 0);
		entity_->getParentSceneNode()->setPosition(position_);
	}

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

	entity_->getParentSceneNode()->setScale( boxSize_, boxSize_, boxSize_);
}


void MousePicking::UpdateMouseFromCamera()
{

	Ogre::Ray mouseRay = m_pCamera_->getCameraToViewportRay(  rayPositionX_, rayPositionY_ );
	m_pRSQ_ = sceneManager_->createRayQuery(Ogre::Ray());
	m_pRSQ_->setRay(mouseRay);
	std::pair<bool, Ogre::Real> hit = mouseRay.intersects(*pickingPlane_);
	if(hit.first)
	{
		position_ = Ogre::Vector3(mouseRay.getPoint(hit.second).x, mouseRay.getPoint(hit.second).y, 0);
		entity_->getParentSceneNode()->setPosition(position_);
	}
}

void MousePicking::UpdateMouse(const KGBMessage message)
{
	const OIS::MouseEvent* tempMouse_ = boost::any_cast<const OIS::MouseEvent*>(message.messageData);

	//OIS::MouseEvent* tempMouse_ = any_cast<OIS::MouseEvent>(message.messageData);

	rayPositionX_ = tempMouse_->state.X.abs/float (tempMouse_->state.width);
	rayPositionY_ = tempMouse_->state.Y.abs/float (tempMouse_->state.height);
	//UpdateMouse();

	Ogre::Ray mouseRay = m_pCamera_->getCameraToViewportRay(rayPositionX_, rayPositionY_);
	m_pRSQ_ = sceneManager_->createRayQuery(Ogre::Ray());
	m_pRSQ_->setRay(mouseRay);
	std::pair<bool, Ogre::Real> hit = mouseRay.intersects(*pickingPlane_);
	if(hit.first)
	{
		position_ = Ogre::Vector3(mouseRay.getPoint(hit.second).x, mouseRay.getPoint(hit.second).y, 0);
		entity_->getParentSceneNode()->setPosition(position_);
	}

	if(tempMouse_->state.Z.rel > 0)
	{
		if(boxSize_ > boxMinSize_)
		{
			boxSize_ -= boxSizeIncrement_;
		}
	}

	if(tempMouse_->state.Z.rel < 0)
	{
		if(boxSize_ < boxMaxSize_)
		{
			boxSize_ += boxSizeIncrement_;
		}
	}

	entity_->getParentSceneNode()->setScale( boxSize_, boxSize_, boxSize_);

}


const Ogre::Vector3 MousePicking::GetPosition()
{
	return position_;
}


bool MousePicking::HandleMessage(const KGBMessage message)
{
	switch(message.messageType)
	{
		case UPDATE_MOUSE:
			{
				UpdateMouse(message);
				return true;
			}
	}

	return false;
}

bool MousePicking::SpawnBox()
{

	if(box_[incrementer_ % 3] != 0)
	{
		delete box_[incrementer_ % 3];
	}
	box_[incrementer_%3] = new HoltBox(sceneManager_, b2Vec2(position_.x, position_.y), boxSize_/2);
	incrementer_++;
	return true;
}

void MousePicking::SetVisibility(bool visible)
{
	sceneNode_->setVisible(visible);
}