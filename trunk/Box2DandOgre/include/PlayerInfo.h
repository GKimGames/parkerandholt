/*=============================================================================

		PlayerInfo.h

		Stores information about the character

=============================================================================*/

#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include "GameObject.h"
#include <Box2D/Box2D.h>
#include "PickUp.h"


class PlayerInfo : public GameObject
{
public:

	PlayerInfo();

	/// Updates the last checkpoint touched.
	void SetCheckPoint(b2Vec2 checkPoint);
	b2Vec2 GetCheckPoint();

	/// Adds the number of items indicated in the parameter.
	void AddToInventory(int number);

	/// Returns the number of items carried by the player.
	int GetInventory();

	bool HandleMessage(const KGBMessage message);
	bool Update(double timeSinceLastFrame);

	/// Increments the number of times the player has Died.
	void PlayerDied();



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
	int				deaths_;

	std::vector<PickUp*>	pickUpResetList_;
};

#endif