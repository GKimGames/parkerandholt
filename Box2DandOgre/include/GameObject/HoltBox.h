/*=============================================================================

  HoltBox.h

  Author: Matt King

=============================================================================*/

#ifndef HOLTBOX_H
#define HOLTBOX_H

#include "GameObjectOgreBox2D.h"


/// HoltBox is a cube
class HoltBox : public GameObjectOgreBox2D
{
public:

	// Center is where the box is centered at. 
	HoltBox(Ogre::SceneManager* sceneManager, b2Vec2 center);
	~HoltBox(){}

	void Initialize()
	{
		prevAngle = 0;
		initialized_ = true;
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
	

	virtual void BeginContact(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){}
	virtual void EndContact(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){}

protected:

	/// This is half the width of the cube
	double boxWidth_;

	/// Position to initially draw the box at.
	b2Vec2 position_;
};


#endif
