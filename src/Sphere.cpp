// Sphere.cpp
#include "Sphere.h"
#include "Utility.h"
#include "GfxEngine.h"
#include "Level.h"
#include "PhyxEngine.h"
#include "Obstacle.h"
#include "Geometry.h"
#include "SfxEngine.h"

namespace Pixy
{
	
    Sphere::Sphere()
    {
		
	    mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Sphere");
		  
		  mLog->infoStream() << "creating";
		  
		  mName = "Sphere";
		  mType = SPHERE;
		  mMesh = "SphereMesh";
		  mMoveSpeed = 0;
		  mMaxSpeed = 0;
      mScore = 0;
      mSpeedStep = 0;
      mRegenRate = 0;
      
      mUpdate = 0;
      mCollide = 0;
      
		  mCurrentShield = (rand() % 2 == 0) ? FIRE : ICE;
		  mShields[FIRE] = 0;
		  mShields[ICE] = 0;
		  
		  mNrMisses = mNrHits = 0;

		  
		  using Ogre::Vector3;
		  
		  // a default direction
		  mDirection = Vector3(0,-1,1);
		  
		  // create our graphical component
		  Geometry::createSphere(mMesh, 14, 32, 32);
		
		  GfxEngine::getSingletonPtr()->attachToScene(this);
      mMasterNode = GfxEngine::getSingletonPtr()->getSM()->createSceneNode("SphereMasterNode");
      mMasterNode->addChild(mSceneNode);

      // preload our materials
      static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Ice");
      static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Fire");		  
		  
		  // create our physical component
		  // initial position
      btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(0,35,30));
		
      mPhyxShape = new btSphereShape(14);
		  mPhyxMS = new MotionState(trans, mMasterNode);
          btScalar mass = 1000;
          btVector3 fallInertia(0,0,0);
		
      mPhyxShape->calculateLocalInertia(mass,fallInertia);
          
		  btRigidBody::btRigidBodyConstructionInfo
			  mPhyxBodyCI(mass,mPhyxMS,mPhyxShape,fallInertia);
          
		  mPhyxBody = new btRigidBody(mPhyxBodyCI);
		  mPhyxBody->proceedToTransform(trans);
		
		  mGhostObject = new btGhostObject();
      mGhostObject->setCollisionShape(new btSphereShape(14));
      mGhostObject->setWorldTransform(trans);
      mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

      
      /* -- prepare sound effects -- */
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();

      //mMasterNode->attachObject(mSfxBeep);

      mSfxBeep = mSoundMgr->createSound("SphereBeep" + stringify(idObject), "beep.wav", false, false, true);        
      mSfxBeep->setRolloffFactor(2.f);
      mSfxBeep->setReferenceDistance(1000.f);
      mSfxBeep->setVolume(0.5f);
      
      // we don't use OGG on Mac, we use WAV instead
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
      mSfxWarning = mSoundMgr->createSound("SphereWarning" + stringify(idObject), "warning_siren.wav", false, false, true);
#else      
      mSfxWarning = mSoundMgr->createSound("SphereWarning" + stringify(idObject), "warning_siren.ogg", false, false, true);
#endif
      mSfxWarning->disable3D(true);
      mSfxWarning->setVolume(1);
      
      mSfxFlip = mSoundMgr->createSound("SphereFlipShield" + stringify(idObject), "flip.wav", false, false, true);
      mSfxFlip->disable3D(true);
      mSfxFlip->setVolume(1);
      
      // prepare our particle systems
      ParticleUniverse::ParticleSystemManager* fxMgr = 
      ParticleUniverse::ParticleSystemManager::getSingletonPtr();
      
      mFireEffect = fxMgr->createParticleSystem(
        Ogre::String("FxSphereFireEffect" + stringify(idObject)),
        "Vertigo/FX/Blaze",
        GfxEngine::getSingletonPtr()->getSM());
      mFireEffect->prepare();
               
      mIceEffect = fxMgr->createParticleSystem(
        Ogre::String("FxSphereIceEffect" + stringify(idObject)),
        "Vertigo/FX/Steam",
        GfxEngine::getSingletonPtr()->getSM());
      mIceEffect->prepare();
         
      mMasterNode->attachObject(mFireEffect);
      mMasterNode->attachObject(mIceEffect);
      
      fDead = true;
  
      bindToName("SettingsChanged", this, &Sphere::evtSettingsChanged);
      bindToName("GameStarted", this, &Sphere::evtGameStarted);
      bindToName("ZoneEntered", this, &Sphere::evtZoneEntered);
      bindToName("ObstacleCollided", this, &Sphere::evtObstacleCollided);
      bindToName("PortalEntered", this, &Sphere::evtPortalEntered);
      bindToName("PortalSighted", this, &Sphere::evtPortalSighted);
      
      mUpdate = &Sphere::updatePreparation;
      
      mLog->infoStream() << "created";
    };

	Sphere::~Sphere()
	{
	  mLog->infoStream() <<"destructed";
		
		// destroy particle systems
	  ParticleUniverse::ParticleSystemManager* fxMgr = 
    ParticleUniverse::ParticleSystemManager::getSingletonPtr();
    
    fxMgr->destroyParticleSystem(mFireEffect,GfxEngine::getSingletonPtr()->getSM());
    fxMgr->destroyParticleSystem(mIceEffect,GfxEngine::getSingletonPtr()->getSM());
    
	  mFireEffect = 0;
	  mIceEffect = 0;
		
		// destroy the sound effects
    OgreOggSound::OgreOggSoundManager *mSoundMgr;
    mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
    
    mSoundMgr->destroySound(mSfxBeep);
    mSoundMgr->destroySound(mSfxWarning);
    mSoundMgr->destroySound(mSfxFlip);
    
    mSoundMgr = 0;
    mSfxBeep = 0;
    mSfxWarning = 0;
    mSfxFlip = 0;

    // destroy physical component
    PhyxEngine::getSingletonPtr()->world()->removeCollisionObject(mGhostObject);
    delete mGhostObject;
		delete mPhyxBody->getMotionState();
    delete mPhyxBody;
		delete mPhyxShape;
    mPhyxShape = 0;
    mPhyxBody = 0;

    // destroy our ogre entity and scene nodes
    GfxEngine::getSingletonPtr()->getSM()->destroySceneNode(mMasterNode);
    GfxEngine::getSingletonPtr()->detachFromScene(this);

    mMasterNode = 0;
    mSceneNode = 0;
    mSceneObject = 0;
    
		if (mLog) {
			delete mLog;
			mLog = 0;
		}
	};
	

	
	void Sphere::live() {

    //PhyxEngine::getSingletonPtr()->attachToWorld(this);
    PhyxEngine::getSingletonPtr()->world()->addRigidBody(mPhyxBody, COL_SPHERE, COL_WALLS | COL_OBSTACLES);
    PhyxEngine::getSingletonPtr()->world()->addCollisionObject(mGhostObject, COL_SPHERE, COL_WALLS | COL_OBSTACLES);
    PhyxEngine::getSingletonPtr()->world()->
      getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());    
    
    GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->addChild(mMasterNode);
    
    mScore = 0;
    fDead = false;
    
		mLog->debugStream() << "sphere alive & rendered";
	};
	
	void Sphere::die() {
	
    Event* mEvt = EventManager::getSingletonPtr()->createEvt("SphereDied");
    EventManager::getSingletonPtr()->hook(mEvt);
    mEvt = NULL;
    
	  PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  PhyxEngine::getSingletonPtr()->world()->
      getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(0); 
	  PhyxEngine::getSingletonPtr()->world()->removeCollisionObject(mGhostObject);
      
	  GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->removeChild(mMasterNode);

	  mIceEffect->stop();
	  mFireEffect->stop();
	  
	  if (fHasSfx) {
	    if (mSfxWarning->isPlaying())
	      mSfxWarning->startFade(false, 1.0f);
	  }
	  fDead = true;
	};
	
	void Sphere::render() {
	
	  if (!fHasFx) {
	    mIceEffect->stop();
	    mFireEffect->stop();
	  }
	  
		if (mCurrentShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Fire");
		  
		  if (fHasFx) {
		    if (mIceEffect->isAttached()) {
		      //mIceEffect->setVisible(false);
		      mIceEffect->stop();
		    }

		    //mFireEffect->setVisible(true);
		    mFireEffect->start();
		  }
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Ice");
		  
		  if (fHasFx) {
		    if (mFireEffect->isAttached()) {
		      //mFireEffect->setVisible(false);
		      mFireEffect->stop();
		    }

		    //mIceEffect->setVisible(true);
		    mIceEffect->start();
		  }
		  
		}
	};
		
	void Sphere::copyFrom(const Sphere& src) {
	};
	
	
	void Sphere::keyPressed( const OIS::KeyEvent &e )
	{
	
	  if (fPortalSighted || fDead)
	    return;
		
		switch (e.key) {
			case OIS::KC_W:
				mDirection.z = 2;
				//mMaxSpeed += mMoveSpeed;
				break;
			case OIS::KC_A:
			case OIS::KC_LEFT:
				//mPhyxBody->clearForces();
				mDirection.x = 1;
				
				//if (Level::getSingleton().currentZone()->getSettings().mMode == DODGY)
	      //  mDirection.x *= 3;
				
				
				//mDirection.z = mMoveSpeed;
				break;
			case OIS::KC_D:
			case OIS::KC_RIGHT:
				//mPhyxBody->clearForces();
				mDirection.x = -1;
				
				//if (Level::getSingleton().currentZone()->getSettings().mMode == DODGY)
	      //  mDirection.x *= 3;
				//mDirection.z = mMoveSpeed;
				break;
			/*case OIS::KC_S:
				mPhyxBody->clearForces();
				break;
			case OIS::KC_Q:
				mDirection.y = mMoveSpeed;
				break;
			case OIS::KC_E:
				mDirection.y = -mMoveSpeed;
				break;			*/
			case OIS::KC_SPACE:
			  flipShields();
			  break;
		}
		
		
		
	}
	
	void Sphere::keyReleased( const OIS::KeyEvent &e ) {

	  if (fPortalSighted || fDead)
	    return;
		
		switch (e.key) {
			case OIS::KC_W:
				mDirection.z = 1;
				//mMaxSpeed -= mMoveSpeed;
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
	
	void Sphere::mouseMoved( const OIS::MouseEvent& e ) {
    /*if (e.state.X.rel > 0) {
      mDirection.x = -1 * (mMoveSpeed / 10.0f);
    } else {
      mDirection.x = mMoveSpeed / 10.0f;
    }*/
	};
	
  void Sphere::updatePreparation(unsigned long lTimeElapsed) {
    
  };
  
  void Sphere::updateGame(unsigned long lTimeElapsed) {
	  //mDirection.z += mMoveSpeed;
	  //if (mMoveSpeed >= mMaxSpeed)
	  //  mMoveSpeed = mMaxSpeed;
	  
		mPhyxBody->activate(true);
		mPhyxBody->setLinearVelocity(
		  btVector3(mDirection.x * mMoveSpeed * lTimeElapsed, 
		            mDirection.y * mMoveSpeed * lTimeElapsed, 
		            mDirection.z * mMoveSpeed * lTimeElapsed));
		            
	  //if (fHasSfx)
	  //  mSfxBeep->update(lTimeElapsed);
	  
		//mPhyxBody->applyCentralForce(btVector3(mDirection.x *lTimeElapsed, mDirection.y *lTimeElapsed, mDirection.z * mMoveSpeed * lTimeElapsed));
		
	  if (!fPortalSighted && mMasterNode->getPosition().z >= mLastTunnelSegment) {
	    mLog->debugStream() << "exit portal is sighted";
	    
	    Event* evt = EventManager::getSingleton().createEvt("PortalSighted");
	    EventManager::getSingleton().hook(evt);
	    
	    mLog->debugStream() << "Portal reached? " << (fPortalReached ? "yes" : "no")
	      << ", node position z : " << mMasterNode->getPosition().z;
	    
	    fPortalReached = false;
	    fPortalSighted = true;
	  };

	  if (!fPortalReached && mMasterNode->getPosition().z >= mTunnelLength) {
	    mLog->debugStream() << "exit portal is reached";
	    
	    Event* evt = EventManager::getSingleton().createEvt("PortalReached");
	    EventManager::getSingleton().hook(evt);
	    
	    fPortalReached = true;
	    
	    evt = 0;
      
      return;		
	  };
	  
	  mGhostObject->setWorldTransform(mPhyxBody->getWorldTransform());
	  
	  
    int numObjectsInGhost = mGhostObject->getNumOverlappingObjects(); //numObjectsInGhost is set to 0xcdcdcdcd
    //czPrintf("number of objects inside ghost: %d\n", numObjectsInGhost);
    for(int i=0; i<numObjectsInGhost;++i)
    {
      btCollisionObject* obj = mGhostObject->getOverlappingObject(i);
      Obstacle* tObs = static_cast<Obstacle*>(obj->getUserPointer());
      if (!tObs->dead()) {
        //mLog->debugStream() << "sphere is colliding with Obstacle" << tObs->getObjectId();
        Event* evt = EventManager::getSingleton().createEvt("ObstacleCollided");
        evt->setAny(obj->getUserPointer());
        EventManager::getSingleton().hook(evt);
        evt = 0;
        tObs->die();
        break;
      }
    }
    
  };
  
	void Sphere::update(unsigned long lTimeElapsed) {
	  processEvents();
	  
    (this->*mUpdate)(lTimeElapsed);
		
	};
	
	void Sphere::flipShields() {
	  // no flipping shields in dodgy mode
	  if (Level::getSingleton().currentZone()->getSettings().mMode == DODGY)
	    return;
	  
	  if (fHasSfx)  
	    mSfxFlip->play(true);
	  
	  mCurrentShield = (mCurrentShield == FIRE) ? ICE : FIRE;
	  render();
	};
	
	const Vector3& Sphere::getPosition() {
	  return mMasterNode->getPosition();
	};
	
	SHIELD Sphere::shield() { return mCurrentShield; };

	bool Sphere::collideNervebreak(Event* inEvt) {
	  Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  
    // hit our shields and calculate our new score
    if (mCurrentShield != mObs->shield()) {
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
      GfxEngine::getSingletonPtr()->applyMotionBlur(0.5f);
    }	  
	}
	bool Sphere::collideCommon(Event* inEvt) {
	  
	  Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  
    // hit our shields and calculate our new score
    if (mCurrentShield == mObs->shield()) {
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
      
      /*mLog->debugStream() << "MISSED against: Obstacle" << mObs->getObjectId() 
        << " whose shield was " << ((mObs->shield() == FIRE) ? "Fire" : "Ice")
        << " and my shield was " << ((mCurrentShield == FIRE) ? "Fire" : "Ice")
        << ". Obstacle's class is: " << ((mObs->getClass() == DUETTE) ? "Duette" : "Chase");*/  
      GfxEngine::getSingletonPtr()->applyMotionBlur(0.5f);
    }
    
	  
	};
	bool Sphere::evtObstacleCollided(Event* inEvt) {
	  
	  //mLog->debugStream() << "collided with object at " << getPosition().z;
	  if (fPortalSighted)
	    return true;
	  
	  (this->*mCollide)(inEvt);

    if (fHasSfx && mShields[mCurrentShield] < 300 && !mSfxWarning->isPlaying()) {
      mSfxWarning->loop(true);
      mSfxWarning->play(true);
    }
    
    if (mScore <= 0)
      mScore = 0;
    
    UIEngine::getSingletonPtr()->_updateShields();
    
    if (mShields[mCurrentShield] <= 0) {
      die();
    };
	  
	  return true;
	};
	
	bool Sphere::evtPortalEntered(Event* inEvt) {
	  mLog->debugStream() << "entered portal";
	  //mDirection = Vector3(0,0,1);
	  //mPhyxBody->activate(true);
	  //mPhyxBody->applyCentralForce(btVector3(0,-10 * mMoveSpeed,100));
	  //if (fHasFx)
	    //GfxEngine::getSingletonPtr()->playEffect("Despawn", this);
	  /*
	  if (mMoveSpeed == 0)
	    mMoveSpeed = 6;
	  
	  setMoveSpeed(mMoveSpeed / 2);
	  
	  // default max speed
	  if (mMaxSpeed == 0)
	    mMaxSpeed = mMoveSpeed * 2;*/
	  
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
      //mPhyxBody->activate(true);
      //mPhyxBody->clearForces();
      //mPhyxBody->setLinearVelocity(btVector3(0,0,0));
    }
    
    mTunnelLength = tZone->currentTunnel()->_getLength();
	  mLastTunnelSegment = mTunnelLength - tZone->currentTunnel()->_getSegmentLength();
        
	  return true;
	};
	
	bool Sphere::evtPortalSighted(Event* inEvt) {
	  Vector3 dest = Level::getSingletonPtr()->getTunnel()->getExitPortal()->getPosition();
	  Vector3 dir = dest - mMasterNode->getPosition();
	  dir.normalise();
	  
	  //mDirection = Vector3(0, 1, 5) * mMoveSpeed;
	  mDirection = dir;
	  
	  fPortalSighted = true;
	  
	  return true;
	};
	
	int Sphere::getShieldState() {
	  return mShields[mCurrentShield];
	};
	int Sphere::getShieldState(SHIELD inShield) {
	  return mShields[inShield];
	}
	
	const int Sphere::score() {
	  return mScore;
	};
	
	bool Sphere::evtGameStarted(Event* inEvt) {
    mUpdate = &Sphere::updateGame;	
    
    // set default speed
	  //if (mMoveSpeed == 0)
	    //mMoveSpeed = 6;
	  Zone* tZone = Level::getSingleton().currentZone();
	  
	  mMoveSpeed = tZone->getSettings().mMoveSpeed;
	  mMaxSpeed = mMoveSpeed * tZone->getSettings().mMaxSpeedFactor;
	  mSpeedStep = tZone->getSettings().mSpeedStep;
	  
	  mMoveSpeed *= 10;
	  mMaxSpeed *= 10;
	  
	  mTunnelLength = tZone->currentTunnel()->_getLength();
	  mLastTunnelSegment = mTunnelLength - tZone->currentTunnel()->_getSegmentLength();
	  //setMoveSpeed(mMoveSpeed / 2);
	  
	  fPortalSighted = false;
    fPortalReached = false;
    
    UIEngine::getSingletonPtr()->_updateShields();
	  // default max speed
	  //if (mMaxSpeed == 0)
	    //mMaxSpeed = mMoveSpeed * 2;
	        
    return true;
	};
	
	bool Sphere::evtZoneEntered(Event* inEvt) {
	  mUpdate = &Sphere::updatePreparation;
	    
	  reset();
	  
	  return true;
	};
	void Sphere::reset() {
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
	  mCollide = &Sphere::collideCommon;
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
	    mCollide = &Sphere::collideNervebreak;
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

	  // a default direction
	  mDirection = Vector3(0,-1,1);
	  mPhyxBody->activate(true);
	  mPhyxBody->setLinearVelocity(btVector3(0,-1,1));
	  btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(0,70,30));
	  mPhyxBody->proceedToTransform(trans);
	  mMasterNode->setPosition(Vector3(0,70,30));
	};
	
	bool Sphere::evtSettingsChanged(Event* inEvt) {
	  fHasFx = Level::getSingleton().areFxEnabled();
	  fHasSfx = Level::getSingleton().areSfxEnabled();
	  
	  render();
	  
	  if (!fHasSfx) {
	    if (mSfxWarning->isPlaying())
	      mSfxWarning->stop();
	  }
	  return true;
	};
	
  unsigned long Sphere::getNrHits() const {
    return mNrHits;
  };
	unsigned long Sphere::getNrMisses() const {
	  return mNrMisses;
	};	
} // end of namespace
