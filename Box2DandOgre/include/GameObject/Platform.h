/*=============================================================================

  Platform.h

  Author: Matt King

=============================================================================*/
#ifndef PLATFORM_H
#define PLATFORM_H

#include <OgreMeshManager.h>
#include <OgreMovablePlane.h>
#include "OgrePlatform.h"

#include "GameObjectOgreBox2D.h"

class Platform : public GameObjectOgreBox2D
{
friend class PlatformCreator; 
public:

	Platform();
	Platform(Ogre::SceneManager* sceneManager, b2Vec2 p1, b2Vec2 p2);
	~Platform();

	bool Update(double timeSinceLastFrame)
	{ 
		return true;
	};

	virtual bool Initialize();
	virtual bool Initialize(Ogre::String str);

protected:

	Ogre::MovablePlane* plane;

	b2Vec2 point1;
	b2Vec2 point2;
	
};



#endif