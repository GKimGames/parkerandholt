#ifndef PAH_ENTITYBOX2DRESPONDER_H
#define PAH_ENTITYBOX2DRESPONDER_H

#include "EntityBox2D.h"

namespace PaH {

	class EntityBox2DResponder : virtual public EntityBox2D
	{
	public:

		/// Called when two fixtures begin to touch.
		virtual void BeginContact(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)=0;

		/// Called when two fixtures cease to touch.
		virtual void EndContact(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)=0;
	};

}

#endif