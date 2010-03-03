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
	}

	b2Body* body;
	bool	detected;
	bool	inContact;
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

		if(body->GetType() == b2_dynamicBody && !DoIgnoreBody(body))
		{
			if(body->GetUserData())
			{
				GameObjectOgreBox2D* goob = (GameObjectOgreBox2D*) body->GetUserData();

				goob->SetBodyPosition(partner_->GetBodyPosition());
			}
		}
		
	}


	/// Called when two fixtures cease to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	/// By default this does nothing.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		
	}


	void IgnoreThisBody(b2Body* b)
	{
		ignoreBodies_.push_back( new IgnoreBody(b));
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

			if((*iter)->inContact = false)
			{
				delete (*iter);
				ignoreBodies_.erase(iter);
			}

			iter++;
		}
	}

	std::vector<IgnoreBody*> ignoreBodies_;
	Teleporter* partner_;
	

};

#endif