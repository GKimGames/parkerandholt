/*=============================================================================

		AppState.h

		Author: spacegaier
		This was taken from the Advanced Ogre Framework by spacegaier.

		Updated by: Matt King
		
		Included Parker and Holt specific classes

=============================================================================*/

#ifndef APP_STATE_HPP
#define APP_STATE_HPP



#include "GameFramework.h"
#include "Message.h"
#include "GameCamera.h"

class AppState;
class GameObject;


class AppStateListener
{
public:
	AppStateListener(void){};
	virtual ~AppStateListener(void){};

	virtual void manageAppState(Ogre::String stateName, AppState* state) = 0;

	virtual AppState*	findByName(Ogre::String stateName) = 0;
	virtual void		changeAppState(AppState *state) = 0;
	virtual bool		pushAppState(AppState* state) = 0;
	virtual void		popAppState() = 0;
	virtual void		shutdown() = 0;
};



class AppState : public OIS::KeyListener, public OIS::MouseListener
{
public:
	static void create(AppStateListener* parent, const Ogre::String name){};

	void destroy(){delete this;}

	virtual void enter(void) = 0;
	virtual void exit(void) = 0;
	virtual bool pause(void){return false;}
	virtual void resume(void){};
	virtual bool update(double timeSinceLastFrame) = 0;

protected:
	AppState(void){};
	
	AppState*	findByName(Ogre::String stateName){return m_pParent->findByName(stateName);}
	void		changeAppState(AppState* state){m_pParent->changeAppState(state);}
	bool		pushAppState(AppState* state){return m_pParent->pushAppState(state);}
	void		popAppState(void){m_pParent->popAppState();}
	void		shutdown(void){m_pParent->shutdown();}
	
	AppStateListener*		m_pParent;
	GameCamera*				gameCamera_;
	
	Ogre::Camera*			m_pCamera;
	Ogre::SceneManager*		m_pSceneMgr;

	Ogre::Camera*			camera_;
	Ogre::SceneManager*		sceneManager_;

};



#define DECLARE_APPSTATE_CLASS(T)										\
static void create(AppStateListener* parent, const Ogre::String name)	\
{																		\
	T* myAppState = new T();											\
	myAppState->m_pParent = parent;										\
	parent->manageAppState(name, myAppState);							\
}



#endif


