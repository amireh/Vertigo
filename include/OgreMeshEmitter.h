#ifndef __MeshEmitter_H__
#define __MeshEmitter_H__
 
#include "OGRE/Plugins/ParticleFX/OgreParticleFXPrerequisites.h"
#include "OgreParticleEmitter.h"
#include "OgreMesh.h"
 
namespace Ogre {
 
    class _OgreParticleFXExport MeshEmitter : public ParticleEmitter
    {
    public:
        /** Command object for random emitters (see ParamCommand).*/
        class CmdRandom : public ParamCommand
        {
        public:
            String doGet(const void* target) const;
            void doSet(void* target, const String& val);
        };
        /** Command object for percentage coverage emitters (see ParamCommand).*/
        class CmdCoverage : public ParamCommand
        {
        public:
            String doGet(const void* target) const;
            void doSet(void* target, const String& val);
        };
        /** Command object for using vertex normal directions (see ParamCommand).*/
        class CmdUseVertexNormals : public ParamCommand
        {
        public:
            String doGet(const void* target) const;
            void doSet(void* target, const String& val);
        };
        /** Command object for emitter mesh (see ParamCommand).*/
        class CmdMesh : public ParamCommand
        {
        public:
            String doGet(const void* target) const;
            void doSet(void* target, const String& val);
        };
 
        MeshEmitter(ParticleSystem* psys);// : AreaEmitter(psys) {}
 
        void genEmissionDirection(Vector3& destVector);
        unsigned short _getEmissionCount(Real timeElapsed);
 
        /** Overloaded to update the trans. matrix */
        void setDirection( const Vector3& direction );
 
        /** See ParticleEmitter. */
        void _initParticle(Particle* pParticle);
 
        void setRandom(bool random);
        bool getRandom(void) const;
 
        void setCoverage(Real coverge);
        Real getCoverage(void) const;
 
        void setUseVertexNormals(bool vertexNormals);
        bool getUseVertexNormals(void) const;
 
        /** Sets the name of mesh of the emitter. */
        void setMesh(String name);
        /** Gets the name of mesh of the emitter. */
        MeshPtr getMesh(void) const;
        String getMeshName(void) const;
 
    protected:
        String meshName;
        MeshPtr mesh;
    bool mRandom;
    bool mUseVertexNormals;
    Real mCoverage;
    size_t vertex_count,index_count;
    Vector3* vertices;
    Vector3* normals;
    unsigned* indices;
 
    int currentVertex;
 
    void getMeshInformation(MeshPtr mesh,size_t &vertex_count,Vector3* &vertices, Vector3* &normals, size_t &index_count, unsigned* &indices, const Vector3 &position = Vector3::ZERO,const Quaternion &orient = Quaternion::IDENTITY,const Vector3 &scale = Vector3::UNIT_SCALE);
 
        /// Command objects
        static CmdRandom msCmdRandom;
        static CmdCoverage msCmdCoverage;
        static CmdUseVertexNormals msCmdUseVertexNormals;
        static CmdMesh msCmdMesh;
 
    };
 
}
 
#endif
