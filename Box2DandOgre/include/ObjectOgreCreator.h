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

	ObjectOgreCreator(GameObjectFactory* gameObjectFactory):GameObjectCreator(gameObjectFactory){}


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

	virtual CreatorResult LoadFromXML(TiXmlElement* element, GameObjectOgre* gameObjectOgre)
	{
		CreatorResult result;
		if(element != 0)
		{
			if(gameObjectOgre != 0)
			{
				result = GameObjectCreator::LoadFromXML(element, gameObjectOgre);

				if(result == CREATOR_OK)
				{
					TiXmlHandle ogreObjectElement(element->FirstChild( "OgreObject" ));
					
					TiXmlElement* meshElement = ogreObjectElement.FirstChild("Mesh").ToElement();

					while(meshElement)
					{
						ParseMesh(meshElement, gameObjectOgre);
						meshElement = meshElement->NextSiblingElement("Mesh");
					}

					TiXmlElement* lightElement = ogreObjectElement.FirstChild("Light").ToElement();

					while(lightElement)
					{
						ParseLight(lightElement, gameObjectOgre);
						lightElement = lightElement->NextSiblingElement("Light");
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

	void ParseMesh(TiXmlElement* element, GameObjectOgre* gameObjectOgre)
	{
		static int meshCounter = 0;
		meshCounter++;

		// Get the mesh for the Game Ogre Object
		Ogre::String meshName = TinyXMLHelper::GetAttribute(element, "mesh");
		if(!meshName.empty())
		{
			if(gameObjectOgre->Initialize())
			{
				Ogre::String entityName = gameObjectOgre->objectName_ + "/entity/";
				entityName += TinyXMLHelper::GetAttribute(element, "name", Ogre::StringConverter::toString(meshCounter));

				Ogre::Entity* meshEntity = GAMEFRAMEWORK->sceneManager->createEntity(entityName,meshName);

				// Get the position for the sceneNode of the Game Ogre Object
				Ogre::Vector3 sceneNodePosition = TinyXMLHelper::GetAttributeVector3(element, "position");
				Ogre::SceneNode* meshSceneNode = gameObjectOgre->sceneNode_->createChildSceneNode(sceneNodePosition);

				// Attach the mesh entity.
				meshSceneNode->attachObject(meshEntity);

				// Get the scaling factor for the mesh
				Ogre::Vector3 sceneNodeScale = TinyXMLHelper::GetAttributeVector3(element, "scale", Ogre::Vector3::UNIT_SCALE);
				meshSceneNode->scale(sceneNodeScale);

			}
			else
			{
				Ogre::String s = "ObjectOgreCreator: Failed to initialize GameObjectOgre";
				GAMEFRAMEWORK->log_->logMessage(s);
				DEBUG_LOG(s);
			}
		}
		else
		{
			Ogre::String s = "ObjectOgreCreator: Failed to initialize GameObjectOgre: no mesh attribute";
			GAMEFRAMEWORK->log_->logMessage(s);
			DEBUG_LOG(s);
		}
	}

	void ParseLight(TiXmlElement* element, GameObjectOgre* gameObjectOgre)
	{
		//Ogre::String name = getAttrib(element, "name");
	}

};



#endif