/*
 *  PhysicsEngine.cpp
 *  Vertigo
 *
 *  Created by Ahmad Amireh on 3/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "PhyxEngine.h"
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

    mWorld->setGravity(btVector3(0,-1,0));

    mFloorShape = new btStaticPlaneShape(btVector3(0,1,0),0);
    mFloorMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-30,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mFloorRBCI(0,mFloorMS,mFloorShape,btVector3(0,0,0));
    mFloorRBCI.m_friction = 5.0f;
    
    mFloorBody = new btRigidBody(mFloorRBCI);
    mWorld->addRigidBody(mFloorBody);
    


    mCeilingShape = new btStaticPlaneShape(btVector3(0,-1,0),0);
    mCeilingMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,30,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mCeilingRBCI(0,mCeilingMS,mCeilingShape,btVector3(0,0,0));
    mCeilingBody = new btRigidBody(mCeilingRBCI);
    mWorld->addRigidBody(mCeilingBody);    


    
    
    // create our triangular walls now
    btTriangleMesh *mTriMesh = new btTriangleMesh();

    for (int i=-10; i<10000; ++i) {
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
    mWorld->addRigidBody(mLWallBody);
    
    // right wall
    mRWallShape = new btBvhTriangleMeshShape(mTriMesh,true);

    mRWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(-200,-30,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mRWallRBCI(0,mRWallMS,mRWallShape,btVector3(0,0,0));

    mRWallBody = new btRigidBody(mRWallRBCI);
    mWorld->addRigidBody(mRWallBody);
        
		fSetup = true;
		return fSetup;
	}
	
	
	void PhyxEngine::update(unsigned long lTimeElapsed) {
		mWorld->stepSimulation(lTimeElapsed);
		
	}
	
	bool PhyxEngine::deferredSetup() {
		
		return true;
	}
	
	void PhyxEngine::attachToWorld(Entity* inEntity) {
		mWorld->addRigidBody(inEntity->getRigidBody());
	}
	void PhyxEngine::detachFromWorld(Entity* inEntity) {
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
	
}
