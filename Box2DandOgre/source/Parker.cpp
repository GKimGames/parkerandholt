#include "Parker.h"
#include "ParkerStateOnGround.h"
#include "ParkerStateInAir.h"

//=============================================================================
//								Constructor
//
CharacterParker::CharacterParker(Ogre::SceneManager* sceneManager)
:Character(sceneManager)
{
	stateMachine_ = new FSMStateMachine<CharacterParker>(this);

	onGroundState_ = new ParkerStateOnGround(this);
	inAirState_ = new ParkerStateInAir(this);

	stateMachine_->SetCurrentState(onGroundState_);
	stateMachine_->ChangeState(onGroundState_);
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

	timeBetweenJump_ = .25;

}

//=============================================================================
//								CreatePhysics
//
/// Create the Box2D representation of Parker.
void CharacterParker::CreatePhysics()
{
	// Set up the Box2D part of Parker
	b2BodyDef bd;
	bd.position.Set(0, 2);
	bd.fixedRotation = true;
	body_ = world_->CreateBody(&bd);

	b2PolygonDef bodyShapeDef;

	// The extents are the half-widths of the box.
	bodyShapeDef.SetAsBox(boundingBoxWidth_/2, boundingBoxHeight_/2);
	bodyShapeDef.density = 1.013f;
	bodyShapeDef.friction = 0.2;
	bodyShapeDef.restitution = restitution_;

	// Create the sensor for the feet
	b2PolygonDef feetSensor_Def;
	feetSensor_Def.isSensor = true;
	feetSensor_Def.vertexCount = 4;

	feetSensor_Def.vertices[0].Set(-boundingBoxWidth_/2 + 0.05, -boundingBoxHeight_/2 + 0.1);
	feetSensor_Def.vertices[1].Set(-boundingBoxWidth_/2 + 0.05, -boundingBoxHeight_/2 - 0.1);
	feetSensor_Def.vertices[2].Set(boundingBoxWidth_/2  - 0.05,  -boundingBoxHeight_/2 - 0.1);
	feetSensor_Def.vertices[3].Set(boundingBoxWidth_/2  - 0.05,  -boundingBoxHeight_/2 + 0.1);

	feetSensor_Def.userData = &gameObjectType_;

	// Create the definition of the polygon for the wall jump sensor
	b2PolygonDef wallJumpSensor_Def;
	wallJumpSensor_Def.isSensor = true;
	wallJumpSensor_Def.vertexCount = 4;

	wallJumpSensor_Def.vertices[0].Set(boundingBoxWidth_/2 + 0.1, -boundingBoxHeight_/2);
	wallJumpSensor_Def.vertices[1].Set(boundingBoxWidth_/2 + 0.1, boundingBoxHeight_/2);
	wallJumpSensor_Def.vertices[2].Set(boundingBoxWidth_/2 - 0.1, boundingBoxHeight_/2);
	wallJumpSensor_Def.vertices[3].Set(boundingBoxWidth_/2 - 0.1, -boundingBoxHeight_/2);

	wallJumpSensor_Def.userData = &gameObjectType_;

	// Create the definition of the polygon for the shin sensor
	b2PolygonDef shinSensor_Def;
	shinSensor_Def.isSensor = true;	    
	shinSensor_Def.vertexCount = 4;

	shinSensor_Def.SetAsBox(0.1,boundingBoxHeight_/6,b2Vec2(boundingBoxWidth_/2 + 0.1, -boundingBoxHeight_/6),0); 

	shinSensor_Def.userData = &gameObjectType_;

	// Create the definition of the polygon for the thigh sensor
	b2PolygonDef thighSensor_Def;
	thighSensor_Def.isSensor = true;
	thighSensor_Def.vertexCount = 4;

	thighSensor_Def.SetAsBox(0.1,boundingBoxHeight_/16,b2Vec2(boundingBoxWidth_/2 + 0.1, -boundingBoxHeight_/16),0); 
	thighSensor_Def.userData = &gameObjectType_;

	// Create the definition of the polygon for the torso sensor
	b2PolygonDef torsoSensor_Def;
	torsoSensor_Def.isSensor = true;
	torsoSensor_Def.vertexCount = 4;

	torsoSensor_Def.SetAsBox(0.1,boundingBoxHeight_/6,b2Vec2(boundingBoxWidth_/2 + 0.1, boundingBoxHeight_/6),0); 
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

	body_->SetMassFromShapes();

	b2MassData md = body_->GetMassData();
	md.mass = mass_;
	body_->SetMassData(&md);

	body_->SetLinearDamping(linearDamping_);

	world_->Refilter(f);

	body_->SetUserData(this);
}


//=============================================================================
//								CreateGraphics
//
/// Create the OGRE stuff for Parker.
void CharacterParker::CreateGraphics()
{
	// Set up the Graphics part of Parker
	entity_ = sceneManager_->createEntity("Parker", meshName_);
	animationState_ = entity_->getAnimationState("Walk");
	animationState_->setLoop(true);
	animationState_->setEnabled(true);

	// Create a scene node for Parker
	sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode( "ParkerNode" );

	// Attaching a seperate node for his mesh
	bodyNode_ = sceneNode_->createChildSceneNode( "ParkerBodyNode" );

	// Attach Parker's mesh to the node
	bodyNode_->attachObject(entity_);

	// Scale the bodyNode to the apppropriate size.
	bodyNode_->scale(scaleX_,scaleY_,scaleZ_);
	bodyNode_->translate(translateX,translateY,translateZ);
	bodyNode_->rotate(Ogre::Vector3::UNIT_Y,Ogre::Degree(rotateY));
	bodyNode_->setInitialState();
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
	meshName_ = meshNode->Attribute("name");
	meshNode->QueryDoubleAttribute("translateX", &translateX);
	meshNode->QueryDoubleAttribute("translateY", &translateY);
	meshNode->QueryDoubleAttribute("translateZ", &translateZ);
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

	// Get the walking and running information
	TiXmlElement* movementNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "WalkingInfo" ).Element();
	movementNode->QueryDoubleAttribute("walkingForce", &walkingForce_);
	movementNode->QueryDoubleAttribute("maximumVelocity", &maximumWalkingVelocity_);

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
	wallJumpNode->QueryDoubleAttribute("jumpingForce", &wallJumpForce_);

	return true;
}

//=============================================================================
//								CharacterParker
/// Applies "friction" to the character if they are on a surface.
/// It applies a force in the opposite direction the character's velocity is
/// going. 
void CharacterParker::ApplyWalkingFriction(double timeSinceLastFrame)
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

	animationState_->addTime(timeSinceLastFrame);
}