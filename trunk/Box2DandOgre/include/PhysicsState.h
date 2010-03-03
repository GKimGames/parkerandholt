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

#include "GameObjectOgreBox2D.h"

#include "AppState.hpp"
#include "GameState.hpp"

#include "GameFramework.h"
#include "MessageDispatcher.h"
#include "PressureSwitch.h"


#include "MyGUI.h"

#include "MousePicking.h"
#include "GameCamera.h"
#include "CheckPoint.h"
#include "TraumaMeter.h"
#include "PlayerInfo.h"
#include "PickUp.h"

#define DEBUG_DRAW_ON 1
class CharacterParker;

class PhysicsState : public AppState, public b2ContactListener, public b2DestructionListener
{

typedef std::vector<ContactPoint*> ContactList;
typedef std::vector<PostSolveData*> PostSolveList;

public:

	PhysicsState();
	
	DECLARE_APPSTATE_CLASS(PhysicsState)

	void enter();
	void createScene();
	void createPhysics();
	void exit();
	bool pause();
	void resume();

	void moveCamera();
	void getInput();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);


	bool update(double timeSinceLastFrame);

	void setBufferedMode();
	void setUnbufferedMode();

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	void ProcessPostSolveData();

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

	void ProcessContacts();

	void CreateBox();

	void MakeExit(MyGUI::WidgetPtr _widget)
	{
		m_bQuit = true;
	}

protected:

	/// My GUI Stuff
	MyGUI::Gui* myGUI;
	// End

	GameObject* gameObject_;
	CharacterParker* parker_;

	b2World* world;
	double timeStep; 

	Character* myCharacter_;
	TraumaMeter* myMeter_;
	float testing_;


#ifdef DEBUG_DRAW_ON
	OgreB2DebugDraw* debugDraw_;
	bool			 debugDrawOn_;
#endif

private:

	bool						m_bQuit;
	bool						pause_;
	Ogre::Vector3				m_TranslateVector;
	Ogre::Real					m_MoveSpeed; 
	Ogre::Degree				m_RotateSpeed; 
	float						m_MoveScale; 
	Ogre::Degree				m_RotScale;

	Ogre::RaySceneQuery*		m_pRSQ;
	Ogre::SceneNode*			m_pCurrentObject;
	Ogre::Entity*				m_pCurrentEntity;
	bool						m_bLMouseDown, m_bRMouseDown;
	bool						m_bChatMode;

	MousePicking*				myMouse_;
	KeyHandler*					myKeyHandler_;
	PlayerInfo*					playerInfo_;
	
	b2Vec2						gravity_;
	
	int							curvature_;

	ContactList beginContactList_;
	ContactList endContactList_;
	PostSolveList postSolveList_;

	//ContactPoint m_points[k_maxContactPoints];
	int32 m_pointCount;

	GameCamera*		gameCamera_;
	Ogre::Vector3	camPosition;

};


#endif