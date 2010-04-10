/*=============================================================================

	  HoltBox.h

	  Author: Matt King

=============================================================================*/

#ifndef HOLTBOX_H
#define HOLTBOX_H

#include "GameObjectOgreBox2D.h"


/// HoltBox is a cube. b2Body and some Ogre mesh.
class HoltBox : public GameObjectOgreBox2D
{
public:

	// Center is where the box is centered at. 
	HoltBox(Ogre::SceneManager* sceneManager, b2Vec2 center);
	HoltBox(Ogre::SceneManager* sceneManager, b2Vec2 center, double size, double density = 10);
	virtual ~HoltBox()
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
	void ModifyBoxWidth(double delta)
	{
		boxWidth_ += delta;
		sceneNode_->scale(boxWidth_*2,boxWidth_*2,boxWidth_*2);
	};

	/// Sets the position of the box
	void SetPosition(b2Vec2 position){position_ = position;};

	/// This tries to create a box at position_, returns false if theres other
	/// Box2D shapes there.
	bool CreateBox2DBox();
	bool Update(double timeSinceLastFrame);
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	/// Sets the box invisible and inactive
	void SetInactive();

	

protected:

	/// This is half the width of the cube
	double boxWidth_;

	double density_;

	/// Position to initially draw the box at.
	b2Vec2 position_;
	bool	placementTest_;
	bool	badPlacement_;
};


#endif
