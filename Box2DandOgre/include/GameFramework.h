#ifndef GAME_FRAMEWORK_H
#define GAME_FRAMEWORK_H


#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <Box2D/Box2D.h>
#include "BetaGUI.h"

#define GAMEFRAMEWORK GameFramework::getSingletonPtr()

/// GameFramework srarts up Ogre and loads resource to be used as well as
/// storing the current states b2World reference
class GameFramework : public Ogre::Singleton<GameFramework>, OIS::KeyListener, OIS::MouseListener
{
public:

	GameFramework();
	~GameFramework();

	bool InitOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0);
	void UpdateOgre(double timeSinceLastFrame);
	void UpdateStats();

	bool keyPressed(const OIS::KeyEvent &keyEventRef){return KeyPressed(keyEventRef);};
	bool keyReleased(const OIS::KeyEvent &keyEventRef){return KeyReleased(keyEventRef);};

	bool mouseMoved(const OIS::MouseEvent &evt){return MouseMoved(evt);};
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){return MousePressed(evt,id);};
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){return MouseReleased(evt,id);};
	
	bool KeyPressed(const OIS::KeyEvent &keyEventRef);
	bool KeyReleased(const OIS::KeyEvent &keyEventRef);

	bool MouseMoved(const OIS::MouseEvent &evt);
	bool MousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 
	bool MouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	void SetWorld(b2World* world){world_ = world;};
	b2World* GetWorld(){return world_;};
	
	double GetTimeSinceLastFrame(){ return timeSinceLastFrame_; }
	void SetTimeSinceLastFrame(double timeSinceLastFrame){ timeSinceLastFrame_ = timeSinceLastFrame; }

	Ogre::Root*					root_;
	Ogre::RenderWindow*			renderWindow_;
	Ogre::Viewport*				viewport_;
	Ogre::Log*					log_;
	Ogre::Timer*				timer_;
	
	OIS::InputManager*			inputManager_;
	OIS::Keyboard*				keyboard_;
	OIS::Mouse*					mouse_;

	b2World*					world_;


private:

	GameFramework(const GameFramework&);
	GameFramework& operator= (const GameFramework&);

	Ogre::Overlay*			debugOverlay_;
	Ogre::Overlay*			infoOverlay_;
	int						numScreenShots_;
	double					timeSinceLastFrame_;
};



#endif 
