//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef CHARACTER_H
#define CHARACTER_H

#define OGRE_LOG(t) OgreFramework::getSingletonPtr()->m_pLog->logMessage(t)

#define DEBUGGING 1

#ifdef DEBUGGING
#define DLOG(X) OutputDebugString(X); OutputDebugString("\n") 
#else
#define DLOG(X) 
#endif
//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <NxOgre.h>
#include <NxOgreOGRE3D.h>

#include <tinyxml.h>

#include <NxActor.h>


class Character : public NxOgre::Callback
{
public:
	Character(OGRE3DRenderSystem* renderSystem, NxOgre::Scene * scene);
	~Character();

	virtual void KeyPressed(const OIS::KeyEvent &keyEventRef);
	virtual void KeyReleased(const OIS::KeyEvent &keyEventRef);

	void MouseMoved(const OIS::MouseEvent &arg);
	void MousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 
	void MouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	bool Update(double timeSinceLastFrame);
	void GetInput(double timeSinceLastFrame);

	void OnContact(const NxOgre::ContactPair& cPair);

protected:
		 
	OGRE3DKinematicBody* mKB;

	Ogre::String			meshName_;
	Ogre::AnimationState*	animationState_; // The current animation state of the object
	Ogre::AnimationState*	jumpAnim_; // The current animation state of the object
	OGRE3DRenderSystem*		renderSystem_;
	NxOgre::Scene*			scene_;

	OGRE3DBody*			body_;
	NxOgre::Capsule*	capsuleShape_;
	NxOgre::Material*	capsuleMaterial_;

	NxOgre::Actor*		feetActor_;

	NxOgre::Real		mass_;

	double				capsuleHeight_;
	double				capsuleRadius_;

	bool				isRunning_;
	bool				isTouchingSurface_;

	double				runningForce_;
	double				walkingForce_;
	double				maximumVelocity_;
	double				maximumWalkingVelocity_;
	double				maximumRunningVelocity_;

	double				translateX;
	double				translateY;
	double				translateZ;	

	double				rotateY;

	bool				isKeyUP;
	bool				justJumped;
	

};

#endif 