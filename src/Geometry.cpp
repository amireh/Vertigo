/*
 *  This file is part of Vertigo.
 *
 *  Vertigo - a cross-platform arcade game powered by Ogre3D.
 *  Copyright (C) 2011  Ahmad Amireh <ahmad@amireh.net>
 * 
 *  Vertigo is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Vertigo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Vertigo.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
#include "Geometry.h"

namespace Pixy {

	void Geometry::createSphere(const std::string& strName, const float r, const int nRings, const int nSegments)
	{
		using namespace Ogre;
		
		MeshPtr pSphere = MeshManager::getSingleton().createManual(strName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		SubMesh *pSphereVertex = pSphere->createSubMesh();
		
		pSphere->sharedVertexData = new VertexData();
		VertexData* vertexData = pSphere->sharedVertexData;
		
		// define the vertex format
		VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
		size_t currOffset = 0;
		// positions
		vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_POSITION);
		currOffset += VertexElement::getTypeSize(VET_FLOAT3);
		// normals
		vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_NORMAL);
		currOffset += VertexElement::getTypeSize(VET_FLOAT3);
		// two dimensional texture coordinates
		vertexDecl->addElement(0, currOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
		currOffset += VertexElement::getTypeSize(VET_FLOAT2);
		
		// allocate the vertex buffer
		vertexData->vertexCount = (nRings + 1) * (nSegments+1);
		HardwareVertexBufferSharedPtr vBuf = HardwareBufferManager::getSingleton().createVertexBuffer(vertexDecl->getVertexSize(0), vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
		VertexBufferBinding* binding = vertexData->vertexBufferBinding;
		binding->setBinding(0, vBuf);
		float* pVertex = static_cast<float*>(vBuf->lock(HardwareBuffer::HBL_DISCARD));
		
		// allocate index buffer
		pSphereVertex->indexData->indexCount = 6 * nRings * (nSegments + 1);
		pSphereVertex->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(HardwareIndexBuffer::IT_16BIT, pSphereVertex->indexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
		HardwareIndexBufferSharedPtr iBuf = pSphereVertex->indexData->indexBuffer;
		unsigned short* pIndices = static_cast<unsigned short*>(iBuf->lock(HardwareBuffer::HBL_DISCARD));
		
		float fDeltaRingAngle = (Math::PI / nRings);
		float fDeltaSegAngle = (2 * Math::PI / nSegments);
		unsigned short wVerticeIndex = 0 ;
		
		// Generate the group of rings for the sphere
		for( int ring = 0; ring <= nRings; ring++ ) {
			float r0 = r * sinf (ring * fDeltaRingAngle);
			float y0 = r * cosf (ring * fDeltaRingAngle);
			
			// Generate the group of segments for the current ring
			for(int seg = 0; seg <= nSegments; seg++) {
				float x0 = r0 * sinf(seg * fDeltaSegAngle);
				float z0 = r0 * cosf(seg * fDeltaSegAngle);
				
				// Add one vertex to the strip which makes up the sphere
				*pVertex++ = x0;
				*pVertex++ = y0;
				*pVertex++ = z0;
				
				Vector3 vNormal = Vector3(x0, y0, z0).normalisedCopy();
				*pVertex++ = vNormal.x;
				*pVertex++ = vNormal.y;
				*pVertex++ = vNormal.z;
				
				*pVertex++ = (float) seg / (float) nSegments;
				*pVertex++ = (float) ring / (float) nRings;
				
				if (ring != nRings) {
					// each vertex (except the last) has six indices pointing to it
					*pIndices++ = wVerticeIndex + nSegments + 1;
					*pIndices++ = wVerticeIndex;               
					*pIndices++ = wVerticeIndex + nSegments;
					*pIndices++ = wVerticeIndex + nSegments + 1;
					*pIndices++ = wVerticeIndex + 1;
					*pIndices++ = wVerticeIndex;
					wVerticeIndex ++;
				}
			}; // end for seg
		} // end for ring
		
		// Unlock
		vBuf->unlock();
		iBuf->unlock();
		// Generate face list
		pSphereVertex->useSharedVertices = true;
		
		// the original code was missing this line:
		pSphere->_setBounds( AxisAlignedBox( Vector3(-r, -r, -r), Vector3(r, r, r) ), false );
		pSphere->_setBoundingSphereRadius(r);
		// this line makes clear the mesh is loaded (avoids memory leaks)
		//pSphere->load();
		
		using namespace Ogre;
    MeshPtr pMesh = MeshManager::getSingleton().load(strName,
      ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
     // so we can still read it
 
     // build tangent vectors, all our meshes use only one texture coordset 
     // Note: we can build into VES_TANGENT now (SM2+)
 
     unsigned short src, dest;
     if (!pMesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
     {
        pMesh->buildTangentVectors(VES_TANGENT, src, dest);
     }
	}
}
