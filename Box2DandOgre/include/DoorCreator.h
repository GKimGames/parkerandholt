/*=============================================================================

	DoorCreator.h

	Author: Matt King

=============================================================================*/


#ifndef DOOR_CREATOR_H
#define DOOR_CREATOR_H

#include "GameObjectCreator.h"
#include "Door.h"

class DoorCreator : public GameObjectCreator
{

public:

	DoorCreator(GameObjectFactory* gameObjectFactory) : GameObjectCreator(gameObjectFactory){}
	
	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		Door* door; 

		if(element != 0)
		{

			bool isEntrance = TinyXMLHelper::GetAttributeBool(element, "isEntrance");
			b2Vec2 position = TinyXMLHelper::GetAttributeb2Vec2(element, "position");
			door = new Door("SomeDoor", isEntrance, position);

		}
		return door;

	}

};

#endif