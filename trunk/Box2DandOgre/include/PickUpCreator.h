/*=============================================================================

	PickUpCreator.h

	Author: Matt King

=============================================================================*/


#ifndef PICKUP_CREATOR_H
#define PICKUP_CREATOR_H

#include "GameObjectCreator.h"
#include "PickUp.h"

/// Creates a pickup from XML.
/// For simplicity of the XML this extends the base GameObjectCreator.
class PickUpCreator : public GameObjectCreator
{

public:

	PickUpCreator(GameObjectFactory* gameObjectFactory) : GameObjectCreator(gameObjectFactory){}
	
	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		 
		if(element != 0)
		{
			// Grab the position of the PickUp.
			b2Vec2 position = TinyXMLHelper::GetAttributeb2Vec2(element, "position");
			PickUp* pickUp = 0;

			// If there is a breaking force, its now a breakable pickup.
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