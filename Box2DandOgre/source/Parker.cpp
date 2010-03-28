/*=============================================================================

		Parker.cpp

		Author: Matt King

==============================================================================*/

#include "Parker.h"
#include "GameFramework.h"
#include "GameObjectFactory.h"

#include "ParkerStateOnGround.h"
#include "ParkerStateInAir.h"
#include "ParkerStateLedgeGrab.h"
#include "HoltStatePlacingPlatform.h"
#include "HoltStatePlacingGravityVector.h"
#include "HoltStatePlacingStatic.h"

#include "CameraStateWatch.h"
#include "CameraStateGoToPoint.h"

#include "TinyXMLHelper.h"

#include "Door.h"

//=============================================================================
//								Constructor
//
CharacterParker::CharacterParker(Ogre::SceneManager* sceneManager, MousePicking* mousePicking, PlayerInfo* info)
:Character(sceneManager)
{
	mousePicking_ = mousePicking;
	parkerId_ = objectId_;

	stateMachine_ = new FSMStateMachine<CharacterParker>(this);

	onGroundState_ = new ParkerStateOnGround(this, stateMachine_);
	inAirState_ = new ParkerStateInAir(this, stateMachine_);
	ledgeGrabState_ = new ParkerStateLedgeGrab(this, stateMachine_);
	placingPlatform_ = new HoltStatePlacingPlatform(this, stateMachine_);
	placingGravityVector_ = new HoltStatePlacingGravityVector(this, stateMachine_);


	stateMachine_->SetCurrentState(onGroundState_);
	stateMachine_->ChangeState(onGroundState_);

	elevator_ = NULL;

	traumaMeter_ = new TraumaMeter();
	objectName_ = "Parker";
	playerInfo_ = info;

	active_ = true;
}


//=============================================================================
//								Constructor
//
CharacterParker::CharacterParker(Ogre::SceneManager* sceneManager, MousePicking* mousePicking, PlayerInfo* info, bool holtActive)
:Character(sceneManager)
{
	mousePicking_ = mousePicking;
	holtId_ = objectId_;

	stateMachine_ = new FSMStateMachine<CharacterParker>(this);

	onGroundState_ = new ParkerStateOnGround(this, stateMachine_);
	inAirState_ = new ParkerStateInAir(this, stateMachine_);
	ledgeGrabState_ = new ParkerStateLedgeGrab(this, stateMachine_);
	placingPlatform_ = new HoltStatePlacingPlatform(this, stateMachine_);
	placingGravityVector_ = new HoltStatePlacingGravityVector(this, stateMachine_);
	placingStatic_ = new HoltStatePlacingStatic(this, stateMachine_);

	stateMachine_->SetCurrentState(onGroundState_);
	stateMachine_->ChangeState(onGroundState_);

	elevator_ = NULL;

	traumaMeter_ = new TraumaMeter();
	objectName_ = "Holt";
	playerInfo_ = info;

	active_ = holtActive;
}



//=============================================================================
//								Initialize
//
bool CharacterParker::Initialize()
{
	InitVariables();

	if(!ReadXMLConfig())
		return false;

	CreateGraphics();

	CreatePhysics();

	animationBlender_ = new AnimationBlender(entity_);
	animationBlender_->Initialize("idle");

	initialized_ = true;

	return true;
}

//=============================================================================
//								InitVariables
//
/// Initialize some variables.
void CharacterParker::InitVariables()
{

	gameObjectType_ = GOType_Character_Parker;

	justJumped = false;
	justWallJumped_ = false;

	// Set up the sensors hit counts
	canJump_ = 0;
	shinSensorHitCount_ = 0;
	thighSensorHitCount_ = 0;
	torsoSensorHitCount_ = 0;
	feetSensorHitCount_ = 0;

	timeBetweenJump_ = .08;

	moveLeft_ = false;
	moveRight_ = false;
	jump_ = false;
}

//=============================================================================
//								CreatePhysics
//
/// Create the Box2D representation of Parker.
void CharacterParker::CreatePhysics()
{
	b2BodyDef bd;
	bd.position.Set(-5, 5);
	bd.fixedRotation = true;
	bd.bullet = true;
	bd.type = b2_dynamicBody;
	body_ = world_->CreateBody(&bd);

	b2PolygonShape bodyShapeDef;

	// The extents are the half-widths of the box.

	b2Vec2 bodyVecs[4];
	bodyShapeDef.SetAsBox(boundingBoxWidth_/2, boundingBoxHeight_/2.3, b2Vec2(0,.1),0);

	b2FixtureDef fd;
	fd.shape = &bodyShapeDef;
	// This effectively makes the mass of the body the correct amount
	// by setting the density to the appropriate amount.
	fd.density = (boundingBoxWidth_ * boundingBoxHeight_) * mass_;
	fd.friction = DEFAULT_FRICTION;
	fd.restitution = restitution_;
	fd.filter.maskBits = 0x0001;
	fd.filter.categoryBits = 0x0100;

	body_->CreateFixture(&fd);

	b2CircleShape circleShape;
	circleShape.m_radius = boundingBoxWidth_/2 + 0.05;
	circleShape.m_p = b2Vec2(0,-boundingBoxHeight_/2 + boundingBoxWidth_/2);
	fd.shape = &circleShape;
	fd.density = (boundingBoxWidth_ * boundingBoxHeight_) * mass_;
	feetCircle_ = body_->CreateFixture(&fd);

	//body_->SetLinearDamping(linearDamping_);


	// Create the sensor for the feet
	b2PolygonShape feetSensor_Def;
	feetSensor_Def.m_vertexCount = 4;
	feetSensor_Def.m_vertices[0].Set(-boundingBoxWidth_/2 + 0.05, -boundingBoxHeight_/2 + 0.1);
	feetSensor_Def.m_vertices[1].Set(-boundingBoxWidth_/2 + 0.05, -boundingBoxHeight_/2 - 0.1);
	feetSensor_Def.m_vertices[2].Set(boundingBoxWidth_/2  - 0.05,  -boundingBoxHeight_/2 - 0.1);
	feetSensor_Def.m_vertices[3].Set(boundingBoxWidth_/2  - 0.05,  -boundingBoxHeight_/2 + 0.1);

	fd.shape = &feetSensor_Def;
	fd.isSensor = true;
	fd.userData = &gameObjectType_;

	feetSensor_ = body_->CreateFixture(&fd);


	// Create the definition of the polygon for the shin sensor
	b2PolygonShape shinSensorRight_Def;

	shinSensorRight_Def.m_vertexCount = 4;
	shinSensorRight_Def.SetAsBox(0.1,boundingBoxHeight_/6,b2Vec2(boundingBoxWidth_/2 + 0.11, -boundingBoxHeight_/6),0); 

	fd.shape = &shinSensorRight_Def;
	fd.isSensor = true;
	fd.userData = &gameObjectType_;

	shinSensorRight_ = body_->CreateFixture(&fd);

	// Create the definition of the polygon for the torso sensor
	b2PolygonShape torsoSensorRight_Def;
	torsoSensorRight_Def.m_vertexCount = 4;
	torsoSensorRight_Def.SetAsBox(0.1,boundingBoxHeight_/6,b2Vec2(boundingBoxWidth_/2 + .11, boundingBoxHeight_/2 - boundingBoxHeight_/6),0); 

	fd.shape = &torsoSensorRight_Def;
	fd.userData = &gameObjectType_;
	torsoSensorRight_ = body_->CreateFixture(&fd);

	// Create the definition of the polygon for the shin sensor
	b2PolygonShape shinSensorLeft_Def;

	shinSensorLeft_Def.m_vertexCount = 4;
	shinSensorLeft_Def.SetAsBox(0.1,boundingBoxHeight_/6,b2Vec2(-boundingBoxWidth_/2 - 0.11, -boundingBoxHeight_/6),0); 

	fd.shape = &shinSensorLeft_Def;
	fd.isSensor = true;
	fd.userData = &gameObjectType_;

	shinSensorLeft_ = body_->CreateFixture(&fd);

	// Create the definition of the polygon for the torso sensor
	b2PolygonShape torsoSensorLeft_Def;
	torsoSensorLeft_Def.m_vertexCount = 4;
	torsoSensorLeft_Def.SetAsBox(0.1,boundingBoxHeight_/6,b2Vec2(-boundingBoxWidth_/2 - .11, boundingBoxHeight_/2 - boundingBoxHeight_/6),0); 

	fd.shape = &torsoSensorLeft_Def;
	fd.userData = &gameObjectType_;
	torsoSensorLeft_ = body_->CreateFixture(&fd);

	body_->SetUserData(this); 

}



//=============================================================================
//								CreateGraphics
//
/// Create the OGRE stuff for Parker.
void CharacterParker::CreateGraphics()
{
	// Set up the Graphics part of Parker
	entity_ = sceneManager_->createEntity(objectName_, meshName_);
	animationState_ = entity_->getAnimationState("run");
	animationState_->setLoop(true);
	animationState_->setEnabled(true);


	Ogre::String name;
	name = objectName_;
	name += "Node";


	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingletonPtr()->getByName("cube.1m.mesh");


	// Create a scene node for Parker
	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode(name);
	
	Ogre::String name2;
	name2 = objectName_;
	name2 += "BodyNode";

	// Attaching a seperate node for his mesh
	bodyNode_ = sceneNode_->createChildSceneNode(name2);

	// Attach Parker's mesh to the node
	bodyNode_->attachObject(entity_);

	// Scale the bodyNode to the apppropriate size.
	bodyNode_->scale(scaleX_,scaleY_,scaleZ_);
	bodyNode_->translate(translate_);
	bodyNode_->rotate(Ogre::Vector3::UNIT_Y,Ogre::Degree(rotateY));


	if(GetId() == holtId_)
	{
		entity_->setMaterial(Ogre::MaterialManager::getSingletonPtr()->getByName("CheckPoint/CheckPoint"));
	}
}

//=============================================================================
//								ReadXMLConfig
//
/// Read the XML config file for Parker.
bool CharacterParker::ReadXMLConfig()
{
	TiXmlDocument configXML_( "..\\ParkerSettings.xml" );
	configXML_.LoadFile();
	TiXmlHandle hDoc(&configXML_);

	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = hDoc.FirstChildElement().Element();

	// The XML file didn't load, we can't do anything.
	if (!pElem) 
		return false;

	hRoot = TiXmlHandle(pElem);
	TiXmlElement* meshNode = hRoot.FirstChild( "MeshInfo" ).FirstChild().Element();

	meshName_ = Ogre::String(meshNode->Attribute("name"));
	OgreXMLLoader::GetVector3(meshNode,"translate", &translate_);
	meshNode->QueryDoubleAttribute("rotateY", &rotateY);

	TiXmlElement* sizeNode = hRoot.FirstChild( "ShapeInfo" ).FirstChild( "Size" ).Element();

	sizeNode->QueryDoubleAttribute("width", &boundingBoxWidth_);
	sizeNode->QueryDoubleAttribute("height", &boundingBoxHeight_);
	sizeNode->QueryDoubleAttribute("scaleX", &scaleX_);
	sizeNode->QueryDoubleAttribute("scaleY", &scaleY_);
	sizeNode->QueryDoubleAttribute("scaleZ", &scaleZ_);

	TiXmlElement* shapeDefNode = hRoot.FirstChild( "ShapeInfo" ).FirstChild( "ShapeDef" ).Element();
	shapeDefNode->QueryDoubleAttribute("friction", &friction_);
	shapeDefNode->QueryDoubleAttribute("restitution", &restitution_);
	shapeDefNode->QueryDoubleAttribute("mass", &mass_);
	shapeDefNode->QueryDoubleAttribute("linearDamping", &linearDamping_);

	// Running force and running maximum velocity
	TiXmlElement* runningNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "RunningInfo" ).Element();
	runningNode->QueryDoubleAttribute("runningForce", &runningForce_);
	runningNode->QueryDoubleAttribute("maximumVelocity", &maximumRunningVelocity_);

	// Jumping force and force that can be applied upwards while in the air
	TiXmlElement* jumpingNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "JumpingInfo" ).Element();
	jumpingNode->QueryDoubleAttribute("jumpingForce", &jumpingForce_);
	jumpingNode->QueryDoubleAttribute("jumpingAfterForce", &jumpingAfterForce_);

	// Air movement force and the maximum velocity you can still alter your movement in the air
	TiXmlElement* airNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "AirInfo" ).Element();
	airNode->QueryDoubleAttribute("airForce", &airForce_);
	airNode->QueryDoubleAttribute("maximumVelocity", &maximumAirVelocity_);

	// The force Parker can jump off the wall with
	TiXmlElement* wallJumpNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "WallJumpInfo" ).Element();
	wallJumpForce_ = TinyXMLHelper::GetAttributeb2Vec2(wallJumpNode, "jumpingForce");

	TiXmlElement* loaderElement = hRoot.FirstChildElement( "Bodys" ).Element();
	Box2DXMLLoader* loader = new Box2DXMLLoader(loaderElement,world_);

	//TiXmlElement* mp = hRoot.FirstChild( "MovingPlatform" ).Element();
	//GAMEFRAMEWORK->gameObjectFactory->CreateGameObject(mp);

	/*	
	TiXmlElement* bodys = hRoot.FirstChildElement( "Bodys" ).ToElement();
	bodys = bodys->FirstChildElement();

	int i = 0;
	while(bodys != 0)
	{
	Ogre::String str = "goober_";
	str += Ogre::StringConverter::toString(i);

	b2Body* b = 0;
	b = Box2DXMLLoader::Createb2Body(world_, bodys);
	GameObjectOgreBox2D* goob = new GameObjectOgreBox2D(str,b);
	goob->Initialize();
	goob->InitializeDebugDraw(Ogre::ColourValue(.7,1,.2,1));

	bodys = bodys->NextSiblingElement();

	++i;
	}

	*/
	return true;
}

//=============================================================================
//								CharacterParker
//
/// Applies "friction" to the character if they are on a surface.
/// It applies a force in the opposite direction the character's velocity is
/// going. 
void CharacterParker::ApplyWalkingFriction(double timeSinceLastFrame)
{


	b2Vec2 frictionVector(0,0);
	b2Vec2 lv = body_->GetLinearVelocity();

	if(elevator_ != 0)
	{
		lv -= elevator_->GetLinearVelocity();
	}

	// Set the velocity of the character to 0 if the velocity is less than 0.1.
	if(lv.x < 0.132 && lv.x > -0.132)
	{
		//body_->SetLinearVelocity(b2Vec2(0.0f,body_->GetLinearVelocity().y));
	}
	else
	{
		// Determine which direction the friction should be applied.
		// It has to be in the opposite direction the character is travelling.
		if(lv.x > 0)
		{
			frictionVector.Set(-friction_ * mass_ * -world_->GetGravity().y * timeSinceLastFrame,0);
		}
		else if(lv.x < 0)
		{
			frictionVector.Set(friction_ * mass_ * -world_->GetGravity().y * timeSinceLastFrame,0);
		}

		body_->ApplyForce(frictionVector, body_->GetPosition());
	}

}


//=============================================================================
//								UpdateAnimation
void CharacterParker::UpdateAnimation(double timeSinceLastFrame)
{
	double maxVel = 0;

	if(isRunning_)
	{
		maxVel = maximumRunningVelocity_;
	}
	// Not running, walking
	else
	{
		maxVel = maximumWalkingVelocity_;
	}

	//if(!animationBlender_->complete)
	//animationBlender_->addTime(timeSinceLastFrame / 80);

	//animationState_->addTime(timeSinceLastFrame);
}

bool CharacterParker::HandleMessage(const KGBMessage message)
{ 

	if(GameObjectOgreBox2D::HandleMessage(message))
	{
		return true;
	}
	if(active_)
	{
		if(message.messageType == KGBMessageType::PLAYER_DIED)
		{
			body_->SetActive(true);
			b2Vec2 temp(playerInfo_->GetCheckPoint());
			SetBodyPosition(temp);

			CameraStateWatchDef def;
			def.initialPosition = Ogre::Vector3(0, 6, 18);
			def.targetObject = this;
			GAMEFRAMEWORK->gameCamera_->InitializeDef(&def);			
		}

		else
		{
			if(message.messageType == CREATE_BOX)
			{
				new HoltBox(sceneManager_, b2Vec2(-7, 10), 1, 20);
			}
		}
	}

	return stateMachine_->HandleMessage(message); 
}



bool CharacterParker::Update(double timeSinceLastFrame)
{
	
	static b2Color color(1,1,0);
	//bodyVec1 = body_->GetWorldPoint(b2Vec2(-boundingBoxWidth_/2,	0));
	//bodyVec2 = body_->GetWorldPoint(b2Vec2(boundingBoxWidth_/2,	0));
	//feetVec1 = body_->GetWorldPoint(b2Vec2(-boundingBoxWidth_/2,	-boundingBoxHeight_/2 - .3));
	//feetVec2 = body_->GetWorldPoint(b2Vec2(boundingBoxWidth_/2,	-boundingBoxHeight_/2  - .3));


	/*bodyVec1 = body_->GetWorldPoint(b2Vec2(-boundingBoxWidth_/2 - .1,	-boundingBoxHeight_/2));
	bodyVec2 = body_->GetWorldPoint(b2Vec2(boundingBoxWidth_/2 + .1,	-boundingBoxHeight_/2));
	feetVec1 = body_->GetWorldPoint(b2Vec2(-boundingBoxWidth_/2 - .1,	-boundingBoxHeight_/2 - .1));
	feetVec2 = body_->GetWorldPoint(b2Vec2(boundingBoxWidth_/2 + .1,	-boundingBoxHeight_/2  - .1));

	world_->RayCast(this, bodyVec1, feetVec1);
	rayCastId = 0;
	world_->RayCast(this, bodyVec2, feetVec2);
	rayCastId = 1;

	if(GAMEFRAMEWORK->GetDebugDraw())
	{
		GAMEFRAMEWORK->GetDebugDraw()->DrawSegment(bodyVec1, feetVec1, color);
		GAMEFRAMEWORK->GetDebugDraw()->DrawSegment(bodyVec2, feetVec2, color);
	}*/

	if(GameObjectOgreBox2D::Update(timeSinceLastFrame))
	{
	
	}
	else
	{
		return false;
	}

	feetSensorHit_ = false;
	shinRightHit_ = false;
	shinLeftHit_ = false;
	torsoLeftHit_ = false;
	torsoRightHit_ = false;
	ledge_ = 0;
	sensorLedge_ = 0;

	b2ContactEdge* contacts = body_->GetContactList();
	while(contacts)
	{
		if(contacts->contact->IsTouching())
		{
			b2Fixture* A = contacts->contact->GetFixtureA();
			b2Fixture* B = contacts->contact->GetFixtureB();

			GameObject* goA = (GameObject*) A->GetBody()->GetUserData();
			GameObject* goB = (GameObject*) B->GetBody()->GetUserData();

			GameObject* collidedObject;
			b2Fixture*	collidedFixture;
			b2Fixture*	myFixture;

			if(goA == this)
			{
				collidedObject =	goB;
				collidedFixture =	B;
				myFixture =			A;
			}
			else
			{
				collidedObject =	goA;
				collidedFixture =	A;
				myFixture =			B;
			}

			if(collidedObject)
			{
				if(ledge_ == 0 && collidedObject->GetGameObjectType() == GOType_Sensor)
				{
					GameObjectSensor* sensor = (GameObjectSensor*) collidedObject;
					if(sensor->GetSensorType() == SENSORTYPE_LEDGE)
					{
						if(myFixture == torsoSensorRight_)
						{
							sensorLedge_ = torsoSensorRight_;
							ledge_ = sensor;
						}
						else if(myFixture == torsoSensorLeft_)
						{
							sensorLedge_ = torsoSensorLeft_;
							ledge_ = sensor;
						}
					}
				}
				else if(collidedObject->GetGameObjectType() == GOType_Door)
				{
					door_ = (Door*) collidedObject;
				}
			}

			// Check which sensor is in contact
			if(myFixture == feetSensor_ && !collidedFixture->IsSensor())
			{
				feetSensorHit_ = true;
			}
			else if(myFixture == feetSensor_ && !collidedFixture->IsSensor())
			{
				feetSensorHit_ = true;
			}
			else if(myFixture == shinSensorRight_ && !collidedFixture->IsSensor())
			{
				shinRightHit_ = true;
			}
			else if(myFixture == shinSensorLeft_ && !collidedFixture->IsSensor())
			{
				shinLeftHit_ = true;
			}
			else if(myFixture == torsoSensorRight_ && !collidedFixture->IsSensor())
			{
				torsoRightHit_ = true;
			}
			else if(myFixture == torsoSensorLeft_ && !collidedFixture->IsSensor())
			{
				torsoLeftHit_ = true;
			}
		}

		contacts = contacts->next;
	}

	traumaMeter_->Update();
	if(traumaMeter_->GetTrauma() > .5)
	{
		ReturnToCheckPoint();
	}

	if(body_->GetLinearVelocity().Length() > 40)
	{
		traumaMeter_->AddTrauma(0.01);
	}
	return stateMachine_->Update();
}

float32 CharacterParker::ReportFixture(b2Fixture* fixture, const b2Vec2& point,
									   const b2Vec2& normal, float32 fraction)
{
	
	//static b2Color color(1,0,234.0/255.0);

	//if(rayCastId == 0)
	//{
	//	GAMEFRAMEWORK->GetDebugDraw()->DrawSegment(bodyVec1, point, color);
	//}
	//else
	//{
	//	GAMEFRAMEWORK->GetDebugDraw()->DrawSegment(bodyVec2, point, color);
	//}

	//if(fixture != feetSensor_)
	//{
	//	this->SetBodyPosition(b2Vec2(body_->GetPosition().x, point.y + boundingBoxHeight_/2 + .1 ));

	//	//body_->ApplyForce(b2Vec2(0,abs(body_->GetMass() * world_->GetGravity().y * 2)), body_->GetPosition());
	//	body_->SetLinearVelocity(b2Vec2(body_->GetLinearVelocity().x,0));
	//	b2Vec2 force((body_->GetLinearVelocity().x + 0.01) * body_->GetMass() * world_->GetGravity().x,
	//				 (body_->GetLinearVelocity().y + 0.01) * body_->GetMass() * world_->GetGravity().y);
	//	fixture->GetBody()->ApplyImpulse(force, point);
	//}

	return 0;

}

/// Called when two fixtures begin to touch.
void CharacterParker::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(collidedFixture->IsSensor() == false)
	{
		if(contactFixture == feetSensor_ && collidedFixture != feetCircle_)
		{
			feetSensorHitCount_++;
		}
	}

	stateMachine_->BeginContact(contact,contactFixture, collidedFixture);
}

/// Called when two fixtures cease to touch.
void CharacterParker::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(collidedFixture->IsSensor() == false)
	{
		if(contactFixture == feetSensor_ && collidedFixture != feetCircle_)
		{
			feetSensorHitCount_--;
		}
	}

	stateMachine_->EndContact(contact,contactFixture, collidedFixture);
}

//returns character to the currently active checkpoint
void CharacterParker::ReturnToCheckPoint()
{
	traumaMeter_->ResetTrauma();

	body_->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	body_->SetActive(false);

	CameraStateGoToPointDef def;
	def.target = Ogre::Vector3(playerInfo_->GetCheckPoint().x, playerInfo_->GetCheckPoint().y + 6, 18);
	def.toleranceDistance = 0.1;
	def.factor = 5.0;
	def.initialPosition = GAMEFRAMEWORK->camera_->getPosition();
	def.messageType = KGBMessageType::PLAYER_DIED;
	GAMEFRAMEWORK->gameCamera_->InitializeDef(&def);

	placingPlatform_->GetGravityVector()->RemovePlayer();

}


void CharacterParker::PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse)
{
	if(contact->GetFixtureA()->GetBody()->GetUserData() != NULL)
	{
		GameObject* object = (GameObject*) contact->GetFixtureA()->GetBody()->GetUserData();
		GameObjectOgreBox2D* contactableA;

		contactableA = dynamic_cast<GameObjectOgreBox2D*> (object);
		if(contactableA == this)
		{
			float totalImpulse = abs(impulse->normalImpulses[0]);
			if(totalImpulse > 600 && totalImpulse < 500000)
			{
				float tempTrauma = (totalImpulse-600)/1000;
				traumaMeter_->AddTrauma(tempTrauma*2);
			}
			
		}
	}

	if(contact->GetFixtureB()->GetBody()->GetUserData() != NULL)
	{
		GameObject* object = (GameObject*) contact->GetFixtureB()->GetBody()->GetUserData();
		GameObjectOgreBox2D* contactableB;

		contactableB = dynamic_cast<GameObjectOgreBox2D*> (object);
		if(contactableB == this)
		{
			float totalImpulse = abs(impulse->normalImpulses[0]);
			if(totalImpulse > 600 && totalImpulse < 500000)
			{
				float tempTrauma = (totalImpulse-600)/1000;
				traumaMeter_->AddTrauma(tempTrauma*2);
			}
		}
	}

	stateMachine_->PostSolve(contact,contactFixture, collidedFixture, impulse);

}
//=============================================================================
//				IsFootSensor
//
bool CharacterParker::IsFootSensor(b2Fixture* fixture)
{

	if(fixture == feetSensor_)
	{
		return true;
	}

	return false;
}

void CharacterParker::SetActive(bool active)
{
	traumaMeter_->SetActive(active);
	active_ = active;
}

PlayerInfo* CharacterParker::GetPlayerInfo()
{
	return playerInfo_;
}