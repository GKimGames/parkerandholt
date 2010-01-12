#include "PressureSwitch.h"


//=============================================================================
//							PressureSwitch::Constructor
//
PressureSwitch::PressureSwitch(Ogre::SceneManager* sceneManager,
							   b2Body* body, Ogre::Entity* entity)
{
	sceneManager_ = sceneManager;

	minimumTime_ = .075; // seconds
	timeAccum_ = 0;
	hitBegun_ = false;
	hitEnd_ = false;

	// This is the static body that anchors
	// the moving pressure plate
	b2BodyDef bd;
	bd.type = b2_staticBody;
	staticBody_ = world_->CreateBody(&bd);

	b2PolygonShape shape1;
	shape1.SetAsEdge(b2Vec2(-0.5,-.4),b2Vec2(0.5,-.4));
	hitFixture_ = staticBody_->CreateFixture(&shape1);
	staticBody_->SetUserData(this);

	// Create the moving part of the switch
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
	fd.restitution = 0.1221f;

	movingFixture_ = body_->CreateFixture(&fd);
	body_->SetUserData(0);

	// Create the joint for this switch.
	b2PrismaticJointDef pjd;
	pjd.Initialize(staticBody_, body_, // Bodies
		b2Vec2(0.0f, 5.0f), // Anchor Point
		b2Vec2(0.0f, 1.0f)  // Axis of movement
		);

	pjd.motorSpeed = 10.0f;
	pjd.maxMotorForce = body_->GetMass() * world_->GetGravity().y * 2;
	//pjd.maxMotorForce = 600.0f;
	pjd.enableMotor = true;
	pjd.lowerTranslation = -1.0f;
	pjd.upperTranslation = 0.0f;
	pjd.enableLimit = true;
	pjd.collideConnected = true;

	world_->CreateJoint(&pjd);


	messageList_.insert(
		std::make_pair<GameObjectId, SensorMessage>(
		SEND_TO_ALL,SensorMessage(STUPID_MESSAGE,STUPID_MESSAGE))
		);

	messageList_.insert(
		std::make_pair<GameObjectId, SensorMessage>(
		SEND_TO_ALL,SensorMessage(CHARACTER_JUMP,CHARACTER_JUMP))
		);

}

//=============================================================================
//							BeginContact
//
/// Called when two fixtures begin to touch.
/// Contact fixture is the fixture in this Object's body.
/// Collided fixture is the fixture that hit this Object's body.
void PressureSwitch::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(collidedFixture == movingFixture_)
	{
		hitBegun_ = true;
		timeAccum_ = 0;

		// This helps prevent bouncing
		body_->SetLinearVelocity(b2Vec2(0,0));
		
	}
}

//=============================================================================
//							EndContact
//
/// Called when two fixtures cease to touch.
/// Contact fixture is the fixture in this Object's body.
/// Collided fixture is the fixture that hit this Object's body.
void PressureSwitch::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(collidedFixture == movingFixture_)
	{
		if(hitBegun_ == false)
		{
			if(timeAccum_ > minimumTime_)
			{
				std::string* s = new std::string("assss");
				SendOffMessageToList(s);
			}
			else
			{
				timeAccum_ = 0;
				hitBegun_ = false;
			}
		}
		else
		{
			hitBegun_ = false;
			timeAccum_ = 0;
		}
	}
}

//=============================================================================
//								Update
//
bool PressureSwitch::Update(double timeSinceLastFrame)
{ 
	if(hitBegun_)
	{
		timeAccum_ += timeSinceLastFrame;
		
		if(timeAccum_ > minimumTime_)
		{
			std::string* s = new std::string("assss");
			SendOnMessageToList(s);
			hitBegun_ = false;
		}
	}
	return true;
};