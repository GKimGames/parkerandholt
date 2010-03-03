/*=============================================================================

		ObjectOgreCreator.h

		Author: Matt King

=============================================================================*/

#ifndef OBJECTOGRE_CREATOR_H
#define OBJECTOGRE_CREATOR_H

#include "GameObjectCreator.h"
#include "GameObjectOgre.h"
#include "OgreXMLLoader.h"
#include "Ogre.h"

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
		Ogre::String type = TinyXMLHelper::GetAttribute(element, "type");
		Ogre::String meshName = TinyXMLHelper::GetAttribute(element, "mesh");
		// If there is a mesh name we load that, if not we check type of Object Ogre this is
		if(!meshName.empty())
		{
			Ogre::String entityName = gameObjectOgre->objectName_ + "/entity/";
			entityName += TinyXMLHelper::GetAttribute(element, "name", Ogre::StringConverter::toString(meshCounter));

			Ogre::Entity* meshEntity = GAMEFRAMEWORK->sceneManager->createEntity(entityName,meshName);

			// Get the position for the sceneNode of the Game Ogre Object
			Ogre::Vector3 sceneNodePosition = TinyXMLHelper::GetAttributeVector3(element, "position");
			Ogre::SceneNode* meshSceneNode = gameObjectOgre->sceneNode_->createChildSceneNode();
			
			Ogre::Vector3 rotation = TinyXMLHelper::GetAttributeVector3(element, "rotation");
			
			meshSceneNode->pitch(Ogre::Radian(Ogre::Degree(rotation.x)));
			meshSceneNode->yaw(Ogre::Radian(Ogre::Degree(rotation.y)));
			meshSceneNode->roll(Ogre::Radian(Ogre::Degree(rotation.z)));
			meshSceneNode->setPosition(sceneNodePosition);
			// Attach the mesh entity.
			meshSceneNode->attachObject(meshEntity);

			// Get the scaling factor for the mesh
			Ogre::Vector3 sceneNodeScale = TinyXMLHelper::GetAttributeVector3(element, "scale", Ogre::Vector3::UNIT_SCALE);
			meshSceneNode->scale(sceneNodeScale);

		}
		else if(type.compare("platform") == 0)
		{
			CreatePlatform(element,gameObjectOgre);
		}
		else if(type.compare("particleSystem") == 0)
		{
			CreateParticle(element,gameObjectOgre);
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
		Ogre::String name = TinyXMLHelper::GetAttribute(element, "name");
		Ogre::Light* light = GAMEFRAMEWORK->sceneManager->createLight(name);
		
		light->setType(Ogre::Light::LT_SPOTLIGHT);
		light->setPosition(TinyXMLHelper::GetAttributeVector3(element, "position"));
		light->setDirection(TinyXMLHelper::GetAttributeVector3(element, "direction"));
		
		Ogre::Radian innerAngle(TinyXMLHelper::GetAttributeReal(element, "innerAngle", 35));
		Ogre::Radian outerAngle(TinyXMLHelper::GetAttributeReal(element, "outerAngle", 50));
		light->setSpotlightRange(innerAngle, outerAngle);
	
		gameObjectOgre->sceneNode_->attachObject(light);
	}

	void CreateParticle(TiXmlElement* element, GameObjectOgre* gameObjectOgre)
	{
		Ogre::SceneNode* node = gameObjectOgre->sceneNode_->createChildSceneNode();
		Ogre::String name = TinyXMLHelper::GetAttribute(element, "name");
		Ogre::String particleSystem = TinyXMLHelper::GetAttribute(element, "particleSystem");
		Ogre::ParticleSystem* ps = gameObjectOgre->sceneManager_->createParticleSystem(name, particleSystem);
		node->attachObject(ps);
		node->setPosition(TinyXMLHelper::GetAttributeVector3(element, "position"));

		ps->getEmitter(0)->setDirection(TinyXMLHelper::GetAttributeVector3(element, "direction"));
	}

	void CreatePlatform(TiXmlElement* element, GameObjectOgre* gameObjectOgre)
	{
		Ogre::String material = TinyXMLHelper::GetAttribute(element, "material","Matt/Road");
		b2Vec2 point1, point2;
		Box2DXMLLoader::GetB2Vec2(element, "point1", &point1);
		Box2DXMLLoader::GetB2Vec2(element, "point2", &point2);

		Ogre::Real zLength = TinyXMLHelper::GetAttributeReal(element, "zLength", 15);
		Ogre::Real xTile = TinyXMLHelper::GetAttributeReal(element, "xTile", 1);
		Ogre::Real yTile = TinyXMLHelper::GetAttributeReal(element, "yTile", 1);
		Ogre::Real xSegments = TinyXMLHelper::GetAttributeReal(element, "xSegments", 1);
		Ogre::Real ySegments = TinyXMLHelper::GetAttributeReal(element, "ySegments", 1);

		if(point1.x > point2.x)
		{
			b2Vec2 holder = point2;
			point2 = point1;
			point1 = holder;
		}

		Ogre::String planeName("PlatformPlane");
		Ogre::String planeMeshName("PlatformPlaneMesh");
		Ogre::String planeEntityName("PlatformPlaneEntity");

		planeName += Ogre::StringConverter::toString(gameObjectOgre->objectId_);
		planeMeshName += Ogre::StringConverter::toString(gameObjectOgre->objectId_);
		planeEntityName += Ogre::StringConverter::toString(gameObjectOgre->objectId_);

		Ogre::MovablePlane* plane = new Ogre::MovablePlane(planeName);
		plane->d = 0; 
		plane->normal = Ogre::Vector3(0.0, 1.0, 0.0);

		float lengthOfPlane = sqrt((point2.x - point1.x) * (point2.x - point1.x) + (point2.y - point1.y) * (point2.y - point1.y));
		
		Ogre::MeshManager::getSingleton().createPlane(planeMeshName, 
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			*plane,
			lengthOfPlane,  // X Length
			zLength,				// Z Length
			xSegments, ySegments,			// Segments x ,y
			true,
			1, 
			xTile,		// Tile x
			yTile,							// Tile y
			Ogre::Vector3::UNIT_Z);

		Ogre::Entity* planeEnt = GAMEFRAMEWORK->sceneManager->createEntity(planeEntityName , planeMeshName);
		planeEnt->setMaterial(Ogre::MaterialManager::getSingleton().getByName(material));

		Ogre::SceneNode* planeNode = gameObjectOgre->sceneNode_->createChildSceneNode();
		planeNode->attachObject(planeEnt);
		
		// Rotate the node to fit the points
		planeNode->roll(Ogre::Radian(atan2(point2.y - point1.y, point2.x - point1.x)));
		
		// Center the node on the midpoint of the two points
		planeNode->setPosition(Ogre::Real(point1.x + point2.x) / 2.0, Ogre::Real(point1.y + point2.y) / 2.0, 0.0);
	}

};



#endif