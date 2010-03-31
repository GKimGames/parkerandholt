/*=============================================================================

	  Triangle.h

	  Author: Matt King

=============================================================================*/

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "GameObjectOgreBox2D.h"


/// HoltBox is a cube
class Triangle : public GameObjectOgreBox2D
{
public:

	// Center is where the box is centered at. 
	Triangle(Ogre::SceneManager* sceneManager, b2Vec2 center, double size);
	Triangle(Ogre::SceneManager* sceneManager, b2Vec2 center, double size, float TTL);
	virtual ~Triangle()
	{

	}

	bool Initialize()
	{
		prevAngle = 0;
		initialized_ = true;

		return true;
	}


	/// This changes the width by delta
	/// @param delta is the amount to change the width by.
	/// @warning This only changes the width before the Box2D box is created
	void ModifyWidth(double delta)
	{
		triangleWidth_ += delta;
		sceneNode_->scale(triangleWidth_*2, triangleWidth_*2, triangleWidth_*2);
	};

	/// Sets the position of the box
	void SetPosition(b2Vec2 position){position_ = position;};

	/// This tries to create a box at position_, returns false if theres other
	/// Box2D shapes there.
	bool CreateTriangle();
	bool Update(double timeSinceLastFrame);
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	void SetInactive();
	

protected:

	/// This is half the width of the cube
	double	triangleWidth_;

	double	density_;

	/// Position to initially draw the box at.
	b2Vec2	position_;
	bool	placementTest_;
	bool	badPlacement_;
	bool	temporary_;
	float	TTL_;
	float	timeAlive_;
	Ogre::Vector3 emitterSize_;
};


#endif
