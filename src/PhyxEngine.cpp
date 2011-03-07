/*
 *  PhysicsEngine.cpp
 *  Vertigo
 *
 *  Created by Ahmad Amireh on 3/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "PhyxEngine.h"
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <GL/gle.h>

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
		
		mGroundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
		
        mSphereShape = new btSphereShape(1);
		
        mGroundMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
		
        btRigidBody::btRigidBodyConstructionInfo
			mGroundRBCI(0,mGroundMS,mGroundShape,btVector3(0,0,0));
		
        mGroundRB = new btRigidBody(mGroundRBCI);
        mWorld->addRigidBody(mGroundRB);
		
		btDefaultMotionState* mSphereMS = 
			new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,80,0)));
        btScalar mass = 1;
        btVector3 fallInertia(0,0,0);
		
        mSphereShape->calculateLocalInertia(mass,fallInertia);
        
		btRigidBody::btRigidBodyConstructionInfo
			mSphereRBCI(mass,mSphereMS,mSphereShape,fallInertia);
        
		mSphereRB = new btRigidBody(mSphereRBCI);
        mWorld->addRigidBody(mSphereRB);
		
		
		fSetup = true;
		return fSetup;
	}
	
	
	void PhyxEngine::update(unsigned long lTimeElapsed) {
		mWorld->stepSimulation(1/120.f,10);
		
		btTransform trans;
		mSphereRB->getMotionState()->getWorldTransform(trans);
		
		glPushMatrix ();
		// get sphere body position
		glTranslatef (trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
		glColor3f (1.0, 0.0, 0.0);
		glutSolidSphere(5.0, 18, 12);
		
		
	}
	
	bool PhyxEngine::cleanup() {
		mWorld->removeRigidBody(mSphereRB);
        delete mSphereRB->getMotionState();
        delete mSphereRB;
		
        mWorld->removeRigidBody(mGroundRB);
        delete mGroundRB->getMotionState();
        delete mGroundRB;
		
		delete mSphereShape;
		
        delete mGroundShape;
		
		
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