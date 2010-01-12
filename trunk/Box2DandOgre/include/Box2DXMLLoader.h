/*=============================================================================

  Box2DXMLLoader.h

  Author: Matt King

=============================================================================*/
#ifndef BOX2D_XML_LOADER_H
#define BOX2D_XML_LOADER_H

#include <map>

#include <tinyxml.h>
#include <Box2D/Box2D.h>
#include "Ogre.h"


typedef std::map<Ogre::String, b2Body*> BodyMap;
typedef std::map<Ogre::String, b2Fixture*> FixtureMap;

// b2Body
// String	Id
// b2Vec2	position
// float	angle
// float	linearVelocityX
// float	linearVelocityY
// bool		allowSleep
// bool		fixedRotation
// bool		bullet
// string	bodytype
// bool		active


// fixture and shape are stuck together, there is no explicit b2Shape, just fixture
// b2Fixture
// String	Id
// b2Shape	shape
// float	boxWidth
// float	boxHeight
// float	angle
// float	friction
// float	restitution
// float	density
// float	mass
// string	category
// string	group
// bool		isSensor
// b2Shape
// string	type
// float	radius
// float	centerX
// float	centerY
// b2Vec2	position
// int		pointCount
// b2Vec2[]	points
// 

// b2PrismaticJoint
// String	Id
// String	bodyA			Bodies use the XML Id, the body must be previously declared
// String	bodyB
// bool		collideConnect
// bool		enableLimit
// bool		enableMotor
// float	maxMotorForce
// float	motorSpeed
// float	lowerTranslation
// float	upperTranslation
// b2Vec2	worldAxis
// b2Vec2	worldAnchor


class Box2DXMLLoader
{

public:
	static std::vector<b2Shape*> shapeVector;
	// These methods can be called anywhere
	static bool Createb2Body(b2Body* body, b2World* world, TiXmlElement* element);
	static b2Body* Createb2Body(b2World* world, TiXmlElement* element);
	static bool Getb2BodyDefAttributes(b2BodyDef* bodyDef, TiXmlElement* element);
	static bool Getb2FixtureDefAttributes(b2FixtureDef* fixtureDef, TiXmlElement* element);
	static void ClearShapeVector();
	static void GetB2Vec2(TiXmlElement* element, const char* name, b2Vec2* vector);

	// You must actually instantiate the class to use these methods as they use Id's
	bool Getb2PrismaticJointdefAttributes(b2PrismaticJointDef* jointDef, TiXmlElement* element);

protected:

	BodyMap bodyMap_;
	FixtureMap fixtureMap_;
	
};

#endif

