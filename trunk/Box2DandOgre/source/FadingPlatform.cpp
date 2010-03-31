/*=============================================================================

	FadingPlatform.h

	Author: Matt King

=============================================================================*/

#include "FadingPlatform.h"
#include "Message.h"
#include "Parker.h"

FadingPlatform::FadingPlatform(Ogre::String name,b2Body* body,Ogre::Entity* entity):
GameObjectOgreBox2D(name, body, entity)
{
	fadeSpeed_ = 0.4;
	isFading_ = false;
	fadeTimer_ = 0.0;
	fadeTime_ = 1.0;
	fadeReappearTime_ = 1.5 + fadeTime_;

	gameObjectType_ = GOType_FadingPlatform;

	GameObject::Initialize();

	particleSystem_ = sceneManager_->createParticleSystem(objectName_ + "psystem", "PaH/fadingPlatform");
	//particleSystem_->setVisible(false);
	particleSystem_->getEmitter(0)->setEnabled(false);
	sceneNode_->attachObject(particleSystem_);
}

FadingPlatform::~FadingPlatform()
{

}

bool FadingPlatform::Initialize()
{
	if(body_)
	{
		initialPosition_ = body_->GetPosition();
	}

	fadeReappearTime_ = fadeReappearTime_ + fadeTime_;
	
	return GameObjectOgreBox2D::Initialize();
}

bool FadingPlatform::Update(double timeSinceLastFrame)
{ 
	if(GameObjectOgreBox2D::Update(timeSinceLastFrame))
	{
		if(isFading_ == false)
		{
			b2ContactEdge* contacts = body_->GetContactList();
			while(contacts)
			{
				if(contacts->contact->IsTouching())
				{
					GameObject* goA = (GameObject*) contacts->contact->GetFixtureA()->GetBody()->GetUserData();
					GameObject* collidedObject;

					b2Fixture* touchedFixture;
					if(goA == this) 
					{
						collidedObject = (GameObject*) contacts->contact->GetFixtureB()->GetBody()->GetUserData();
						touchedFixture = contacts->contact->GetFixtureB();
					} 
					else
					{
						collidedObject = goA;
						touchedFixture = contacts->contact->GetFixtureA();
					}

					if(collidedObject)
					{
						// If the collided object is the character then the platform starts to fade
						if(collidedObject->GetGameObjectType() == GOType_Character_Holt ||
							collidedObject->GetGameObjectType() == GOType_Character_Parker)
						{
							CharacterParker* cp = (CharacterParker*) collidedObject;
							if(cp->IsFootSensor(touchedFixture))
							{
								isFading_ = true;
								fadeTimer_ = 0.0;
								body_->SetLinearVelocity(b2Vec2(0, fadeSpeed_));

								Ogre::ParticleEmitter* emitter = particleSystem_->getEmitter(0);
		
								emitter->setParameter("height", Ogre::StringConverter::toString(emitterSize_.y));
								emitter->setParameter("width", Ogre::StringConverter::toString(emitterSize_.x));
								emitter->setParameter("depth", Ogre::StringConverter::toString(emitterSize_.z));

								particleSystem_->getEmitter(0)->setEnabled(true);
							}
						}
					}

					
				}
				contacts = contacts->next;
			}
		}
		// isFading_ is true
		else
		{
			fadeTimer_ += timeSinceLastFrame;

			if(fadeTimer_ < fadeTime_)
			{
				SetTransparency(fadeTimer_ / fadeTime_);
			}
			else
			{
				body_->SetLinearVelocity(b2Vec2(0,0));
				body_->SetActive(false);
				//particleSystem_->setVisible(false);
				particleSystem_->getEmitter(0)->setEnabled(false);

				if(fadeTimer_ >= fadeReappearTime_)
				{
					body_->SetActive(true);
					SetBodyPosition(initialPosition_);
					SetTransparency(0.0);
					isFading_ = false;
					fadeTimer_ = 0.0;	
				}
			}

		}
	} 
	else
	{
		return false;
	}

	return true;
}

bool FadingPlatform::HandleMessage(const KGBMessage message)
{
	if(GameObjectOgreBox2D::HandleMessage(message))
	{
		return true;
	}
	else
	{

	}

	return false;
}