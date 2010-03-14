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
					gameObjectSensor->gameObjectType_ = GOType_Sensor;
					
					TiXmlElement* sensorElement = element->FirstChildElement( "ObjectSensor");
					TiXmlElement* messages = sensorElement->FirstChildElement("Messages");
 
					std::map<Ogre::String, KGBMessage> messageList;

					if(messages != 0)
					{
						TiXmlElement* messageElement = messages->FirstChildElement("Message");
						
						while(messageElement)
						{
							Ogre::String messageId = TinyXMLHelper::GetAttribute(messageElement, "id", "");
							double sendDelay = TinyXMLHelper::GetAttributeReal(messageElement, "delay", SEND_IMMEDIATELY);
							KGBMessageType type = TinyXMLHelper::GetMessage(messageElement, "type", MESSAGE_NULL);
							TiXmlElement* dataElement = messages->FirstChildElement("Data");
							boost::any data = GetData(dataElement);

							KGBMessage message(sendDelay,gameObjectSensor->objectId_, 0,type, data);

							messageList.insert(std::make_pair(messageId, message));
							messageElement = messageElement->NextSiblingElement("Message");
						}
					}
					
					gameObjectSensor->defaultMessageOn_.messageType = TinyXMLHelper::GetMessage(sensorElement, "on", GAME_SENSOR_ON);
					gameObjectSensor->defaultMessageOff_.messageType = TinyXMLHelper::GetMessage(sensorElement, "off", GAME_SENSOR_ON);
					gameObjectSensor->messageObjectOnContact_ = TinyXMLHelper::GetAttributeBool(sensorElement, "messageOnContact");

					TiXmlElement* objects = sensorElement->FirstChildElement("Object");
 
					if(objects != 0)
					{
						Ogre::String objectID = TinyXMLHelper::GetAttribute(objects, "objectID", "");
						GameObject* go = GameObject::GetObjectById(objectID);
						
						if(go)
						{
							KGBMessageType onMessage = TinyXMLHelper::GetMessage(objects, "on", GAME_SENSOR_ON);
							KGBMessageType offMessage = TinyXMLHelper::GetMessage(objects, "off", GAME_SENSOR_ON);
							Ogre::String sensorTypeString = TinyXMLHelper::GetAttribute(objects, "sensorType", "");
							gameObjectSensor->sensorType_ = StringToSensorType(sensorTypeString);
							gameObjectSensor->gameObjectType_ = GOType_Sensor;

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


	boost::any GetData(TiXmlElement* element)
	{
		if(element)
		{

		}

		return boost::any(0);
	}
};


#endif