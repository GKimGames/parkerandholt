/*=============================================================================

  Platform.h

  Author: Matt King

=============================================================================*/
#ifndef PLATFORM_H
#define PLATFORM_H

#include <OgreMeshManager.h>
#include <OgreMovablePlane.h>

#include "GameFramework.h"

#include "GameObjectOgreBox2D.h"

class Platform : public GameObjectOgreBox2D
{

public:

	Platform(Ogre::SceneManager* sceneManager, b2Vec2 p1, b2Vec2 p2);
	~Platform();

	bool Update(double timeSinceLastFrame)
	{ 
		return true;
	};



protected:

	Ogre::MovablePlane* plane;
	
};

#endif