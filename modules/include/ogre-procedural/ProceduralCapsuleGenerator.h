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
#ifndef PROCEDURAL_CAPSULE_GENERATOR_INCLUDED
#define PROCEDURAL_CAPSULE_GENERATOR_INCLUDED

#include "ProceduralMeshGenerator.h"
#include "ProceduralPlatform.h"

namespace Procedural
{
/**
 * Generates a capsule mesh, i.e. a sphere-terminated cylinder
 */
class _ProceduralExport CapsuleGenerator : public MeshGenerator<CapsuleGenerator>
{
	Ogre::Real radius;
	unsigned int numRings;
	unsigned int numSegments;
	unsigned int numSegHeight;
	Ogre::Real height;

public:
	CapsuleGenerator() : radius(1.0),
		numRings(8), numSegments(16), height(1.0), numSegHeight(1)
	{}
	
	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;

	/** Sets the radius of the cylinder part (default=1)*/
	inline CapsuleGenerator & setRadius(Ogre::Real radius)
	{
		this->radius = radius;
		return *this;
	}

	/** Sets the number of segments of the sphere part (default=8)*/
	inline CapsuleGenerator & setNumRings(unsigned int numRings)
	{
		this->numRings = numRings;
		return *this;
	}

	/** Sets the number of segments when rotating around the cylinder (default=16)*/
	inline CapsuleGenerator & setNumSegments(unsigned int numSegments)
	{
		this->numSegments = numSegments;
		return *this;
	}

	/** Sets the number of segments along the axis of the cylinder (default=1)*/
	inline CapsuleGenerator & setNumSegHeight(unsigned int numSegHeight)
	{
		this->numSegHeight = numSegHeight;
		return *this;
	}

	/** Sets the height of the cylinder part of the capsule (default=1)*/
	inline CapsuleGenerator & setHeight(Ogre::Real height)
	{
		this->height = height;
		return *this;
	}


};
}
#endif
