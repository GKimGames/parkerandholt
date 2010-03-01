/*=============================================================================

		  GameObjectOgre.h

		  Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_OGRE_H
#define GAME_OBJECT_OGRE_H

#include "GameObject.h"
#include "GameFramework.h"
#include "EntityMaterialInstance.h"

/// GameObjectOgre extends GameObject
class GameObjectOgre : public GameObject
{

	friend class ObjectOgreCreator;

public:


	GameObjectOgre(Ogre::String = "GameObjectOgre");

	virtual ~GameObjectOgre();

	virtual bool Initialize();

	/// Initialize this objecti with a mesh.
	virtual bool Initialize(Ogre::String meshName);

	/// Update calls UpdataGraphics
	virtual bool Update(double timeSinceLastFrame);

	virtual bool UpdateGraphics(double timeSinceLastFrame);

	virtual bool HandleMessage(const KGBMessage message){return false;}

//===========================================================================//
//																			 //
//						Start Getters and Setters							 //
//																			 //
//===========================================================================//

	/// Returns the position of the scene node 
	virtual const Ogre::Vector3 GetPosition()
	{
		return sceneNode_->getPosition();
	}

	/// Returns the Ogre::SceneNode for this Object
	virtual Ogre::SceneNode* GetSceneNode()
	{
		return sceneNode_;
	}

	/// Returns the Ogre::Entity for this Object
	virtual Ogre::Entity* GetEntity()
	{
		return entity_;
	}

	virtual void SetTransparency(Ogre::Real alpha);

	virtual void SetBlendType(Ogre::SceneBlendType type);

protected:

	Ogre::SceneManager*     sceneManager_;   //< \brief Node's parent.
	Ogre::SceneNode*        sceneNode_;      //< \brief Scene Node.
	Ogre::Entity*           entity_;         //< \brief Attached Entity or NULL.
	EntityMaterialInstance* transparency_;

};


#endif