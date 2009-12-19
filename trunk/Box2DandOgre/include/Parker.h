/*=============================================================================

  Parker.h

  Author: Matt King

=============================================================================*/

#ifndef CHARACTER_PARKER_H
#define CHARACTER_PARKER_H

#include "Character.h"

/// Base class for a character. Character extends PaH::EntityDrawableContactResponder
class CharacterParker :public PaH::EntityBox2DResponder
{

public:

	CharacterParker();


private:

	FSMStateMachine<CharacterParker>*	stateMachine_;

};