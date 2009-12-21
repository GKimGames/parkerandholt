/*=============================================================================

  Parker.h

  Author: Matt King

=============================================================================*/

#ifndef CHARACTER_PARKER_H
#define CHARACTER_PARKER_H

#include "Character.h"

/// Parkers class
class CharacterParker :public Character
{

public:

	CharacterParker(Ogre::SceneManager* sceneManager);

	bool HandleMessage(const KGBMessage message)
	{ 
		stateMachine_->HandleMessage(message); 
	}



private:

	FSMStateMachine<CharacterParker>*	stateMachine_;

	// These sensors are used to tell where things are in front of Parker
	// Such as if something is only shin high, thigh high, or up to the torso.
	// Used to check if something above the character is within a grabbable reach.
	b2Fixture*			shinSensor_;
	b2Fixture*			thighSensor_;
	b2Fixture*			torsoSensor_;

	int					shinSensorHitCount_;
	int					thighSensorHitCount_;
	int					torsoSensorHitCount_;

	double boundingBoxHeight_;
	double boundingBoxWidth_;
	double wallJumpForce_;

};


#endif