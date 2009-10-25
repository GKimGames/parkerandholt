/*=====================================================<
**
*> Box2D debug drawing implementation using Ogre graphics.
*> Copyright (c) 2009, Stephan Aelmore
**
*> This code is provided 'as-is', without any express or implied warranty.
*> In no event will the author be held liable for any damages arising from the
*> use of this code.
**
*> Permission is granted to anyone to use this code for any purpose,
*> including commercial applications, and to alter it and redistribute it
*> freely, subject to the following restrictions:
**
*> 1. If you use this code or a derivation thereof, you must cite the origin
*> of the code, giving credit to the author for the original work.
**
*> 2. If you alter the source in any way (the most likely circumstance, given
*> the nature of the code), then you must mark it as such, and not misrepresent
*> the altered work as the original code.
**
>=====================================================*/

#include "cDebugDraw.h"

cDebugDraw::cDebugDraw(Ogre::Root* root, Ogre::RenderTarget* target, int ZOrder) :
b2DebugDraw(),
m_root(root),
m_target(target)
{
	// Create a default scene manager and add a camera
	m_debugScene = m_root->createSceneManager(Ogre::ST_GENERIC, "DebugSceneManager");
	m_debugCam = m_debugScene->createCamera("debugCam");

	// Create a viewport on window with Z-Order 10, as it's
	// supposed to render on top of the normal scene.
	m_viewport = target->addViewport(m_debugCam, ZOrder);

	m_viewport->setBackgroundColour(Ogre::ColourValue::ZERO);

	// We don't want the color buffer to clear each time we
	// update the viewport, as we want drawn shapes to accumulate in
	// the framebuffer; but we clear the depth buffer so shapes are
	// drawn on top of each other (rather than in each other, which looks nasty).
	m_viewport->setClearEveryFrame(true, Ogre::FBT_DEPTH);

	// Default position and orientation, feel free to change these.
	// note that if your camera moves a lot then you will use CopyCamera
	// and this won't matter much (just use CopyCamera right after creating
	// the cDebugDraw and it won't matter at all!)
	m_debugCam->setPosition( Ogre::Vector3(0,0,10) );
	m_debugCam->lookAt( Ogre::Vector3(0,0,-300) );

	m_debugCam->setNearClipDistance(1);
	m_debugCam->setAspectRatio(
		Ogre::Real( m_viewport->getActualWidth() ) / Ogre::Real( m_viewport->getActualHeight() ));

	// Create the manual objects.
	m_shape = m_debugScene->createManualObject("debugShape");
	m_debugScene->getRootSceneNode()->createChildSceneNode("debugNode")->attachObject(m_shape);
	m_edge = m_debugScene->createManualObject("debugEdge");
	m_debugScene->getSceneNode("debugNode")->attachObject(m_edge);

	// Make them dynamic since we will be rewriting the data many times each frame.
	m_shape->setDynamic(true);
	m_edge->setDynamic(true);
}
cDebugDraw::~cDebugDraw()
{
	// In destroying the scene we destroy the manual objects and node.
	m_root->destroySceneManager(m_debugScene);
	// Remove the viewport from the target.
	m_target->removeViewport( m_viewport->getZOrder() );
}

void cDebugDraw::BuildPolygon(Ogre::ManualObject* man, const b2Vec2* vertices,
											 int32 vertexCount, const b2Color& color, float alpha)
{
	// We estimate the vertex count beforehand to optimize allocation.
	// Note that we add one since Box2D does not count overlap vertex.
	man->estimateVertexCount(vertexCount+1);

	// Place the vertices.
	for (int32 i = 0; i < vertexCount; ++i)
	{
		man->position(vertices[i].x, vertices[i].y, 0);
		man->colour(color.r, color.g, color.b, alpha);
	}
	// Place the overlap vertex at position 0.
	man->position(vertices[0].x, vertices[0].y, 0);
	man->colour(color.r, color.g, color.b, alpha);
}

void cDebugDraw::BuildCircle(Ogre::ManualObject* man, const b2Vec2& center,
											float32 radius, const b2Color& color, float alpha)
{
	// circle variables
	const float k_segments = 32.0f;
	const float k_increment = 2.0f * b2_pi / k_segments;
	float theta = 0.0f;

	// We estimate the vertex count beforehand to optimize allocation.
	// Note that we add one since Box2D does not count overlap vertex.
	man->estimateVertexCount(k_segments+1);

	// Update object data.
	b2Vec2 v;
	for (int32 i = 0; i < k_segments; ++i)
	{
		v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		man->position(v.x, v.y, 0);
		man->colour(color.r, color.g, color.b, alpha);
		theta += k_increment;
	}
	// Place the overlap vertex at the start position.
	v = center + b2Vec2(radius, 0.0f);
	man->position(v.x, v.y, 0);
	man->colour(color.r, color.g, color.b, alpha);
}

void cDebugDraw::BuildSegment(Ogre::ManualObject* man, const b2Vec2& p1,
											 const b2Vec2& p2, const b2Color& color, float alpha)
{
	// We estimate the vertex count beforehand to optimize allocation.
	man->estimateVertexCount(2);

	// Place the first vertex.
	man->position(p1.x, p1.y, 0);
	man->colour(color.r, color.g, color.b, alpha);

	// Place the second vertex.
	man->position(p2.x, p2.y, 0);
	man->colour(color.r, color.g, color.b, alpha);
}

void cDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	// Open edge data.
	m_edge->begin("debugDraw", Ogre::RenderOperation::OT_LINE_STRIP);

	// Call helper method to generate vertex data.
	BuildPolygon(m_edge, vertices, vertexCount, color);

	// Finalize the edge.
	m_edge->end();

	// Send the object to the framebuffer.
	m_viewport->update();

	// Clear the object data.
	m_edge->clear();
}

void cDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	// Open edge data.
	m_edge->begin("debugDraw", Ogre::RenderOperation::OT_LINE_STRIP);

	// Call helper method to generate vertex data.
	BuildPolygon(m_edge, vertices, vertexCount, color);

	// Finalize the edge.
	m_edge->end();

	// Open shape data.
	m_shape->begin("debugDraw", Ogre::RenderOperation::OT_TRIANGLE_FAN);

	// Call helper method to generate vertex data.
	BuildPolygon(m_shape, vertices, vertexCount, color, 0.2f);

	// Finalize the shape.
	m_shape->end();

	// Send the objects to the framebuffer.
	m_viewport->update();

	// Clear the object data.
	m_edge->clear();
	m_shape->clear();
}

void cDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	// Open edge data.
	m_edge->begin("debugDraw", Ogre::RenderOperation::OT_LINE_STRIP);

	// Call helper method to generate vertex data.
	BuildCircle(m_edge, center, radius, color);

	// Finalize the edge.
	m_edge->end();

	// Send the object to the framebuffer.
	m_viewport->update();

	// Clear the object data.
	m_edge->clear();
}

void cDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	// Open edge data.
	m_edge->begin("debugDraw", Ogre::RenderOperation::OT_LINE_STRIP);

	// Call helper method to generate vertex data.
	BuildCircle(m_edge, center, radius, color);

	// Finalize the edge.
	m_edge->end();

	// Open shape data
	m_shape->begin("debugDraw", Ogre::RenderOperation::OT_TRIANGLE_FAN);

	// Call helper method to generate vertex data.
	BuildCircle(m_shape, center, radius, color, 0.2f);

	// Finalize the shape.
	m_shape->end();

	// Send the objects to the framebuffer.
	m_viewport->update();

	// Clear the object data.
	m_edge->clear();
	m_shape->clear();
}

void cDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	// Open edge data
	m_edge->begin("debugDraw", Ogre::RenderOperation::OT_LINE_LIST);

	// Call helper method to generate vertex data.
	BuildSegment(m_edge, p1, p2, color);

	// Finalize the edge.
	m_edge->end();

	// Send the object to the framebuffer.
	m_viewport->update();

	// Clear the object data.
	m_edge->clear();
}

void cDebugDraw::DrawXForm(const b2XForm& xf)
{
	// XForm data
	b2Vec2 p1 = xf.position, p2;
	const float32 k_axisScale = 1.0f;

	// Open edge data
	m_edge->begin("debugDraw", Ogre::RenderOperation::OT_LINE_LIST);

	// Generate a red segment.
	p2 = p1 + k_axisScale * xf.R.col1;
	BuildSegment(m_edge, p1, p2, b2Color(1,0,0));

	// Generate a green segment.
	p2 = p1 + k_axisScale * xf.R.col2;
	BuildSegment(m_edge, p1, p2, b2Color(0,1,0));

	// Finalize the edge.
	m_edge->end();

	// Send the object to the framebuffer.
	m_viewport->update();

	// Clear the object data.
	m_edge->clear();
}