#include "GameObjectFactory.h"

#include "tinyxml.h"
#include "TinyXMLHelper.h"

#include "GameObjectCreator.h"
#include "ObjectOgreCreator.h"
#include "GameObjectOgreBox2DCreator.h"

#include "PlatformCreator.h"
#include "MovingPlatformCreator.h"

void GameObjectFactory::LoadFile(Ogre::String fileName)
{
	TiXmlDocument configXML_(fileName.c_str());
	configXML_.LoadFile();
	TiXmlHandle hDoc(&configXML_);

	TiXmlElement* rootElement;
	TiXmlHandle rootHandle(0);

	rootElement = hDoc.FirstChildElement().Element();

	// The XML file didn't load, we can't do anything.
	if (!rootElement) 
		return;

	rootHandle = TiXmlHandle(rootElement);
	TiXmlElement* box2DNode = rootHandle.FirstChildElement( "Box2D" ).ToElement();

	box2DLoader_ = new Box2DXMLLoader(box2DNode, GAMEFRAMEWORK->world_);

	TiXmlHandle gameObjectsNode(rootHandle.FirstChildElement( "GameObjects" ));
	TiXmlElement* objectNode = gameObjectsNode.FirstChildElement("GameObject").ToElement();

	while(objectNode)
	{
		CreateGameObject(objectNode);
		objectNode = objectNode->NextSiblingElement("GameObject");
	}
	
}

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
	AddCreator("GameObject",		new GameObjectCreator(this));
	AddCreator("ObjectOgre",		new ObjectOgreCreator(this));
	AddCreator("OgreBox2D",			new GameObjectOgreBox2DCreator(this));
	AddCreator("Platform",			new PlatformCreator(this));
	AddCreator("MovingPlatform",	new MovingPlatformCreator(this));
}


b2Body*	GameObjectFactory::GetBody(Ogre::String id)
{
	box2DLoader_->GetBody(id);
}


b2Fixture* GameObjectFactory::GetFixture(Ogre::String id)
{
	box2DLoader_->GetFixture(id);
}

b2Joint* GameObjectFactory::GetJoint(Ogre::String id)
{
	box2DLoader_->GetJoint(id);
}