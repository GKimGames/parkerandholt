#include "PlayerInfo.h"
#include "Message.h"

PlayerInfo::PlayerInfo()
{
	inventory_ = 0;
	checkPoint_ = b2Vec2(0.0f, 3.0f);


}

void PlayerInfo::AddToInventory(int number)
{
	inventory_ += number;
}

int PlayerInfo::GetInventory()
{
	return inventory_;
}

b2Vec2 PlayerInfo::GetCheckPoint()
{
	return checkPoint_;
}

void PlayerInfo::SetCheckPoint(b2Vec2 checkPoint)
{
	checkPoint_ = checkPoint;
}

bool PlayerInfo::HandleMessage(const KGBMessage message)
{
	switch(message.messageType)
	{
	case UPDATE_CHECKPOINT:
		{
			checkPoint_ = boost::any_cast<b2Vec2>(message.messageData);
			return true;
		}
	case ADD_ITEM:
		{
			AddToInventory(boost::any_cast<int>(message.messageData));
			//inventory_ += boost::any_cast<int>(message.messageData);
			return true;
		}
	}

	return false;
}

bool PlayerInfo::Update(double timeSinceLastFrame)
{
	return true;
}