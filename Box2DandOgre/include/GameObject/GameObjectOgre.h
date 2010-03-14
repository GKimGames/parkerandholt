/*=============================================================================

		  GameObjectOgre.h

		  Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_OGRE_H
#define GAME_OBJECT_OGRE_H

#include "GameObject.h"
#include "EntityMaterialInstance.h"
#include "AnimationBlender.h"

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

	virtual bool HandleMessage(const KGBMessage message);
	
	virtual void SetTransparency(Ogre::Real alpha);

	virtual void SetBlendType(Ogre::SceneBlendType type);

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

	void DisableParticleSystem()
	{
		
		if(particleSystem_)
		{
			particleSystem_->setNonVisibleUpdateTimeout(0.1);
			particleSystem_->setVisible(false);
		}
	}

	void EnableParticleSystem()
	{
		if(particleSystem_)
		{
			particleSystem_->setNonVisibleUpdateTimeout(0.0);
			particleSystem_->setVisible(true);
		}
	}

protected:

	AnimationBlender*		animationBlender_;
	Ogre::SceneManager*     sceneManager_;		//< \brief Node's parent.
	Ogre::SceneNode*        sceneNode_;			//< \brief Scene Node.
	Ogre::Entity*           entity_;			//< \brief Attached Entity or NULL.
	Ogre::ParticleSystem*	particleSystem_; 
	EntityMaterialInstance* transparency_;

};

// Messages needed
/*
	SetTransparency	double
	Blend			BlendData
	StartAnimation
	StopAnimation
	DisableParticleSystem
	EnableParticleSystem

*/
#endif