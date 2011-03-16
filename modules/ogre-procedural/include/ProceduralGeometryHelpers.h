/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://code.google.com/p/ogre-procedural/

Copyright (c) 2010 Michael Broutin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef PROCEDURAL_GEOMETRYHELPERS_INCLUDED
#define PROCEDURAL_GEOMETRYHELPERS_INCLUDED

#include "OgrePlane.h"
#include "OgreVector2.h"
#include "OgreVector3.h"

namespace Procedural
{
	class Line;
//-----------------------------------------------------------------------
class Circle
{
	private:
	Ogre::Vector2 mCenter;
	Ogre::Real mRadius;
	
	public:

	Circle() {}
	
	Circle(Ogre::Vector2 center, Ogre::Real radius) : mCenter(center), mRadius(radius)
	{}
	
	Circle(Ogre::Vector2 p1, Ogre::Vector2 p2, Ogre::Vector2 p3);	
	
	bool isPointInside(const Ogre::Vector2& p) const
	{
		return (p-mCenter).length() < mRadius;
	}
};
//-----------------------------------------------------------------------
class Plane : public Ogre::Plane
{
public:
	Plane() : Ogre::Plane() {}

	Plane(const Ogre::Vector3& normal, const Ogre::Vector3& pos) : Ogre::Plane(normal, pos) {}

	/**
	 * Checks whether 2 planes intersect and compute intersecting line if it is the case.
	 * @arg other the other plane with which to check for intersection
	 * @arg outputLine the intersecting line, if planes actually intersect
	 * @returns true if planes intersect, false otherwise
	 */
	bool intersect(const Plane& other, Line& outputLine) const;
};
//-----------------------------------------------------------------------
class Line
{
	Ogre::Vector3 mPoint;
	Ogre::Vector3 mDirection;

public:
	Line() {}
	Line(Ogre::Vector3 point, Ogre::Vector3 direction) : mPoint(point), mDirection(direction.normalisedCopy()) {}

	void setFrom2Points(const Ogre::Vector3& a, const Ogre::Vector3& b)
	{
		mPoint = a;
		mDirection = (b-a).normalisedCopy();
	}
	Ogre::Vector3 shortestPathToPoint(const Ogre::Vector3& point) const;
};
//-----------------------------------------------------------------------
class Segment2D
{
	Ogre::Vector2 mA;
	Ogre::Vector2 mB;
	public:
	Segment2D(Ogre::Vector2 a, Ogre::Vector2 b) : mA(a), mB(b) {}

	/**
	 * Computes the interesction between two segments [p1p2] and [p3p4]
	 * @arg p1 Point p1
	 * @arg p2 Point p2
	 * @arg p3 Point p3
	 * @arg p4 Point p4
	 * @arg intersection the point of intersection if outputed there if it exists
	 * @return true if segments intersect, false otherwise
	 */
	bool intersect(const Segment2D& other, Ogre::Vector2& intersection) const;

};
}
#endif