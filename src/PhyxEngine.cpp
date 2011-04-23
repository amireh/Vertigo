/*
 *  PhysicsEngine.cpp
 *  Vertigo
 *
 *  Created by Ahmad Amireh on 3/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "PhyxEngine.h"
#include "Level.h"
#include "Sphere.h"
#include "MotionState.h"
#include "Procedural.h"
#include "GfxEngine.h"

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
		
		mUpdater = &PhyxEngine::updateNothing;
	}
	
	PhyxEngine::~PhyxEngine() {
		mLog->infoStream() << "shutting down";
		if (fSetup) {
			delete mLog;
			mLog = 0;
			
			fSetup = false;
		}
		
		if (_myPhyxEngine)
		  _myPhyxEngine = NULL;
	}
	
  void PhyxEngine::myTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    //printf("The world just ticked by %f seconds\n", (float)timeStep);
    btRigidBody* mShipBody = Level::getSingletonPtr()->getSphere()->getRigidBody();
    
    // mShipBody is the spaceship's btRigidBody
    btVector3 velocity = mShipBody->getLinearVelocity();
    btScalar speed = velocity.length();
    if(speed > _myPhyxEngine->getMaxSpeed()) {
        velocity *= _myPhyxEngine->getMaxSpeed()/speed;
        mShipBody->setLinearVelocity(velocity);
    }
    
    // regulate the velocity of all obstacles
    /*std::list<Obstacle*> mObstacles = Level::getSingletonPtr()->getObstacles();
    std::list<Obstacle*>::const_iterator _itr;
    for (_itr = mObstacles.begin(); _itr != mObstacles.end(); ++_itr) {
      velocity = (*_itr)->getRigidBody()->getLinearVelocity();
      if(velocity.length() > _myPhyxEngine->getMaxSpeed()) {
          velocity *= (*_itr)->getMaxSpeed()/velocity.length();
          (*_itr)->getRigidBody()->setLinearVelocity(velocity);
      }
      
    }*/
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

    mWorld->setGravity(btVector3(0,-10,0));

    Ogre::SceneManager* mSceneMgr = GfxEngine::getSingletonPtr()->getSM();
    
    /*mDbgdraw = new BtOgre::DebugDrawer(mSceneMgr->getRootSceneNode(), mWorld);
    mWorld->setDebugDrawer(mDbgdraw);
    mDbgdraw->setDebugMode(true);
    */
    int wallsCollideWith = COL_SPHERE | COL_OBSTACLES;
    
    //mObstacleShape = new btSphereShape(12);
    /*
    mFloorShape = new btStaticPlaneShape(btVector3(0,1,0),0);
    mFloorMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-100,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mFloorRBCI(0,mFloorMS,mFloorShape,btVector3(0,0,0));
    mFloorRBCI.m_friction = 1.0f;
    
    mFloorBody = new btRigidBody(mFloorRBCI);
    //mWorld->addRigidBody(mFloorBody);
    
    
    mCeilingShape = new btStaticPlaneShape(btVector3(0,-1,0),0);
    mCeilingMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,100,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mCeilingRBCI(0,mCeilingMS,mCeilingShape,btVector3(0,0,0));
    mCeilingBody = new btRigidBody(mCeilingRBCI);
    //mWorld->addRigidBody(mCeilingBody);    
    

    mLWallShape = new btStaticPlaneShape(btVector3(1,0.5f,0),0);
    mLWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(-35,0,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mLWallRBCI(0,mLWallMS,mLWallShape,btVector3(0,0,0));

    mLWallBody = new btRigidBody(mLWallRBCI);
    //mWorld->addRigidBody(mLWallBody);
    
    // right wall
    mRWallShape = new btStaticPlaneShape(btVector3(-1,0.5f,0),0);
    mRWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(30,0,0)));
    btRigidBody::btRigidBodyConstructionInfo
      mRWallRBCI(0,mRWallMS,mRWallShape,btVector3(0,0,0));

    mRWallBody = new btRigidBody(mRWallRBCI);
    //mWorld->addRigidBody(mRWallBody);
    
    mBWallShape = new btStaticPlaneShape(btVector3(0,0,1),0);
    mBWallMS = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,-100)));
    btRigidBody::btRigidBodyConstructionInfo
      mBWallRBCI(0,mBWallMS,mBWallShape,btVector3(0,0,0));

    mBWallBody = new btRigidBody(mBWallRBCI);
    //mWorld->addRigidBody(mBWallBody);    
    
    */
    //----------------------------------------------------------
    // Ground!
    //----------------------------------------------------------

    //Create Ogre stuff.
    //MeshManager::getSingleton().createPlane("groundPlane", "General", Plane(Vector3::UNIT_Y, 0), 100, 100,
    //10, 10, true, 1, 5, 5, Vector3::UNIT_Z);

    if (!mSceneMgr->hasSceneNode("PhysicsTunnelNode")) {
      
      Procedural::Root::getInstance()->sceneManager = mSceneMgr;
      
      Procedural::TubeGenerator()
      .setOuterRadius(80)
      .setInnerRadius(78.0f)
      .setHeight(500)
      .setNumSegBase(32)
      .setNumSegHeight(1)
      .realizeMesh("PhysicsTunnelMesh");
      
      //Create the ground shape.
      mTunnelEntity = mSceneMgr->createEntity("PhysicsTunnelEntity", "PhysicsTunnelMesh");
      Ogre::SceneNode *tmpNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PhysicsTunnelNode");
      tmpNode->attachObject(mTunnelEntity);
      tmpNode->pitch(Ogre::Degree(90));
	    tmpNode->setPosition(Vector3(0, 70, 0 ));  
	    tmpNode->setVisible(false);  
	  } else {
	    mTunnelEntity = mSceneMgr->getEntity("PhysicsTunnelEntity");
	  }
	  
    BtOgre::StaticMeshToShapeConverter converter(mTunnelEntity);
    mTunnelShape = converter.createTrimesh();
    mTunnelShape->setLocalScaling(BtOgre::Convert::toBullet(Vector3(1.0f,100.0f,1.0f)));
    
    
    //Create MotionState (no need for BtOgre here, you can use it if you want to though).
    btDefaultMotionState* tunnelState = new btDefaultMotionState(
      btTransform(btQuaternion(1,0,0,1),btVector3(0,70,0)));

    //Create the Body.
    btRigidBody::btRigidBodyConstructionInfo
      mTunnelRBCI(0,tunnelState,mTunnelShape,btVector3(0,0,0));
    mTunnelRBCI.m_friction = 1000.0f;    
    mTunnelBody = new btRigidBody(mTunnelRBCI);
    
    mWorld->addRigidBody(mTunnelBody);	    

    mLog->infoStream() << "set up!";
    
    mUpdater = &PhyxEngine::updateGame;
    
		fSetup = true;
		return fSetup;
	}
	
	void PhyxEngine::updateNothing(unsigned long lTimeElapsed) {
	
	};
	
	void PhyxEngine::updateGame(unsigned long lTimeElapsed) {
		mWorld->stepSimulation((float)(lTimeElapsed / 1000.0f), 7);
		
		//mWorld->debugDrawWorld();

    
    //mDbgdraw->step();
	}
	
	void PhyxEngine::update(unsigned long lTimeElapsed) {
	  (this->*mUpdater)(lTimeElapsed);
	};
	
	bool PhyxEngine::deferredSetup() {
		mSphere = Level::getSingleton().getSphere();
		

    
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
		if (!fSetup)
		  return true;
		
		mWorld->setInternalTickCallback(0);
		mWorld->removeRigidBody(mTunnelBody);
		delete mTunnelBody->getMotionState();
		delete mTunnelBody;
		delete mTunnelShape;
    /*mWorld->removeRigidBody(mFloorBody);
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
    delete mBWallShape;*/

    delete mWorld;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfig;
    delete mBroadphase;
    
    fSetup = false;
    mUpdater = &PhyxEngine::updateNothing; 
    
		return true;
	}
	
	btDiscreteDynamicsWorld* PhyxEngine::world() { return mWorld; };
	
	void PhyxEngine::pauseDynamics() {
	  mUpdater = &PhyxEngine::updateNothing;
	};
	
	void PhyxEngine::resumeDynamics() {
	  mUpdater = &PhyxEngine::updateGame;
	};
}
