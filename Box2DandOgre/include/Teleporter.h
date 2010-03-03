/*=============================================================================

		Teleporter.h

		Author: Matt King

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
	}


	void SetPartner(Teleporter* t) { partner_ = t; }


	bool Update(double timeSinceLastFrame)
	{
		/*
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
		*/

		return true;
	}

	~Teleporter(){}

	/// Called when two fixtures begin to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		b2Body* body = collidedFixture->GetBody();

		bool ignoreBody = DoIgnoreBody(body);
		if(body->GetType() == b2_dynamicBody && !ignoreBody)
		{
			if(body->GetUserData())
			{
				GameObjectOgreBox2D* goob = (GameObjectOgreBox2D*) body->GetUserData();

				goob->SetBodyPosition(partner_->GetBodyPosition());
				partner_->IgnoreThisBody(body);
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

	/// Called when two fixtures begin to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
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

	void IgnoreThisBody(b2Body* b)
	{
		ignoreBodies_.push_back( new IgnoreBody(b) );
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
	Teleporter* partner_;
	

};

#endif