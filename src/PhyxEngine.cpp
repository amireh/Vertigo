/*
 *  PhysicsEngine.cpp
 *  Vertigo
 *
 *  Created by Ahmad Amireh on 3/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "PhyxEngine.h"
#include "Intro.h"
#include "Sphere.h"
#include "MotionState.h"
//#include <OpenGL/gl.h>
//#include <GLUT/glut.h>
//#include <GL/gle.h>

namespace Pixy {
	PhyxEngine* PhyxEngine::_myPhyxEngine = NULL;
	
	PhyxEngine* PhyxEngine::getSingletonPtr() {
		if( !_myPhyxEngine ) {
		    _myPhyxEngine = new PhyxEngine();
		}
		
		return _myPhyxEngine;
	}
	
	PhyxEngine::PhyxEngine() {
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "PhyxEngine");
		mLog->infoStream() << "firing up";
		fSetup = false;		
	}
	
	PhyxEngine::~PhyxEngine() {
		mLog->infoStream() << "shutting down";
		if (fSetup) {
			delete mLog;
			mLog = 0;
			
			fSetup = false;
		}
	}
	
	bool PhyxEngine::setup() {
		if (fSetup)
			return true;
		
		mBroadphase = new btDbvtBroadphase();
		
    mCollisionConfig = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfig);

    mSolver = new btSequentialImpulseConstraintSolver;

    mWorld = new btDiscreteDynamicsWorld(mDispatcher,mBroadphase,mSolver,mCollisionConfig);
    //mCWorld = new btCollisionWorld(mDispatcher, mBroadphase, mCollisionConfig);
    
    mWorld->setGravity(btVector3(0,-3,0));

    int wallsCollideWith = COL_SPHERE | COL_OBSTACLES;
    
    mObstacleShape = new btSphereShape(12);
    
    mFloorShape = new btStaticPlaneShape(btVector3(0,1,0),0);
    mFloorMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-30,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mFloorRBCI(0,mFloorMS,mFloorShape,btVector3(0,0,0));
    mFloorRBCI.m_friction = 5.0f;
    
    mFloorBody = new btRigidBody(mFloorRBCI);
    mWorld->addRigidBody(mFloorBody, COL_WALLS, wallsCollideWith);
    /*
    mFloor = new btCollisionObject();
    mFloor->setCollisionShape(mFloorShape);
    mFloor->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);    
    mWorld->addCollisionObject(mFloor);
    */
    
    mCeilingShape = new btStaticPlaneShape(btVector3(0,-1,0),0);
    mCeilingMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,30,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mCeilingRBCI(0,mCeilingMS,mCeilingShape,btVector3(0,0,0));
    mCeilingBody = new btRigidBody(mCeilingRBCI);
    mWorld->addRigidBody(mCeilingBody, COL_WALLS, wallsCollideWith);    

    // create our triangular walls now
    btTriangleMesh *mTriMesh = new btTriangleMesh();

    for (int i=-10; i<22500; ++i) {
      btVector3 v0(0,0,i);
      btVector3 v1(80,110,i);
      btVector3 v2(160,0,i);

      mTriMesh->addTriangle(v0,v1,v2);
    }

    mLWallShape = new btBvhTriangleMeshShape(mTriMesh,true);

    mLWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(40,-30,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mLWallRBCI(0,mLWallMS,mLWallShape,btVector3(0,0,0));

    mLWallBody = new btRigidBody(mLWallRBCI);
    mWorld->addRigidBody(mLWallBody, COL_WALLS, wallsCollideWith);
    
    // right wall
    mRWallShape = new btBvhTriangleMeshShape(mTriMesh,true);

    mRWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(-200,-30,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mRWallRBCI(0,mRWallMS,mRWallShape,btVector3(0,0,0));

    mRWallBody = new btRigidBody(mRWallRBCI);
    mWorld->addRigidBody(mRWallBody, COL_WALLS, wallsCollideWith);
        
		fSetup = true;
		return fSetup;
	}
	
	
	void PhyxEngine::update(unsigned long lTimeElapsed) {
		mWorld->stepSimulation(lTimeElapsed, 4, 1.0f / 120.0f);
		
    mWorld->performDiscreteCollisionDetection();
   btCollisionObjectArray arrayBullet = mWorld->getCollisionObjectArray();
   int numManifolds = mWorld->getDispatcher()->getNumManifolds();
   for(int i=0; i<numManifolds; i++) {
      btPersistentManifold* pm = mWorld->getDispatcher()->getManifoldByIndexInternal(i);
      if(pm->getNumContacts() > 0) {
         btRigidBody* co1 = static_cast<btRigidBody*>(pm->getBody0());
         btRigidBody* co2 = static_cast<btRigidBody*>(pm->getBody1());
         // find out if it's a Sphere and an Obstacle that r colliding...
         // i'm currently doing this by testing the shapes
         if ((co1->getCollisionShape() == mSphere->getCollisionShape() &&
             co2->getCollisionShape() == mObstacleShape) ||
             (co2->getCollisionShape() == mSphere->getCollisionShape() &&
             co1->getCollisionShape() == mObstacleShape)
            )
         {
          // now here's the funny part:
          // we grab a handle to the Entity's MotionState using the RigidBody
          // to get a handle of the Ogre::SceneNode which has our Entity casted
          // as void* ... friggin clean ah?
          if (co1->getCollisionShape() == mSphere->getCollisionShape()) {
            MotionState *ms = static_cast<MotionState*>(co2->getMotionState());
            mSphere->collide(static_cast<Obstacle*>(Ogre::any_cast<Entity*>(ms->getNode()->getUserAny())));
          } else {
            MotionState *ms = static_cast<MotionState*>(co1->getMotionState());
            mSphere->collide(static_cast<Obstacle*>(Ogre::any_cast<Entity*>(ms->getNode()->getUserAny())));
          }

      }
      mWorld->getDispatcher()->clearManifold(pm);
    }
	}
	}
	
	btCollisionShape* PhyxEngine::obstaclesShape() { return mObstacleShape; };
	 
	bool PhyxEngine::deferredSetup() {
		mSphere = Intro::getSingleton().getSphere();
		return true;
	}
	
	void PhyxEngine::attachToWorld(Entity* inEntity) {
		mWorld->addRigidBody(inEntity->getRigidBody());
		mWorld->addCollisionObject(inEntity);
	}
	void PhyxEngine::detachFromWorld(Entity* inEntity) {
		mWorld->removeCollisionObject(inEntity);
		mWorld->removeRigidBody(inEntity->getRigidBody());
	}
	bool PhyxEngine::cleanup() {
		
    mWorld->removeRigidBody(mFloorBody);
    mWorld->removeRigidBody(mCeilingBody);
    mWorld->removeRigidBody(mLWallBody);
    mWorld->removeRigidBody(mRWallBody);
    
    delete mFloorBody->getMotionState();
    delete mCeilingBody->getMotionState();
    delete mLWallBody->getMotionState();
    delete mRWallBody->getMotionState();
    
    delete mFloorBody;
    delete mCeilingBody;
    delete mLWallBody;
    delete mRWallBody;
    
    delete mObstacleShape;
    delete mFloorShape;
    delete mCeilingShape;
    delete mLWallShape;
    delete mRWallShape;

    delete mWorld;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfig;
    delete mBroadphase;
		return true;
	}
	/*
	void PhyxEngine::mouseMoved( const OIS::MouseEvent &e )
	{
	}
	
	void PhyxEngine::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
	{
	}
	
	void PhyxEngine::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
	{
	}*/
	
	btDiscreteDynamicsWorld* PhyxEngine::world() { return mWorld; };
}
