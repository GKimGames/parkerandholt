/*=============================================================================

  EntityDrawable.h

  Author: Matt King

=============================================================================*/

#ifndef PAH_ENTITYDRAWABLE_H
#define PAH_ENTITYDRAWABLE_H

#include "Entity.h"

namespace PaH 
{
	/// EntityDrawable extends Entity
	class EntityDrawable : virtual public Entity
	{
	public:

		EntityDrawable(){;};
		~EntityDrawable(){;};

		virtual bool UpdateGraphics(double timeSinceLastFrame)=0;

		/// Returns the position of the scene node 
		virtual Ogre::Vector3 GetPosition()
		{
			return sceneNode_->getPosition();
		};

		/// Update calls UpdataGraphics
		virtual bool Update(double timeSinceLastFrame)
		{
			UpdateGraphics(timeSinceLastFrame);
			return true;
		};

		/// Returns the Ogre::SceneNode for this Entity
		virtual Ogre::SceneNode* GetSceneNode()
		{
			return sceneNode_;
		};
		
		/// Returns the Ogre::Entity for this Entity
		virtual Ogre::Entity*    GetEntity()
		{
			return entity_;
		};

	protected:

		Ogre::SceneManager*     sceneManager_;   //< \brief Node's parent.
		Ogre::SceneNode*        sceneNode_;      //< \brief Scene Node.
		Ogre::Entity*           entity_;         //< \brief Attached Entity or NULL.
	};
}

#endif