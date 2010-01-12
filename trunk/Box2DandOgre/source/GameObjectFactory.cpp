#include "GameObjectFactory.h"
#include "PlatformCreator.h"
#include "ObjectOgreCreator.h"

void GameObjectFactory::AddCreator(std::string str, GameObjectCreator* objectCreator)
{
	creatorMap_.insert(
		std::make_pair<std::string, GameObjectCreator*>(
		str,
		objectCreator
		));
}



GameObject* GameObjectFactory::CreateGameObject(std::string creator,TiXmlElement* element)
{
	std::map<std::string, GameObjectCreator*>::iterator it = creatorMap_.find(creator);

	if(it == creatorMap_.end())
	{
		return 0;
	}

	GameObjectCreator* goc = (*it).second;

	return goc->LoadFromXML(element);


}



void GameObjectFactory::AddObjectCreators()
{
	AddCreator("GameObject", new GameObjectCreator());
	AddCreator("Platform", new PlatformCreator());
	AddCreator("ObjectOgre", new ObjectOgreCreator());
}