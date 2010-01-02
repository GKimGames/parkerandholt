/*=============================================================================

  Platform.h

  Author: Matt King

=============================================================================*/
#ifndef PRESSURE_SWITCH_H
#define PRESSURE_SWITCH_H

#include <OgreMeshManager.h>
#include <OgreMovablePlane.h>

#include "GameFramework.h"

#include "GameObjectOgreBox2D.h"

class PressureSwitch : public GameObjectOgreBox2D
{

public:

	PressureSwitch(Ogre::SceneManager* sceneManager,
				   b2Body* body = 0, Ogre::Entity* entity = 0)
	{
		sceneManager_ = sceneManager;

		// This is the static body that anchors
		// the moving pressure plate
		b2BodyDef bd;
		bd.type = b2_staticBody;
		staticBody_ = world_->CreateBody(&bd);

		b2PolygonShape shape1;
		shape1.SetAsEdge(b2Vec2(-0.5,0),b2Vec2(0.5,0));
		staticBody_->CreateFixture(&shape1);

		b2PolygonShape shape;
		
		shape.m_vertexCount = 4;
		b2Vec2 verts[4];
		verts[0].Set(-1.0/2.0 - 0.6,	0);
		verts[1].Set(1.0/2.0 + 0.6,		0);
		verts[2].Set(1.0/2.0,			0.4);
		verts[3].Set(-1.0/2.0,			0.4);
		shape.Set(verts, 4);

		bd.active = true;
		bd.type = b2_dynamicBody;
		bd.position.Set(0.0f, 0.0f);
		body_ = world_->CreateBody(&bd);

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.friction = DEFAULT_FRICTION;
		fd.filter.groupIndex = STATIC_MAP_GROUP;
		fd.density = 5.0f;

		body_->CreateFixture(&fd);

		b2PrismaticJointDef pjd;

		pjd.Initialize(staticBody_, body_, // Bodies
					   b2Vec2(0.0f, 5.0f), // Anchor Point
					   b2Vec2(0.0f, 1.0f)  // Axis of movement
					   );

		pjd.motorSpeed = 10.0f;
		pjd.maxMotorForce = 600.0f;
		pjd.enableMotor = true;
		pjd.lowerTranslation = -1.0f;
		pjd.upperTranslation = 0.0f;
		pjd.enableLimit = true;

		world_->CreateJoint(&pjd);


	}


	~PressureSwitch(){};

	bool Update(double timeSinceLastFrame)
	{ 
		return true;
	};



protected:

	b2Body* staticBody_;
	
};

#endif