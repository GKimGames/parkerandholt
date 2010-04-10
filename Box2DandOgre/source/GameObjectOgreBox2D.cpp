/*=============================================================================

	GameObjectOgreBox2D.cpp

	Author: Matt King

=============================================================================*/

#include "GameObjectOgreBox2D.h"
#include "GameObjectOgre.h"
#include "GameFramework.h"

//=============================================================================
//								Constructor
//
/// Constructor grabs the reference from the OgreFramework of the b2World.
/// The object is not initialized until Initialize is called and will only
/// be initialized if the body doesn't equal 0.
GameObjectOgreBox2D::GameObjectOgreBox2D(Ogre::String name, b2Body* body, Ogre::Entity* entity)
: GameObjectOgre(name)
{
	// There is only ever one world at a time and all objects get the
	// current world from the GameFramework singleton.
	world_ = GAMEFRAMEWORK->GetWorld();

	entity_ = entity;

	if(entity_)
	{
		sceneNode_->attachObject(entity_);
	}

	body_ = body;

	debugSceneNode_ = sceneNode_;
	
	debugDrawInitialized_ = false;
	debugDrawOn_ = false;
	previousAngle_ = 0;
	wasAwake_ = false;
}

//=============================================================================
//								Destructor
//
/// Tells the b2World to destroy the b2Body associated with this object.
GameObjectOgreBox2D::~GameObjectOgreBox2D()
{
	if(body_ != 0)
	{
		world_->DestroyBody(body_);
	}
}

//=============================================================================
//								Initialize
//
/// Initializes the object if the b2Body exists. When the object is initialized
/// it can update the Ogre graphics with the b2Body parameters.
/// All children of the class must call this Initialize function before 
/// their own. This pattern should extend through all children.
bool GameObjectOgreBox2D::Initialize()
{
	initialized_ = GameObjectOgre::Initialize();
	
	if(initialized_)
	{
		if(body_ != 0)
		{
			
			initialized_ = true;
		}
	}

	return initialized_;
}

//=============================================================================
//								InitializeDebugDraw
//
/// This creates a manual object out of the b2Body's fixtures and shapes so
/// that the DebugDraw doesn't have to every frame.
bool GameObjectOgreBox2D::InitializeDebugDraw(Ogre::ColourValue color, Ogre::String materialName)
{
	
	if(body_ != 0)
	{
		debugMaterialName_ = materialName;
		debugColor_ = color;

		debugDrawInitialized_ = true;
		debugDrawOn_ = true;

		Ogre::String manName = "debug_";
		manName += objectName_;

		manualObject_ = sceneManager_->createManualObject(manName);

		RedrawDebug();

		debugSceneNode_->attachObject(manualObject_);
	}

	return debugDrawInitialized_;
}


//=============================================================================
//								Update
//
/// Updates the DebugDraw if it is enabled and calls it's parent's class
/// update method.
/// All children of the class must call this Update function before their own.
/// This pattern should extend through all children.
bool GameObjectOgreBox2D::Update(double timeSinceLastFrame)
{
	if(GameObjectOgre::Update(timeSinceLastFrame))
	{
		if(debugDrawOn_)
		{
			UpdateDebugDraw();
		}

		return true;
	}
	else
	{
		return false;
	}
}

//=============================================================================
//								UpdateDebugDraw
//
/// Updates the DebugDraw scenenode and mesh.
void GameObjectOgreBox2D::UpdateDebugDraw()
{
	
	// Since rotating a scenenode actually rotates it by an amount
	// and not to a specific value we must store the previous angle
	// that it was rotated by and rotate back by that and then rotate
	// to our new angle.
	if(debugDrawInitialized_)
	{

		/// Change the material color of the debugMesh if the object
		/// has changed whether it is awake or not.
		if(body_->IsAwake() == true && wasAwake_ == false)
		{
			for(int i = 0; i < manualObject_->getNumSections(); ++i)
			{
				manualObject_->setMaterialName(i,debugMaterialName_);
			}
		}
		else if(body_->IsAwake() == false && wasAwake_ == true)
		{
			for(int i = 0; i < manualObject_->getNumSections(); ++i)
			{
				manualObject_->setMaterialName(i,"debugDraw/Sleeping");
			}
		}
		
		b2Vec2 v = body_->GetPosition();		

		debugSceneNode_->setPosition(Ogre::Real(v.x),Ogre::Real(v.y),0);
		debugSceneNode_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(-previousAngle_)); 
		debugSceneNode_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(body_->GetAngle()));

		previousAngle_ = body_->GetAngle();
		wasAwake_ = body_->IsAwake();
	}
	
}


//=============================================================================
//								UpdateDebugDraw
//
/// Sets the scene node to be positioned the same as as the Box2D body this
/// object has. This is called by GameObjectOgre's update function.
bool GameObjectOgreBox2D::UpdateGraphics(double timeSinceLastFrame)
{
	// Since rotating a scenenode actually rotates it by an amount
	// and not to a specific value we must store the previous angle
	// that it was rotated by and rotate back by that and then rotate
	// to our new angle.
	if(initialized_)
	{

		sceneNode_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(-previousAngle_)); 
		
		b2Vec2 v = body_->GetPosition();
		sceneNode_->setPosition(Ogre::Real(v.x),Ogre::Real(v.y),0);

		float32 angle = body_->GetAngle();
		sceneNode_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(angle));

		previousAngle_ = body_->GetAngle();

	}

	return true;
}


//=============================================================================
//								HandleMessage
//
/// Handles a message.
bool GameObjectOgreBox2D::HandleMessage(const KGBMessage message)
{	
	if(GameObjectOgre::HandleMessage(message))
	{
		return true;
	}
	else
	{
		if(message.messageType == SET_POSITION)
		{
			if(message.messageData.empty() == false)
			{	
				Ogre::String typeName(message.messageData.type().name());
				Ogre::Vector3 pos((boost::any_cast<Ogre::Vector3>(message.messageData)));
				SetBodyPosition(b2Vec2(pos.x,pos.y));
			}
		}
	}

	return false;
}

//=============================================================================
//								RedrawDebug
//
/// Updates the manual object of this object if the fixtures in the b2body have
/// changed.
void GameObjectOgreBox2D::RedrawDebug()
{
	if(body_ != 0)
	{
		manualObject_->clear();
		DrawBody(manualObject_, body_, debugMaterialName_, debugColor_);
	}
}