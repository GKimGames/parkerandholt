#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include "GameObject.h"
#include <Box2D/Box2D.h>



class PlayerInfo : GameObject
{
public:

	PlayerInfo();

	void SetCheckPoint(b2Vec2 checkPoint);
	b2Vec2 GetCheckPoint();
	void AddToInventory(int number);
	int GetInventory();
	bool HandleMessage(const KGBMessage message);
	bool Update(double timeSinceLastFrame);



	virtual bool Initialize()
	{
		GameObject::GameObject();
		initialized_ = true;

		return true;
	}

protected:
	b2Vec2			checkPoint_;
	int				inventory_;
	Ogre::String	itemDisplay_;
};

#endif