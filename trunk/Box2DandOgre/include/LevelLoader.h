/*=============================================================================

		LevelLoader.h

		Author: Matt King

=============================================================================*/

#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include "GameObject.h"
#include "GameFramework.h"

class TiXmlElement;
class GameObjectFactory;
class LevelLoader : public GameObject
{
public:

	LevelLoader(Ogre::String name);

	LoadLevel(TiXmlElement* element);

	void SetFactory(GameObjectFactory* gameObjectFactory);
	GameObjectFactory* GetFactory();

protected:

	GameObjectFactory* gameObjectFactory_;
	TiXmlElement* element_;

private:
};