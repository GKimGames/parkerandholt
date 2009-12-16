#ifndef PAH_ENTITYCONTACTRESPONDER_H
#define PAH_ENTITYCONTACTRESPONDER_H

#include "Entity.h"

namespace PaH {

	class EntityContactResponder : virtual public Entity
	{
	public:

		EntityContactResponder(){};

		/// Called when two fixtures begin to touch.
		virtual void BeginContact(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)=0;

		/// Called when two fixtures cease to touch.
		virtual void EndContact(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)=0;
	protected:
		
		virtual void EvaluateContacts(){;};
	};

}

#endif