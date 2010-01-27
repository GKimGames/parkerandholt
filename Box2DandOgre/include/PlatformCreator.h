/*=============================================================================

	PlatformCreator.h

	Author: Matt King

=============================================================================*/

#ifndef PLATFORM_CREATOR_H
#define PLATFORM_CREATOR_H

#include "GameObjectCreator.h"
#include "Platform.h"

class PlatformCreator : public GameObjectCreator
{

public:

	PlatformCreator(GameObjectFactory* gameObjectFactory):GameObjectCreator(gameObjectFactory){}

	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		
		if(element != 0)
		{
			Platform* platform = new Platform();

			std::string str;
			element->QueryValueAttribute("name", &str);
			platform->objectName_ = str;

			element->QueryFloatAttribute("px1", &platform->point1.x);
			element->QueryFloatAttribute("py1", &platform->point1.y);
			element->QueryFloatAttribute("px2", &platform->point2.x);
			element->QueryFloatAttribute("py2", &platform->point2.y);

			platform->sceneManager_ = GAMEFRAMEWORK->gameObjectFactory->sceneManager;

			platform->Initialize();
			
			return platform;
		}
		
		return 0;
	}

};


#endif