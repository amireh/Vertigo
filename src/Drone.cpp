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
 
#include <cstdlib>
#include "Drone.h"
#include "Utility.h"
#include "GfxEngine.h"
#include "Level.h"
#include "PhyxEngine.h"
#include "SfxEngine.h"
#include "Geometry.h"

namespace Pixy
{
	
	bool Drone::ourMeshCreated = false;
	btCollisionShape* Drone::ourShape = NULL;
	 
  Drone::Drone() {
	  mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Drone");
    //mLog->debugStream() << "created";

    mName = "Drone";
	  mMesh = "DroneMesh";
	  mMoveSpeed = 0;
	  mMaxSpeed = 0;
	  fHasFullFx = GameManager::getSingleton().getSettings()["Visual Detail"] == "Full";
    mDuetteTwin = 0;
    
    // set a default class
	  mClass = CHASE;
	  
	  if (!ourMeshCreated) {
	    Geometry::createSphere("DroneMesh", 12, 16, 16);
	    ourMeshCreated = true;
	  }
	  
    if (!ourShape)
      ourShape = new btSphereShape(12);

	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
	  
	  // prepare our renderable component
	  GfxEngine::getSingletonPtr()->attachToScene(this);
    mMasterNode = GfxEngine::getSingletonPtr()->getSM()->createSceneNode();
    mMasterNode->addChild(mSceneNode);
	  
	  // prepare our visual effects; particle systems
	  mBlaze = 0;
	  mMortar = 0;
	  mSteam = 0;
	  mTide = 0;

    {
	    ParticleUniverse::ParticleSystemManager* fxMgr = 
      ParticleUniverse::ParticleSystemManager::getSingletonPtr();
      
      mBlaze = fxMgr->createParticleSystem(
        Ogre::String("FxBlaze" + stringify(idObject)),
        "Vertigo/FX/Blaze",
        GfxEngine::getSingletonPtr()->getSM());
      mBlaze->prepare();
      
      mTide = fxMgr->createParticleSystem(
        Ogre::String("FxTide" + stringify(idObject)),
        "Vertigo/FX/Tide",
        GfxEngine::getSingletonPtr()->getSM());
      mTide->prepare();
      
      mMortar = fxMgr->createParticleSystem(
        Ogre::String("FxMortar" + stringify(idObject)),
        "Vertigo/FX/Mortar",
        GfxEngine::getSingletonPtr()->getSM());
      mMortar->prepare();
      
      mSteam = fxMgr->createParticleSystem(
        Ogre::String("FxSteam" + stringify(idObject)),
        "Vertigo/FX/Steam",
        GfxEngine::getSingletonPtr()->getSM());
      mSteam->prepare();
      
      mMasterNode->attachObject(mBlaze);
      mMasterNode->attachObject(mSteam);
      mMasterNode->attachObject(mMortar);
      mMasterNode->attachObject(mTide);
    }
    
    // prepare our sound effects
    {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
      mSfxExplosion = mSoundMgr->createSound(Ogre::String("Explosion" + stringify(idObject)), "explosion.wav", false, false, true) ;
      mSfxShatter = mSoundMgr->createSound(Ogre::String("Shatter" + stringify(idObject)), "shatter3.wav", false, false, true) ;
      
      mSfxExplosion->disable3D(true);
      mSfxShatter->disable3D(true);
      
      //mLog->debugStream() << "created sound effect";

      if (mShield == FIRE)
        mSfx = &mSfxExplosion;
      else
        mSfx = &mSfxShatter;
    }
	 
	  btTransform trans = btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0));
	      
    mPhyxShape = ourShape;
	  mPhyxMS = new MotionState(trans, mMasterNode);
    btScalar mass = 1;
    btVector3 fallInertia(0,0,1);
	
    mPhyxShape->calculateLocalInertia(mass,fallInertia);
        
	  btRigidBody::btRigidBodyConstructionInfo
		  mPhyxBodyCI(mass,mPhyxMS,mPhyxShape,fallInertia);
        
	  mPhyxBody = new btRigidBody(mPhyxBodyCI);
    mPhyxBody->proceedToTransform(trans);
	      
    setClass(CHASE);
    
	  fDead = true;
  };

	Drone::~Drone()
	{
    //mLog->infoStream() << "destructed";

		mMasterNode = NULL;

		GfxEngine::getSingletonPtr()->detachFromScene(this);	
		
		delete mPhyxBody->getMotionState();
    delete mPhyxBody;
    
    // clean up our sound effects
    {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
        mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
      
      mSoundMgr->destroySound(mSfxExplosion);
      mSoundMgr->destroySound(mSfxShatter);
      
      mSoundMgr = 0;
      mSfxExplosion = 0;
      mSfxShatter = 0;
    }
    
	  // clean up our particle systems
	  {
      ParticleUniverse::ParticleSystemManager* fxMgr = 
        ParticleUniverse::ParticleSystemManager::getSingletonPtr();
      
      fxMgr->destroyParticleSystem(mBlaze,GfxEngine::getSingletonPtr()->getSM());
      fxMgr->destroyParticleSystem(mSteam,GfxEngine::getSingletonPtr()->getSM());
      fxMgr->destroyParticleSystem(mTide,GfxEngine::getSingletonPtr()->getSM());
      fxMgr->destroyParticleSystem(mMortar,GfxEngine::getSingletonPtr()->getSM());
      
      mBlaze = 0;
      mSteam = 0;
      mMortar = 0;
      mTide = 0;
      fxMgr = 0;
	  }
	  
		if (ourShape) {
		  mLog->debugStream() << "deleted our physics shape";
		  delete ourShape;
		  ourShape = NULL;
		}
    
		if (mLog) {
			delete mLog;
			mLog = 0;
		}
	};
	
	Vector3 Drone::randomPosition() {
	  int qualifier = rand();
	  
	  // move to the left or right?
	  int sign = (qualifier % 2 == 0) ? 1 : -1;

    // fixed distance from the probe
	  float z = mProbe->getPosition().z + 1200;

    // clamp to our portal position if it was further
    // & reduce our speed if we're this close to the portal
	  float portalZ = Level::getSingleton().getTunnel()->getExitPortal()->getPosition().z;
	  if (z > portalZ) {
	    z = portalZ;
	    mMoveSpeed /= 2;
	  }
	  
	  if (Level::getSingleton().currentZone()->getSettings().fFixedSpawnPosition)
	    return Vector3(0,20,z);
	  else
	    return Vector3( (qualifier % 30) * sign, (qualifier % 70), z); 
	  
	}
	void Drone::live() {

	  
    mProbe = Level::getSingleton().getProbe();
	  fHasFx = Level::getSingleton().areFxEnabled();
	  fHasFullFx = GameManager::getSingleton().getSettings()["Visual Detail"] == "Full";
	  fHasSfx = Level::getSingleton().areSfxEnabled();
    
	  // parse zone settings
	  Zone* tZone = Level::getSingleton().currentZone();
	  
	  mMoveSpeed = tZone->getSettings().mOMoveSpeed;
	  mMaxSpeed = mMoveSpeed * tZone->getSettings().mOMaxSpeedFactor;
	  mMaxSpeed += mMaxSpeed * (tZone->currentTunnelNr() * tZone->getSettings().mOMaxSpeedStep);
	  
	  mDirection = Vector3::ZERO;
	  
	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;

    mSfx = (mShield == FIRE) ? &mSfxExplosion : &mSfxShatter;
        
	  mPosition = randomPosition();
	  mMasterNode->setPosition(mPosition); 
	  
	  PhyxEngine::getSingletonPtr()->attachToWorld(this);
	  
	  mPhyxBody->activate(true);
	  mPhyxBody->clearForces();
    mPhyxBody->proceedToTransform(
      btTransform(
        btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z)
	    )
	  );
    
    mDuetteTwin = 0;
    
    render();
	  GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->addChild(mMasterNode);
	  
	  fDead = false;	  
	  //mLog->debugStream() << mName << idObject << " is alive";
	};
	
	void Drone::die() {
	  
	  //mLog->debugStream() << mName << idObject << " is dead";
	  
	  // detach our physics body
	  mPhyxBody->activate(true);
	  mPhyxBody->clearForces();
	  mPhyxBody->setLinearVelocity(btVector3(0,0,0));
	  
	  PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  
	  // hide our mesh and particle systems
	  if (fHasFx || (mBlaze && mTide && mSteam && mMortar)) {
      mBlaze->stop();
      mTide->stop();
      mMortar->stop();
      mSteam->stop();
	  }
	  	  
	  GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->removeChild(mMasterNode);
	  
	  mUpdater = &Drone::updateNothing;
	  
	  fDead = true;
	};
	
	void Drone::render() {
	  // switch our materials and visual effects based on our shield
		if (mShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Drone/Fire");
		  
		  if (fHasFx) {
	      if (fHasFullFx)
	        mSteam->stop();

		    if (fHasFullFx)
		      mMortar->start();
		    mBlaze->start();
		  }
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Drone/Ice");
		  if (fHasFx) {
	      mBlaze->stop();

        mTide->start();
        if (fHasFullFx)
          mSteam->start();
		  }
		}
		
		mSceneNode->setVisible(true);
		mMasterNode->setVisible(true);
	};
	
	void Drone::copyFrom(const Drone& src) {
	  
	};
	
	void Drone::updateNothing(unsigned long lTimeElapsed) {
	
	};
	
	void Drone::update(unsigned long lTimeElapsed) {
	  updateCommon(lTimeElapsed);
	  
	  (this->*mUpdater)(lTimeElapsed);
	};
	
	void Drone::updateCommon(unsigned long lTimeElapsed) {
    	
	  // have i passed the player? then kill me
    if (mProbe->getPosition().z > mMasterNode->getPosition().z + 100) {
      die();
      return;
    }
    
    // colliding with the player?
    if (mSceneNode->_getWorldAABB().intersects(mProbe->getSceneNode()->_getWorldAABB())) {
      Event* evt = EventManager::getSingleton().createEvt("DroneCollided");
      evt->setAny((void*)this);
      EventManager::getSingleton().hook(evt);
      evt = 0;
      
      if (fHasSfx) {
        (*mSfx)->stop();
        (*mSfx)->play(true);
      }

      return die();
    }	
	};
	
  void Drone::updateChase(unsigned long lTimeElapsed) {
    
    mDirection = mProbe->getPosition() - mMasterNode->getPosition();
    mDirection.normalise();
    
		mPhyxBody->activate(true);
		mPhyxBody->setLinearVelocity(btVector3(
		  mDirection.x * 2, 
		  -1, 
		  mDirection.z * 2
		  ) * mMoveSpeed * lTimeElapsed);

	};
	
	void Drone::updateDumb(unsigned long lTimeElapsed) {
	  
	  //mLog->debugStream() << "I'm dumb and my x direction is : " << mDirection.x << ", y: " << mDirection.y;
	  mPhyxBody->activate(true);
	  mPhyxBody->setLinearVelocity(btVector3(
	    mDirection.x, mDirection.y, mDirection.z * 2
		) * mMoveSpeed * lTimeElapsed);
		
	};

  // not functional, and not used
	void Drone::updateStationary(unsigned long lTimeElapsed) {
 
	  mPhyxBody->activate(true);
		mPhyxBody->applyCentralForce(btVector3(
		  mDirection.x, mDirection.y, mDirection.z
		) * lTimeElapsed * mMoveSpeed);
		
	};	
	
	void Drone::setClass(DRONE_CLASS inClass) {
	  
	  mClass = inClass;
	  switch (mClass) {
	    case CHASE:
	      mUpdater = &Drone::updateChase;
	      break;
	    case DUMB:
	    case DUETTE:
	      mUpdater = &Drone::updateDumb;
	      break;
	  };
	  if (mClass == DUMB)
      mDirection = Vector3(0,-1,-1); // set some default direction
	};

} // end of namespace
