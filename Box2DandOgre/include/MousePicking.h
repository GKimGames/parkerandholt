//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef MOUSEPICKING_H
#define MOUSEPICKING_H

#include "AppState.hpp"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <Ogre.h>
#include <tinyxml.h>

#include "Box2D.h"

#include "AnimationBlender.h"
#include "Entity.h"


class MousePicking
{
public:
	MousePicking(Ogre::SceneManager* sceneManager, b2World* world, Ogre::Camera* camera, Ogre::Plane pPlane);
	~MousePicking();

	void MouseMoved(const OIS::MouseEvent &arg);
	void MousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 
	void MouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	void CreateBox(Vector3 position);

	void Update(double timeSinceLastFrame);


protected:

	b2World*					world_;
	Ogre::SceneManager*			sceneManager_;
	Ogre::RaySceneQuery*		m_pRSQ_;
	Ogre::Camera*				m_pCamera_;
	Ogre::Plane					pickingPlane_;
	
	
	bool						m_bLMouseDown_;
	bool						m_bRMouseDown_;
	bool						changingSize_;
	bool						hasHit_;
	b2Body*						cubes_[4];
	Entity**					boxes_;
	Vector3						pointClicked_;
	int							boxCount_;
	double						boxMax_;
	double						boxMin_;
	double						boxIncriment_;
	double						boxSize_;
	double						boxDensity_;
	double						boxFriction_;
	
};
#endif 