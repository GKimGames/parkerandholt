/*=============================================================================

	ObjectOgreCreator.h

	Author: Matt King

=============================================================================*/

#ifndef OBJECTOGRE_CREATOR_H
#define OBJECTOGRE_CREATOR_H

#include "GameObjectCreator.h"
#include "GameObjectOgre.h"

class ObjectOgreCreator : public GameObjectCreator
{

public:

	ObjectOgreCreator(){}

	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		
		if(element != 0)
		{
			GameObjectOgre* gameObjectOgre = 0;
		
			std::string str = "";
			element->QueryValueAttribute("name", &str);
			if(str != "")
			{
				gameObjectOgre = new GameObjectOgre(str); 
			}
			else
			{
				gameObjectOgre = new GameObjectOgre(); 
			}

			CreatorResult result = GameObjectCreator::LoadFromXML(element, gameObjectOgre);
	

			return 0;
			
		}
		else
		{

		}

		return 0;
	}

	virtual CreatorResult LoadFromXML(TiXmlElement* element, GameObjectOgre* gameObjectOgre )
	{
		if(element != 0)
		{
			CreatorResult result = GameObjectCreator::LoadFromXML(element, gameObjectOgre);
			
			if(result == CREATOR_OK)
			{
				
		
			}

			return result;
			
		}
		else
		{

		}

		return CREATOR_ELEMENT_IS_ZERO;
	}

};


#endif