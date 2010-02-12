/*=============================================================================

	PlatformCreator.h

	Author: Matt King

=============================================================================*/

#ifndef PLATFORM_CREATOR_H
#define PLATFORM_CREATOR_H

#include "GameObjectOgreBox2DCreator.h"
#include "Platform.h"
#include "Box2DXMLLoader.h"

class PlatformCreator : public GameObjectOgreBox2DCreator
{

public:

	PlatformCreator(GameObjectFactory* gameObjectFactory):GameObjectOgreBox2DCreator(gameObjectFactory){}

	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{

		Platform* platform = new Platform();
		CreatorResult result = LoadFromXML(element, platform);

		if(result == CREATOR_OK)
		{
			return platform;
		}

		// It didn't get created ok.
		delete platform;
		
		return 0;
	}

	virtual CreatorResult LoadFromXML(TiXmlElement* element, Platform* platform )
	{
		CreatorResult result= CREATOR_OK;
		if(element != 0)
		{
			if(platform != 0)
			{
				//result = GameObjectCreator::LoadFromXML(element, platform);
				if(result == CREATOR_OK)
				{
					 
					TiXmlElement* platformNode = element->FirstChildElement( "Platform" );

					std::string str;
					platformNode->QueryValueAttribute("name", &str);
					platform->objectName_ = str;
			
					Box2DXMLLoader::GetB2Vec2(platformNode, "point1", &platform->point1);
					Box2DXMLLoader::GetB2Vec2(platformNode, "point2", &platform->point2);

					platform->sceneManager_ = GAMEFRAMEWORK->gameObjectFactory->sceneManager;

					platform->Initialize();
				}
	
			}
		}

	
		return result;
	}

};


#endif