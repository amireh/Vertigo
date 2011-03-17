#include "OgreMeshEmitter.h"
#include "OgreParticle.h"
#include "OgreQuaternion.h"
#include "OgreException.h"
#include "OgreStringConverter.h"
#include "OgreMeshManager.h"
#include "Ogre.h"
 
namespace Ogre {
 
    // Instatiate statics
    MeshEmitter::CmdMesh MeshEmitter::msCmdMesh;
    MeshEmitter::CmdRandom MeshEmitter::msCmdRandom;
    MeshEmitter::CmdCoverage MeshEmitter::msCmdCoverage;
    MeshEmitter::CmdUseVertexNormals MeshEmitter::msCmdUseVertexNormals;
 
    //-----------------------------------------------------------------------
    MeshEmitter::MeshEmitter(ParticleSystem* psys)
        : ParticleEmitter(psys)
    {
        mType = "Mesh";
 
        mDirection = Vector3::UNIT_Z;
        mUp = Vector3::UNIT_Z;
 
    currentVertex = 0;
    mRandom = true;
    mCoverage = 0.0;
    mUseVertexNormals = true;
        if (createParamDictionary("MeshEmitter"))
        {
 
            addBaseParameters();
            ParamDictionary* pDict = getParamDictionary();
 
            pDict->addParameter(ParameterDef("mesh", 
                "Mesh for use as emitter",
                PT_REAL),&msCmdMesh);
            pDict->addParameter(ParameterDef("random", 
                "vertices emit in random order",
                PT_BOOL),&msCmdRandom);
            pDict->addParameter(ParameterDef("coverage", 
                "percentage of vertices emitting",
                PT_BOOL),&msCmdCoverage);
            pDict->addParameter(ParameterDef("use_vertex_normals", 
                "get particle direction from vertex normal",
                PT_BOOL),&msCmdUseVertexNormals);
        }
    }
 
    void MeshEmitter::_initParticle(Particle* pParticle)
    {
        Vector3 offset;
    Vector3 scale;
 
    // Call superclass
        ParticleEmitter::_initParticle(pParticle);
 
    if (mRandom)
        currentVertex = rand() % vertex_count;
    else
    {
        currentVertex++;
        if (currentVertex==vertex_count)
            currentVertex = 0;
    }
    offset = vertices[currentVertex];
 
    scale = mParent->getParentNode()->getScale();
    pParticle->position = mPosition + (offset * scale);
 
 
    // Generate complex data by reference
    genEmissionColour(pParticle->colour);
    genEmissionDirection(pParticle->direction);
    genEmissionVelocity(pParticle->direction);
 
    // Generate simpler data
    pParticle->timeToLive = pParticle->totalTimeToLive = genEmissionTTL();
    }
 
    //-----------------------------------------------------------------------
    void MeshEmitter::setRandom(bool random)
    {
        mRandom = random;
    }
    //-----------------------------------------------------------------------
    bool MeshEmitter::getRandom(void) const
    {
        return mRandom;
    }
    //-----------------------------------------------------------------------
    void MeshEmitter::setCoverage(Real coverage)
    {
        mCoverage = coverage;
    }
    //-----------------------------------------------------------------------
    Real MeshEmitter::getCoverage(void) const
    {
        return mCoverage;
    }
    //-----------------------------------------------------------------------
    void MeshEmitter::setUseVertexNormals(bool vertexNormals)
    {
        mUseVertexNormals = vertexNormals;
    }
    //-----------------------------------------------------------------------
    bool MeshEmitter::getUseVertexNormals(void) const
    {
        return mUseVertexNormals;
    }
    //-----------------------------------------------------------------------
    void MeshEmitter::setMesh(String name)
    {
    meshName = name;
    mesh = MeshManager::getSingleton().load(name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    getMeshInformation(mesh,vertex_count,vertices,normals,index_count,indices);
    }
    //-----------------------------------------------------------------------
    String MeshEmitter::getMeshName(void) const
    {
        return meshName;
    }
    //-----------------------------------------------------------------------
    MeshPtr MeshEmitter::getMesh(void) const
    {
        return mesh;
    }
    //-----------------------------------------------------------------------
    void MeshEmitter::genEmissionDirection(Vector3& destVector)
    {
    if (mUseVertexNormals)
    {
        if (mAngle != Radian(0))
        {
        // Randomise angle
        Radian angle = Math::UnitRandom() * mAngle;
 
        // Randomise direction
        destVector = normals[currentVertex].randomDeviant(angle, mUp);
        }
        else
        {
        // Constant angle
        destVector = normals[currentVertex];
        }
    }
    else
        ParticleEmitter::genEmissionDirection(destVector);
    }
    //-----------------------------------------------------------------------
    unsigned short MeshEmitter::_getEmissionCount(Real timeElapsed)
    {
        // Use basic constant emission 
    unsigned short mod = vertex_count*mCoverage;
    if (mod==0)
        mod = 1;
        return genConstantEmissionCount(timeElapsed)*mod;
    }
    //-----------------------------------------------------------------------
    void MeshEmitter::setDirection( const Vector3& direction )
    {
        ParticleEmitter::setDirection( direction );
 
    }
 
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String MeshEmitter::CmdRandom::doGet(const void* target) const
    {
        return StringConverter::toString(
            static_cast<const MeshEmitter*>(target)->getRandom() );
    }
    void MeshEmitter::CmdRandom::doSet(void* target, const String& val)
    {
        static_cast<MeshEmitter*>(target)->setRandom(StringConverter::parseBool(val));
    }
    //-----------------------------------------------------------------------
    String MeshEmitter::CmdCoverage::doGet(const void* target) const
    {
        return StringConverter::toString(
            static_cast<const MeshEmitter*>(target)->getCoverage() );
    }
    void MeshEmitter::CmdCoverage::doSet(void* target, const String& val)
    {
        static_cast<MeshEmitter*>(target)->setCoverage(StringConverter::parseReal(val));
    }
    //-----------------------------------------------------------------------
    String MeshEmitter::CmdUseVertexNormals::doGet(const void* target) const
    {
        return StringConverter::toString(
            static_cast<const MeshEmitter*>(target)->getUseVertexNormals() );
    }
    void MeshEmitter::CmdUseVertexNormals::doSet(void* target, const String& val)
    {
        static_cast<MeshEmitter*>(target)->setUseVertexNormals(StringConverter::parseBool(val));
    }
    //-----------------------------------------------------------------------
    String MeshEmitter::CmdMesh::doGet(const void* target) const
    {
        return static_cast<const MeshEmitter*>(target)->getMeshName();
    }
    void MeshEmitter::CmdMesh::doSet(void* target, const String& val)
    {
        static_cast<MeshEmitter*>(target)->setMesh(val);
    }
 
    void MeshEmitter::getMeshInformation(MeshPtr mesh,size_t &vertex_count,Vector3* &vertices, Vector3* &normals, size_t &index_count, unsigned* &indices, const Vector3 &position,const Quaternion &orient,const Vector3 &scale)
    {
        vertex_count = index_count = 0;
 
        bool added_shared = false;
        size_t current_offset = vertex_count;
        size_t shared_offset = vertex_count;
        size_t next_offset = vertex_count;
        size_t index_offset = index_count;
        size_t prev_vert = vertex_count;
        size_t prev_ind = index_count;
 
        // Calculate how many vertices and indices we're going to need
        for(int i = 0;i < mesh->getNumSubMeshes();i++)
        {
            SubMesh* submesh = mesh->getSubMesh(i);
 
            // We only need to add the shared vertices once
            if(submesh->useSharedVertices)
            {
            if(!added_shared)
            {
                VertexData* vertex_data = mesh->sharedVertexData;
                vertex_count += vertex_data->vertexCount;
                added_shared = true;
            }
            }
            else
            {
            VertexData* vertex_data = submesh->vertexData;
            vertex_count += vertex_data->vertexCount;
            }
 
            // Add the indices
            Ogre::IndexData* index_data = submesh->indexData;
            index_count += index_data->indexCount;
        }
 
        // Allocate space for the vertices and indices
        vertices = new Vector3[vertex_count];
        normals = new Vector3[vertex_count];
        indices = new unsigned[index_count];
 
        added_shared = false;
 
        // Run through the submeshes again, adding the data into the arrays
        for(int i = 0;i < mesh->getNumSubMeshes();i++)
        {
            SubMesh* submesh = mesh->getSubMesh(i);
 
            Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
            if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
            {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }
 
            const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
            const Ogre::VertexElement* normalElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_NORMAL);
            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
            Ogre::Real* pReal;
 
            for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
 
                Vector3 pt;
 
                pt.x = (*pReal++);
                pt.y = (*pReal++);
                pt.z = (*pReal++);
 
                pt = (orient * (pt * scale)) + position;
 
                vertices[current_offset + j].x = pt.x;
                vertices[current_offset + j].y = pt.y;
                vertices[current_offset + j].z = pt.z;
 
                normalElem->baseVertexPointerToElement(vertex, &pReal);
                pt.x = (*pReal++);
                pt.y = (*pReal++);
                pt.z = (*pReal++);
 
                normals[current_offset + j].x = pt.x;
                normals[current_offset + j].y = pt.y;
                normals[current_offset + j].z = pt.z;
 
            }
            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
            }
 
            Ogre::IndexData* index_data = submesh->indexData;
 
            size_t numTris = index_data->indexCount / 3;
            unsigned short* pShort;
            unsigned int* pInt;
            Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
            bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
            if (use32bitindexes) pInt = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
            else pShort = static_cast<unsigned short*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
 
            for(size_t k = 0; k < numTris; ++k)
            {
            size_t offset = (submesh->useSharedVertices)?shared_offset:current_offset;
 
            unsigned int vindex = use32bitindexes? *pInt++ : *pShort++;
            indices[index_offset + 0] = vindex + offset;
            vindex = use32bitindexes? *pInt++ : *pShort++;
            indices[index_offset + 1] = vindex + offset;
            vindex = use32bitindexes? *pInt++ : *pShort++;
            indices[index_offset + 2] = vindex + offset;
 
            index_offset += 3;
            }
            ibuf->unlock();
            current_offset = next_offset;
        }
    }
 
}
