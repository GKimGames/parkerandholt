/*=============================================================================

  Box2DXMLLoader.cpp

  Author: Matt King

=============================================================================*/

#include "Box2DXMLLoader.h"
#include "GameObjectOgreBox2D.h"
#include "HelperFunctions.h"

std::vector<b2Shape*> Box2DXMLLoader::shapeVector;

//=============================================================================
//								Getb2BodyDefAttributes
//
/// This has to be called as shapes for fixtures can't be built on the stack.
/// Call if after you're done with fixtureDefs return from this class.
void Box2DXMLLoader::ClearShapeVector()
{
	shapeVector.clear();
}

//=============================================================================
//								Createb2Body
//
bool Box2DXMLLoader::Createb2Body(b2Body* body, b2World* world, TiXmlElement* element)
{
	if(element->NoChildren())
	{
		// The element has no children and can't possibly define a body
		return false;
	}
	else
	{
		// We first look for the b2BodyDef for the body as the body must be
		// created before everything else.
		TiXmlElement* bodyDefNode = element->FirstChildElement("BodyDef");

		b2BodyDef bodyDef;
		Getb2BodyDefAttributes(&bodyDef, bodyDefNode);

		body = world->CreateBody(&bodyDef);

		TiXmlElement* fixtureDefNode;

		// The bodydefNode doesn't have to exist for the body to be created correctly
		// So we need to check that it existed to get the sibling elements.
		if(bodyDefNode)
		{
			fixtureDefNode = bodyDefNode->NextSiblingElement();
		}
		else
		{
			fixtureDefNode = element->FirstChildElement();
		}


		b2FixtureDef fixtureDef;
		
		while(fixtureDefNode != 0)
		{
			if(Getb2FixtureDefAttributes(&fixtureDef,fixtureDefNode));
			{
				body->CreateFixture(&fixtureDef);
			}

			fixtureDefNode = fixtureDefNode->NextSiblingElement();
		}

		return true;
	}

	return false;
}


//=============================================================================
//								Createb2Body
//
b2Body* Box2DXMLLoader::Createb2Body(b2World* world, TiXmlElement* element)
{
	b2Body* body = 0;

	if(element->NoChildren())
	{
		// The element has no children and can't possibly define a body
		return body;
	}
	else
	{
		// We first look for the b2BodyDef for the body as the body must be
		// created before everything else.
		TiXmlElement* bodyDefNode = element->FirstChildElement("BodyDef");

		b2BodyDef bodyDef;
		Getb2BodyDefAttributes(&bodyDef, bodyDefNode);

		body = world->CreateBody(&bodyDef);

		TiXmlElement* fixtureDefNode;

		// The bodydefNode doesn't have to exist for the body to be created correctly
		// So we need to check that it existed to get the sibling elements.
		if(bodyDefNode)
		{
			fixtureDefNode = bodyDefNode->NextSiblingElement();
		}
		else
		{
			fixtureDefNode = element->FirstChildElement();
		}


		b2FixtureDef fixtureDef;
		
		while(fixtureDefNode != 0)
		{
			if(Getb2FixtureDefAttributes(&fixtureDef,fixtureDefNode));
			{
				body->CreateFixture(&fixtureDef);
			}

			fixtureDefNode = fixtureDefNode->NextSiblingElement();
		}

		return body;
	}

	return body;
}


//=============================================================================
//								Getb2BodyDefAttributes
//
///
bool Box2DXMLLoader::Getb2BodyDefAttributes(b2BodyDef* bodyDef, TiXmlElement* element)
{
	if(element != 0)
	{
		element->QueryBoolAttribute("allowSleep", &bodyDef->allowSleep);
		element->QueryFloatAttribute("angle", &bodyDef->angle);
		element->QueryFloatAttribute("angularDamping", &bodyDef->angularDamping);
		element->QueryBoolAttribute("fixedRotation", &bodyDef->fixedRotation);
		element->QueryBoolAttribute("bullet", &bodyDef->bullet);
		element->QueryBoolAttribute("awake", &bodyDef->awake);
		element->QueryBoolAttribute("active", &bodyDef->active);
		element->QueryFloatAttribute("linearDamping", &bodyDef->linearDamping);

		GetB2Vec2(element, "linearVelocity", &bodyDef->linearVelocity);

		GetB2Vec2(element, "position", &bodyDef->position);

		std::string str;
		element->QueryValueAttribute("bodyType", &str);

		if(str.compare("dynamicBody") == 0)
		{
			bodyDef->type = b2_dynamicBody;
		}
		else if(str.compare("kinematicBody") == 0)
		{
			bodyDef->type = b2_kinematicBody;
		}

		return true;
	}
	
	DEBUG_LOGC("Fixture Get Body Def Error: Element is 0.");
	return false;
}



//=============================================================================
//								Getb2FixtureDefAttributes
//
bool Box2DXMLLoader::Getb2FixtureDefAttributes(b2FixtureDef* fixtureDef, TiXmlElement* element)
{
	if(element != 0)
	{
		b2PolygonShape* polyDef = new b2PolygonShape();
		b2CircleShape* circleDef = new b2CircleShape();
		std::string str;

		// It is necessary to define what kind of shape the fixture is.
		if(element->QueryValueAttribute("shape", &str) == TIXML_SUCCESS)
		{
			// Check what kind of shape it is
			if(str.compare("box") == 0)
			{
				shapeVector.push_back(polyDef);
				delete circleDef;

				float boxWidth = 1.0, boxHeight = 1.0, angle = 0.0f;
				element->QueryFloatAttribute("boxWidth", &boxWidth);
				element->QueryFloatAttribute("boxHeight", &boxHeight);
				b2Vec2 point;
				GetB2Vec2(element, "center",&point);
				element->QueryFloatAttribute("angle", &angle);

				polyDef->SetAsBox(boxWidth/2, boxHeight/2, point,angle);

				fixtureDef->shape = polyDef;
			}
			else if(str.compare("edge") == 0)
			{
				shapeVector.push_back(polyDef);
				delete circleDef;

				float x1 = 0.0f, y1 = 0.0f,x2 = 0.0f, y2 = 0.0f;

				b2Vec2 point1, point2;
				
				GetB2Vec2(element, "point1", &point1);
				GetB2Vec2(element, "point2", &point2);

				polyDef->SetAsEdge(point1,point2);

				fixtureDef->shape = polyDef;
			}
			else if(str.compare("circle") == 0)
			{
				shapeVector.push_back(circleDef);
				delete polyDef;

				element->QueryFloatAttribute("radius", &circleDef->m_radius);

				GetB2Vec2(element, "center", &circleDef->m_p);

				fixtureDef->shape = circleDef;

			}
			else if(str.compare("polygon") == 0)
			{
				shapeVector.push_back(polyDef);
				delete circleDef;

				b2Vec2 bodyVecs[8];

				int pointCount = 0;
				element->QueryIntAttribute("pointCount", &pointCount);

				if(pointCount)
				{
					for(int i = 0; i < pointCount; ++i)
					{
						std::string stringI = Ogre::StringConverter::toString(i);

						std::string point = "point";
						point += stringI;
						
						b2Vec2 center;
						GetB2Vec2(element, point.c_str(), &center);
						bodyVecs[i].Set(center.x, center.y);

					}

					polyDef->Set(bodyVecs, pointCount);
					fixtureDef->shape = polyDef;

				}
				else
				{
					;
					DEBUG_LOGC("Fixture Creation Error: No point count set while creating polygon.");
					return false;
				}
			}
			else
			{
				DEBUG_LOGC("Fixture Creation Error: No shape type defined.");
				return false;
			}
		}

		element->QueryBoolAttribute("sensor", &fixtureDef->isSensor);

		// If the fixture is a sensor it shouldn't have any mass or friction
		// or restitution, therefor doesn't need to read the valus.
		if(fixtureDef->isSensor)
		{
			fixtureDef->density = 0.0;
			fixtureDef->friction = 0.0;
			fixtureDef->restitution = 0.0;
		}
		else
		{
			element->QueryFloatAttribute("density", &fixtureDef->density);
			element->QueryFloatAttribute("friction", &fixtureDef->friction);
			element->QueryFloatAttribute("restitution", &fixtureDef->restitution);
		}

		element->QueryValueAttribute("filter-categoryBits", &fixtureDef->filter.categoryBits);
		element->QueryValueAttribute("filter-groupIndex", &fixtureDef->filter.groupIndex);
		element->QueryValueAttribute("filter-maskBits", &fixtureDef->filter.maskBits);

		return true;
	}

	DEBUG_LOGC("Fixture Creation Error: Element is 0.");

	return false;
	
}



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

//=============================================================================
//								Getb2PrismaticJointdefAttributes
//
bool Box2DXMLLoader::Getb2PrismaticJointdefAttributes(b2PrismaticJointDef* jointDef, TiXmlElement* element)
{
	if(element != 0)
	{

	}
	else
	{
		return false;
	}
	return true;
}



//=============================================================================
//								Getb2Vec2
//
void Box2DXMLLoader::GetB2Vec2(TiXmlElement* element, const char* name, b2Vec2* vector)
{
	
	if(element != 0)
	{
		if(name != 0)
		{
			const char* str1 = element->Attribute(name);
			
			if(str1 != 0)
			{
				Ogre::String str = Ogre::String(str1);
				Ogre::Vector2 v = Ogre::StringConverter::parseVector2(str);
				vector->x = v.x;
				vector->y = v.y;
			}
			else
			{
				vector->x = 0;
				vector->y = 0;
				DEBUG_LOG("GetB2Vec2 Error: Attribute name not found");
			}
		}
		else
		{
			DEBUG_LOG("GetB2Vec2 Error: Attribute name is zero");
		}
	}
	else
	{
		DEBUG_LOG("GetB2Vec2 Error: Element is zero");
	}
}

