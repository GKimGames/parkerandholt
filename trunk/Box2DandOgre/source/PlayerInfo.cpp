/*=============================================================================

		PlaerInfo.cpp

=============================================================================*/

#include "PlayerInfo.h"
#include "Message.h"

//=============================================================================
//								Constructor
//
PlayerInfo::PlayerInfo()
{
	inventory_ = 0;
	checkPoint_ = b2Vec2(0.0f, 3.0f);
	deaths_ = 0;
	pickUpResetList_.clear();
}

//=============================================================================
//								AddToInventory
//
void PlayerInfo::AddToInventory(int number)
{
	inventory_ += number;
}

//=============================================================================
//								GetInventory
//
int PlayerInfo::GetInventory()
{
	return inventory_;
}

//=============================================================================
//								GetCheckPoint
//
b2Vec2 PlayerInfo::GetCheckPoint()
{
	return checkPoint_;
}

//=============================================================================
//								SetCheckPoint
//
void PlayerInfo::SetCheckPoint(b2Vec2 checkPoint)
{
	checkPoint_ = checkPoint;
	pickUpResetList_.clear();
}

//=============================================================================
//								HandleMessage
//
bool PlayerInfo::HandleMessage(const KGBMessage message)
{
	switch(message.messageType)
	{
		case UPDATE_CHECKPOINT:
		{
			SetCheckPoint(boost::any_cast<b2Vec2>(message.messageData));
			return true;
		}
		case ADD_ITEM:
		{
			AddToInventory(1);
			pickUpResetList_.push_back(boost::any_cast<PickUp*>(message.messageData));
			return true;
		}
	}

	return false;
}

//=============================================================================
//								Update
//
bool PlayerInfo::Update(double timeSinceLastFrame)
{
	return true;
}

//=============================================================================
//								PlayerDied
//
/// Increases the death counts and replaces any items that were not
/// saved at a checkpoint
void PlayerInfo::PlayerDied()
{
	deaths_++;
	for(int i = 0; i < pickUpResetList_.size(); i++)
	{
		AddToInventory(-1);
		pickUpResetList_[i]->HasBeenPickedUp(false);
	}
}

