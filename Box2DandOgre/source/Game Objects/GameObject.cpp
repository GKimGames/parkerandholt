/*=============================================================================

  GameObject.cpp

  Author: Matt King

=============================================================================*/

#include "GameObject.h"

unsigned int GameObject::staticObjectId_ = 1;

GameObjectMap GameObject::objectList;

/// GameObject represents the lowest form of object possible in the game.
/// An Object doesn't have to have a graphical representation  or a
/// physical representation. It is added to a vector of all Object's
/// when it is created. Objects also have a unique Id number.
GameObject::GameObject()
{
	// Increase the staticObjectId value so each Object has its own
	// unique identifying number.
	staticObjectId_++;
	objectId_ = staticObjectId_;

	objectList.insert(std::make_pair(objectId_,this));
}
