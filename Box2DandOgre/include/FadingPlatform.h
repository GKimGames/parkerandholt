/*=============================================================================

		FadingPlatform.h

		Author: Matt King

=============================================================================*/

#ifndef FADING_PLATFORM_H
#define FADING_PLATFORM_H

#include "HelperFunctions.h"
#include "GameObjectOgreBox2D.h"
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

	double	fadeSpeed_;

	double	fadeTime_;
	double	fadeTimer_;
	double	fadeReappearTime_;

	bool	isFading_;

	b2Vec2	initialPosition_;


};



#endif