/*=============================================================================
-------------------------------------------------------------------------------
  Entity.cpp

  Author: Matt King

=============================================================================*/

#include "Entity.h"

using namespace PaH;

unsigned int Entity::entityId_ = 1;
EntityMap Entity::entityList;

/// Entity represents the lowest form of object possible in the game.
/// An Entity doesn't have to have a graphical representation  or a
/// physical representation. It is added to a vector of all entity's
/// when it is created. Entitys also have a unique Id number.
Entity::Entity()
{
	// Increase the static entityId value so each entity has its own
	// unique identifying number.
	entityId_++;
	entityUniqueId_ = entityId_;

	entityList.insert(std::make_pair(entityId_,this));
}
