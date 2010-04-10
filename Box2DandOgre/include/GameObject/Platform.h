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
	Platform(Ogre::SceneManager* sceneManager, b2Vec2 p1, b2Vec2 p2, bool placeable);
	virtual ~Platform();

	bool Update(double timeSinceLastFrame);
	

	virtual bool Initialize();
	virtual bool Initialize(Ogre::String str);
	bool InitializePlaceable();
	
	/// Used to set the graphics position when there is no physical body, placeables only
	bool SetGraphics(b2Vec2 position, float length, float angle, bool final);
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	void SetInactive();

protected:

	Ogre::MovablePlane* plane;

	b2Vec2	point1;
	b2Vec2	point2;
	float32	previousAngle_;
	float	length_;
	bool	placementTest_;
	bool	badPlacement_;
	float	maxLength_;
};



#endif