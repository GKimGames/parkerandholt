/*=============================================================================

		Teleporter.h

		Author: Matt King


		NOT USED IN THE GAME CURRENTLY.

=============================================================================*/

#ifndef TELEPORTER_H
#define TELEPORTER_H

#include "GameObjectOgreBox2D.h"

struct IgnoreBody
{
	IgnoreBody(b2Body* b)
	{
		body = b;
		detected = false;
		inContact = false;
		contactCount = 0;
	}

	b2Body* body;
	bool	detected;
	bool	inContact;
	int		contactCount;
};


class Teleporter : public GameObjectOgreBox2D
{
	

public:
	
	Teleporter(Ogre::String name = "GameObjectOgreBox2D",Teleporter* teleporter = 0, b2Body* body = 0, Ogre::Entity* entity = 0):
	GameObjectOgreBox2D(name,body,entity)
	{
		partner_ = teleporter;
		timer_ = 0;
	}


	void SetPartner(Teleporter* t) { partner_ = t; }


	bool Update(double timeSinceLastFrame)
	{
		timer_ -= timeSinceLastFrame;

		if(timer_ <= 0)
		{
			SetAllNotInContact();

			b2ContactEdge* contacts = body_->GetContactList();
			while(contacts)
			{
				if(contacts->contact->IsTouching())
				{
					b2Body* A = contacts->contact->GetFixtureA()->GetBody();
					b2Body* B = contacts->contact->GetFixtureB()->GetBody();

					if(A != body_)
					{
						DoIgnoreBody(A);
					}
					else
					{
						DoIgnoreBody(B);
					}
				}
				contacts = contacts->next;
			}

			RemoveNotInContact();
			
			teleportingBodies_.clear();
			
		}

		return true;
	}

	~Teleporter(){}

	/// Called when two fixtures begin to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		if(contactFixture->IsSensor() && collidedFixture->IsSensor() == false)
		{
			b2Body* body = collidedFixture->GetBody();

			bool ignoreBody = DoIgnoreBody(body);
			if(body->GetType() == b2_dynamicBody && !ignoreBody)
			{
				if(body->GetUserData())
				{
					
					GameObjectOgreBox2D* goob = (GameObjectOgreBox2D*) body->GetUserData();

					b2Body* pBody = partner_->GetBody();
					float32 angle = body_->GetAngle();
					float x = cos(angle) * 1.8;
					float y = sin(angle) * 1.8;
					b2Vec2 position(partner_->GetBodyPosition().x + x, partner_->GetBodyPosition().y + y);
					goob->SetBodyPosition(position);
					
					// Rotate the velocity
					//float32 angle2 = (pBody->GetAngle() - body_->GetAngle() + 3.14159);
					float32 angle2 = (angle - pBody->GetAngle() - 3.14159);
					b2Vec2 velocity = goob->GetBody()->GetLinearVelocity();
					velocity.x = (velocity.x * cos(angle2)) + (velocity.y * sin(angle2));
					velocity.y = (velocity.x * sin(angle2)) + (velocity.y * cos(angle2));
					velocity.y = 0 - velocity.y;
					goob->GetBody()->SetLinearVelocity(velocity);
					
					partner_->IgnoreThisBody(body);
					teleportingBodies_.push_back(body);

					partner_->Teleport();
					Teleport();
				}
			}
			else if(ignoreBody)
			{
				std::vector<IgnoreBody*>::iterator iter = ignoreBodies_.begin();
				while(iter != ignoreBodies_.end())
				{
					if((*iter)->body == body)
					{
						(*iter)->contactCount += 1;
					}

					iter++;
				}
			}
		}
	}

	/// Called when two fixtures begin to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		if(contactFixture->IsSensor() && collidedFixture->IsSensor() == false)
		{
			b2Body* body = collidedFixture->GetBody();

			bool ignoreBody = DoIgnoreBody(body);
			if(ignoreBody)
			{
				std::vector<IgnoreBody*>::iterator iter = ignoreBodies_.begin();
				while(iter != ignoreBodies_.end())
				{
					if((*iter)->body == body)
					{
						IgnoreBody* b = (*iter);
						b->contactCount -= 1;
						if(b->contactCount == 0)
						{
							iter = ignoreBodies_.erase(iter);
							delete b;
						}
						else
						{
							iter++;
						}
					}
					else
					{
						iter++;
					}
				}
			}
		}
	}

	void IgnoreThisBody(b2Body* b)
	{
		ignoreBodies_.push_back( new IgnoreBody(b) );
	}

	void Teleport()
	{
		timer_ = 0.5;
	}

protected:

	bool DoIgnoreBody(b2Body* b)
	{	
		
		std::vector<IgnoreBody*>::iterator iter = ignoreBodies_.begin();

		while(iter != ignoreBodies_.end())
		{
			if((*iter)->body == b)
			{
				(*iter)->detected = true;
				(*iter)->inContact = true;
				return true;
			}

			iter++;
		}

		std::vector<b2Body*>::iterator iter2 = teleportingBodies_.begin();

		while(iter2 != teleportingBodies_.end())
		{
			if((*iter2) == b)
			{
				return true;
			}

			iter2++;
		}
		
		return false;
	}

	void SetAllNotInContact()
	{	
		
		std::vector<IgnoreBody*>::iterator iter = ignoreBodies_.begin();

		while(iter != ignoreBodies_.end())
		{

			(*iter)->inContact = false;
			iter++;
		}
		
	}

	void RemoveNotInContact()
	{	
		
		std::vector<IgnoreBody*>::iterator iter = ignoreBodies_.begin();

		while(iter != ignoreBodies_.end())
		{

			if((*iter)->inContact == false)
			{
				IgnoreBody* b = (*iter);
				iter = ignoreBodies_.erase(iter);
				delete b;
			}
			else
			{
				iter++;
			}
		}
	}

	std::vector<IgnoreBody*> ignoreBodies_;
	std::vector<b2Body*>	 teleportingBodies_;
	Teleporter* partner_;

	double timer_;
	

};

#endif

