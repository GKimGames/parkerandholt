#include "MeshTools.h"
#include "OgreHelper.h"

// Implementation of Monotone Chain Convex Hull algorithm.
using namespace std;
typedef Ogre::Vector3 Point;
 
class MyPredicate
{
public:

	static bool predicate( const Ogre::Vector3& m1, const Ogre::Vector3& m2 )
	{
		return m1.x < m2.x || (m1.x == m2.x && m1.y < m2.y);
	}
};

// 2D cross product.
// Return a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the points are collinear.
double MeshTools::cross(const Point &O, const Point &A, const Point &B)
{
	return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}
 
// Returns a list of points on the convex hull in counter-clockwise order.
// Note: the last point in the returned list is the same as the first one.
std::vector<Point> MeshTools::convexHull(std::vector<Point> P)
{
	
	int n = P.size(), k = 0;
	std::vector<Point> H(2*n);
 
	// Sort points lexicographically
	std::sort(P.begin(), P.end(),MyPredicate::predicate);
 
	// Build lower hull
	for (int i = 0; i < n; i++) {
		while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}
 
	// Build upper hull
	for (int i = n-2, t = k+1; i >= 0; i--) {
		while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}
 
	H.resize(k);
	return H;
}



std::vector<Ogre::Vector3> MeshTools::GetBox2DOutLine(Ogre::Entity* entity)
{
	
	if(!entity->getMesh()->isEdgeListBuilt())
	{
		entity->getMesh()->buildEdgeList();
	}


	size_t vertex_count,index_count;
	Ogre::Vector3* vertices;
	unsigned long* indices;

	GetMeshInformation(entity->getMesh().getPointer(),vertex_count,vertices,index_count,indices);
	Ogre::Vector3 *vertexArray = new Ogre::Vector3[vertex_count];
	vertexArray = vertices;
	
	Ogre::EdgeData* edgeData = entity->getMesh()->getEdgeList(0);
	Ogre::EdgeData::EdgeGroupList::iterator i, iend;
	Ogre::EdgeData::EdgeList::iterator ei, eiend;

	std::vector<Ogre::Vector3> myVecs;

	Ogre::Vector3 normal(0,0,1);
	Ogre::Plane plane(normal, 0);
	Ogre::Ray ray;

	iend = edgeData->edgeGroups.end();
	Ogre::String stringPoints;
	for (i = edgeData->edgeGroups.begin(); i != iend; ++i)
	{
		
		int num = 0;
		eiend = i->edges.end();
		for (ei = i->edges.begin(); ei != eiend; ++ei, ++num)
		{

			Ogre::EdgeData::Edge& e = *ei;

			Ogre::Vector3 start = Ogre::Vector3(vertexArray[e.vertIndex[0]]);
			Ogre::Vector3 end = Ogre::Vector3(vertexArray[e.vertIndex[1]]);
			
			if((start.z < 0 && end.z > 0) || (start.z > 0 && end.z < 0))
			{
				ray.setOrigin(start);
				ray.setDirection(end - start);
				Ogre::Vector3 pointy = ray.getPoint(ray.intersects(plane).second);
				myVecs.push_back(pointy);

				stringPoints += Ogre::StringConverter::toString(pointy.x);
				 //+ " " + Ogre::StringConverter::toString(pointy.y);
				stringPoints += "\\n";
			}
		}
	}

	return myVecs;
}
