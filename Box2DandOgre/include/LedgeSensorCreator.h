/*=============================================================================

	LedgeSensorCreator.h

	Author: Matt King

	NOT USED, REPLACED BY GAMEOBJECTSENSORCREATOR.

=============================================================================*/

#ifndef LEDGE_SENSOR_CREATOR
#define LEDGE_SENSOR_CREATOR

#include "GameObjectOgreBox2D.h"
#include "GameObjectSensorCreator.h"
#include "OgreXMLLoader.h"
#include "Box2DXMLLoader.h"

class LedgeSensorCreator : public GameObjectSensorCreator
{

public:

	LedgeSensorCreator(GameObjectFactory* gameObjectFactory):GameObjectSensorCreator(gameObjectFactory){}
	
	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		GameObjectOgreBox2D* object = new GameObjectOgreBox2D();
		CreatorResult result = LoadFromXML(element, object);

		if(result == CREATOR_OK)
		{
			return object;
		}

		// It didn't get created ok.
		delete object;
		
		return 0;
	}

	virtual CreatorResult LoadFromXML(TiXmlElement* element, GameObjectOgreBox2D* gameObjectOgreBox2D )
	{
		CreatorResult result;
		if(element != 0)
		{
			if(gameObjectOgreBox2D != 0)
			{
				result = ObjectOgreCreator::LoadFromXML(element, gameObjectOgreBox2D);
				if(result == CREATOR_OK)
				{
					 
					TiXmlElement* bodys = element->FirstChildElement( "Box2DObject" );

					Ogre::String bodyId;
					bodys->QueryValueAttribute("bodyID", &bodyId);

					gameObjectOgreBox2D->body_ = gameObjectFactory_->GetBody(bodyId);
					gameObjectOgreBox2D->body_->SetUserData(gameObjectOgreBox2D);
					gameObjectOgreBox2D->Initialize();
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