/*=============================================================================

		PhysicsState.h

		Author: Matt King

=============================================================================*/

#ifndef PHYSICS_STATE_H
#define PHYSICS_STATE_H

#include "DotSceneLoader.hpp"

#include <vector>

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <OgreMeshManager.h>
#include <Box2D/Box2D.h>

#include "Character.h"

#include "OgreB2DebugDraw.h"
#include "MousePicking.h"
#include "KeyHandler.h"

#include "Platform.h"
#include "MovingPlatform.h"

#include "LedgeSensor.h"
#include "HoltBox.h"
#include "Holt.h"

#include "GameObjectOgreBox2D.h"

#include "AppState.hpp"
#include "GameState.hpp"

#include "GameFramework.h"
#include "MessageDispatcher.h"
#include "PressureSwitch.h"


#include "MyGUI.h"

#include "MousePicking.h"
#include "CheckPoint.h"
#include "TraumaMeter.h"
#include "PlayerInfo.h"
#include "PickUp.h"

#include "Triangle.h"


#define DEBUG_DRAW_ON 1
class CharacterParker;

/// This classes extends AppState and the Box2D interfaces. It creates and ogre scene
/// and a b2World object. This allows for simulating physics with a graphical front end.
class PhysicsState : public AppState, public b2ContactListener, public b2DestructionListener
{

public:

	typedef std::vector<ContactPoint*>	ContactList;
	typedef std::vector<PostSolveData*> PostSolveList;
	typedef std::vector<PreSolveData*>	PreSolveList;

	PhysicsState(Ogre::String levelName);
	

	// Should be moved to the source file, too lazy.
	/// Adds the AppState to the AppStateManager and sets the name of the level PhysicsState should load.
	static void create(AppStateListener* parent, const Ogre::String name, Ogre::String levelName = "..\\LevelTwo.xml")	
	{																		
		PhysicsState* myAppState = new PhysicsState(levelName);											
		myAppState->m_pParent = parent;										
		parent->manageAppState(name, myAppState);							
	}


	/// Enter sets up everything the state needs to render and simulate physics.
	/// Enter creates a new GameObjectList for this state, it's own sceneManager
	/// and camera. It calls createScene.
	void enter();

	/// Create the Ogre scene, this calls createPhysics once it is done.
	void createScene();

	/// Create the b2World and load the level.
	void createPhysics();

	void exit();
	bool pause();
	void resume();

	void moveCamera();

	/// Poll the input devices for the states and do stuff with them. This is done
	/// once per update.
	void getInput();

	/// OIS callback for when a key is pressed.
	bool keyPressed(const OIS::KeyEvent &keyEventRef);

	/// OIS callback for when a key is released.
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	/// OIS callback for when the mouse is moved.
	bool mouseMoved(const OIS::MouseEvent &arg);

	/// OIS callback for when the mouse button is pressed.
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 
	
	/// OIS callback for when the mouse button is released.
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	/// Update
	bool update(double timeSinceLastFrame);


	// ==== Box2D Callbacks

	// These callbacks can't do much while Box2D is being updated so
	// they're buffered and handles afterwards.
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

	/// Called when two fixtures begin to touch.
	void BeginContact(b2Contact* contact);

	/// Called when two fixtures cease to touch.
	void EndContact(b2Contact* contact);

	/// Called when any joint is about to be destroyed due
	/// to the destruction of one of its attached bodies.
	void SayGoodbye(b2Joint* joint);

	/// Called when any fixture is about to be destroyed due
	/// to the destruction of its parent body.
	void SayGoodbye(b2Fixture* fixture);
	// ===== end Box2D Callbacks

	/// Process the buffered PostSolve data. This processes the data of b2Contacts
	/// before they go to Box2D's contact solver.
	void ProcessPostSolveData();

	/// Process the buffered PreSolve data. This processes the data of b2Contacts
	/// before they go to Box2D's contact solver.
	void ProcessPreSolveData();

	/// Process the buffered b2Contacts.
	void ProcessContacts();

	/// This creates a HoltBox, it isn't necessary but it used right now for
	/// a switch.
	void CreateBox();

	void setBufferedMode();
	void setUnbufferedMode();

	Character* activeCharacter_;

protected:

	Ogre::String levelName_;

	/// Update the overlays' values.
	void UpdateOverlay();

	MyGUI::Gui* myGUI;

	// This is to be used a game object that calls methods on this class.
	// Not implemented.
	GameObject* gameObject_;

	CharacterParker* parker_;
	CharacterHolt* holt_;

	b2World* world;
	double timeStep;

	double levelTimeLeft_;

	TraumaMeter* myMeter_;


#ifdef DEBUG_DRAW_ON
	OgreB2DebugDraw* debugDraw_;
	bool			 debugDrawOn_;
#endif

private:

	bool			quit_;
	bool			pause_;

	MousePicking*	myMouse_;
	KeyHandler*		myKeyHandler_;
	PlayerInfo*		parkerInfo_;
	PlayerInfo*		holtInfo_;
	
	b2Vec2			gravity_;

	ContactList		beginContactList_;
	ContactList		endContactList_;
	PostSolveList	postSolveList_;

	PreSolveList	preSolveList_;

	Ogre::Vector3	camPosition;


	Ogre::Overlay* timeLeftOverlay_;
	Ogre::Overlay* pickupOverlay_;

	// Variables for how long the player has been in the level.
	double time_;
	int seconds_;
	int minutes_;
};


#endif