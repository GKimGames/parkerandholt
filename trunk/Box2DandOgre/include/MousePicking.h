/*=============================================================================

		MousePicking.h

		Author: Greg King

=============================================================================*/
#ifndef MOUSEPICKING_H
#define MOUSEPICKING_H

#include "AppState.hpp"
#include <Ogre.h>

#include <Box2D\Box2D.h>

#include "GameObjectOgreBox2D.h"
#include "HoltBox.h"




struct KGBMessage;

class MousePicking : public GameObjectOgreBox2D
{
public: 
	MousePicking(Ogre::SceneManager* sceneManager, Ogre::Camera* camera);
	~MousePicking(){}

	void MouseMoved(const OIS::MouseEvent &arg);
	void MousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 
	void MouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	void UpdateMouse(const KGBMessage message);
	void UpdateMouseFromCamera();
	bool HandleMessage(const KGBMessage message);
	bool SpawnBox();
	void SetVisibility(bool visible);
	const Ogre::Vector3 GetPosition();
	
	double		boxSize_;

protected:
	Ogre::SceneManager*			sceneManager_;
	Ogre::RaySceneQuery*		m_pRSQ_;
	Ogre::Camera*				m_pCamera_;
	Ogre::Plane*				pickingPlane_;
	Ogre::Vector3				position_;
	Ogre::Vector3				pointClicked_;
	Ogre::Entity*				mouseIcon_;
	Ogre::Real					rayPositionX_;
	Ogre::Real					rayPositionY_;
	HoltBox*					box_[3];
	double						boxMinSize_;
	double						boxSizeIncrement_;
	double						boxMaxSize_;
	int							incrementer_;

};
#endif 