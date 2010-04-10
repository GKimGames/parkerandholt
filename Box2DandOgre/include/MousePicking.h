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

	void UpdateMouse(const OIS::MouseEvent &evt, b2Vec2 holtPosition);
	bool HandleMessage(const KGBMessage message);
	void SetVisibility(bool visible);

	/// Returns the position of the mouse icon
	const Ogre::Vector3 GetPosition();

	/// Deletes placeable objects the mouse is in contact with
	void DeletePlaceables();
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);
	bool Update(double timeSinceLastFrame);

	bool Initialize()
	{
		prevAngle = 0;
		initialized_ = true;

		return true;
	}

	double		boxSize_;

protected:
	Ogre::SceneManager*			sceneManager_;
	Ogre::RaySceneQuery*		RSQ_;
	Ogre::Camera*				camera_;
	Ogre::Plane*				pickingPlane_;
	Ogre::Vector3				position_;
	Ogre::Vector3				pointClicked_;
	Ogre::Entity*				mouseIcon_;
	Ogre::Real					rayPositionX_;
	Ogre::Real					rayPositionY_;
	double						boxMinSize_;
	double						boxSizeIncrement_;
	double						boxMaxSize_;
	bool						deleteCheck_;

};
#endif 