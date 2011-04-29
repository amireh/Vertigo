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

#include "Probe.h"
#include "Utility.h"
#include "GfxEngine.h"
#include "Level.h"
#include "PhyxEngine.h"
#include "Drone.h"
#include "Geometry.h"
#include "SfxEngine.h"

namespace Pixy
{
	
  Probe::Probe()
  {
    mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Probe");
	  
	  mLog->infoStream() << "creating";
	  
	  mName = "Probe";
	  mMesh = "ProbeMesh";
	  mMoveSpeed = 0; // speed is set in evtPortalEntered
	  mMaxSpeed = 0;
    mScore = 0;
    mSpeedStep = 0;
    mRegenRate = 0;
    
    mUpdate = 0;
    mCollide = 0;
    
	  mCurrentShield = (rand() % 2 == 0) ? FIRE : ICE;
	  mShields[FIRE] = 1000;
	  mShields[ICE] = 1000;
	  
	  mNrMisses = mNrHits = 0;

	  using Ogre::Vector3;
	  int tRadius = 14;
	  
	  // a default direction
	  mDirection = Vector3(0,-1,1);
	  
	  // create our graphical component
	  Geometry::createSphere(mMesh, tRadius, 32, 32); // our mesh
	  GfxEngine::getSingletonPtr()->attachToScene(this);
    mMasterNode = GfxEngine::getSingletonPtr()->getSM()->createSceneNode("ProbeMasterNode");
    mMasterNode->addChild(mSceneNode);

    // preload our materials
    static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Probe/Ice");
    static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Probe/Fire");		  
	  
	  // create our physical component
	  // initial position
    btTransform tTrans = btTransform(btQuaternion(0,0,0,1),btVector3(0,35,30));
	
    mPhyxShape = new btSphereShape(tRadius);
	  mPhyxMS = new MotionState(tTrans, mMasterNode);
    btScalar tMass = 1000;
    btVector3 tFallInertia(0,0,0);
	
    mPhyxShape->calculateLocalInertia(tMass,tFallInertia);
        
	  btRigidBody::btRigidBodyConstructionInfo
		  mPhyxBodyCI(tMass,mPhyxMS,mPhyxShape,tFallInertia);
        
	  mPhyxBody = new btRigidBody(mPhyxBodyCI);
	  mPhyxBody->proceedToTransform(tTrans);
	
	  // prepare our sound effects
    OgreOggSound::OgreOggSoundManager *mSoundMgr;
    mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
    mSfxBeep = mSoundMgr->createSound("ProbeBeep" + stringify(idObject), "beep.wav", false, false, true);
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    mSfxWarning = mSoundMgr->createSound("ProbeWarning" + stringify(idObject), "warning_siren.wav", false, false, true);
#else
    mSfxWarning = mSoundMgr->createSound("ProbeWarning" + stringify(idObject), "warning_siren.ogg", false, false, true);
#endif
    mSfxFlip = mSoundMgr->createSound("ProbeFlipShield" + stringify(idObject), "flip.wav", false, false, true);
    
    mSfxBeep->disable3D(true);
    mSfxBeep->setVolume(0.5f);
    
    mSfxWarning->disable3D(true);
    mSfxWarning->setVolume(1);
    
    mSfxFlip->disable3D(true);
    mSfxFlip->setVolume(1);
        
    // prepare our visual effects
    ParticleUniverse::ParticleSystemManager* fxMgr = 
    ParticleUniverse::ParticleSystemManager::getSingletonPtr();
    
    mFireEffect = fxMgr->createParticleSystem(
      Ogre::String("FxProbeFireEffect" + stringify(idObject)),
      "Vertigo/FX/Blaze",
      GfxEngine::getSingletonPtr()->getSM());
    mFireEffect->prepare();
             
    mIceEffect = fxMgr->createParticleSystem(
      Ogre::String("FxProbeIceEffect" + stringify(idObject)),
      "Vertigo/FX/Steam",
      GfxEngine::getSingletonPtr()->getSM());
    mIceEffect->prepare();
       
    mMasterNode->attachObject(mFireEffect);
    mMasterNode->attachObject(mIceEffect);
    
    fDead = true;

    bindToName("SettingsChanged", this, &Probe::evtSettingsChanged);
    bindToName("GameStarted", this, &Probe::evtGameStarted);
    bindToName("ZoneEntered", this, &Probe::evtZoneEntered);
    bindToName("DroneCollided", this, &Probe::evtDroneCollided);
    bindToName("PortalEntered", this, &Probe::evtPortalEntered);
    bindToName("PortalSighted", this, &Probe::evtPortalSighted);
    
    mUpdate = &Probe::updatePreparation;
    
    mLog->infoStream() << "created";
  };

	Probe::~Probe()
	{
	  mLog->infoStream() <<"destructed";
			
	  ParticleUniverse::ParticleSystemManager* fxMgr = 
    ParticleUniverse::ParticleSystemManager::getSingletonPtr();
    
    fxMgr->destroyParticleSystem(mFireEffect,GfxEngine::getSingletonPtr()->getSM());
    fxMgr->destroyParticleSystem(mIceEffect,GfxEngine::getSingletonPtr()->getSM());
    
	  mFireEffect = NULL;
	  mIceEffect = NULL;
	
    OgreOggSound::OgreOggSoundManager *mSoundMgr;
    mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
    
    mSoundMgr->destroySound(mSfxBeep);
    mSoundMgr->destroySound(mSfxWarning);
    mSoundMgr->destroySound(mSfxFlip);
    
    mSoundMgr = NULL;
    mSfxBeep = NULL;
    mSfxWarning = 0;
    mSfxFlip = 0;
    
		delete mPhyxBody->getMotionState();
    delete mPhyxBody;

		delete mPhyxShape;

    GfxEngine::getSingletonPtr()->detachFromScene(this);
    
    mPhyxShape = 0;
    mPhyxBody = 0;
    // these r handled by ogre
    mMasterNode = 0;
    mSceneNode = 0;
    mSceneObject = 0;
    //GfxEngine::getSingletonPtr()->getSM()->destroyMovableObject(mSceneObject);
    
    //GfxEngine::getSingletonPtr()->getSM()->destroySceneNode(mMasterNode);
    
		if (mLog) {
			delete mLog;
			mLog = 0;
		}
	};
	

	
	void Probe::live() {

    PhyxEngine::getSingletonPtr()->attachToWorld(this);
    GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->addChild(mMasterNode);
    
    fDead = false;
    
		mLog->debugStream() << "sphere alive & rendered";
	};
	
	void Probe::die() {
	  PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->removeChild(mMasterNode);

	  mIceEffect->stop();
	  mFireEffect->stop();
	  
	  if (fHasSfx) {
	    if (mSfxWarning->isPlaying())
	      mSfxWarning->startFade(false, 1.0f);
	  }
	  fDead = true;
	};
	
	void Probe::render() {
	
	  if (!fHasFx) {
	    mIceEffect->stop();
	    mFireEffect->stop();
	  }
	  
	  // swap the materials and our particle systems
		if (mCurrentShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Probe/Fire");
		  
		  if (fHasFx) {
		    if (mIceEffect->isAttached())
		      mIceEffect->stop();
		    
		    mFireEffect->start();
		  }
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Probe/Ice");
		  
		  if (fHasFx) {
		    if (mFireEffect->isAttached())
		      mFireEffect->stop();
		    
		    mIceEffect->start();
		  }
		  
		}
	};
		
	void Probe::copyFrom(const Probe& src) {
	};
	
	
	void Probe::keyPressed( const OIS::KeyEvent &e )
	{
	
	  if (fPortalSighted || fDead)
	    return;
		
		switch (e.key) {
			case OIS::KC_W:
			  // give the player a small speed boost
				mDirection.z = 2;
				break;
			case OIS::KC_A:
			case OIS::KC_LEFT:
				mDirection.x = 1;
				break;
			case OIS::KC_D:
			case OIS::KC_RIGHT:
				mDirection.x = -1;
				break;
			case OIS::KC_SPACE:
			  flipShields();
			  break;
		}

	}
	
	void Probe::keyReleased( const OIS::KeyEvent &e ) {

	  if (fPortalSighted || fDead)
	    return;
		
		switch (e.key) {
			case OIS::KC_W:
				mDirection.z = 1;
				break;
			case OIS::KC_A:
			case OIS::KC_LEFT:
				mDirection.x = (mDirection.x > 0) ? 0 : mDirection.x;
				break;
			case OIS::KC_D:
			case OIS::KC_RIGHT:
				mDirection.x = (mDirection.x < 0) ? 0 : mDirection.x;
				break;
		}
		
	}
	
  void Probe::updatePreparation(unsigned long lTimeElapsed) {
    
  };
  
  void Probe::updateGame(unsigned long lTimeElapsed) {

		mPhyxBody->activate(true);
		mPhyxBody->setLinearVelocity(
		  btVector3(mDirection.x * mMoveSpeed * lTimeElapsed, 
		            mDirection.y * mMoveSpeed * lTimeElapsed, 
		            mDirection.z * mMoveSpeed * lTimeElapsed));
		            
		// are we close to the portal?
	  if (!fPortalSighted && mMasterNode->getPosition().z >= mLastTunnelSegment) {
	    // flipping shields and movement is disabled now, and we are invincible
	    Event* evt = EventManager::getSingleton().createEvt("PortalSighted");
	    EventManager::getSingleton().hook(evt);
	    
	    mLog->debugStream() << "Portal reached? " << (fPortalReached ? "yes" : "no")
	      << ", node position z : " << mMasterNode->getPosition().z;
	    
	    fPortalReached = false;
	    fPortalSighted = true;
	  };

    // did we reach the portal?
	  if (!fPortalReached && mMasterNode->getPosition().z >= mTunnelLength) {
	    mLog->debugStream() << "exit portal is reached";
	    
	    Event* evt = EventManager::getSingleton().createEvt("PortalReached");
	    EventManager::getSingleton().hook(evt);
	    
	    fPortalReached = true;
	    
	    evt = 0;
      
      return;		
	  };		
  };
  
	void Probe::update(unsigned long lTimeElapsed) {
	  processEvents();
	  
    (this->*mUpdate)(lTimeElapsed);
		
	};
	
	void Probe::flipShields() {
	  // no flipping shields in dodgy mode
	  if (Level::getSingleton().currentZone()->getSettings().mMode == DODGY)
	    return;
	  
	  if (fHasSfx)  
	    mSfxFlip->play(true);
	  
	  mCurrentShield = (mCurrentShield == FIRE) ? ICE : FIRE;
	  render();
	};
	

	bool Probe::collideNervebreak(Event* inEvt) {
	  Drone* mObs = static_cast<Drone*>(inEvt->getAny());
	  
    // hit our shields and calculate our new score
    if (mCurrentShield != mObs->getShield()) {
      if (mShields[mCurrentShield] < 1000)
        mShields[mCurrentShield] += mRegenRate;
      
      mPhyxBody->activate(true);
      mPhyxBody->clearForces();
      
      mScore += mPhyxBody->getLinearVelocity().length() / 3;
      
      // speed the player up a bit
      setMaxSpeed(mMaxSpeed+mSpeedStep);
      
      // if we hit both pairs of a duette, then it's a miss and not an actual hit
      if (!(mObs->getClass() == DUETTE && mObs->getDuetteTwin()->dead()))
        ++mNrHits;
      
        
    } else {
      // deteriorate the shield
      mShields[mCurrentShield] -= 100;
      // slow the player down
      setMaxSpeed(mMaxSpeed-mSpeedStep);
      
      mScore -= mPhyxBody->getLinearVelocity().length() / 2;
      
      if (fHasSfx) {
        // play beep sound
        mSfxBeep->stop();
        mSfxBeep->play(true);
      }
      
      ++mNrMisses;
      GfxEngine::getSingletonPtr()->applyMotionBlur(0.5f);
    }	  
	}
	bool Probe::collideCommon(Event* inEvt) {
	  
	  Drone* mObs = static_cast<Drone*>(inEvt->getAny());
	  
    // hit our shields and calculate our new score
    if (mCurrentShield == mObs->getShield()) {
      if (mShields[mCurrentShield] < 1000)
        mShields[mCurrentShield] += mRegenRate;
      
      mPhyxBody->activate(true);
      mPhyxBody->clearForces();
      
      mScore += mPhyxBody->getLinearVelocity().length() / 3;
      
      // speed the player up a bit
      //float step = mMoveSpeed / 2;
      setMaxSpeed(mMaxSpeed+mSpeedStep);
      
      // if we hit both pairs of a duette, then it's a miss and not an actual hit
      if (!(mObs->getClass() == DUETTE && mObs->getDuetteTwin()->dead()))
        ++mNrHits;
      
        
    } else {
      // deteriorate the shield
      mShields[mCurrentShield] -= 100;
      // slow the player down
      setMaxSpeed(mMaxSpeed-mSpeedStep);
      
      mScore -= mPhyxBody->getLinearVelocity().length() / 2;
      
      if (fHasSfx) {
        // play beep sound
        mSfxBeep->stop();
        mSfxBeep->play(true);
      }
      
      ++mNrMisses;
      
      // the reason we're calling this directly instead of GfxEngine listening
      // to the event is strictly for performance reasons
      GfxEngine::getSingletonPtr()->applyMotionBlur(0.5f);
    }
   
	};
	
	bool Probe::evtDroneCollided(Event* inEvt) {
	  
	  //mLog->debugStream() << "collided with object at " << getPosition().z;
	  if (fPortalSighted)
	    return true;
	  
	  (this->*mCollide)(inEvt);

    // scare the player if their shields are failing
    if (fHasSfx && mShields[mCurrentShield] < 300 && !mSfxWarning->isPlaying()) {
      mSfxWarning->loop(true);
      mSfxWarning->play(true);
    }
    
    if (mScore <= 0)
      mScore = 0;
    
    UIEngine::getSingletonPtr()->_updateShields();
    
    if (mShields[mCurrentShield] <= 0) {
      Event* mEvt = EventManager::getSingletonPtr()->createEvt("ProbeDied");
      EventManager::getSingletonPtr()->hook(mEvt);
      mEvt = NULL;
      die();
      mLog->debugStream() << "oops! i'm dead";
    };
	  
	  return true;
	};
	
	bool Probe::evtPortalEntered(Event* inEvt) {
	  mLog->debugStream() << "entered portal";
	  
	  // increase max speed every time the player enters a new tunnel
	  Zone* tZone = Level::getSingleton().currentZone();
	  float tMaxSpeedStep = tZone->getSettings().mMaxSpeedStep;
	  setMaxSpeed(mMaxSpeed + mMaxSpeed * tMaxSpeedStep);
	  
	  // __DEBUG__
	  //setMaxSpeed(0);
    
    fPortalSighted = false;
    fPortalReached = false;
    
    mDirection = Vector3(0, -1, 1);
    if (Level::getSingleton().currentZone()->getSettings().fResetVelocity) {
      PhyxEngine::getSingletonPtr()->detachFromWorld(this);
      PhyxEngine::getSingletonPtr()->attachToWorld(this);
      mPhyxBody->setLinearVelocity(btVector3(0,0,0));
      // TODO: this is wrong, mMaxSpeed and mMoveSpeed should be reassigned here
    }
    
    mTunnelLength = tZone->currentTunnel()->_getLength();
	  mLastTunnelSegment = mTunnelLength - tZone->currentTunnel()->_getSegmentLength();
        
	  return true;
	};
	
	bool Probe::evtPortalSighted(Event* inEvt) {
	  Vector3 dest = Level::getSingletonPtr()->getTunnel()->getExitPortal()->getPosition();
	  Vector3 dir = dest - mMasterNode->getPosition();
	  dir.normalise();
	  
	  //mDirection = Vector3(0, 1, 5) * mMoveSpeed;
	  mDirection = dir;
	  
	  fPortalSighted = true;
	  
	  return true;
	};
	
	
	bool Probe::evtGameStarted(Event* inEvt) {
    mUpdate = &Probe::updateGame;	
    
    // parse the zone settings
	  Zone* tZone = Level::getSingleton().currentZone();
	  
	  mMoveSpeed = tZone->getSettings().mMoveSpeed;
	  mMaxSpeed = mMoveSpeed * tZone->getSettings().mMaxSpeedFactor;
	  mSpeedStep = tZone->getSettings().mSpeedStep;
	  
	  // mTunnelLength is used to check whether we reached the portal or not
	  mTunnelLength = tZone->currentTunnel()->_getLength();
	  mLastTunnelSegment = mTunnelLength - tZone->currentTunnel()->_getSegmentLength();
	  
	  fPortalSighted = false;
    fPortalReached = false;
    
    // reset the shield HUDs
    // NOTE: the reason we call it from here is we cannot guarantee whether
    // UIEngine will be dispatched the event before Probe does, and if it did
    // then it will not reflect the updated shields state  
    UIEngine::getSingletonPtr()->_updateShields();
	        
    return true;
	};
	
	bool Probe::evtZoneEntered(Event* inEvt) {
	  mUpdate = &Probe::updatePreparation;
	  
	  // a new game is starting, reset the shizzle
	  reset();
	  
	  return true;
	};
	
	void Probe::reset() {
	  mScore = 0;
    mNrMisses = 0;
    mNrHits = 0;
    
	  if (fDead)
	    live();

	  mCurrentShield = (rand() % 2 == 0) ? FIRE : ICE;
	  render();
	  
	  // default stuff across game modes
	  mShields[FIRE] = 1000;
	  mShields[ICE] = 1000;	  
	  mCollide = &Probe::collideCommon;
	  mRegenRate = 1;
	  
	  // set game mode specific stuff
	  GAMEMODE tMode = Level::getSingleton().currentZone()->getSettings().mMode;
	  if (tMode == DODGY) {
	    mShields[mCurrentShield] = 2000;
	    mShields[(mCurrentShield == FIRE) ? ICE : FIRE] = 0;
	    mRegenRate = 10;
	  } else if (tMode == SURVIVAL) {
	    mRegenRate = 2;
	  } else if (tMode == NERVEBREAK) {
	    mCollide = &Probe::collideNervebreak;
	  }
	  
	  fPortalSighted = false;
    fPortalReached = false;
	  
	  // reset our position and clear forces
	  //PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  //PhyxEngine::getSingletonPtr()->attachToWorld(this);
	  
	  // reset our speed
	  mMoveSpeed = 0;
	  mMaxSpeed = 0;
	  
	  if (mSfxWarning->isPlaying())
	    mSfxWarning->stop();

	  // a default direction and a starting position
	  mDirection = Vector3(0,-1,1);
	  mPhyxBody->activate(true);
	  mPhyxBody->setLinearVelocity(btVector3(0,-1,1));
	  btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(0,70,30));
	  mPhyxBody->proceedToTransform(trans);
	  mMasterNode->setPosition(Vector3(0,70,30));
	};
	
	bool Probe::evtSettingsChanged(Event* inEvt) {
	  fHasFx = Level::getSingleton().areFxEnabled();
	  fHasSfx = Level::getSingleton().areSfxEnabled();
	  
	  render();
	  
	  if (!fHasSfx) {
	    if (mSfxWarning->isPlaying())
	      mSfxWarning->stop();
	  }
	  return true;
	};

} // end of namespace
