/*=============================================================================

	CheckPointCreator.h

	Author: Matt King

=============================================================================*/


#ifndef CHECK_POINT_CREATOR_H
#define CHECK_POINT_CREATOR_H

#include "GameObjectCreator.h"
#include "CheckPoint.h"

/// Creates a check point from XML.
/// For simplicity of the XML this extends the base GameObjectCreator.
class CheckPointCreator : public GameObjectCreator
{

public:

	CheckPointCreator(GameObjectFactory* gameObjectFactory) : GameObjectCreator(gameObjectFactory){}
	
	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		 
		if(element != 0)
		{
			b2Vec2 position = TinyXMLHelper::GetAttributeb2Vec2(element, "position");
			return  (new CheckPoint(GAMEFRAMEWORK->sceneManager,position));
		}

		// Error Happened
		return 0;
	}

};

#endif