/*=============================================================================

ObjectOgreCreator.h

Author: Matt King

=============================================================================*/

#ifndef OBJECTOGRE_CREATOR_H
#define OBJECTOGRE_CREATOR_H

#include "GameObjectCreator.h"
#include "GameObjectOgre.h"
#include "OgreXMLLoader.h"

class ObjectOgreCreator : public GameObjectCreator
{

public:

	ObjectOgreCreator(){}


	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{

		GameObjectOgre* object = new GameObjectOgre();
		CreatorResult result = LoadFromXML(element, object);

		if(result == CREATOR_OK)
		{
			return object;
		}

		// It didn't get created ok.
		delete object;
		
		return 0;
	}
	
	virtual CreatorResult LoadFromXML(TiXmlElement* element, GameObjectOgre* gameObjectOgre )
	{
		CreatorResult result;
		if(element != 0)
		{
			if(gameObjectOgre != 0)
			{
				result = GameObjectCreator::LoadFromXML(element, gameObjectOgre);
				
				if(result == CREATOR_OK)
				{
					TiXmlElement* ogreObjectElement = element->FirstChild( "OgreObject" )->ToElement();
					// Get the mesh for the Game Ogre Object
					std::string meshName;
					int xmlResult = ogreObjectElement->QueryValueAttribute("mesh", &meshName);

					if(xmlResult == TIXML_SUCCESS)
					{
						if(gameObjectOgre->Initialize(meshName))
						{
							// Get the position for the sceneNode of the Game Ogre Object
							Ogre::Vector3 sceneNodePosition;
							OgreXMLLoader::GetVector3(ogreObjectElement, "position", &sceneNodePosition);
							gameObjectOgre->sceneNode_->setPosition(sceneNodePosition);

							// Get the position for the sceneNode of the Game Ogre Object
							Ogre::Vector3 sceneNodeScale;
							if(OgreXMLLoader::GetVector3(ogreObjectElement, "scale", &sceneNodeScale) == OGREXML_OK)
							{
								gameObjectOgre->sceneNode_->scale(sceneNodeScale);
							}

						}
						else
						{
							result = CREATOR_INITIALIZED_FAILED;
							Ogre::String s = "ObjectOgreCreator: Failed to initialize GameObjectOgre";
							GAMEFRAMEWORK->log_->logMessage(s);
							DEBUG_LOG(s);
						}
					}
					else
					{
						result = CREATOR_NO_MESH;
						Ogre::String s = "ObjectOgreCreator: Failed to initialize GameObjectOgre: no mesh attribute";
						GAMEFRAMEWORK->log_->logMessage(s);
						DEBUG_LOG(s);
					}
				}

			}// if(gameObjectOgre != 0)
			else
			{
				result = CREATOR_OBJECT_IS_ZERO;
			}
		}// if(element != 0)
		else
		{
			result = CREATOR_ELEMENT_IS_ZERO;
		}

		return result;
	}
	
};



#endif