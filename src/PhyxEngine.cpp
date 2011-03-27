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
	
  void PhyxEngine::myTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    //printf("The world just ticked by %f seconds\n", (float)timeStep);
    btRigidBody* mShipBody = Intro::getSingletonPtr()->getSphere()->getRigidBody();
    
    // mShipBody is the spaceship's btRigidBody
    btVector3 velocity = mShipBody->getLinearVelocity();
    btScalar speed = velocity.length();
    if(speed > _myPhyxEngine->getMaxSpeed()) {
        velocity *= _myPhyxEngine->getMaxSpeed()/speed;
        mShipBody->setLinearVelocity(velocity);
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
    
    mWorld->setInternalTickCallback(PhyxEngine::myTickCallback);

    mWorld->setGravity(btVector3(0,-1,0));

    int wallsCollideWith = COL_SPHERE | COL_OBSTACLES;
    
    mObstacleShape = new btSphereShape(8);
    
    mFloorShape = new btStaticPlaneShape(btVector3(0,1,0),0);
    mFloorMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mFloorRBCI(0,mFloorMS,mFloorShape,btVector3(0,0,0));
    mFloorRBCI.m_friction = 1.0f;
    
    mFloorBody = new btRigidBody(mFloorRBCI);
    mWorld->addRigidBody(mFloorBody);
    /*
    mFloor = new btCollisionObject();
    mFloor->setCollisionShape(mFloorShape);
    mFloor->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);    
    mWorld->addCollisionObject(mFloor);
    */
    
    mCeilingShape = new btStaticPlaneShape(btVector3(0,-1,0),0);
    mCeilingMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,75,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mCeilingRBCI(0,mCeilingMS,mCeilingShape,btVector3(0,0,0));
    mCeilingBody = new btRigidBody(mCeilingRBCI);
    mWorld->addRigidBody(mCeilingBody);    


    mLWallShape = new btStaticPlaneShape(btVector3(1,0.5f,0),0);

    mLWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(-15,0,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mLWallRBCI(0,mLWallMS,mLWallShape,btVector3(0,0,0));

    mLWallBody = new btRigidBody(mLWallRBCI);
    mWorld->addRigidBody(mLWallBody);
    
    // right wall
    mRWallShape = new btStaticPlaneShape(btVector3(-1,0.5f,0),0);

    mRWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(15,0,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mRWallRBCI(0,mRWallMS,mRWallShape,btVector3(0,0,0));

    mRWallBody = new btRigidBody(mRWallRBCI);
    mWorld->addRigidBody(mRWallBody);
    // create our triangular walls now
    /*btTriangleMesh *mTriMesh = new btTriangleMesh();

    for (int i=-10; i<22500; ++i) {
      btVector3 v0(0,0,i);
      btVector3 v1(160,110,i);
      btVector3 v2(320,0,i);

      mTriMesh->addTriangle(v0,v1,v2);
    }

    mLWallShape = new btBvhTriangleMeshShape(mTriMesh,true);

    mLWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(-300,-35,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mLWallRBCI(0,mLWallMS,mLWallShape,btVector3(0,0,0));

    mLWallBody = new btRigidBody(mLWallRBCI);
    mWorld->addRigidBody(mLWallBody);
    
    // right wall
    mRWallShape = new btBvhTriangleMeshShape(mTriMesh,true);

    mRWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(25,-35,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mRWallRBCI(0,mRWallMS,mRWallShape,btVector3(0,0,0));

    mRWallBody = new btRigidBody(mRWallRBCI);
    mWorld->addRigidBody(mRWallBody);
    */
    mLog->infoStream() << "set up!";
    
		fSetup = true;
		return fSetup;
	}
	
	
	void PhyxEngine::update(unsigned long lTimeElapsed) {
		mWorld->stepSimulation(lTimeElapsed, 7);
		/*
    mWorld->performDiscreteCollisionDetection();
   btCollisionObjectArray arrayBullet = mWorld->getCollisionObjectArray();
   int numManifolds = mWorld->getDispatcher()->getNumManifolds();
   for(int i=0; i<numManifolds; i++) {
      btPersistentManifold* pm = mWorld->getDispatcher()->getManifoldByIndexInternal(i);
      if(pm->getNumContacts() > 0) {
        try {
         btRigidBody* co1 = static_cast<btRigidBody*>(pm->getBody0());
         btRigidBody* co2 = static_cast<btRigidBody*>(pm->getBody1());
         // find out if it's a Sphere and an Obstacle that r colliding...
         // i'm currently doing this by testing the shapes
         if (!co1 || !co2)
           continue;
         
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
            MotionState *ms = dynamic_cast<MotionState*>(co2->getMotionState());
            if (ms)
              mSphere->collide(static_cast<Obstacle*>(Ogre::any_cast<Entity*>(ms->getNode()->getUserAny())));
          } else {
            MotionState *ms = dynamic_cast<MotionState*>(co1->getMotionState());
            if (ms)
              mSphere->collide(static_cast<Obstacle*>(Ogre::any_cast<Entity*>(ms->getNode()->getUserAny())));
          }
        } 
        } catch (...) {
          // do nothing
        }
      mWorld->getDispatcher()->clearManifold(pm);
    }
	}*/
	}
	
	btCollisionShape* PhyxEngine::obstaclesShape() { return mObstacleShape; };
	 
	bool PhyxEngine::deferredSetup() {
		mSphere = Intro::getSingleton().getSphere();
		//mMaxSpeed = mSphere->getMaxSpeed();
		
		return true;
	}
	
	float PhyxEngine::getMaxSpeed() const { return mSphere->getMaxSpeed(); };
	
	void PhyxEngine::attachToWorld(Entity* inEntity) {
		mWorld->addRigidBody(inEntity->getRigidBody());
    inEntity->getRigidBody()->activate(true);
		//mWorld->addCollisionObject(inEntity);
	}
	void PhyxEngine::detachFromWorld(Entity* inEntity) {
		//mWorld->removeCollisionObject(inEntity);
		inEntity->getRigidBody()->activate(false);
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
