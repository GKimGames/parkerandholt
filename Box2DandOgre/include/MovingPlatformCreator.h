/*=============================================================================

	MovingPlatformCreator.h

	Author: Matt King

=============================================================================*/

#ifndef MOVING_PLATFORM_CREATOR_H
#define MOVING_PLATFORM_CREATOR_H

#include "GameObjectCreator.h"
#include "MovingPlatform.h"

class MovingPlatformCreator : public GameObjectCreator
{

public:

	MovingPlatformCreator(){}

	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		
		if(element != 0)
		{
			
			std::string str;
			element->QueryValueAttribute("name", &str);

			MovingPlatform* movingPlatform = new MovingPlatform(str);
			
			

			b2Vec2 point1, point2, start, end;
			Box2DXMLLoader::GetB2Vec2(element, "point1", &movingPlatform->point1);
			Box2DXMLLoader::GetB2Vec2(element, "point2", &movingPlatform->point2);
			Box2DXMLLoader::GetB2Vec2(element, "start", &movingPlatform->startVec_);
			Box2DXMLLoader::GetB2Vec2(element, "end", &movingPlatform->endVec_);
			element->QueryValueAttribute("speed", &movingPlatform->speed_);

			
			movingPlatform->sceneManager_ = GAMEFRAMEWORK->sceneManager;

			movingPlatform->Initialize();
			
			return movingPlatform;
		}
		
		return 0;
	}

};


#endif