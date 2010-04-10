/*=============================================================================

		  GameObjectOgreBox2D.h

		  Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_OGRE_BOX2D_H
#define GAME_OBJECT_OGRE_BOX2D_H

#include <Box2D/Box2D.h>
#include "GameObjectOgre.h"
#include "OgreB2DebugDraw.h"
#include "ManualObjectHelper.h"

/// Structure to represent a contact point.
struct ContactPoint
{
	b2Contact* contact;
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
};

/// Structure to represent PostSolve contact point.
struct PostSolveData
{
	b2Contact* contact;
	const b2ContactImpulse* impulse;

	~PostSolveData()
	{
		delete impulse;
	}

};


/// Structure to represent PreSolve contact point.
struct PreSolveData
{
	b2Contact* contact;
	const b2Manifold* oldManifold;
};


/// The default value of friction for all Box2D objects in the game.
const static double DEFAULT_FRICTION = 0.4;

/// Group of Static map objects, objects in the same group of a negative number
/// will never collide.
const static signed short STATIC_MAP_GROUP = -10;

/// This extends GameObjectOgre and adds a Box2D component.
class GameObjectOgreBox2D  : public GameObjectOgre
{

	friend class GameObjectOgreBox2DCreator;

public:

	/// Constructor grabs the reference from the OgreFramework of the b2World.
	/// The object is not initialized until Initialize is called and will only
	/// be initialized if the body doesn't equal 0.
	GameObjectOgreBox2D(Ogre::String name = "GameObjectOgreBox2D", b2Body* body = 0, Ogre::Entity* entity = 0);

	/// Tells the b2World to destroy the b2Body associated with this object.
	virtual ~GameObjectOgreBox2D();

	/// Updates the DebugDraw if it is enabled and calls it's parent's class
	/// update method.
	virtual bool Update(double timeSinceLastFrame);

	/// Each class that extends this class should call this HandleMessage before
	/// their own in case this class can handle the message.
	/// If you want this class to do something with a message and its children
	/// to something with it as well just return false for that message.
	virtual bool HandleMessage(const KGBMessage message);

	/// Initializes the object if the b2Body exists. When the object is initialized
	/// it can update the Ogre graphics with the b2Body parameters.
	/// All children of the class must call this Initialize function before 
	/// their own. This pattern should extend through all children.
	virtual bool Initialize();

	/// This creates a manual object out of the b2Body's fixtures and shapes so
	/// that the DebugDraw doesn't have to every frame.
	virtual bool InitializeDebugDraw(Ogre::ColourValue color = Ogre::ColourValue(1,1,1,1), Ogre::String materialName = "debugDraw");

	/// Updates the DebugDraw scenenode and mesh.
	virtual void UpdateDebugDraw();

	/// Called when two fixtures begin to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){}

	/// Called when two fixtures cease to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){}

	virtual void PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse){}

	/// You cannot create/destroy Box2D entities inside this callback.
	virtual void PreSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2Manifold* manifold){}

	/// Sets the scene node to be positioned the same as as the Box2D body
	/// this object has.
	virtual bool UpdateGraphics(double timeSinceLastFrame);

	/// Updates the manual object of this object if the fixtures in the b2body have
	/// changed.
	virtual void RedrawDebug();

/*=========================================================================
					Starting Getters and Setters.
=========================================================================*/

	bool GetDebugDrawInitialized(){ return debugDrawInitialized_;}

	void SetDebugDrawOn() 
	{ 
		debugSceneNode_->setVisible(true);
		debugDrawOn_ = true; 
	}

	void SetDebugDrawOff() 
	{ 
		debugSceneNode_->setVisible(false);
		debugDrawOn_ = false; 
	}

	/// Sets the b2Body for this GameObjectOgreBox2D.
	virtual void SetBody(b2Body* b){ body_ = b;}

	/// Returns the b2Body for this GameObjectOgreBox2D.
	virtual b2Body* GetBody(){ return body_; }

	/// Returns the body's position.
	virtual b2Vec2 GetBodyPosition(){ return body_->GetPosition(); }

	void SetBodyPosition(b2Vec2 pos)
	{
		body_->SetTransform(pos, body_->GetAngle());
	}


	void SetBodyAngle(float32 angle)
	{
		body_->SetTransform(body_->GetPosition(), angle);
	}

protected:

	/// Box2D body for this Object
	b2Body* body_;

	/// Reference to the b2World
	b2World* world_;

	float prevAngle;

	/// ManualObject which holds all the shapes for each fixture in the body
    Ogre::ManualObject* manualObject_;

	/// ManualObject which holds all the shapes for each fixture in the body
	bool debugDrawInitialized_;
	bool debugDrawOn_;

    Ogre::SceneNode*	debugSceneNode_;
	Ogre::String	    debugMaterialName_;
	Ogre::ColourValue	debugColor_;

	float32 previousAngle_;
	bool wasAwake_;


};

#endif
