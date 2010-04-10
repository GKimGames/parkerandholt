/*=============================================================================

		KeyHandler.h

		Author: Greg King
		
=============================================================================*/
#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#include "GameObject.h"
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <boost/any.hpp>
#include <Ogre.h>
#include <tinyxml.h>


#include "MessageDispatcher.h"

// Map type that holds our keymap
typedef std::map<OIS::KeyCode, KGBMessageType> KeyMap;

class KeyHandler : public GameObject
{

public:

	KeyHandler(OIS::Keyboard* keyBoard);
	~KeyHandler(){}
	
	KeyMap keyListUp_;
	KeyMap keyListDown_;

	bool Update(double timeSinceLastFrame);
	bool HandleMessage(const KGBMessage message);

	bool KeyPressed(const OIS::KeyEvent &keyEventRef);
	bool KeyReleased(const OIS::KeyEvent &keyEventRef);

	/// Bind a message to key being down
	bool AddKey(const OIS::KeyCode key, const KGBMessageType message);

	/// Bind first message to key down, second to key up
	bool AddKey(const OIS::KeyCode key, KGBMessageType messageType1, KGBMessageType messageType2);

	virtual bool Initialize()
	{
		initialized_ = true;
		return true;
	}

protected:
	OIS::Keyboard*				keyBoard_;

};

#endif