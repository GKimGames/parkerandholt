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

	PressureSwitch(Ogre::SceneManager* sceneManager)
	{
		sceneManager_ = sceneManager;

		// This is the static body that anchors
		// the moving pressure plate
		b2BodyDef bd;
		staticBody_ = world_->CreateBody(&bd);

		b2PolygonShape shape1;
		shape1.SetAsEdge(b2Vec2(-0.5,0),b2Vec2(0.5,0));
		staticBody_->CreateFixture(&shape1);

		b2PolygonShape shape;
		
		shape.m_vertexCount = 4;
		b2Vec2 verts[4];
		verts[0].Set(-1.0/2.0 - 0.3, -0.4/2.0);
		verts[1].Set(1.0/2.0 + 0.3, -0.4/2.0);
		verts[2].Set(1.0/2.0,  0.4/2.0);
		verts[3].Set(-1.0/2.0,  0.4/2.0);
		shape.Set(verts, 4);

		bd.type = b2_dynamicBody;
		bd.position.Set(0.0f, 0.3f);
		body_ = world_->CreateBody(&bd);
		b2FixtureDef fd;
		fd.shape = &shape;
		fd.filter.groupIndex = 0;
		body_->CreateFixture(&shape, 5.0f);

		b2PrismaticJointDef pjd;

		pjd.Initialize(staticBody_, body_, b2Vec2(0.0f, 5.0f), b2Vec2(0.0f, 1.0f));

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