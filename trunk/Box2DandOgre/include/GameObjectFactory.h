/*=============================================================================

	GameObjectFactory.h

	Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_FACTORY_H
#define GAME_OBJECT_FACTORY_H

#include <Ogre.h>
#include "GameObjectCreator.h"


typedef std::map<std::string, GameObjectCreator*> CreatorMap;

class GameObjectFactory
{

public:

	GameObjectFactory(){}

	void AddCreator(std::string str, GameObjectCreator* objectCreator);

	GameObject* CreateGameObject(std::string creator,TiXmlElement* element);
	GameObject* CreateGameObject(TiXmlElement* element);
	
	void AddObjectCreators();
	
	Ogre::SceneManager* sceneManager;
protected:

	std::map<std::string, GameObjectCreator*> creatorMap_;

	

};

#endif