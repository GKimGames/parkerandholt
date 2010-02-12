/*=============================================================================

	Box2DXMLLoader.cpp

	Author: Matt King

=============================================================================*/

#include "Box2DXMLLoader.h"
#include "GameObjectOgreBox2D.h"
#include "HelperFunctions.h"
#include "TinyXMLHelper.h"

std::vector<b2Shape*> Box2DXMLLoader::shapeVector;


//=============================================================================
//								Box2DXMLLoader
//
Box2DXMLLoader::Box2DXMLLoader(TiXmlElement* element, b2World* world)
{

	if(element != 0)
	{
		if(element->NoChildren())
		{
			
		}
		else
		{
			element_ = element;
			world_ = world;
			Initialize();
		}
	}
	else
	{
		result_ = BOX2DXML_ELEMENT_IS_ZERO; 
	}
}


//=============================================================================
//								Initialize
//
bool Box2DXMLLoader::Initialize()
{

	TiXmlElement* bodyElement = element_->FirstChildElement("Body");

	while(bodyElement)
	{
		Createb2Body(bodyElement);

		bodyElement = bodyElement->NextSiblingElement("Body");
	}

	TiXmlElement* jointElement = element_->FirstChildElement("Joint");

	while(jointElement)
	{

		// Find out what type of joint this element contains
		std::string str;
		if(jointElement->QueryValueAttribute("type", &str) == TIXML_SUCCESS)
		{

			// Check what kind of joint it is
			if(str.compare("revolute") == 0)
			{
				Createb2RevoluteJoint(jointElement);
			}
			else if(str.compare("prismatic") == 0)
			{
				Createb2PrismaticJoint(jointElement);
			}
			else if(str.compare("weld") == 0)
			{
				Createb2WeldJoint(jointElement);
			}
			else if(str.compare("pulley") == 0)
			{
				Createb2PulleyJoint(jointElement);
			}

		}
		else
		{
			DEBUG_LOG("Box2DXMLLoader::Initialize Error: Joint contained no type");
		}

		jointElement = jointElement->NextSiblingElement("Joint");
	}


	return true;
}

//=============================================================================
//								GetBody
//
/// Returns a body given its string id
b2Body*	Box2DXMLLoader::GetBody(Ogre::String id)
{
	BodyMap::iterator iter = bodyMap_.find(id);
	
	if(iter != bodyMap_.end())
	{
		return iter->second;
	}

	return 0;
}


//=============================================================================
//								GetFixture
//
/// Returns a fixture given its string id
b2Fixture* Box2DXMLLoader::GetFixture(Ogre::String id)
{
	FixtureMap::iterator iter = fixtureMap_.find(id);
	
	if(iter != fixtureMap_.end())
	{
		return iter->second;
	}

	return 0;
}


//=============================================================================
//								GetJoint
//
/// Returns a joint given its string id
b2Joint* Box2DXMLLoader::GetJoint(Ogre::String id)
{
	JointMap::iterator iter = jointMap_.find(id);
	
	if(iter != jointMap_.end())
	{
		return iter->second;
	}

	return 0;
}


//=============================================================================
//								Createb2Body
//
/// Internal function used to create b2Bodys from an element. This adds the 
/// body and fixtures in that body to maps in this object.
bool Box2DXMLLoader::Createb2Body(TiXmlElement* element)
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

		// Insert the body into the bodyMap
		b2Body* body = world_->CreateBody(&bodyDef);
		
		if(element->Attribute("id") != NULL)
		{
			Ogre::String id = Ogre::String(element->Attribute("id"));
			bodyMap_.insert(std::make_pair(id, body));
		}

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
			if(Getb2FixtureDefAttributes(&fixtureDef,fixtureDefNode))
			{

				b2Fixture* f = body->CreateFixture(&fixtureDef);
				
				// Get the id of the fixture and insert it into the map.
				if(fixtureDefNode->Attribute("id") != NULL)
				{
					Ogre::String id = Ogre::String(fixtureDefNode->Attribute("id"));
					fixtureMap_.insert(std::make_pair(id, f));
				}	
			}

			fixtureDefNode = fixtureDefNode->NextSiblingElement();
		}

		return true;
	}

	return false;
}


//=============================================================================
//								Createb2RevoluteJoint
//
bool Box2DXMLLoader::Createb2RevoluteJoint(TiXmlElement* element)
{
	if(element != 0)
	{
		TiXmlElement* jointDefNode = element->FirstChildElement("JointDef");

		if(jointDefNode != 0)
		{
			Ogre::String strA;
			Ogre::String strB;


			if(jointDefNode->QueryValueAttribute("bodyA", &strA) != TIXML_SUCCESS)
			{
				DEBUG_LOG("Createb2RevoluteJoint Error: bodyA not found");
				return false;
			}

			if(jointDefNode->QueryValueAttribute("bodyB", &strB) != TIXML_SUCCESS)
			{
				DEBUG_LOG("Createb2RevoluteJoint Error: bodyB not found");
				return false;
			}

			b2RevoluteJointDef jointDef;

			// Find the bodys for the joint
			BodyMap::iterator iter;
			iter = bodyMap_.find(strA);

			if(iter == bodyMap_.end())
			{
				DEBUG_LOG("Createb2RevoluteJoint Error: bodyA not found in map");
				return false;
			}

			b2Body* bodyA = iter->second;

			iter = bodyMap_.find(strB);
			if(iter == bodyMap_.end())
			{
				DEBUG_LOG("Createb2RevoluteJoint Error: bodyB not found in map");
				return false;
			}

			b2Body* bodyB = iter->second;

			// Now load all the jointDef's properties

			jointDefNode->QueryBoolAttribute("collideConnected", &jointDef.collideConnected);
			jointDefNode->QueryBoolAttribute("enableLimit", &jointDef.enableLimit);
			jointDefNode->QueryBoolAttribute("enableMotor", &jointDef.enableMotor);
			GetB2Vec2(jointDefNode, "localAnchorA", &jointDef.localAnchorA);
			GetB2Vec2(jointDefNode, "localAnchorB", &jointDef.localAnchorA);
			jointDefNode->QueryFloatAttribute("lowerAngle", &jointDef.lowerAngle);
			jointDefNode->QueryFloatAttribute("maxMotorTorque", &jointDef.maxMotorTorque);
			jointDefNode->QueryFloatAttribute("motorSpeed", &jointDef.motorSpeed);
			jointDefNode->QueryFloatAttribute("upperAngle", &jointDef.upperAngle);

			b2Vec2 worldAnchor(0,0);
			GetB2Vec2(jointDefNode, "worldAnchor", &worldAnchor);
			if(worldAnchor.x == 0 && worldAnchor.y ==0)
				worldAnchor = bodyB->GetWorldCenter();
			jointDef.Initialize(bodyA,bodyB, worldAnchor);
			b2Joint* j = world_->CreateJoint(&jointDef);

			// If the joint has an id we will add it the map of joints
			if(element->Attribute("id") != NULL)
			{
				Ogre::String id = Ogre::String(element->Attribute("id"));
				jointMap_.insert(std::make_pair(id, j));
			}
		}
		else
		{
			DEBUG_LOG("Createb2RevoluteJoint Error: Element contained no JointDef");
		}
	}
	else
	{
		DEBUG_LOG("Createb2RevoluteJoint Error: Element is zero");
		return false;
	}

	return true;
}

//=============================================================================
//							Createb2DistanceJoint
//
bool Box2DXMLLoader::Createb2DistanceJoint(TiXmlElement* element)
{
	if(element != 0)
	{
		TiXmlElement* jointDefNode = element->FirstChildElement("JointDef");

		if(jointDefNode != 0)
		{
			Ogre::String strA;
			Ogre::String strB;


			if(jointDefNode->QueryValueAttribute("bodyA", &strA) != TIXML_SUCCESS)
			{
				DEBUG_LOG("Createb2DistanceJoint Error: bodyA not found");
				return false;
			}

			if(jointDefNode->QueryValueAttribute("bodyB", &strB) != TIXML_SUCCESS)
			{
				DEBUG_LOG("Createb2DistanceJoint Error: bodyB not found");
				return false;
			}

			b2DistanceJointDef jointDef;

			// Find the bodys for the joint
			BodyMap::iterator iter;
			iter = bodyMap_.find(strA);

			if(iter == bodyMap_.end())
			{
				DEBUG_LOG("Createb2DistanceJoint Error: bodyA not found in map");
				return false;
			}

			b2Body* bodyA = iter->second;

			iter = bodyMap_.find(strB);
			if(iter == bodyMap_.end())
			{
				DEBUG_LOG("Createb2DistanceJoint Error: bodyB not found in map");
				return false;
			}

			b2Body* bodyB = iter->second;
			
			// Now load all the jointDef's properties

			jointDefNode->QueryBoolAttribute("collideConnected", &jointDef.collideConnected);
			GetB2Vec2(jointDefNode, "localAnchorA", &jointDef.localAnchorA);
			GetB2Vec2(jointDefNode, "localAnchorB", &jointDef.localAnchorA);
			
			jointDef.dampingRatio = TinyXMLHelper::GetAttributeFloat(jointDefNode, "dampingRatio", jointDef.dampingRatio);
			jointDef.frequencyHz = TinyXMLHelper::GetAttributeFloat(jointDefNode, "frequencyHz", jointDef.frequencyHz);;
			jointDef.length = TinyXMLHelper::GetAttributeFloat(jointDefNode, "length", jointDef.length);

			jointDef.Initialize(bodyA,bodyB, jointDef.localAnchorA, jointDef.localAnchorB);
			b2Joint* j = world_->CreateJoint(&jointDef);

			// If the joint has an id we will add it to the map of joints
			if(element->Attribute("id") != NULL)
			{
				Ogre::String id = Ogre::String(element->Attribute("id"));
				jointMap_.insert(std::make_pair(id, j));
			}
		}
		else
		{
			DEBUG_LOG("Createb2DistanceJoint Error: Element contained no JointDef");
		}
	}
	else
	{
		DEBUG_LOG("Createb2DistanceJoint Error: Element is zero");
		return false;
	}

	return true;
}
//=============================================================================
//								Createb2PrismaticJoint
//
bool Box2DXMLLoader::Createb2PrismaticJoint(TiXmlElement* element)
{
	if(element != 0)
	{
		TiXmlElement* jointDefNode = element->FirstChildElement("JointDef");

		if(jointDefNode != 0)
		{
			Ogre::String strA;
			Ogre::String strB;


			if(jointDefNode->QueryValueAttribute("bodyA", &strA) != TIXML_SUCCESS)
			{
				DEBUG_LOG("Createb2PrismaticJoint Error: bodyA not found");
				return false;
			}

			if(jointDefNode->QueryValueAttribute("bodyB", &strB) != TIXML_SUCCESS)
			{
				DEBUG_LOG("Createb2PrismaticJoint Error: bodyB not found");
				return false;
			}

			b2PrismaticJointDef jointDef;

			// Find the bodys for the joint
			BodyMap::iterator iter;
			iter = bodyMap_.find(strA);

			if(iter == bodyMap_.end())
			{
				DEBUG_LOG("Createb2PrismaticJoint Error: bodyA not found in map");
				return false;
			}

			b2Body* bodyA = iter->second;

			iter = bodyMap_.find(strB);
			if(iter == bodyMap_.end())
			{
				DEBUG_LOG("Createb2PrismaticJoint Error: bodyB not found in map");
				return false;
			}

			b2Body* bodyB = iter->second;
			
			// Now load all the jointDef's properties

			jointDefNode->QueryBoolAttribute("collideConnected", &jointDef.collideConnected);
			jointDefNode->QueryBoolAttribute("enableLimit", &jointDef.enableLimit);
			jointDefNode->QueryBoolAttribute("enableMotor", &jointDef.enableMotor);
			GetB2Vec2(jointDefNode, "localAnchorA", &jointDef.localAnchorA);
			GetB2Vec2(jointDefNode, "localAnchorB", &jointDef.localAnchorA);
			GetB2Vec2(jointDefNode, "localAxis1", &jointDef.localAxis1);
			jointDefNode->QueryFloatAttribute("lowerTranslation", &jointDef.lowerTranslation);
			jointDefNode->QueryFloatAttribute("maxMotorForce", &jointDef.maxMotorForce);
			jointDefNode->QueryFloatAttribute("motorSpeed", &jointDef.motorSpeed);
			jointDefNode->QueryFloatAttribute("upperTranslation", &jointDef.upperTranslation);

			b2Vec2 worldAnchor;
			GetB2Vec2(jointDefNode, "worldAnchor", &worldAnchor);

			jointDef.Initialize(bodyA,bodyB, worldAnchor, jointDef.localAxis1);
			b2Joint* j = world_->CreateJoint(&jointDef);

			// If the joint has an id we will add it to the map of joints
			if(element->Attribute("id") != NULL)
			{
				Ogre::String id = Ogre::String(element->Attribute("id"));
				jointMap_.insert(std::make_pair(id, j));
			}
		}
		else
		{
			DEBUG_LOG("Createb2PrismaticJoint Error: Element contained no JointDef");
		}
	}
	else
	{
		DEBUG_LOG("Createb2PrismaticJoint Error: Element is zero");
		return false;
	}

	return true;
}


//=============================================================================
//								Createb2PulleyJoint
//
bool Box2DXMLLoader::Createb2PulleyJoint(TiXmlElement* element)
{
	if(element != 0)
	{
		TiXmlElement* jointDefNode = element->FirstChildElement("JointDef");

		if(jointDefNode != 0)
		{
			Ogre::String strA;
			Ogre::String strB;


			if(jointDefNode->QueryValueAttribute("bodyA", &strA) != TIXML_SUCCESS)
			{
				DEBUG_LOG("Createb2PulleyJoint Error: bodyA not found");
				return false;
			}

			if(jointDefNode->QueryValueAttribute("bodyB", &strB) != TIXML_SUCCESS)
			{
				DEBUG_LOG("Createb2PulleyJoint Error: bodyB not found");
				return false;
			}

			b2PulleyJointDef jointDef;

			// Find the bodys for the joint
			BodyMap::iterator iter;
			iter = bodyMap_.find(strA);

			if(iter == bodyMap_.end())
			{
				DEBUG_LOG("Createb2PulleyJoint Error: bodyA not found in map");
				return false;
			}

			b2Body* bodyA = iter->second;

			iter = bodyMap_.find(strB);
			if(iter == bodyMap_.end())
			{
				DEBUG_LOG("Createb2PulleyJoint Error: bodyB not found in map");
				return false;
			}

			b2Body* bodyB = iter->second;

			// Now load all the jointDef's properties

			jointDefNode->QueryBoolAttribute("collideConnected", &jointDef.collideConnected);
			GetB2Vec2(jointDefNode, "localAnchorA", &jointDef.localAnchorA);
			GetB2Vec2(jointDefNode, "localAnchorB", &jointDef.localAnchorB);
			GetB2Vec2(jointDefNode, "groundAnchorA", &jointDef.groundAnchorA);
			GetB2Vec2(jointDefNode, "groundAnchorB", &jointDef.groundAnchorB);
			jointDefNode->QueryFloatAttribute("ratio", &jointDef.ratio);

			jointDef.Initialize(
				bodyA,bodyB,
				jointDef.groundAnchorA, jointDef.groundAnchorB,
				jointDef.localAnchorA,jointDef.localAnchorB,
				jointDef.ratio
				);

			jointDefNode->QueryFloatAttribute("lengthA", &jointDef.lengthA);
			jointDefNode->QueryFloatAttribute("maxLengthA", &jointDef.maxLengthA);
			jointDefNode->QueryFloatAttribute("lengthB", &jointDef.lengthB);
			jointDefNode->QueryFloatAttribute("maxLengthB", &jointDef.maxLengthB);
			b2Joint* j = world_->CreateJoint(&jointDef);

			// If the joint has an id we will add it to the map of joints
			if(element->Attribute("id") != NULL)
			{
				Ogre::String id = Ogre::String(element->Attribute("id"));
				jointMap_.insert(std::make_pair(id, j));
			}
		}
		else
		{
			DEBUG_LOG("Createb2PulleyJoint Error: Element contained no JointDef");
		}
	}
	else
	{
		DEBUG_LOG("Createb2PulleyJoint Error: Element is zero");
		return false;
	}

	return true;
}


//=============================================================================
//								Createb2WeldJoint
//
bool Box2DXMLLoader::Createb2WeldJoint(TiXmlElement* element)
{
	if(element != 0)
	{
		TiXmlElement* jointDefNode = element->FirstChildElement("JointDef");

		if(jointDefNode != 0)
		{
			Ogre::String strA;
			Ogre::String strB;

			// It is necessary to define what kind of shape the fixture is.
			if(jointDefNode->QueryValueAttribute("bodyA", &strA) != TIXML_SUCCESS)
			{
				DEBUG_LOG("Createb2WeldJoint Error: bodyA not found");
				return false;
			}

			if(jointDefNode->QueryValueAttribute("bodyB", &strB) != TIXML_SUCCESS)
			{
				DEBUG_LOG("Createb2WeldJoint Error: bodyB not found");
				return false;
			}

			b2WeldJointDef jointDef;

			// Find the bodys for the joint
			BodyMap::iterator iter;
			iter = bodyMap_.find(strA);

			if(iter == bodyMap_.end())
			{
				DEBUG_LOG("Createb2WeldJoint Error: bodyA not found in map");
				return false;
			}

			b2Body* bodyA = iter->second;

			iter = bodyMap_.find(strB);
			if(iter == bodyMap_.end())
			{
				DEBUG_LOG("Createb2WeldJoint Error: bodyB not found in map");
				return false;
			}

			b2Body* bodyB = iter->second;

			// Now load all the jointDef's properties

			jointDefNode->QueryBoolAttribute("collideConnected", &jointDef.collideConnected);
			GetB2Vec2(jointDefNode, "localAnchorA", &jointDef.localAnchorA);
			GetB2Vec2(jointDefNode, "localAnchorB", &jointDef.localAnchorA);

			b2Vec2 worldAnchor;
			GetB2Vec2(jointDefNode, "worldAnchor", &worldAnchor);

			jointDef.Initialize(bodyA,bodyB, worldAnchor);
			b2Joint* j = world_->CreateJoint(&jointDef);

			// If the joint has an id we will add it to the map of joints
			if(element->Attribute("id") != NULL)
			{
				Ogre::String id = Ogre::String(element->Attribute("id"));
				jointMap_.insert(std::make_pair(id, j));
			}
		}
		else
		{
			DEBUG_LOG("Createb2WeldJoint Error: Element contained no JointDef");
		}
	}
	else
	{
		DEBUG_LOG("Createb2WeldJoint Error: Element is zero");
		return false;
	}

	return true;
}



//=============================================================================
//				PUBLIC static members
//=============================================================================

//=============================================================================
//								ClearShapeVector
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
			if(Getb2FixtureDefAttributes(&fixtureDef,fixtureDefNode))
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
			if(Getb2FixtureDefAttributes(&fixtureDef,fixtureDefNode))
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
	
	DEBUG_LOG("Getb2BodyDefAttributes Error: Element is 0.");
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

				//fixtureDef->filter.maskBits = 0x0001;
				//fixtureDef->filter.categoryBits = 0x0100;
				fixtureDef->filter.groupIndex = STATIC_MAP_GROUP;
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
					DEBUG_LOG("Fixture Creation Error: No point count set while creating polygon.");
					return false;
				}
			}
			else
			{
				DEBUG_LOG("Fixture Creation Error: No shape type defined.");
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

	DEBUG_LOG("Fixture Creation Error: Element is 0.");

	return false;
	
}


//=============================================================================
//								Getb2Vec2
//
/// This function takes an element and attribute as its first two parameters
/// to find the vector you want to load that will be put into the pointer to
/// a b2Vec2 that was also passed.
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
