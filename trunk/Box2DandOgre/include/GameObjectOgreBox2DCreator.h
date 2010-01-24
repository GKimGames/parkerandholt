/*=============================================================================

	GameObjectOgreBox2DCreator.h

	Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_OGRE_BOX2D_CREATOR_H
#define GAME_OBJECT_OGRE_BOX2D_CREATOR_H

#include "GameObjectOgreBox2D.h"
#include "ObjectOgreCreator.h"
#include "OgreXMLLoader.h"
#include "Box2DXMLLoader.h"

class GameObjectOgreBox2DCreator : public ObjectOgreCreator
{

public:

	GameObjectOgreBox2DCreator(){}
	
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
					 
					TiXmlElement* bodys = element->FirstChildElement( "Box2DObject" )->FirstChildElement("Body");
					gameObjectOgreBox2D->body_ = Box2DXMLLoader::Createb2Body(GAMEFRAMEWORK->world_, bodys);
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