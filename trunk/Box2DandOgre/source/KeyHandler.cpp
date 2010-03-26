/*=============================================================================

		KeyHandler.cpp

		Author: Greg King

=============================================================================*/
#include "Keyhandler.h"

KeyHandler::KeyHandler(OIS::Keyboard* keyBoard)
{
	keyBoard_ = keyBoard;
}

bool KeyHandler::Update(double timeSinceLastFrame)
{
	return true;
}

bool KeyHandler::KeyPressed(const OIS::KeyEvent &keyEventRef)
{
	KeyMap::const_iterator currentKey_ = keyListDown_.find(keyEventRef.key);
	if(currentKey_ == keyListDown_.end())
	{
		return false;
	}
	else
	{
		Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, SEND_TO_ALL, currentKey_->second, NULL);
		return true;
	}

	return false;
}

bool KeyHandler::KeyReleased(const OIS::KeyEvent &keyEventRef)
{
	KeyMap::const_iterator currentKey_ = keyListUp_.find(keyEventRef.key);
	if(currentKey_ == keyListUp_.end())
	{
		return false;
	}
	else
	{
		Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, SEND_TO_ALL, currentKey_->second, NULL);
		return true;
	}

	return false;
}

bool KeyHandler::HandleMessage(const KGBMessage message)
{
	return false;
}

bool KeyHandler::AddKey(const OIS::KeyCode key, KGBMessageType messageType)
{
	keyListDown_[key] = messageType;
	return true;
}

bool KeyHandler::AddKey(const OIS::KeyCode key, KGBMessageType messageType1, KGBMessageType messageType2)
{
	keyListDown_[key] = messageType1;
	keyListUp_[key] = messageType2;
	return true;
}



void KeyHandler::CheckKeys()
{
	for(KeyMap::const_iterator it = keyListDown_.begin(); it != keyListDown_.end(); it++)
	{
		if(keyBoard_->isKeyDown(it->first))
		{
			Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, SEND_TO_ALL, it->second, NULL);
		}
	}

	for(KeyMap::const_iterator it = keyListUp_.begin(); it != keyListUp_.end(); it++)
	{
		if(keyBoard_->isKeyDown(it->first))
		{
			Dispatch->DispatchMessage(SEND_IMMEDIATELY, 0, SEND_TO_ALL, it->second, NULL);
		}
	}
}