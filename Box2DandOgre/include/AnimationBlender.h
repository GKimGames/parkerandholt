// Taken from the OGRE wiki, must rewrite as it doesn't seem to work correctly.

#ifndef ANIMATONBLENDER_H
#define ANIMATONBLENDER_H

#include "Ogre.h"

class AnimationBlender
{

public:

	enum BlendingTransition
	{
		BlendSwitch,				// stop source and start dest
		BlendWhileAnimating,		// cross fade, blend source animation out while blending destination animation in
		BlendThenAnimate			// blend source to first frame of dest, when done, start dest anim
	};

	AnimationBlender( Ogre::Entity *);

	void Blend( 
		const Ogre::String &animation,
		BlendingTransition transition,
		Ogre::Real duration,
		bool loop = true,
		Ogre::Real targetTime = 9999
		);

	void Initialize( const Ogre::String &animation, bool loop = true );
	void AddTime( Ogre::Real time );

/*=============================================================================
				Getter / Setter methods
=============================================================================*/
	Ogre::Real GetProgress() { return timeleft_ / duration_; }
	Ogre::AnimationState *GetSource() { return source_; }
	Ogre::AnimationState *GetTarget() { return target_; }

private:

	Ogre::Entity* entity_;
	Ogre::AnimationState* source_;
	Ogre::AnimationState* target_;

	BlendingTransition transition_;

	~AnimationBlender() {}

public: 

	Ogre::Real timeleft_, duration_, targetTime_;
	bool loop_;
	bool complete_;



};


#endif
