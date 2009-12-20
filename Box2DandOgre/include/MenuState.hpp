

#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "AppState.hpp"
#include "GameFramework.h"

class MenuState : public AppState, public BetaGUI::BetaGUIListener
{
public:
	DECLARE_APPSTATE_CLASS(MenuState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	void update(double timeSinceLastFrame);
	
	void onButtonPress(BetaGUI::Button *ref)
	{      
		if (ref == enterGameButton_)
			this->pushAppState(findByName("PhysicsState"));
	}

private:

	BetaGUI::Button* enterGameButton_;

	
	bool m_bQuit;
};


#endif
