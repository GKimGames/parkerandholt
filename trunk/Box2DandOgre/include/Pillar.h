/*=============================================================================
-------------------------------------------------------------------------------
  Pillar.h

  Author: Matt King

=============================================================================*/

#ifndef PILLAR_H
#define PILLAR_H

#include "EntityBox2D.h"


/// Pillar is a rectangle!
class Pillar : public PaH::EntityBox2D
{
public:

	// Center is where the box is centered at. 
	Pillar(Ogre::SceneManager* sceneManager, b2Vec2 center);
	~Pillar(){;};

	/// This changes the width by delta
	/// @param delta is the amount to change the width by.
	/// @warning This only changes the width before the Box2D box is created
	void ModifyBoxWidth(double delta)
	{
		pillarWidth_ += delta;
		sceneNode_->scale(pillarWidth_*2,pillarHeight_*2,pillarWidth_*2);
	};

	/// Sets the position of the box
	void SetPosition(b2Vec2 position){position_ = position;};

	/// This tries to create a box at position_, returns false if theres other
	/// Box2D shapes there.
	bool CreateBox2DBox();

protected:

	/// This is half the width of the pillar
	double pillarWidth_;

	/// This is half the height of the pillar
	double pillarHeight_;

	/// Position to initially draw the box at.
	b2Vec2 position_;
};


#endif

