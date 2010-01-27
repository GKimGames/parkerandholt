/*=============================================================================

	GameObjectSensorCreator.h

	Author: Matt King

=============================================================================*/


#ifndef MOVING_PLATFORM_CREATOR_H
#define MOVING_PLATFORM_CREATOR_H

#include "GameObjectOgreBox2DCreator.h
#include "GameObjectSensor.h"

class GameObjectSensorCreator : public GameObjectOgreBox2DCreator
{

public:

	GameObjectSensorCreator(GameObjectFactory* gameObjectFactory):GameObjectOgreBox2DCreator(gameObjectFactory){}

	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		GameObjectSensor* object = new GameObjectSensor();
		CreatorResult result = LoadFromXML(element, object);

		if(result == CREATOR_OK)
		{
			return object;
		}

		// It didn't get created ok.
		delete object;
		
		return 0;
	}

	virtual CreatorResult LoadFromXML(TiXmlElement* element, GameObjectSensor* gameObjectSensor )
	{
		CreatorResult result;
		if(element != 0)
		{
			if(gameObjectSensor != 0)
			{
				result = ObjectOgreBox2DCreator::LoadFromXML(element, gameObjectSensor);
				if(result == CREATOR_OK)
				{
					 
					TiXmlElement* bodys = element->FirstChildElement( "Box2DObject" )->FirstChildElement("Body");
					gameObjectSensor->body_ = Box2DXMLLoader::Createb2Body(GAMEFRAMEWORK->world_, bodys);
					gameObjectSensor->Initialize();
				}
	
			}// end if(gameObjectOgreBox2D != 0)
			else
			{
				result = CREATOR_OBJECT_IS_ZERO;
			}
		}// end if(element != 0)
		else
		{
			result = CREATOR_ELEMENT_IS_ZERO;
		}
	
		return result;
	}

};


#endif