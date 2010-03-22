/*=============================================================================

	PickUpCreator.h

	Author: Matt King

=============================================================================*/


#ifndef PICKUP_CREATOR_H
#define PICKUP_CREATOR_H

#include "GameObjectCreator.h"
#include "PickUp.h"

class PickUpCreator : public GameObjectCreator
{

public:

	PickUpCreator(GameObjectFactory* gameObjectFactory) : GameObjectCreator(gameObjectFactory){}
	
	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		 
		if(element != 0)
		{
			b2Vec2 position = TinyXMLHelper::GetAttributeb2Vec2(element, "position");
			PickUp* pickUp = 0;

			float breakingForce = TinyXMLHelper::GetAttributeFloat(element, "breakingForce");

			if(breakingForce > 0)
			{
				pickUp =  new PickUp(GAMEFRAMEWORK->sceneManager,position, breakingForce);
			}
			else
			{
				pickUp =  new PickUp(GAMEFRAMEWORK->sceneManager,position);
			}
			
			return pickUp;
		}

		// Error Happened
		return 0;
	}

};

#endif