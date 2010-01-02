//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef PHYSICS_STATE_H
#define PHYSICS_STATE_H

//|||||||||||||||||||||||||||||||||||||||||||||||



#include "DotSceneLoader.hpp"

#include <vector>

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <OgreMeshManager.h>
#include <Box2D/Box2D.h>

#include "Character.h"

#include "OgreB2DebugDraw.h"
#include "MousePicking.h"

#include "Platform.h"
#include "MovingPlatform.h"

#include "LedgeSensor.h"
#include "HoltBox.h"

#include "GameObjectOgreBox2D.h"

#include "AppState.hpp"
#include "GameState.hpp"

#include "GameFramework.h"
#include "MessageDispatcher.h"

#include "Parker.h"

#include "PressureSwitch.h"

#define DEBUG_DRAW_ON 1


class PhysicsState : public AppState, public b2ContactListener
{

typedef std::vector<ContactPoint*> ContactList;

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


	void update(double timeSinceLastFrame);

	void setBufferedMode();
	void setUnbufferedMode();

	/// Called when two fixtures begin to touch.
	void BeginContact(b2Contact* contact);

	/// Called when two fixtures cease to touch.
	void EndContact(b2Contact* contact);

	void ProcessContacts();

	void CreateBox();

protected:
	

	GameObject* gameObject_;
	CharacterParker* parker_;

	b2World* world;
	Ogre::Vector3 camPosition;
	Ogre::Vector3 camLook;
	double timeStep; 

	Character* myCharacter_;

	MousePicking* myPicking_;
	Ogre::Plane pickingPlane;

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

	double						gravity_;

	ContactList beginContactList_;
	ContactList endContactList_;

	//ContactPoint m_points[k_maxContactPoints];
	int32 m_pointCount;

};


#endif