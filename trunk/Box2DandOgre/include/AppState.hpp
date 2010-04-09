/*=============================================================================

		AppState.h

		Author: spacegaier
		This was taken from the Advanced Ogre Framework by spacegaier.

		Updated by: Matt King
		
		+ Included GameObject specific methods.
		+ Added Comments.

=============================================================================*/

#ifndef APP_STATE_HPP
#define APP_STATE_HPP

#include "Message.h"
#include "GameCamera.h"
#include "GameObject.h"
#include "GameFramework.h"

class AppState;
class GameObject;

/// AppStateListener has methods to manage several AppStates. When an AppState
/// is created it needs an AppStateListener so the state can be left and
/// entered correctly.
class AppStateListener
{
public:
	AppStateListener(void){};
	virtual ~AppStateListener(void){};

	/// Adds a new string / state pair to the Manager
	virtual void manageAppState(Ogre::String stateName, AppState* state) = 0;

	/// Returns an AppState given it's name. 
	virtual AppState*	findByName(Ogre::String stateName) = 0;

	/// Changes to a different AppState. The current AppState has nothing done to
	/// it, but it will not be updated anymore as it is notthe top AppState. The 
	/// AppState passed in is pushed onto the stack of AppStates.
	virtual void		changeAppState(AppState *state) = 0;

	/// Pushes an AppState to the top of the stack and pauses the current AppState.
	virtual bool		pushAppState(AppState* state) = 0;

	/// This pops an AppState off the top of the stack. It calls exit on the top
	/// AppState in the stack and then calls Resume on the new top AppState. If
	/// there are no more AppStates after popping the application is shutdown.
	virtual void		popAppState() = 0;

	/// ShutDown the application.
	virtual void		shutdown() = 0;

	/// This is the main loop of the game.
	virtual void		start(AppState* state) = 0;
};


/// AppState
class AppState : public OIS::KeyListener, public OIS::MouseListener
{
public:

	/// Adds this AppState to the AppStateListener.
	static void create(AppStateListener* parent, const Ogre::String name){};

	void destroy(){delete this;}

	/// Entering an AppState should set the GameObject class' objectList to
	/// AppStates GameObjectList.
	virtual void enter(void) = 0;
	virtual void exit(void) = 0;
	virtual bool pause(void){return false;}
	
	/// Resuming an AppState sets the GameObject class' objectList to this
	/// AppStates GameObjectList.
	virtual void resume(void)
	{
		GameObject::objectList = gameObjectList_;
		GameObject::objectNameList = gameObjectNameList_;
	}

	virtual bool update(double timeSinceLastFrame) = 0;

protected:

	AppState(void){};
	
	// These just call the AppStateListener's functions.
	AppState*	findByName(Ogre::String stateName){return m_pParent->findByName(stateName);}
	void		changeAppState(AppState* state){m_pParent->changeAppState(state);}
	bool		pushAppState(AppState* state){return m_pParent->pushAppState(state);}
	void		popAppState(void){m_pParent->popAppState();}
	void		shutdown(void){m_pParent->shutdown();}
	
	/// Delete all the GameObjects associated with this AppState.
	void		deleteObjects()
	{
		
		GameObjectMap::iterator it;

		for(it = gameObjectList_->begin(); it != gameObjectList_->end();)
		{
			delete (*it).second;
			gameObjectList_->erase(it++);
		}

		delete gameObjectList_;

		delete gameObjectNameList_;
	}


	AppStateListener*		m_pParent;
	GameCamera*				gameCamera_;
	
	Ogre::Camera*			m_pCamera;
	Ogre::SceneManager*		m_pSceneMgr;

	Ogre::Camera*			camera_;
	Ogre::SceneManager*		sceneManager_;


	std::map<GameObjectId, GameObject*>* gameObjectList_;
	std::map<Ogre::String, GameObject*>* gameObjectNameList_;

};



#define DECLARE_APPSTATE_CLASS(T)										\
static void create(AppStateListener* parent, const Ogre::String name)	\
{																		\
	T* myAppState = new T();											\
	myAppState->m_pParent = parent;										\
	parent->manageAppState(name, myAppState);							\
}



#endif


