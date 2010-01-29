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
typedef std::map<Ogre::String, b2Joint*> JointMap;

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

enum Box2DXMLResult
{
	BOX2DXML_OK,
	BOX2DXML_ELEMENT_IS_ZERO,
	BOX2DXMLRESULT_COUNT
};

const static char* Box2DXMLResultString[BOX2DXMLRESULT_COUNT] = 
{
	"Box2DXML_OK",
	"Box2DXML_Element_Is_Zero"
};

class Box2DXMLLoader
{

public:


/*=============================================================================
				PUBLIC static members
=============================================================================*/
	static bool Createb2Body(b2Body* body, b2World* world, TiXmlElement* element);
	static b2Body* Createb2Body(b2World* world, TiXmlElement* element);
	static bool Getb2BodyDefAttributes(b2BodyDef* bodyDef, TiXmlElement* element);
	static bool Getb2FixtureDefAttributes(b2FixtureDef* fixtureDef, TiXmlElement* element);
	static void ClearShapeVector();
	static void GetB2Vec2(TiXmlElement* element, const char* name, b2Vec2* vector);

	static std::vector<b2Shape*> shapeVector;

/*=============================================================================
				Class Members
=============================================================================*/
	Box2DXMLLoader(TiXmlElement* element, b2World* world);

	Box2DXMLResult GetLastResult();

	b2Body*		GetBody(Ogre::String id);
	b2Fixture*	GetFixture(Ogre::String id);
	b2Joint*	GetJoint(Ogre::String id);

protected:

	bool Initialize();

	bool Createb2Body(TiXmlElement* element);
	bool Createb2RevoluteJoint(TiXmlElement* element);
	bool Createb2PrismaticJoint(TiXmlElement* element);
	bool Createb2WeldJoint(TiXmlElement* element);
	bool Createb2PulleyJoint(TiXmlElement* element);

	b2World*		world_;
	TiXmlElement*	element_;
	Box2DXMLResult	result_;
	BodyMap			bodyMap_;
	FixtureMap		fixtureMap_;
	JointMap		jointMap_;
	
};

#endif

