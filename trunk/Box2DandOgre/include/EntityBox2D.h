/*=============================================================================
-------------------------------------------------------------------------------
  EntityBox2D.h

  Author: Matt King

=============================================================================*/

#ifndef PAH_ENTITYBOX2D_H
#define PAH_ENTITYBOX2D_H

#include "EntityDrawable.h"
#include "Box2D.h"

namespace PaH 
{

	
	/// This extends EntityDrawable and then  
	class EntityBox2D  : public EntityDrawable
	{
	public:

		/// Constructor grabs the reference from the OgreFramework of the b2World.
		EntityBox2D()
		{
			// There is only ever one world at a time and all entities get the
			// current world from the OgreFramework singleton.
			world_ = GameFramework::getSingletonPtr()->GetWorld();

			initialized_ = false;
		}

		/// Returns the b2Body for this EntityBox2D
		virtual b2Body* GetBody()
		{
			return body_;
		};

		/// Returns the body's position
		virtual b2Vec2 GetBodyPosition()
		{
			return body_->GetPosition();
		};

		virtual void Initialize()
		{
			initialized_ = true;
			prevAngle = 0;
		}

		virtual bool UpdateGraphics(double timeSinceLastFrame)
		{

			// Since rotating a scenenode actually rotates it by an amount
			// and not to a specific value we must store the previous angle
			// that it was rotated by and rotate back by that and then rotate
			// to our new angle.

			if(initialized_)
			{
				b2Vec2 v = body_->GetPosition();
				sceneNode_->setPosition(v.x,v.y,0);
				sceneNode_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(-prevAngle)); 
				sceneNode_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(body_->GetAngle()));

				prevAngle = body_->GetAngle();

			}

			return true;
		};

	protected:

		/// Box2D body for this entity
		b2Body* body_;

		/// Reference to the b2World
		b2World* world_;

		bool initialized_;

		float32 prevAngle;


	};
}

#endif