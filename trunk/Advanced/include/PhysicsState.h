//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef PHYSICS_STATE_H
#define PHYSICS_STATE_H

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"
#include "GameState.hpp"

#include "DotSceneLoader.hpp"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <NxOgre.h>
#include <NxOgreOGRE3D.h>

#include "Character.h"

#define DEBUG_DRAW_ON 1

class PhysicsState : public AppState
{
	
public:

	PhysicsState();
	
	DECLARE_APPSTATE_CLASS(PhysicsState)

	void enter();
	void createScene();
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

	void onLeftPressed(const OIS::MouseEvent &evt);
	bool onExitButtonGame(const CEGUI::EventArgs &args);

	void update(double timeSinceLastFrame);

	void setBufferedMode();
	void setUnbufferedMode();

protected:

	// Physics Stuffs
	NxOgre::World* mWorld;
	NxOgre::Scene* mScene;
	OGRE3DRenderSystem* mRenderSystem;
	NxOgre::TimeController* mTimeController;
		
	Ogre::Vector3 camPosition;
	Ogre::Vector3 camLook;
	static double timeStep;

	OGRE3DBody* mCubeTwo;
	OGRE3DBody* capsule;
	OGRE3DKinematicBody*	     kBody;

	NxOgre::KinematicController* kController;
	NxOgre::KinematicActor* kActor;
	
	Character* myCharacter_;

#ifdef DEBUG_DRAW_ON
	NxOgre::VisualDebugger*	mVisualDebugger;
	OGRE3DRenderable*		mVisualDebuggerRenderable;
	Ogre::SceneNode*		mVisualDebuggerNode;
#endif

	

private:
	Ogre::SceneNode*			m_pOgreHeadNode;
	Ogre::Entity*				m_pOgreHeadEntity;
	Ogre::MaterialPtr			m_pOgreHeadMat;
	Ogre::MaterialPtr			m_pOgreHeadMatHigh;

	bool						m_bQuit;
		
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

	CEGUI::Window*				m_pMainWnd;
	CEGUI::Window*				m_pChatWnd;


};


#endif