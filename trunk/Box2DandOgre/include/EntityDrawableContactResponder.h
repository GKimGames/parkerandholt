#ifndef PAH_ENTITYDRAWABLECONTACTRESPONDER_H
#define PAH_ENTITYDRAWABLECONTACTRESPONDER_H

#include "EntityDrawable.h"

namespace PaH 
{
	/// EntityDrawable extends Entity
	class EntityDrawableContactResponder : public EntityDrawable
	{
	public:

		EntityDrawableContactResponder(){;};
		
		virtual bool Update(double timeSinceLastFrame){return true;};
		
		/// Called when two fixtures begin to touch.
		virtual void BeginContact(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){;};

		/// Called when two fixtures cease to touch.
		virtual void EndContact(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){;};


	protected:

		virtual void EvaluateContacts(){;};

		std::vector<b2Contact*> beginContacts_;		//< \brief Vector of begin Contacts
		std::vector<b2Contact*> persistContacts_;	//< \brief Vector of Contacts that have never ended
		std::vector<b2Contact*> endContacts_;		//< \brief Vector of end Contacts
	};
}

#endif