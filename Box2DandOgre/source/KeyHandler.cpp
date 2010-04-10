/*=============================================================================

		KeyHandler.cpp

		Author: Greg King

This was going to bind the keys not only to a message but to a certain game object
This line of thought was dropped as gameobject ID's may not be consisten between maps
or even the reloading of a level
=============================================================================*/
#include "Keyhandler.h"

//=============================================================================
//								Constructor
//
KeyHandler::KeyHandler(OIS::Keyboard* keyBoard)
{
	keyBoard_ = keyBoard;
}

//=============================================================================
//								Update
//
bool KeyHandler::Update(double timeSinceLastFrame)
{
	return true;
}

//=============================================================================
//								KeyPressed
//
/// Called by physics state on keypresses
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

//=============================================================================
//								KeyReleased
//
/// Called by physics state on keyReleases
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

//=============================================================================
//								HandleMessage
//
bool KeyHandler::HandleMessage(const KGBMessage message)
{
	return false;
}

//=============================================================================
//								AddKey
//
/// Adds the entered key as being bound to the entered message
/// @warning only binds to keypressed
bool KeyHandler::AddKey(const OIS::KeyCode key, KGBMessageType messageType)
{
	keyListDown_[key] = messageType;
	return true;
}

//=============================================================================
//								Addkey
//
/// Adds a message to key pressed and key released of entered key
/// @param messageType1 is pressed, messageType2 is released
bool KeyHandler::AddKey(const OIS::KeyCode key, KGBMessageType messageType1, KGBMessageType messageType2)
{
	keyListDown_[key] = messageType1;
	keyListUp_[key] = messageType2;
	return true;
}