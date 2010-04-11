/*=============================================================================

		GameFramework.cpp

		Author: Matt King

=============================================================================*/

#include "GameFramework.h"
#include "GameObject.h"
#include "AppStateManager.hpp"

using namespace Ogre;

template<> GameFramework* Ogre::Singleton<GameFramework>::ms_Singleton = 0;

//=============================================================================
//							GameFramework
//
GameFramework::GameFramework()
{
	numScreenShots_		= 0;

	root_				= 0;
	renderWindow_		= 0;
	viewport_			= 0;
	log_				= 0;
	timer_				= 0;

	inputManager_		= 0;
	keyboard_			= 0;
	mouse_				= 0;

	debugOverlay_		= 0;
	infoOverlay_		= 0;
}


//=============================================================================
//							GameFramework
//
GameFramework::~GameFramework()
{
	
	if(inputManager_)
	{
		delete keyboard_;
		delete mouse_;
		OIS::InputManager::destroyInputSystem(inputManager_);
	}

	delete root_;
}

//=============================================================================
//							InitOgre
//
/// Initialize Ogre. Shows a load image and loads all the resources. It also
/// sets up OIS listeners.
bool GameFramework::InitOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{
	Ogre::LogManager* logMgr = new Ogre::LogManager();
	
	log_ = Ogre::LogManager::getSingleton().createLog("ParkerAndHoltLog.log", true, true, false);
	log_->setDebugOutputEnabled(true);
	
	root_ = new Ogre::Root();

	//try first to restore an existing config 
	//if (!root_->restoreConfig()) 
	{ 
		if(root_->showConfigDialog())
		{
			// If returned true, user clicked OK so initialise
			// Here we choose to let the system create a default rendering window by passing 'true'
			renderWindow_ = root_->initialise(true, wndTitle);        
			
		}
		else
		{
			return false;
		}

	}

	viewport_ = renderWindow_->addViewport(0);

	// Creating a load image
	// First we have to load the image and material, these are stored in the loadScreen folder
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../media/materials/loadScreen", "FileSystem", "LoadScreen");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("LoadScreen");

	// Create background rectangle covering the whole screen
	Rectangle2D* rect = new Rectangle2D(true);
	rect->setCorners(-0.5, 0.5, 0.5, -0.5);
	rect->setMaterial("Materials/LoadMaterial");

	// Use infinite AAB to always stay visible
	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	rect->setBoundingBox(aabInf);

	Ogre::SceneManager* sceneMgr = root_->createSceneManager(ST_GENERIC, "LoadScreen");
	sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
	SceneNode* node = sceneMgr->getRootSceneNode()->createChildSceneNode("Background");
	node->attachObject(rect);

	camera_ = sceneMgr->createCamera("LoadCamera");
	camera_->setNearClipDistance(1);
	
	camera_->setAspectRatio(Real(viewport_->getActualWidth()) / 
		Real(viewport_->getActualHeight()));

	viewport_->setCamera(camera_);

	// Render a frame so our loadScreen is drawn.
	root_->renderOneFrame();


	// Setup the OIS input stuffs.
	unsigned long hWnd = 0;
    OIS::ParamList paramList;
    renderWindow_->getCustomAttribute("WINDOW", &hWnd);

	paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

	inputManager_ = OIS::InputManager::createInputSystem(paramList);

    keyboard_ = static_cast<OIS::Keyboard*>(inputManager_->createInputObject(OIS::OISKeyboard, true));
	mouse_ = static_cast<OIS::Mouse*>(inputManager_->createInputObject(OIS::OISMouse, true));
    
	mouse_->getMouseState().height = renderWindow_->getHeight();
	mouse_->getMouseState().width  = renderWindow_->getWidth();

	if(pKeyListener == 0)
		keyboard_->setEventCallback(this);
	else
		keyboard_->setEventCallback(pKeyListener);

	if(pMouseListener == 0)
		mouse_->setEventCallback(this);
	else
		mouse_->setEventCallback(pMouseListener);

	// Load the resources.
	Ogre::String secName, typeName, archName;
	Ogre::ConfigFile cf;
    cf.load("resources.cfg");

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	

	timer_ = new Ogre::Timer();
	timer_->reset();


	renderWindow_->setActive(true);
	
	return true;
}

//=============================================================================
//							KeyPressed
//
/// 
bool GameFramework::KeyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(keyboard_->isKeyDown(OIS::KC_SYSRQ))
	{
		std::ostringstream ss;
		ss << "screenshot_" << ++numScreenShots_ << ".png";
		renderWindow_->writeContentsToFile(ss.str());
		return true;
	}

	if(keyboard_->isKeyDown(OIS::KC_O))
	{
		if(debugOverlay_)
		{
			if(!debugOverlay_->isVisible())
				debugOverlay_->show();
			else
				debugOverlay_->hide();
		}
	}

	return true;
}

//=============================================================================
//							KeyReleased
//
///
bool GameFramework::KeyReleased(const OIS::KeyEvent &keyEventRef)
{
	return true;
}


//=============================================================================
//							MouseMoved
//
///
bool GameFramework::MouseMoved(const OIS::MouseEvent &evt)
{
	return true;
}


//=============================================================================
//							MousePressed
//
///
bool GameFramework::MousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}


//=============================================================================
//							MouseReleased
//
///
bool GameFramework::MouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}


//=============================================================================
//							UpdateOgre
//
///
void GameFramework::UpdateOgre(double timeSinceLastFrame)
{
	UpdateStats();
}

//=============================================================================
//							UpdateOgre
//
/// Updates the stats, FPS etc.
void GameFramework::UpdateStats() 
{ 
	static String currFps = "Current FPS: "; 
    static String avgFps = "Average FPS: "; 
    static String bestFps = "Best FPS: "; 
    static String worstFps = "Worst FPS: "; 
    static String tris = "Triangle Count: "; 
    static String batches = "Batch Count: "; 

	const RenderTarget::FrameStats& stats = renderWindow_->getStatistics(); 
	
	/*
    OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps"); 
	Ogre::Real left = guiAvg->getParent()->getLeft();
	Ogre::Real top = guiAvg->getParent()->getTop();
	

   
    OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps"); 
    OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps"); 

	
    guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS)); 
   
    guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS) 
            +" "+StringConverter::toString(stats.bestFrameTime)+" ms"); 
    guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS) 
            +" "+StringConverter::toString(stats.worstFrameTime)+" ms"); 

    OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris"); 
    guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount)); 

	OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches"); 
    guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount)); 

	OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText"); 
	guiDbg->setCaption("");
	*/
} 
