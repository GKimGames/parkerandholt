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

	friend class StaticOgreObjectCreator;
	friend class ObjectOgreCreator;

public:


	/// Grabs the SceneManager and creates a SceneNode for this object.
	GameObjectOgre(Ogre::String = "GameObjectOgre");

	/// Destroys the SceneNode.
	virtual ~GameObjectOgre();

	/// All children of the class must call this Initialize function before 
	/// their own. This pattern should extend through all children.
	virtual bool Initialize();

	/// Initialize the object with a mesh.
	/// All children of the class must call this Initialize function before their
	/// own. This pattern should extend through all children.
	virtual bool Initialize(Ogre::String meshName);

	/// Update calls UpdataGraphics
	/// All children of the class must call this Update function before 
	/// their own. This pattern should extend through all children.
	virtual bool Update(double timeSinceLastFrame);

	/// Virtual function so Objects can update their graphics.
	/// This function does nothing.
	virtual bool UpdateGraphics(double timeSinceLastFrame);

	/// Each class that extends this class should call this HandleMessage
	/// before their own in case this class can handle the message.
	/// If you want this class to do something with a message and its children
	/// to something with it as well just return false for that message.
	virtual bool HandleMessage(const KGBMessage message);
	
	/// Set the transparency of the mesh in this object.
	virtual void SetTransparency(Ogre::Real alpha);

	/// Set the blending type of the mesh matieral.
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
		/// We can disable the particle system by setting it to be not visible and
		/// set a timeout so it will disable itself.
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

	/// Blenders between Mesh Animations.
	AnimationBlender*		animationBlender_;
	Ogre::SceneManager*     sceneManager_;		//< \brief Node's parent.
	Ogre::SceneNode*        sceneNode_;			//< \brief Scene Node.
	Ogre::Entity*           entity_;			//< \brief Attached Entity or NULL.
	Ogre::ParticleSystem*	particleSystem_; 
	EntityMaterialInstance* transparency_;

};

#endif