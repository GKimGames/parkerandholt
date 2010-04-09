/*=============================================================================

		StaticOgreObjectCreator.h

		Author: Matt King

=============================================================================*/

#ifndef STATIC_OBJECT_OGRE_CREATOR_H
#define STATIC_OBJECT_OGRE_CREATOR_H

#include "GameObjectCreator.h"
#include "GameObjectOgre.h"
#include "OgreXMLLoader.h"
#include "Ogre.h"
#include "MeshTools.h"
#include <Box2D/Box2D.h>

class StaticOgreObjectCreator : public GameObjectCreator
{

public:

	StaticOgreObjectCreator(GameObjectFactory* gameObjectFactory):GameObjectCreator(gameObjectFactory){}


	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{

		TiXmlElement* meshElement = element->FirstChildElement("Mesh")->ToElement();

		while(meshElement)
		{
			GameObjectOgre* object = new GameObjectOgre("StaticMeshObject");

			object->objectName_ = TinyXMLHelper::GetAttribute(meshElement, "name");
			object->Initialize();

			object->sceneNode_ = GAMEFRAMEWORK->sceneManager->getRootSceneNode()->createChildSceneNode(object->GetName() + "sceneNode");
			ParseMesh(meshElement, object);
			meshElement = meshElement->NextSiblingElement("Mesh");
		}

		TiXmlElement* lightElement = element->FirstChildElement("Light")->ToElement();

		while(lightElement)
		{
			GameObjectOgre* object = new GameObjectOgre("StaticLightObject");
			object->Initialize();

			object->sceneNode_ = GAMEFRAMEWORK->sceneManager->getRootSceneNode()->createChildSceneNode(object->GetName() + "sceneNode");
			ParseLight(lightElement, object);
			lightElement = lightElement->NextSiblingElement("Light");
		}

		TiXmlElement* billboardElement = element->FirstChildElement("BillboardSet")->ToElement();

		while(billboardElement)
		{
			GameObjectOgre* object = new GameObjectOgre("BillboardSetObject");
			object->Initialize();
			object->sceneNode_ = GAMEFRAMEWORK->sceneManager->getRootSceneNode()->createChildSceneNode(object->GetName() + "sceneNode");
			ParseBillBoard(billboardElement, object);

			billboardElement = billboardElement->NextSiblingElement("BillboardSet");
		}

		return 0;
	}


	void ParseBillBoard(TiXmlElement* element, GameObjectOgre* gameObjectOgre)
	{
		gameObjectOgre->sceneNode_->setPosition(TinyXMLHelper::GetAttributeVector3(element, "position"));

		Ogre::BillboardSet* billboardset = gameObjectOgre->sceneManager_->createBillboardSet(gameObjectOgre->GetName() + "bbs", 5);
		billboardset->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
		billboardset->setMaterialName(TinyXMLHelper::GetAttribute(element, "material"));
		
		TiXmlElement* billboardElement = element->FirstChildElement("Billboard")->ToElement();

		while(billboardElement)
		{
			Ogre::Vector3 position = TinyXMLHelper::GetAttributeVector3(billboardElement, "position");
			Ogre::ColourValue color = TinyXMLHelper::GetAttributeColor(billboardElement, "color", Ogre::ColourValue::White);
			Ogre::Billboard* bb = billboardset->createBillboard(position, color);	
			bb->setRotation( Ogre::Radian(TinyXMLHelper::GetAttributeReal(billboardElement, "rotation", 0.0)) );

			billboardElement = billboardElement->NextSiblingElement("Billboard");

		}

		gameObjectOgre->sceneNode_->attachObject(billboardset);
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
			
			Ogre::Vector3 rotation = TinyXMLHelper::GetAttributeVector3(element, "rotation");
			
			gameObjectOgre->sceneNode_->pitch(Ogre::Radian(Ogre::Degree(rotation.x)));
			gameObjectOgre->sceneNode_->yaw(Ogre::Radian(Ogre::Degree(rotation.y)));
			gameObjectOgre->sceneNode_->roll(Ogre::Radian(Ogre::Degree(rotation.z)));
			gameObjectOgre->sceneNode_->setPosition(sceneNodePosition);
			// Attach the mesh entity.
			gameObjectOgre->sceneNode_->attachObject(meshEntity);

			// Get the scaling factor for the mesh
			Ogre::Vector3 sceneNodeScale = TinyXMLHelper::GetAttributeVector3(element, "scale", Ogre::Vector3::UNIT_SCALE);
			gameObjectOgre->sceneNode_->scale(sceneNodeScale);

			if(TinyXMLHelper::GetAttributeBool(element, "fitToPoints"))
			{
				Ogre::Vector3 point1 = TinyXMLHelper::GetAttributeVector3(element, "point1");
				Ogre::Vector3 point2 = TinyXMLHelper::GetAttributeVector3(element, "point2");
				Ogre::Vector3 offSet = TinyXMLHelper::GetAttributeVector3(element, "offset");
				
				Ogre::Vector3 center = point1.midPoint(point2) + offSet;
				
				Ogre::Real length = point1.distance(point2);
				gameObjectOgre->sceneNode_->setPosition(center);

				Ogre::AxisAlignedBox box = meshEntity->getBoundingBox();
				Ogre::Vector3 v3 = box.getSize();

				gameObjectOgre->sceneNode_->setScale(length / v3.x, sceneNodeScale.y, sceneNodeScale.z);
				//meshSceneNode->roll(point1.angleBetween(point2));
			}

			if(TinyXMLHelper::GetAttributeBool(element, "box2DitUp"))
			{
				std::vector<Ogre::Vector3> vectors = MeshTools::GetBox2DOutLine(meshEntity);
				std::vector<Ogre::Vector3> myVecs = MeshTools::convexHull(vectors);

				b2World* world = GAMEFRAMEWORK->world_;
				b2Body* body;

				b2BodyDef bd;
				bd.position.Set(sceneNodePosition.x, sceneNodePosition.y);
				bd.angle = rotation.z * 3.14159 / 180;
				bd.fixedRotation = true;
				body = world->CreateBody(&bd);

				b2FixtureDef fd;
				b2PolygonShape polyDef;
				fd.filter.groupIndex = STATIC_MAP_GROUP;

				fd.friction = DEFAULT_FRICTION;
				fd.restitution = 0.1;
				fd.filter.groupIndex = STATIC_MAP_GROUP;

				for(unsigned int i = 0; i < myVecs.size() - 1;i++)
				{
					polyDef.SetAsEdge( b2Vec2(myVecs.at(i).x   * sceneNodeScale.x, myVecs.at(i).y   * sceneNodeScale.y),
									   b2Vec2(myVecs.at(i+1).x * sceneNodeScale.x, myVecs.at(i+1).y * sceneNodeScale.y)
									   );

					fd.shape = &polyDef;
					
					body->CreateFixture(&fd);
				}

			}

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
			
			Ogre::String s = "StaticOgreObjectCreator: Failed to initialize GameObjectOgre: no mesh attribute";
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
		gameObjectOgre->sceneNode_->attachObject(ps);
		gameObjectOgre->sceneNode_->setPosition(TinyXMLHelper::GetAttributeVector3(element, "position"));

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

		Ogre::SceneNode* planeNode = gameObjectOgre->sceneNode_;
		Ogre::Vector3 rotation = TinyXMLHelper::GetAttributeVector3(element, "rotation");
		planeNode->pitch(Ogre::Radian(Ogre::Degree(rotation.x)));
		planeNode->yaw(Ogre::Radian(Ogre::Degree(rotation.y)));
		planeNode->roll(Ogre::Radian(Ogre::Degree(rotation.z)));

		planeNode->attachObject(planeEnt);
		
		// Rotate the node to fit the points
		planeNode->roll(Ogre::Radian(atan2(point2.y - point1.y, point2.x - point1.x)));
		
		// Center the node on the midpoint of the two points
		planeNode->setPosition(Ogre::Real(point1.x + point2.x) / 2.0, Ogre::Real(point1.y + point2.y) / 2.0, 0.0);
		Ogre::Vector3 offset = TinyXMLHelper::GetAttributeVector3(element, "offset");
		planeNode->translate(offset);

		if(TinyXMLHelper::GetAttributeBool(element, "box2D", true))
		{
			b2World* world = GAMEFRAMEWORK->world_;
			b2Body* body;

			b2BodyDef bd;
			body = world->CreateBody(&bd);

			b2FixtureDef fd;
			b2PolygonShape polyDef;
			fd.filter.groupIndex = STATIC_MAP_GROUP;

			fd.friction = DEFAULT_FRICTION;
			fd.restitution = 0.1;
			fd.filter.groupIndex = STATIC_MAP_GROUP;


			polyDef.SetAsEdge(point1, point2);

			fd.shape = &polyDef;

			body->CreateFixture(&fd);
		}
		

	}

};



#endif