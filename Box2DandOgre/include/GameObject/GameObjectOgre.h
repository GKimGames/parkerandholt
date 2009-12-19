/*=============================================================================

  GameObjectOgre.h

  Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_OGRE_H
#define GAME_OBJECT_OGRE_H

#include "GameObject.h"


/// GameObjectOgre extends GameObject
class GameObjectOgre : public GameObject
{
public:

	GameObjectOgre(){}
	virtual ~GameObjectOgre(){}

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

	/// Returns the Ogre::SceneNode for this Object
	virtual Ogre::SceneNode* GetSceneNode()
	{
		return sceneNode_;
	};

	/// Returns the Ogre::Entity for this Object
	virtual Ogre::Entity* GetEntity()
	{
		return entity_;
	};

protected:

	Ogre::SceneManager*     sceneManager_;   //< \brief Node's parent.
	Ogre::SceneNode*        sceneNode_;      //< \brief Scene Node.
	Ogre::Entity*           entity_;         //< \brief Attached Entity or NULL.
};


#endif