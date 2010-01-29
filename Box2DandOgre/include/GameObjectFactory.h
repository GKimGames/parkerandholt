/*=============================================================================

		GameObjectFactory.h

		Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_FACTORY_H
#define GAME_OBJECT_FACTORY_H

#include <Ogre.h>
#include "Box2DXMLLoader.h"

#include <vector>

class TiXmlElement;
class GameObjectCreator;
class GameObject;

typedef std::map<std::string, GameObjectCreator*> CreatorMap;

class GameObjectFactory
{

public:

	GameObjectFactory(){}

	void LoadFile(Ogre::String fileName);

	void AddCreator(std::string str, GameObjectCreator* objectCreator);

	GameObject* CreateGameObject(std::string creator,TiXmlElement* element);
	GameObject* CreateGameObject(TiXmlElement* element);
	
	void AddObjectCreators();
	
	Ogre::SceneManager* sceneManager;

	b2Body*		GetBody(Ogre::String id);
	b2Fixture*	GetFixture(Ogre::String id);
	b2Joint*	GetJoint(Ogre::String id);

protected:

	Box2DXMLLoader* box2DLoader_;

	std::vector<TiXmlElement*> sensorList_;

	std::map<std::string, GameObjectCreator*> creatorMap_;

	

};

#endif