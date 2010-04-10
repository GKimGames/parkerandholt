/*=============================================================================

		FadingPlatform.h

		Author: Matt King

=============================================================================*/

#ifndef FADING_PLATFORM_H
#define FADING_PLATFORM_H

#include "HelperFunctions.h"
#include "GameObjectOgreBox2D.h"

/// A fading platform falls for a brief moment and then fades out of existence.
/// After a period of time the platform may come back.
class FadingPlatform : public GameObjectOgreBox2D
{
public:

	friend class FadingPlatformCreator;

	FadingPlatform(Ogre::String name = "FadingPlatform", b2Body* body = 0, Ogre::Entity* entity = 0);

	virtual ~FadingPlatform();

	bool Update(double timeSinceLastFrame);

	bool Initialize();

	bool HandleMessage(const KGBMessage message);

protected:

	/// The speed at which the object moves when fading.
	double	fadeSpeed_;

	/// How long the platform takes to fade.
	double	fadeTime_;

	/// How long it takes to reappear.
	double	fadeReappearTime_;

	double	fadeTimer_;
	
	/// Is the platform currently fading.
	bool	isFading_;

	/// Does the platform only fade one time and not reappear?
	bool	isOneShot_;

	b2Vec2	initialPosition_;

	/// The size of the particle emitter.
	Ogre::Vector3 emitterSize_;


};



#endif