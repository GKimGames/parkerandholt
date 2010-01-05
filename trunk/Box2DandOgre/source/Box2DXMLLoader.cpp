/*=============================================================================

  Box2DXMLLoader.cpp

  Author: Matt King

=============================================================================*/

#include "Box2DXMLLoader.h"

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
//								Getb2BodyDefAttributes
//
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
		element->QueryFloatAttribute("linearVelocityX", &bodyDef->linearVelocity.x);
		element->QueryFloatAttribute("linearVelocityY", &bodyDef->linearVelocity.y);
		element->QueryFloatAttribute("positionX", &bodyDef->position.x);
		element->QueryFloatAttribute("positionY", &bodyDef->position.y);

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

				float boxWidth = 1.0, boxHeight = 1.0, x = 0.0f, y = 0.0f, angle = 0.0f;
				element->QueryFloatAttribute("boxWidth", &boxWidth);
				element->QueryFloatAttribute("boxHeight", &boxHeight);
				element->QueryFloatAttribute("centerY", &y);
				element->QueryFloatAttribute("angle", &y);

				polyDef->SetAsBox(boxWidth/2, boxHeight/2, b2Vec2(x,y),angle);

				fixtureDef->shape = polyDef;
			}
			else if(str.compare("edge") == 0)
			{
				shapeVector.push_back(polyDef);
				delete circleDef;

				float x1 = 0.0f, y1 = 0.0f,x2 = 0.0f, y2 = 0.0f;

				element->QueryFloatAttribute("px1", &x1);
				element->QueryFloatAttribute("py1", &y1);
				element->QueryFloatAttribute("px2", &x2);
				element->QueryFloatAttribute("py2", &y2);

				polyDef->SetAsEdge(b2Vec2(x1,y1),b2Vec2(x2,y2));

				fixtureDef->shape = polyDef;
			}
			else if(str.compare("circle") == 0)
			{
				shapeVector.push_back(circleDef);
				delete polyDef;

				element->QueryFloatAttribute("radius", &circleDef->m_radius);
				element->QueryFloatAttribute("centerX", &circleDef->m_p.x);
				element->QueryFloatAttribute("centerY", &circleDef->m_p.y);

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

						std::string px = "px";
						std::string py = "py";

						px += stringI;
						py += stringI;

						element->QueryFloatAttribute(px.c_str(), &bodyVecs[i].x);
						element->QueryFloatAttribute(py.c_str(), &bodyVecs[i].y);
					}
					polyDef->Set(bodyVecs, pointCount);
					fixtureDef->shape = polyDef;
				}
				else
				{
					return false;
				}
			}
			else
			{
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

