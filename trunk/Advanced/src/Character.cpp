
#include "Character.h"

Character::Character()
{
	isKeyUP = false;
	justJumped = false;

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


	/*
	animationState_->setLoop(true);
	animationState_->setEnabled(true);
	*/

	runningForce_ = 0;
	maximumVelocity_ = 0;


	// Get the walking and running information
	TiXmlElement* movementNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "WalkingInfo" ).Element();
	movementNode->QueryDoubleAttribute("walkingForce", &walkingForce_);
	movementNode->QueryDoubleAttribute("maximumVelocity", &maximumWalkingVelocity_);

	TiXmlElement* runningNode = hRoot.FirstChild( "MovementInfo" ).FirstChildElement( "RunningInfo" ).Element();
	runningNode->QueryDoubleAttribute("runningForce", &runningForce_);
	runningNode->QueryDoubleAttribute("maximumVelocity", &maximumRunningVelocity_);
	
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
	static double xPos = 0;

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

			/*
			if(body_->getLinearVelocity().x > -maxVel)
			{
				xPos -= 15 * timeSinceLastFrame;
			}
			*/

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

			/*
			if(body_->getLinearVelocity().x < maxVel)
			{
				xPos -= 15 * timeSinceLastFrame;
			}
			*/
		}
		

		if(!OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPAD8))
		{
			justJumped = false;
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPAD8) && !justJumped)
		{
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
	//animationState_->addTime(timeSinceLastFrame/1000 * fabs(body_->getLinearVelocity().x) / maxVel);


	static float rotation = 0;
	
	/*
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
	*/
	

	return true;
}
