
#include "Character.h"

Character::Character(OGRE3DRenderSystem* renderSystem, NxOgre::Scene * scene)
{
	isKeyUP = false;
	justJumped = false;
	renderSystem_ = renderSystem;
	scene_ = scene;

	TiXmlDocument configXML_( "Z:\\OgreSDK\\bin\\CharacterSettings.xml" );
	configXML_.LoadFile();
	TiXmlHandle hDoc(&configXML_);

	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = hDoc.FirstChildElement().Element();

	if (!pElem) return;

	hRoot = TiXmlHandle(pElem);

	translateX = 20.0;
	translateY = 0.0;
	translateZ = 0.0;

	rotateY = 0;

	TiXmlElement* meshNode = hRoot.FirstChild( "MeshInfo" ).FirstChild().Element();
	meshName_ = meshNode->Attribute("name");
	meshNode->QueryDoubleAttribute("translateX", &translateX);
	meshNode->QueryDoubleAttribute("translateY", &translateY);
	meshNode->QueryDoubleAttribute("translateZ", &translateZ);
	meshNode->QueryDoubleAttribute("rotateY", &rotateY);

	TiXmlElement* sizeNode = hRoot.FirstChild( "SizeInfo" ).FirstChild().Element();

	capsuleRadius_ = 0.0;
	capsuleHeight_ = 0.0;
	double scaleX = 2.0;
	double scaleY = 2.0;
	sizeNode->QueryDoubleAttribute("radius", &capsuleRadius_);
	sizeNode->QueryDoubleAttribute("height", &capsuleHeight_);
	sizeNode->QueryDoubleAttribute("scaleX", &scaleX);
	sizeNode->QueryDoubleAttribute("scaleY", &scaleY);

	// Testing printing debug
	Ogre::String s = Ogre::StringConverter::toString(Ogre::Real(capsuleRadius_));
	DLOG(s.c_str());

	NxOgre::RigidBodyDescription capsuleDesc;
	capsuleDesc.mBodyFlags = NxOgre::Enums::BodyFlags_FreezePositionZ
						   | NxOgre::Enums::BodyFlags_FreezeRotation;


	// This is where we create the material description and the material
	NxOgre::MaterialDescription matDesc;
	matDesc.mRestitution = 0.2;
	matDesc.mDynamicFriction = 2.0;
	capsuleMaterial_ = scene_->createMaterial(matDesc);
	
	// The ShapeBlueprint allows us to set several properties of the shape for the capsule
	// Such as the material.
	NxOgre::ShapeBlueprint* sb = new NxOgre::ShapeBlueprint();
	sb->mMaterial = capsuleMaterial_->getIdentifier();
	capsuleShape_ = new NxOgre::Capsule(capsuleRadius_,capsuleHeight_, sb);

	body_ = renderSystem_->createBody(capsuleShape_, NxOgre::Vec3(0, capsuleHeight_, 0), meshName_,capsuleDesc);
	body_->getEntity()->getParentSceneNode()->scale(scaleX, scaleY, scaleX);
	body_->setContactCallback(this);
	body_->setMass(NxOgre::Real(56));

	body_->setContactReportFlags(NxOgre::Enums::ContactPairFlags_All ||NxOgre::Enums::ContactPairFlags_ContactModifcation);

	NxOgre::Matrix44 m;
	m.identity();
	NxOgre::RigidBodyDescription feetActorDesc;
	//feetActorDesc.mActorFlags |= NxOgre::Enums::ActorFlags_ContactModification
								//|NxOgre::Enums::ActorFlags_DisableCollision;
	feetActorDesc.mBodyFlags |= /*NxOgre::Enums::BodyFlags_DisableGravity */
							 NxOgre::Enums::BodyFlags_FreezeRotation
							 |NxOgre::Enums::BodyFlags_FreezePositionX;
	
	feetActor_ =  scene_->createActor(new NxOgre::Sphere(capsuleRadius_));//,m,feetActorDesc);
	//feetActor_->setGlobalPosition(NxOgre::Vec3(15,capsuleRadius_,0));
	feetActor_->setGlobalPosition(NxOgre::Vec3(50,50,0));
	feetActor_->setContactCallback(this);


	animationState_ = body_->getEntity()->getAnimationState("Walk");
	animationState_->setLoop(true);
	animationState_->setEnabled(true);
	
	runningForce_ = 0;
	maximumVelocity_ = 0;


	// Get the walking and running information
	TiXmlElement* movementNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "WalkingInfo" ).Element();
	movementNode->QueryDoubleAttribute("walkingForce", &walkingForce_);
	movementNode->QueryDoubleAttribute("maximumVelocity", &maximumWalkingVelocity_);

	TiXmlElement* runningNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "RunningInfo" ).Element();
	runningNode->QueryDoubleAttribute("runningForce", &runningForce_);
	runningNode->QueryDoubleAttribute("maximumVelocity", &maximumRunningVelocity_);
	
	
/*
	mKB = renderSystem_->createKinematicBody(new NxOgre::Box(capsuleRadius_, capsuleHeight_, capsuleRadius_), NxOgre::Vec3(-20, 12, 0), "Cube.mesh");
	Ogre::Vector3 mkbSize = mKB->getEntity()->getBoundingBox().getSize();
	mKB->getEntity()->getParentSceneNode()->scale(capsuleRadius_ / mkbSize.x, capsuleHeight_/mkbSize.y, capsuleRadius_ / mkbSize.z);
*/
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

void Character::GetInput(double timeSinceLastFrame)
{
	static NxOgre::Real xPos = 0;

	// The time is passed in milliseconds
	timeSinceLastFrame /= 1000;
	isTouchingSurface_ = true;
	if(isTouchingSurface_)
	{
		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPAD4))
		{
			double maxVel, force;

			if(isRunning_)
			{
				maxVel = maximumRunningVelocity_;
				force = runningForce_;
			}
			// Not running - walking
			else
			{
				maxVel = maximumWalkingVelocity_;
				force = walkingForce_;
			}

			if(body_->getLinearVelocity().x > -maxVel)
			{
				xPos -= 15 * timeSinceLastFrame;
				body_->addForce(NxOgre::Vec3(-runningForce_ * timeSinceLastFrame, 0, 0));

			}

		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPAD6))
		{
			double maxVel, force;

			if(isRunning_)
			{
				maxVel = maximumRunningVelocity_;
				force = runningForce_;
			}
			// Not running - walking
			else
			{
				maxVel = maximumWalkingVelocity_;
				force = walkingForce_;
			}

			if(body_->getLinearVelocity().x < maxVel)
			{
				xPos -= 15 * timeSinceLastFrame;
				body_->addForce(NxOgre::Vec3(runningForce_ * timeSinceLastFrame, 0, 0));

			}
		}
		
		/*
		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPAD8))
		{
			body_->addForce(NxOgre::Vec3(0, runningForce_ * timeSinceLastFrame, 0));
		}
		*/

		if(!OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPAD8))
		{
			justJumped = false;
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPAD8) && !justJumped)
		{
			body_->addForce(NxOgre::Vec3(0, runningForce_ * timeSinceLastFrame * 100, 0));
			justJumped = true;
		}
	}

	

}

bool Character::Update(double timeSinceLastFrame)
{
	static double time = 0;
	time += timeSinceLastFrame;

	double maxVel = 0;
	if(isRunning_)
	{
		maxVel = maximumRunningVelocity_;
	}
	// Not running - walking
	else
	{
		maxVel = maximumWalkingVelocity_;
	}
	animationState_->addTime(timeSinceLastFrame/1000 * fabs(body_->getLinearVelocity().x) / maxVel);


	static float rotation = 0;

	if(body_->getLinearVelocity().x > 0.1)
	{
		rotation = -rotateY;
	}
	else if(body_->getLinearVelocity().x < -0.1)
	{
		rotation = rotateY;
	}

	body_->getEntity()->getParentSceneNode()->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(rotation / 180 * 3.14159));
	body_->getEntity()->getParentSceneNode()->translate(translateX,translateY,translateZ, Ogre::SceneNode::TS_LOCAL);
	
	NxOgre::Vec3 v = body_->getGlobalPosition();
	//feetActor_->setGlobalPosition(NxOgre::Vec3(v.x,v.y-capsuleHeight_/2 -1,v.z));
	

	return true;
}


void Character::OnContact(const NxOgre::ContactPair& cPair)
{	
	NxOgre::Actor* actor = static_cast<NxOgre::Actor*>(cPair.mFirst);
	NxOgre::Actor* actor2 = static_cast<NxOgre::Actor*>(cPair.mSecond);
	
	//if(actor == feetActor_ || actor2 == feetActor_)
	{
		isTouchingSurface_ = true;
	}
}
