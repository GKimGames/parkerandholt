/*=============================================================================

	GameObjectSensorCreator.h

	Author: Matt King

=============================================================================*/


#ifndef GAME_OBJECT_SENSOR_CREATOR_H
#define GAME_OBJECT_SENSOR_CREATOR_H

#include "GameObjectOgreBox2DCreator.h"
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
				result = GameObjectOgreBox2DCreator::LoadFromXML(element, gameObjectSensor);
				
				if(result == CREATOR_OK)
				{ 
					gameObjectSensor->Initialize();
					gameObjectSensor->body_->SetUserData(gameObjectSensor);
					
					TiXmlElement* objects = element->FirstChildElement( "ObjectSensor" )->FirstChildElement("Object");

					if(objects != 0)
					{
						
						Ogre::String objectID = TinyXMLHelper::GetAttribute(objects, "objectID", "");
						KGBMessageType onMessage = TinyXMLHelper::GetMessage(objects, "on", MESSAGE_NULL);
						KGBMessageType offMessage = TinyXMLHelper::GetMessage(objects, "off", MESSAGE_NULL);
						Ogre::String sensorTypeString = TinyXMLHelper::GetAttribute(objects, "sensorType", "");
						gameObjectSensor->sensorType_ = StringToSensorType(sensorTypeString);
						gameObjectSensor->gameObjectType_ = GOType_Sensor;

						GameObject* go = GameObject::GetObjectById(objectID);
						
						if(go)
						{
							GameObjectId id = go->GetId();
							gameObjectSensor->AddToMessageList(id, onMessage, offMessage);
						}

						objects = objects->NextSiblingElement("Object");
					}
					

					
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