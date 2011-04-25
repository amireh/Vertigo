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
 
#include "PhyxEngine.h"
#include "Level.h"
#include "Probe.h"
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
    btRigidBody* mProbeBody = Level::getSingletonPtr()->getProbe()->getRigidBody();
    
    btVector3 tVelocity = mProbeBody->getLinearVelocity();
    btScalar tSpeed = tVelocity.length();
    if(tSpeed > _myPhyxEngine->_getMaxSpeed()) {
        tVelocity *= _myPhyxEngine->_getMaxSpeed()/tSpeed;
        mProbeBody->setLinearVelocity(tVelocity);
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

    mWorld->setInternalTickCallback(PhyxEngine::myTickCallback);
    mWorld->setGravity(btVector3(0,-10,0));

    Ogre::SceneManager* mSceneMgr = GfxEngine::getSingletonPtr()->getSM();
    
    /*mDbgdraw = new BtOgre::DebugDrawer(mSceneMgr->getRootSceneNode(), mWorld);
    mWorld->setDebugDrawer(mDbgdraw);
    mDbgdraw->setDebugMode(true);
    */

    // PREPARE OUR PHYSICAL GEOMETRY
    // Note: Adapted from btOgre's tutorial
    
    // TODO: why not have every Tunnel create its own physical geometry?
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
	    tmpNode->getParentSceneNode()->removeChild(tmpNode);
	  } else {
	    mTunnelEntity = mSceneMgr->getEntity("PhysicsTunnelEntity");
	  }
	  
    BtOgre::StaticMeshToShapeConverter converter(mTunnelEntity);
    mTunnelShape = converter.createTrimesh();
    mTunnelShape->setLocalScaling(BtOgre::Convert::toBullet(Vector3(1.0f,100.0f,1.0f)));
    
    btDefaultMotionState* tunnelState = new btDefaultMotionState(
      btTransform(btQuaternion(1,0,0,1),btVector3(0,70,0)));

    btRigidBody::btRigidBodyConstructionInfo
      mTunnelRBCI(0,tunnelState,mTunnelShape,btVector3(0,0,0));
    mTunnelRBCI.m_friction = 1000.0f;    
    mTunnelBody = new btRigidBody(mTunnelRBCI);
    
    mWorld->addRigidBody(mTunnelBody);	    

    mLog->infoStream() << "set up!";
    
		fSetup = true;
		return fSetup;
	}
	
	
	void PhyxEngine::update(unsigned long lTimeElapsed) {
		mWorld->stepSimulation(lTimeElapsed, 7);
		
		//mWorld->debugDrawWorld();
    //mDbgdraw->step();
	}
		 
	bool PhyxEngine::deferredSetup() {
		mProbe = Level::getSingleton().getProbe();
		
		return true;
	}
		
	void PhyxEngine::attachToWorld(Entity* inEntity) {
		mWorld->addRigidBody(inEntity->getRigidBody());
    inEntity->getRigidBody()->activate(true);
	}
	void PhyxEngine::detachFromWorld(Entity* inEntity) {
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

    delete mWorld;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfig;
    delete mBroadphase;
    
    fSetup = false;
    
		return true;
	}
	
	float PhyxEngine::_getMaxSpeed() const {
	  return mProbe->getMaxSpeed();
	}
}
