//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef MAP_EDITOR_STATE_H
#define MAP_EDITOR_STATE_H

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
#include "DynamicLines.h"

#define DEBUG_DRAW_ON 1


class MapEditorState : public AppState, public b2ContactListener
{

typedef std::vector<ContactPoint*> ContactList;

public:

	MapEditorState();
	
	DECLARE_APPSTATE_CLASS(MapEditorState)

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


	void CreateBox();

protected:
	

	GameObject* gameObject_;

	b2World* world;
	Ogre::Vector3 camPosition;
	Ogre::Vector3 camLook;
	double timeStep; 

	MousePicking* myPicking_;
	Ogre::Plane pickingPlane;

#ifdef DEBUG_DRAW_ON
	OgreB2DebugDraw* debugDraw_;
	bool			 debugDrawOn_;
#endif

private:

	SceneNode*					linesNode_;
	DynamicLines*				dynamicLines_;
	double						linesNodeScale_;
	bool						m_bQuit;
	bool						pause_;
	Ogre::Vector3				m_TranslateVector;
	Ogre::Real					m_MoveSpeed; 
	Ogre::Degree				m_RotateSpeed; 
	float						m_MoveScale; 
	Ogre::Degree				m_RotScale;

	Ogre::RaySceneQuery*		m_pRSQ;
	Ogre::SceneNode*			currentObject_;
	Ogre::Entity*				currentEntity_;
	double						gravity_;

};


#endif