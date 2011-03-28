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
    
    mBWallShape = new btStaticPlaneShape(btVector3(0,0,1),0);
    mBWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,-10)));
    btRigidBody::btRigidBodyConstructionInfo
      mBWallRBCI(0,mBWallMS,mBWallShape,btVector3(0,0,0));

    mBWallBody = new btRigidBody(mBWallRBCI);
    mWorld->addRigidBody(mBWallBody);    
    mLog->infoStream() << "set up!";
    
		fSetup = true;
		return fSetup;
	}
	
	
	void PhyxEngine::update(unsigned long lTimeElapsed) {
		mWorld->stepSimulation(lTimeElapsed, 7);
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
    mWorld->removeRigidBody(mBWallBody);
    
    delete mFloorBody->getMotionState();
    delete mCeilingBody->getMotionState();
    delete mLWallBody->getMotionState();
    delete mRWallBody->getMotionState();
    delete mBWallBody->getMotionState();
    
    delete mFloorBody;
    delete mCeilingBody;
    delete mLWallBody;
    delete mRWallBody;
    delete mBWallBody;
    
    delete mObstacleShape;
    delete mFloorShape;
    delete mCeilingShape;
    delete mLWallShape;
    delete mRWallShape;
    delete mBWallShape;

    delete mWorld;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfig;
    delete mBroadphase;
		return true;
	}
	
	btDiscreteDynamicsWorld* PhyxEngine::world() { return mWorld; };
}
