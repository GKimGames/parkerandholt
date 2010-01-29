/*=============================================================================

		ManualObjectHelper.h

		Author: Matt King

		This class makes it easy to make manual objects from Box2D bodys and
		shapes

=============================================================================*/

#ifndef MANUAL_OBJECT_HELPER_H
#define	MANUAL_OBJECT_HELPER_H

#include <Ogre.h>
#include <Box2D\Box2D.h>

static void DrawBody(Ogre::ManualObject* man,
					 b2Body* body,
					 Ogre::String materialName,
					 Ogre::ColourValue color);

static void MakeManualb2PolygonShape(Ogre::ManualObject* man, 
						 b2PolygonShape* polygon,
						 Ogre::String materialName,
						 Ogre::ColourValue color);

static void MakeManualb2CircleShape(Ogre::ManualObject* man, 
							 b2CircleShape* circle,
							 Ogre::String materialName,
							 Ogre::ColourValue color);

void MakeManualb2PolygonShape(Ogre::ManualObject* man, 
						 b2PolygonShape* polygon,
						 Ogre::String materialName,
						 Ogre::ColourValue color)
{

	// Begin new polygon section.
	man->begin(materialName, Ogre::RenderOperation::OT_LINE_STRIP);

	int32 vertexCount = polygon->GetVertexCount();

	// We estimate the vertex count beforehand to optimize allocation.
	// Note that we add one since Box2D does not count overlap vertex.
	man->estimateVertexCount(vertexCount+1);

	// Place the vertices.
	for (int32 i = 0; i < vertexCount; ++i)
	{
		man->position(polygon->GetVertex(i).x, polygon->GetVertex(i).y, 0);
		man->colour(color);
	}

	// Place the overlap vertex at position 0.
	man->position(polygon->GetVertex(0).x, polygon->GetVertex(0).y, 0);
	man->colour(color);

	// We're done drawing this b2PolygonShape
	man->end();
}



void MakeManualb2CircleShape(Ogre::ManualObject* man, 
							 b2CircleShape* circle,
							 Ogre::String materialName,
							 Ogre::ColourValue color)
{
	if(circle != 0)
	{
		// circle variables
		const float k_segments = 32.0f;
		const float k_increment = 2.0f * b2_pi / k_segments;
		float theta = 0.0f;

		float32 radius = circle->m_radius;
		b2Vec2 center = circle->m_p;

		// Begin new circle section.
		man->begin(materialName, Ogre::RenderOperation::OT_LINE_STRIP);

		// We estimate the vertex count beforehand to optimize allocation.
		// Note that we add one since Box2D does not count overlap vertex.
		man->estimateVertexCount(k_segments+1);

		// Update object data.
		b2Vec2 circlePoint;
		for (int32 i = 0; i < k_segments; ++i)
		{
			circlePoint = center + radius * b2Vec2(cosf(theta), sinf(theta));
			man->position(circlePoint.x, circlePoint.y, 0);
			man->colour(color);
			theta += k_increment;
		}

		// Place the overlap vertex at the start position.
		circlePoint = center + b2Vec2(radius, 0.0f);
		man->position(circlePoint.x, circlePoint.y, 0);
		man->colour(color);

		// We're done drawing this b2CircleShape
		man->end();
	}
}



void DrawBody(Ogre::ManualObject* man, b2Body* body, Ogre::String materialName, Ogre::ColourValue color)
{
	
	b2Fixture* fixture = body->GetFixtureList();

	b2Shape* shape;
	b2PolygonShape* polygon = 0;
	b2CircleShape* circle = 0;

	// While we still have fixtures we draw them.
	while(fixture != 0)
	{
		// Get the fixture's shape so we can draw the shape
		shape = fixture->GetShape();

		if(shape->GetType() == b2Shape::e_polygon)
		{
			polygon = (b2PolygonShape*) shape;
			MakeManualb2PolygonShape(man, polygon, materialName, color);
			
		}
		else if(shape->GetType() == b2Shape::e_circle)
		{
			circle = (b2CircleShape*) circle;	
			MakeManualb2CircleShape(man, circle, materialName, color);
		}

		// Get the next fixture in the this bodies list of fixtures.
		fixture = fixture->GetNext();
	}
}

#endif 