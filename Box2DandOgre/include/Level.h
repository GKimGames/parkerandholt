#ifndef LEVEL_H
#define LEVEL_H
#define DEBUG_DRAW_ON 1

#include <vector>

#include <Box2D/Box2D.h>
#include "Ogre.h"
#include "Parker.h"

#include "OgreB2DebugDraw.h"
#include "MousePicking.h"
#include "KeyHandler.h"

#include "GameFramework.h"
#include "MessageDispatcher.h"

#include "MousePicking.h"
#include "TraumaMeter.h"
#include "PlayerInfo.h"


class CharacterParker;

class Level : public b2ContactListener, public b2DestructionListener
{
	typedef std::vector<ContactPoint*> ContactList;
	typedef std::vector<PostSolveData*> PostSolveList;

public:

	Level(Ogre::String fileName);
	
	~Level();

	void Update(double timeSinceLastFrame);

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	
	void ProcessPostSolveData();

	/// Called when two fixtures begin to touch.
	void BeginContact(b2Contact* contact);

	/// Called when two fixtures cease to touch.
	void EndContact(b2Contact* contact);

	/// Called when any joint is about to be destroyed due
	/// to the destruction of one of its attached bodies.
	void SayGoodbye(b2Joint* joint){};

	/// Called when any fixture is about to be destroyed due
	/// to the destruction of its parent body.
	void SayGoodbye(b2Fixture* fixture){};

	void ProcessContacts();

/*=============================================================================
				Public input methods
=============================================================================*/
	bool KeyPressed(const OIS::KeyEvent &keyEventRef);
	bool KeyReleased(const OIS::KeyEvent &keyEventRef);

	bool MouseMoved(const OIS::MouseEvent &arg);
	bool MousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 
	bool MouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

protected:

	void MoveCamera();
	void LoadLevel();

	void UpdateOverlay();

	Ogre::String levelName_;
	Ogre::String fileName_;

	b2World*				world_;
	Ogre::SceneManager*		sceneManager_;

	GameCamera*				gameCamera_;
	Ogre::Camera*			camera_;
	
	KeyHandler*				myKeyHandler_;

	GameObjectFactory*		gameObjectFactory_;

	OgreB2DebugDraw*		debugDraw_;

	ContactList		beginContactList_;
	ContactList		endContactList_;
	PostSolveList	postSolveList_;

	Ogre::Overlay* timeLeftOverlay_;
	Ogre::Overlay* pickupOverlay_;

	double time_;
	int seconds_;
	int minutes_;

/*=============================================================================
				Character stuff
=============================================================================*/

	CharacterParker*	activeCharacter_;

	CharacterParker*	holt_;
	CharacterParker*	parker_;

	MousePicking*		mousePicking_;
	

};

#endif
