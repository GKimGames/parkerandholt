/*=============================================================================
-------------------------------------------------------------------------------
  Entity.h

  Author: Matt King

=============================================================================*/

#ifndef PAH_ENTITY_H
#define PAH_ENTITY_H

#include <Ogre.h>
#include <tinyxml.h>
#include <vector>

#include "GameFramework.h"
#include "Message.h"

namespace PaH 
{

	class Entity;
	typedef std::map<int, Entity*> EntityMap;

	/// This will identify what type of Game Entity an entity is.
	/// This is useful due to Box2D having all contact callbacks go through a single
	/// world wide callback.
	enum GameEntityType
	{
		Character,		//< \brief Character Entity
		CharacterFoot,
		HoltBox,
		LedgeSensor,
		Platform
	};
	
	/// Entity represents the base class for anything that is in the game.
	///	It has a unique ID number.
	/// All Entitys are added to the entityList and are updated using this
	/// list.
	/// All Entitys also have a GameEntityType.
	class Entity 
	{
	public:

		/// List of all Entitys created.
		static EntityMap entityList;

		/// Update all the Entitys.
		static void UpdateEntityList(double timeSinceLastFrame)
		{
			EntityMap::iterator it;

			for (it = entityList.begin(); it != entityList.end(); it++)
			{
				(*it).second->Update(timeSinceLastFrame);		
			}
		}

		static Entity* GetEntity(unsigned int entityId)
		{
			// Find the entity
			EntityMap::const_iterator entity = entityList.find(entityId);

			return entity->second;
		}

		static EntityMap GetEntityList() { return entityList; }
		

		/// Constructor adds this entity to the entityList.
		/// It also increases the static entityId and assigns this entity an Id.
		Entity();

		/// The destructor removes it from the entityList;
		~Entity()
		{
			entityList.erase(entityList.find(this->entityId_));
		}
		
		virtual bool Update(double timeSinceLastFrame)=0;
		
		 /// Get the GameEntityType
		GameEntityType GetGameEntityType() const
		{
			return gameEntityType_;
		};

		/// Returns this entity's entityUniqueId_
		unsigned int GetId() const
		{ 
			return entityUniqueId_;
		};

		/// Implement this to have this Entity respond to messages
		/// By default this returns false to indicate this Entity
		/// did not handle the message that was sent to it. Return
		/// true if the entity was able to handle the message.
		virtual bool HandleMessage(const KGBMessage message){ return false; };

	protected:

		/// Process an event in the gameEventQueue_.
		/// Each event must have Release() called on it so the event
		/// can be deleted.
		//virtual void ProcessEvent(GameEvent* gameEvent){;};
		
		/// Queue for the GameEvents.
		/// GameEvents are added to this vector by HandleEvent,
		/// the GameEvents are then evaluatedd by ProcessEventQueue

		GameEntityType gameEntityType_;				//< \brief GameEntityType of the entity.


		unsigned int			entityUniqueId_;		//< \brief Unique Id for the entity
		static unsigned int		entityId_;				//< \brief Unique Id for the entity

	};

}

#endif
