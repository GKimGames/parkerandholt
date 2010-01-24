#include "GameObjectFactory.h"
#include "PlatformCreator.h"
#include "MovingPlatformCreator.h"
#include "ObjectOgreCreator.h"
#include "GameObjectOgreBox2DCreator.h"

GameObject* GameObjectFactory::CreateGameObject(std::string creator, TiXmlElement* element)
{
	std::map<std::string, GameObjectCreator*>::iterator it = creatorMap_.find(creator);

	if(it == creatorMap_.end())
	{
		return 0;
	}

	GameObjectCreator* goc = (*it).second;

	return goc->LoadFromXML(element);

}

GameObject* GameObjectFactory::CreateGameObject(TiXmlElement* element)
{
	if(element!=0)
	{
		std::string creator;
		int result = element->QueryValueAttribute("type", &creator);

		std::map<std::string, GameObjectCreator*>::iterator it = creatorMap_.find(creator);

		if(it == creatorMap_.end())
		{
			return 0;
		}

		GameObjectCreator* goc = (*it).second;

		return goc->LoadFromXML(element);
	}

	return 0;
}

void GameObjectFactory::AddCreator(std::string str, GameObjectCreator* objectCreator)
{
	creatorMap_.insert(
		std::make_pair<std::string, GameObjectCreator*>(
			str,
			objectCreator
			)
		);
}


void GameObjectFactory::AddObjectCreators()
{
	AddCreator("GameObject",		new GameObjectCreator());
	AddCreator("ObjectOgre",		new ObjectOgreCreator());
	AddCreator("OgreBox2D",			new GameObjectOgreBox2DCreator());
	AddCreator("Platform",			new PlatformCreator());
	AddCreator("MovingPlatform",	new MovingPlatformCreator());
}
