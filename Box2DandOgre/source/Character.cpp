
#include "Character.h"

Character::Character(Ogre::SceneManager* sceneManager, b2World* world)
{

	gameObjectType_ = GOType_Character;
	InitVariables();

	TiXmlDocument configXML_( "..\\CharacterSettings.xml" );
	configXML_.LoadFile();
	TiXmlHandle hDoc(&configXML_);

	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = hDoc.FirstChildElement().Element();

	// The XML file didn't load, we can't do anything.
	if (!pElem) return;

	hRoot = TiXmlHandle(pElem);

	TiXmlElement* meshNode = hRoot.FirstChild( "MeshInfo" ).FirstChild().Element();
	meshName_ = meshNode->Attribute("name");
	meshNode->QueryDoubleAttribute("translateX", &translateX);
	meshNode->QueryDoubleAttribute("translateY", &translateY);
	meshNode->QueryDoubleAttribute("translateZ", &translateZ);
	meshNode->QueryDoubleAttribute("rotateY", &rotateY);

	TiXmlElement* sizeNode = hRoot.FirstChild( "ShapeInfo" ).FirstChild( "Size" ).Element();

	sizeNode->QueryDoubleAttribute("radius", &capsuleRadius_);
	sizeNode->QueryDoubleAttribute("height", &capsuleHeight_);
	sizeNode->QueryDoubleAttribute("scaleX", &scaleX_);
	sizeNode->QueryDoubleAttribute("scaleY", &scaleY_);
	sizeNode->QueryDoubleAttribute("scaleZ", &scaleZ_);

	TiXmlElement* shapeDefNode = hRoot.FirstChild( "ShapeInfo" ).FirstChild( "ShapeDef" ).Element();
	shapeDefNode->QueryDoubleAttribute("friction", &friction_);
	shapeDefNode->QueryDoubleAttribute("restitution", &restitution_);
	shapeDefNode->QueryDoubleAttribute("mass", &mass_);
	shapeDefNode->QueryDoubleAttribute("linearDamping", &linearDamping_);


	// Testing printing debug
	Ogre::String s = Ogre::StringConverter::toString(Ogre::Real(capsuleRadius_));
	DLOG(s.c_str());

	// Get the walking and running information
	TiXmlElement* movementNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "WalkingInfo" ).Element();
	movementNode->QueryDoubleAttribute("walkingForce", &walkingForce_);
	movementNode->QueryDoubleAttribute("maximumVelocity", &maximumWalkingVelocity_);

	TiXmlElement* runningNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "RunningInfo" ).Element();
	runningNode->QueryDoubleAttribute("runningForce", &runningForce_);
	runningNode->QueryDoubleAttribute("maximumVelocity", &maximumRunningVelocity_);

	TiXmlElement* jumpingNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "JumpingInfo" ).Element();
	jumpingNode->QueryDoubleAttribute("jumpingForce", &jumpingForce_);
	jumpingNode->QueryDoubleAttribute("jumpingAfterForce", &jumpingAfterForce_);

	TiXmlElement* airNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "AirInfo" ).Element();
	airNode->QueryDoubleAttribute("airForce", &airForce_);
	airNode->QueryDoubleAttribute("maximumVelocity", &maximumAirVelocity_);

	sceneManager_ = sceneManager;
	world_ = world;

	entity_ = sceneManager_->createEntity("Ninja", meshName_);
	animationState_ = entity_->getAnimationState("Walk");
	animationState_->setLoop(true);
	animationState_->setEnabled(true);

	/*
	animationBlender_ = new AnimationBlender(entity_);
	animationBlender_->init("Walk");
	*/

	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode( "CharacterNode" );
	bodyNode_ = sceneNode_->createChildSceneNode( "CharacterBodyNode" );
	bodyNode_->attachObject(entity_);

	Ogre::Vector3 sizeV = entity_->getBoundingBox().getSize();

	bodyNode_->scale(scaleX_,scaleY_,scaleZ_);
	bodyNode_->translate(translateX,translateY,translateZ);
	bodyNode_->rotate(Ogre::Vector3::UNIT_Y,Ogre::Degree(rotateY));
	bodyNode_->setInitialState();

	b2BodyDef bd;
	bd.position.Set(0, 2);
	bd.fixedRotation = true;
	body_ = world_->CreateBody(&bd);

	b2PolygonDef bodyShapeDef;

	// The extents are the half-widths of the box.
	bodyShapeDef.SetAsBox(capsuleRadius_/2, capsuleHeight_/2);
	bodyShapeDef.density = 1.0f;
	bodyShapeDef.friction = 0.2;
	bodyShapeDef.restitution = restitution_;

	// Create the sensor for the feet
	b2PolygonDef feetSensor_Def;
	feetSensor_Def.isSensor = true;
	feetSensor_Def.vertexCount = 4;

	feetSensor_Def.vertices[0].Set(-capsuleRadius_/2 + 0.05, -capsuleHeight_/2 + 0.1);
	feetSensor_Def.vertices[1].Set(-capsuleRadius_/2 + 0.05, -capsuleHeight_/2 - 0.1);
	feetSensor_Def.vertices[2].Set(capsuleRadius_/2  - 0.05,  -capsuleHeight_/2 - 0.1);
	feetSensor_Def.vertices[3].Set(capsuleRadius_/2  - 0.05,  -capsuleHeight_/2 + 0.1);

	feetSensor_Def.userData = &gameObjectType_;

	// Create the definition of the polygon for the wall jump sensor
	b2PolygonDef wallJumpSensor_Def;
	wallJumpSensor_Def.isSensor = true;
	wallJumpSensor_Def.vertexCount = 4;

	wallJumpSensor_Def.vertices[0].Set(capsuleRadius_/2 + 0.1, -capsuleHeight_/2);
	wallJumpSensor_Def.vertices[1].Set(capsuleRadius_/2 + 0.1, capsuleHeight_/2);
	wallJumpSensor_Def.vertices[2].Set(capsuleRadius_/2 - 0.1, capsuleHeight_/2);
	wallJumpSensor_Def.vertices[3].Set(capsuleRadius_/2 - 0.1, -capsuleHeight_/2);

	wallJumpSensor_Def.userData = &gameObjectType_;

	// Create the definition of the polygon for the shin sensor
	b2PolygonDef shinSensor_Def;
	shinSensor_Def.isSensor = true;	    
	shinSensor_Def.vertexCount = 4;

	shinSensor_Def.SetAsBox(0.1,capsuleHeight_/6,b2Vec2(capsuleRadius_/2 + 0.1, -capsuleHeight_/6),0); 

	shinSensor_Def.userData = &gameObjectType_;

	// Create the definition of the polygon for the thigh sensor
	b2PolygonDef thighSensor_Def;
	thighSensor_Def.isSensor = true;
	thighSensor_Def.vertexCount = 4;

	thighSensor_Def.SetAsBox(0.1,capsuleHeight_/16,b2Vec2(capsuleRadius_/2 + 0.1, -capsuleHeight_/16),0); 
	thighSensor_Def.userData = &gameObjectType_;

	// Create the definition of the polygon for the torso sensor
	b2PolygonDef torsoSensor_Def;
	torsoSensor_Def.isSensor = true;
	torsoSensor_Def.vertexCount = 4;

	torsoSensor_Def.SetAsBox(0.1,capsuleHeight_/6,b2Vec2(capsuleRadius_/2 + 0.1, capsuleHeight_/6),0); 
	torsoSensor_Def.userData = &gameObjectType_;

	b2Fixture* f = body_->CreateFixture(&bodyShapeDef);
	b2FilterData fd;
	//fd.maskBits = 0x0000;
	fd.maskBits = 0x0001;
	fd.categoryBits = 0x0100;
	f->SetFilterData(fd);

	feetSensor_ = body_->CreateFixture(&feetSensor_Def);
	shinSensor_ = body_->CreateFixture(&shinSensor_Def);
	//thighSensor_ = body_->CreateFixture(&thighSensor_Def);
	torsoSensor_ = body_->CreateFixture(&torsoSensor_Def);

	//wallJumpSensor_ = body_->CreateFixture(&wallJumpSensor_Def);



	body_->SetMassFromShapes();

	b2MassData md = body_->GetMassData();
	md.mass = mass_;
	body_->SetMassData(&md);

	body_->SetLinearDamping(linearDamping_);

	world_->Refilter(f);

	body_->SetUserData(this);

}

void Character::InitVariables()
{
	gameObjectType_ = GOType_Character;

	justJumped = false;
	justWallJumped_ = false;

	translateX = 0.0;
	translateY = 0.0;
	translateZ = 0.0;

	scaleX_ = 2.0;
	scaleY_ = 2.0;
	scaleZ_ = 2.0;
	rotateY = 0;

	capsuleRadius_ = 0.0;
	capsuleHeight_ = 0.0;
	jumpingAfterForce_ = 0.0;
	jumpingForce_ = 0.0;
	runningForce_ = 0;
	
	airForce_ = 0;
	maximumAirVelocity_ = 0;

	friction_ = 0;
	restitution_ = 0;
	linearDamping_ = 0;
	mass_ = 0;


	// Set up the sensors hit counts
	canJump_ = 0;
	shinSensorHitCount_ = 0;
	thighSensorHitCount_ = 0;
	torsoSensorHitCount_ = 0;

	//stateMachine_ = new FSMStateMachine<Character>(this);
}

Character::~Character()
{
}

void Character::KeyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(keyEventRef.key == OIS::KC_R)
	{
		isRunning_ = !isRunning_;
	}
}

void Character::KeyReleased(const OIS::KeyEvent &keyEventRef)
{

}


void Character::MoveLeft()
{

	double timeSinceLastFrame = GameFramework::getSingletonPtr()->GetTimeSinceLastFrame() / 1000;

	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_NUMPAD4))
	{

		if(canJump_ > 0)
		{
			if(body_->GetLinearVelocity().x > -maximumRunningVelocity_)
			{
				body_->ApplyForce(b2Vec2(-runningForce_ * timeSinceLastFrame,0), body_->GetPosition());
			}
		}
		else
		{
			if(body_->GetLinearVelocity().x > -maximumAirVelocity_)
			{
				body_->ApplyForce(b2Vec2(-airForce_ * timeSinceLastFrame,0), body_->GetPosition());
			}
		}
	}

}

void Character::MoveRight()
{
	double timeSinceLastFrame = GameFramework::getSingletonPtr()->GetTimeSinceLastFrame() / 1000;

	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_NUMPAD6))
	{

		if(canJump_ > 0)
		{

			if(body_->GetLinearVelocity().x < maximumRunningVelocity_)
			{
				body_->ApplyForce(b2Vec2(runningForce_ * timeSinceLastFrame,0), body_->GetPosition());
			}

		}
		else
		{
			if(body_->GetLinearVelocity().x < maximumAirVelocity_)
			{
				body_->ApplyForce(b2Vec2(airForce_ * timeSinceLastFrame,0), body_->GetPosition());
			}
		}
	}
}

void Character::GetInput(double timeSinceLastFrame)
{
	static double xPos = 0;


	// The time is passed in milliseconds
	//timeSinceLastFrame /= 1000;
	isTouchingSurface_ = true;

	

	if(GameFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_NUMPAD8))
	{
		Jump();
		//animationBlender_->blend("JumpNoHeight",AnimationBlender::BlendWhileAnimating,1, false); 
	}
}

//	Jump
/// Make the character jump
///	The character can only jump if he's on something.
///
/// Calling jump while the character is in the air will add slightly more
/// height to the jump.
void Character::Jump()
{
	// If the character just jumped and jump is called slightly more height
	// is added to the jump.
	if(justJumped)
	{
		double timeLeft = (animationState_->getLength() - animationState_->getTimePosition()) / animationState_->getLength();
		body_->ApplyForce(b2Vec2(0,jumpingAfterForce_ * timeLeft), body_->GetPosition());
	}

	// If the character is on the ground and he didn't just jump he can jump!
	// The character is on the ground if they have any contacts, canJump_ stores
	// the contacts feetSensor_ has.
	if(canJump_ > 0  && !justJumped)
	{
		body_->ApplyImpulse(b2Vec2(0,jumpingForce_), body_->GetPosition());
		justJumped = true;

		animationState_->setEnabled(false);
		animationState_ = entity_->getAnimationState("JumpNoHeight");
		animationState_->setLoop(false);
		animationState_->setEnabled(true);
	}
	else
	{
		if(torsoSensorHitCount_ > 0 && justWallJumped_ == false)
		{
			body_->SetLinearVelocity(b2Vec2(0,body_->GetLinearVelocity().y)); 
			body_->ApplyImpulse(b2Vec2(-jumpingForce_/2,jumpingForce_), body_->GetPosition());
			justWallJumped_ = true;

			animationState_->setEnabled(false);
			animationState_ = entity_->getAnimationState("JumpNoHeight");
			animationState_->setTimePosition(Ogre::Real(0));
			animationState_->setLoop(false);
			animationState_->setEnabled(true);
		}
	}

	
}


/// Applies "friction" to the character if they are on a surface.
/// It applies a force in the opposite direction the character's velocity is
/// going.
void Character::ApplyWalkingFriction(double timeSinceLastFrame)
{

	// If the character is on the ground
	if(canJump_ > 0)
	{
		b2Vec2 frictionVector(0,0);
		b2Vec2 lv = body_->GetLinearVelocity();

		// Set the velocity of the character to 0 if the velocity is less than 0.1.
		if(lv.x < 0.1 && lv.x > -0.1)
		{
			body_->SetLinearVelocity(b2Vec2(0.0f,body_->GetLinearVelocity().y));
		}
		else
		{
			// Determine which way the friction should be applied.
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
}


bool Character::Update(double timeSinceLastFrame)
{
	static double time = 0;
	static Ogre::Vector3 direction;
	static float rotation;
	time += timeSinceLastFrame;

	if(justJumped)
	{
		if(animationState_->hasEnded())
		{
			justJumped = false;
			justWallJumped_ = false;

			animationState_->setTimePosition(0);
			animationState_->setEnabled(false);
			animationState_ = entity_->getAnimationState("Walk");
			animationState_->setLoop(true);
			animationState_->setEnabled(true);
		}
	}

	ApplyWalkingFriction(timeSinceLastFrame);

	b2Vec2 v = body_->GetPosition();
	sceneNode_->setPosition(v.x, v.y,0);

	if(body_->GetLinearVelocity().x > 0.1)
	{
		direction = Ogre::Vector3(0,0,1);
		rotation = 180;
	}
	else if(body_->GetLinearVelocity().x < -0.1)
	{
		direction = Ogre::Vector3(0,0,-1);
		rotation = 0;
	}

	sceneNode_->setDirection(direction,Ogre::Node::TS_WORLD);

	UpdateAnimation(timeSinceLastFrame);

	return true;
}


/*
void Character::ProcessEvent(PaH::GameEvent* gameEvent)
{
	if(gameEvent->GetType() == PaH::GameEventType::Jump)
	{
		if(justJumped)
		{
			double timeLeft = (animationState_->getLength() - animationState_->getTimePosition()) / animationState_->getLength();
			body_->ApplyForce(b2Vec2(0,jumpingAfterForce_ * timeLeft), body_->GetPosition());
		}

		if(canJump_ > 0  && !justJumped)
		{
			body_->ApplyImpulse(b2Vec2(0,jumpingForce_), body_->GetPosition());
			justJumped = true;

			animationState_->setEnabled(false);
			animationState_ = entity_->getAnimationState("JumpNoHeight");
			animationState_->setLoop(false);
			animationState_->setEnabled(true);
		}
	}

	gameEvent->Release();
}
*/

void Character::UpdateAnimation(double timeSinceLastFrame)
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

	animationState_->addTime(timeSinceLastFrame);

}

/// Called when two fixtures begin to touch.
void Character::BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(contactFixture == feetSensor_)
	{
		canJump_++;
	}
	else if(contactFixture == torsoSensor_)
	{
		torsoSensorHitCount_++;
	}
	else if(contactFixture == shinSensor_)
	{
		shinSensorHitCount_++;
	}
	else if(contactFixture == thighSensor_)
	{
		thighSensorHitCount_++;
	}
}

/// Called when two fixtures cease to touch.
void Character::EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
{
	if(contactFixture == feetSensor_)
	{
		canJump_--;
	}
	else if(contactFixture == torsoSensor_)
	{
		torsoSensorHitCount_--;
	}
	else if(contactFixture == shinSensor_)
	{
		shinSensorHitCount_--;
	}
	else if(contactFixture == thighSensor_)
	{
		thighSensorHitCount_--;
	}
}
