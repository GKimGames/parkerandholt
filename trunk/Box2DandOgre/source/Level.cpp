#include "Level.h"

#include "PhysicsState.h"
#include "GameObjectFactory.h"
#include "TinyXMLHelper.h"
#include "CameraStateWatch.h"
#include "CameraStateGoToPoint.h"
#include "Teleporter.h"
#include "Parker.h"

using namespace Ogre;

//=============================================================================
//							Constructor::Level
//
Level::Level(Ogre::String fileName): fileName_(fileName)
{	
	fileName_ = "..\\Levels\\" + fileName_;

	TiXmlDocument* configXML_ = 0;
	TiXmlHandle* handle = TinyXMLHelper::GetRootFromFile(fileName_,configXML_);
	TiXmlElement* infoElement = handle->FirstChildElement("LevelInfo").ToElement();
	levelName_ = TinyXMLHelper::GetAttribute(infoElement, "name");
	Ogre::Vector3 camPosition = TinyXMLHelper::GetAttributeVector3(infoElement, "camPosition");
	TiXmlElement* parkerElement = handle->FirstChildElement("Parker").ToElement();

	delete handle;
	delete configXML_;

	sceneManager_ = GAMEFRAMEWORK->root_->createSceneManager(ST_GENERIC, levelName_ + "/Manager");
	GAMEFRAMEWORK->sceneManager = sceneManager_;

	sceneManager_->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));		

	camera_ = sceneManager_->createCamera(levelName_ + "/Camera");
	//camera_->setPosition(camPosition);
	//camera_->lookAt(camLook);
	camera_->setNearClipDistance(1);
	
	camera_->setAspectRatio(Real(GAMEFRAMEWORK->viewport_->getActualWidth()) / 
		Real(GAMEFRAMEWORK->viewport_->getActualHeight()));

	GAMEFRAMEWORK->camera_ = camera_;
	GAMEFRAMEWORK->viewport_->setCamera(camera_);

	//gameCamera_ = new GameCamera(camera_);
	//GAMEFRAMEWORK->gameCamera_ = gameCamera_;
	
	CameraStateWatchDef def;
	def.initialPosition = camPosition;
	def.targetObject = parker_;

	gameCamera_->InitializeDef(&def);

	time_ = minutes_ = seconds_ = 0;

	// Drawing the debug overlay, it shows FPS stats.
	timeLeftOverlay_ = OverlayManager::getSingleton().getByName("PaH/Timer");
	timeLeftOverlay_->show();

	pickupOverlay_ = OverlayManager::getSingleton().getByName("PaH/Pickups");
	pickupOverlay_->show();

}


//=============================================================================
//							LoadLevel
//
void Level::LoadLevel()
{

//	GAMEFRAMEWORK->viewport_->setBackgroundColour(Ogre::ColourValue(0.231, 0.631, 1.0));
//	sceneManager_->setAmbientLight(ColourValue(0.7, 0.7, 0.7));
//	sceneManager_->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
//
//	b2Vec2 gravity(gravity_);
//
//	// Do we want to let bodies sleep?
//	bool doSleep = true;
//
//	// Construct a world object, which will hold and simulate the rigid bodies.
//	world = new b2World(gravity,doSleep);
//	GAMEFRAMEWORK->SetWorld(world);
//	world->SetContactListener(this);
//
//	GameObjectFactory* gof = new GameObjectFactory();
//	gof->LoadFile(fileName_);
//
//	TiXmlDocument* configXML_ = 0;
//	TiXmlHandle* handle = TinyXMLHelper::GetRootFromFile(fileName_,configXML_);
//	
//	TiXmlElement* element = handle->FirstChildElement("LevelInfo").ToElement();
//	
//	delete handle;
//	delete configXML_;
//	
//	holtInfo_ = new PlayerInfo();
//	parkerInfo_ = new PlayerInfo();
//
//	myMouse_ = new MousePicking(sceneManager_, camera_);
//
//	parker_  = new CharacterParker(sceneManager_, myMouse_, parkerInfo_, camera_);
//	parker_->Initialize();
//	parker_->InitializeDebugDraw();
//	parker_->SetBodyPosition(b2Vec2(0, 1.3));
//
//	activeCharacter_ = parker_;
//
//	holt_ = new CharacterParker(sceneManager_, myMouse_, holtInfo_, camera_, false);
//	holt_->Initialize();
//	holt_->GetBody()->SetTransform(b2Vec2(4, 1), 0);
//
//
//	//new MovingPlatform(sceneManager_, b2Vec2(10.0f, 1.0f), b2Vec2(0.0f, 1.0f), b2Vec2(5.0f, 1.0f), b2Vec2(0.0f, 5.0f), 2);
//	PressureSwitch* ps = new PressureSwitch(sceneManager_);
//
//	ps->AddToMessageList(parker_->GetId(), CREATE_BOX);
//
//	myKeyHandler_ = new KeyHandler(GameFramework::getSingletonPtr()->keyboard_);
//
//	myKeyHandler_->AddKey(OIS::KC_RIGHT, CHARACTER_MOVE_RIGHT_PLUS, CHARACTER_MOVE_RIGHT_MINUS);
//	myKeyHandler_->AddKey(OIS::KC_LEFT, CHARACTER_MOVE_LEFT_PLUS, CHARACTER_MOVE_LEFT_MINUS);
//	myKeyHandler_->AddKey(OIS::KC_UP, CHARACTER_JUMP_PLUS, CHARACTER_JUMP_MINUS);
//	myKeyHandler_->AddKey(OIS::KC_DOWN, CHARACTER_MOVE_DOWN_PLUS, CHARACTER_MOVE_DOWN_MINUS);
//	myKeyHandler_->AddKey(OIS::KC_D, CHARACTER_MOVE_RIGHT_PLUS, CHARACTER_MOVE_RIGHT_MINUS);
//	myKeyHandler_->AddKey(OIS::KC_A, CHARACTER_MOVE_LEFT_PLUS, CHARACTER_MOVE_LEFT_MINUS);
//	myKeyHandler_->AddKey(OIS::KC_W, CHARACTER_JUMP_PLUS, CHARACTER_JUMP_MINUS);
//	myKeyHandler_->AddKey(OIS::KC_S, CHARACTER_MOVE_DOWN_PLUS, CHARACTER_MOVE_DOWN_MINUS);
//	myKeyHandler_->AddKey(OIS::KC_1, CHARACTER_ENTER_PLATFORMSTATE);
//	myKeyHandler_->AddKey(OIS::KC_2, CHARACTER_ENTER_STATICSTATE);
//	myKeyHandler_->AddKey(OIS::KC_Q, CHARACTER_EXIT_PLACINGSTATE);
//	myKeyHandler_->AddKey(OIS::KC_F, CHARACTER_GRAB_LEDGE);
//	myKeyHandler_->AddKey(OIS::KC_RETURN, CHARACTER_OPEN_DOOR);
//	
//	/*
//	Teleporter* t1 = new Teleporter("Steeeeeeve");
//	Teleporter* t2 = new Teleporter("Steeeeve");
//
//	t1->SetPartner(t2);
//	t2->SetPartner(t1);
//
//	
//	b2BodyDef bdef;
//	bdef.position.Set(12, -1);
//	bdef.angle = 3.14159/2.0;
//
//	b2PolygonShape teleShape;
//	teleShape.SetAsBox(1.1, 0.15, b2Vec2(0.125, 0), (3.14159 / 2.0));
//
//	b2PolygonShape squareBack;
//	squareBack.SetAsBox(1.3, 0.1, b2Vec2(-0.1, 0), (3.14159 / 2.0));
//	
//	b2FixtureDef fdBack;
//	fdBack.shape = &squareBack;
//
//	b2FixtureDef fd;
//	fd.shape = &teleShape;
//	fd.isSensor = true;
//
//	b2Body* b = world->CreateBody(&bdef);
//	b->SetUserData(t1);
//	b->CreateFixture(&fd);
//	b->CreateFixture(&fdBack);
//	t1->SetBody(b);
//
//	bdef.position.Set(-4, 4);
//	bdef.angle = 3.14159 / 4.0;
//	b = world->CreateBody(&bdef);
//	b->SetUserData(t2);
//	b->CreateFixture(&fd);
//	b->CreateFixture(&fdBack);
//	t2->SetBody(b);
//	*/
//
//#if DEBUG_DRAW_ON
//	debugDraw_ = new OgreB2DebugDraw(sceneManager_, "debugDraw", 0);
//	debugDraw_->SetFlags(
//		b2DebugDraw::e_jointBit	
//		| b2DebugDraw::e_shapeBit
//		//| b2DebugDraw::e_aabbBit 
//		);
//
//	world->SetDebugDraw(debugDraw_);
//	GAMEFRAMEWORK->SetDebugDraw(debugDraw_);
//	debugDrawOn_ = false;
//#endif

}

//=============================================================================
//							Level::Destructor
//
Level::~Level()
{

	delete world_;

	GameFramework::getSingletonPtr()->log_->logMessage("Leaving Level");

	sceneManager_->destroyCamera(camera_);

	//GameObject::objectList.clear();

	if(sceneManager_)
	{
		GameFramework::getSingletonPtr()->root_->destroySceneManager(sceneManager_);
	}

	timeLeftOverlay_->hide();
	pickupOverlay_->hide();
}

//=============================================================================
//							Update
//
void Level::Update(double timeSinceLastFrame)
{

}


//=============================================================================
//							ProcessContacts
//
void Level::ProcessContacts()
{
	ContactPoint* contact; 
	ContactList::iterator iter;

	// Process the BeginContact list of b2Contacts
	for(iter = beginContactList_.begin(); iter != beginContactList_.end(); iter++)
	{
		contact = (*iter);

		// Check if fixtureA's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->fixtureA->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->fixtureA->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableA;

			contactableA = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableA)
			{
				contactableA->BeginContact(contact,contact->fixtureA,contact->fixtureB);
			}
		}

		// Check if fixtureB's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->fixtureB->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->fixtureB->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableB;

			contactableB = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableB)
			{
				contactableB->BeginContact(contact,contact->fixtureB,contact->fixtureA);
			}
		}
	}

	// Process the EndContact list of b2Contacts
	for(iter = endContactList_.begin(); iter != endContactList_.end(); iter++)
	{
		contact = (*iter);

		// Check if fixtureA's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->fixtureA->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->fixtureA->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableA;

			contactableA = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableA)
			{
				contactableA->EndContact(contact,contact->fixtureA,contact->fixtureB);
			}
		}

		// Check if fixtureA's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->fixtureB->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->fixtureB->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableB;

			contactableB = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableB)
			{
				contactableB->EndContact(contact,contact->fixtureB,contact->fixtureA);
			}
		}
	}

	for(iter = endContactList_.begin(); iter != endContactList_.end(); iter++)
	{
		delete (*iter);
	}

	for(iter = beginContactList_.begin(); iter != beginContactList_.end(); iter++)
	{
		delete (*iter);
	}

	endContactList_.clear();
	beginContactList_.clear();
}


//=============================================================================
//						ProcessPostSolveData
//
void Level::ProcessPostSolveData()
{
	PostSolveData* psData;
	PostSolveList::iterator iter;

	// Process the PostSolve data
	for(iter = postSolveList_.begin(); iter != postSolveList_.end(); iter++)
	{
		psData = (*iter);
		b2Contact* contact = psData->contact;

		// Check if fixtureA's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->GetFixtureA()->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->GetFixtureA()->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableA;

			contactableA = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableA)
			{
				contactableA->PostSolve(contact,contact->GetFixtureA(),contact->GetFixtureB(),psData->impulse);
			}
		}

		// Check if fixtureB's body has some user data
		// If it does check if the Object responds to contacts
		if(contact->GetFixtureB()->GetBody()->GetUserData() != NULL)
		{
			GameObject* object = (GameObject*) contact->GetFixtureB()->GetBody()->GetUserData();
			GameObjectOgreBox2D* contactableB;

			contactableB = dynamic_cast<GameObjectOgreBox2D*> (object);

			if(contactableB)
			{
				contactableB->PostSolve(contact,contact->GetFixtureB(),contact->GetFixtureA(),psData->impulse);
			}
		}
	}

	for(iter = postSolveList_.begin(); iter != postSolveList_.end(); iter++)
	{
		delete (*iter);
	}

	postSolveList_.clear();
}


//=============================================================================
//							PostSolve
//
void Level::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
		PostSolveData* d = new PostSolveData();

	d->contact = contact;
	d->impulse = new b2ContactImpulse((*impulse));

	postSolveList_.push_back(d);
}

//=============================================================================
//							BeginContact
//
/// Called when two fixtures begin to touch.
/// This calls the BeginContact method of an GameObjectOgreBox2D if the user data 
/// for one the touching fixtures is there. The called BeginContact method passes
/// which fixture was of the GameObjectOgreBox2D and which one it collided with.
void Level::BeginContact(b2Contact* contact)
{
	ContactPoint* c = new ContactPoint();
	c->fixtureA = contact->GetFixtureA();
	c->fixtureB = contact->GetFixtureB();

	// Should probably be making a deep copy of contact.
	c->contact = contact;
	beginContactList_.push_back(c);
}

//=============================================================================
//							EndContact
//
/// Called when two fixtures cease to touch.
/// This calls the EndContact method of an GameObjectOgreBox2D if the user data 
/// for one the touching fixtures is there. The called EndContact method passes
/// which fixture was of the GameObjectOgreBox2D and which one it collided with.
void Level::EndContact(b2Contact* contact)
{
	ContactPoint* c = new ContactPoint();
	c->fixtureA = contact->GetFixtureA();
	c->fixtureB = contact->GetFixtureB();

	// Should probably be making a deep copy of contact.
	c->contact = contact;
	endContactList_.push_back(c);
}

//=============================================================================
//							MoveCamera
//
void Level::MoveCamera()
{

}


//=============================================================================
//							UpdateOverlay
//
void Level::UpdateOverlay()
{
	time_ += GAMEFRAMEWORK->GetTimeSinceLastFrame();

	minutes_ = time_ / 60;
	seconds_ = (int)(time_) % 60;

	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("PaH/Time"); 
	Ogre::String strSeconds;
	Ogre::String strMinutes;

	if(minutes_ < 10)
	{
		strMinutes +="0" + StringConverter::toString(minutes_);
	}
	else
	{
		strMinutes += StringConverter::toString(minutes_);
	}

	if(seconds_ < 10)
	{
		strSeconds +="0" + StringConverter::toString(seconds_);
	}
	else
	{
		strSeconds += StringConverter::toString(seconds_);
	}

	guiCurr->setCaption(strMinutes+":"+strSeconds); 
	guiCurr->setColour(Ogre::ColourValue(seconds_ / 60.0,1,1,1));
}

/*=============================================================================
			Public input methods
=============================================================================*/
bool KeyPressed(const OIS::KeyEvent &keyEventRef){return true;}
bool KeyReleased(const OIS::KeyEvent &keyEventRef){return true;}

bool MouseMoved(const OIS::MouseEvent &arg){return true;}
bool MousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id){return true;} 
bool MouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id){return true;}

