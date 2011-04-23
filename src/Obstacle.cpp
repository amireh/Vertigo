// Obstacle.cpp
#include "Obstacle.h"
#include "Utility.h"
#include "GfxEngine.h"
#include "Level.h"
#include "PhyxEngine.h"
#include <cstdlib>
#include "SfxEngine.h"
#include "Geometry.h"

namespace Pixy
{
	
	bool Obstacle::ourMeshCreated = false;
	btCollisionShape* Obstacle::ourShape = NULL;
	 
  Obstacle::Obstacle() {
	  mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Obstacle");
    //mLog->infoStream() << "created";

    mName = "Obstacle";
	  mType = OBSTACLE;
	  mMesh = "ObstacleMesh";
	  mMoveSpeed = 1.2f;
	  mMaxSpeed = mMoveSpeed * 20;
	  fDying = false;
	  fHasFullFx = GameManager::getSingleton().getSettings()["Visual Detail"] == "Full";
    mDuetteTwin = 0;
    
	  mClass = CHASE;
	  
	  if (!ourMeshCreated)
	    Geometry::createSphere("ObstacleMesh", 12, 16, 16);
    if (!ourShape)
      ourShape = new btSphereShape(12);

	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
	  
	  
	  GfxEngine::getSingletonPtr()->attachToScene(this);
	  
	  //mSceneNode->setPosition(mPosition);

    mMasterNode = GfxEngine::getSingletonPtr()->getSM()->createSceneNode();
    //mSceneNode->getParent()->removeChild(mSceneNode);
    mMasterNode->addChild(mSceneNode);
    //mMasterNode->setPosition(mPosition);
	  
	  // create visual effects; particle systems
	  mBlaze = 0;
	  mMortar = 0;
	  mSteam = 0;
	  mTide = 0;
	  //if (fHasFx) {
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
      
      //mTide->stop();
      //mBlaze->stop();
         
      mMasterNode->attachObject(mBlaze);
      mMasterNode->attachObject(mSteam);
      mMasterNode->attachObject(mMortar);
      mMasterNode->attachObject(mTide);
      
    //}
    
    // create sound effects
    //if (fHasSfx) {
      //if (!fSfxCreated) {
        OgreOggSound::OgreOggSoundManager *mSoundMgr;
        mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
        mSfxExplosion = mSoundMgr->createSound(Ogre::String("Explosion" + stringify(idObject)), "explosion.wav", false, false, true) ;
        mSfxShatter = mSoundMgr->createSound(Ogre::String("Shatter" + stringify(idObject)), "shatter3.wav", false, false, true) ;
        //mMasterNode->attachObject(mSfxExplosion);
        //mMasterNode->attachObject(mSfxShatter);
        
        mSfxExplosion->setRolloffFactor(2.f);
        mSfxExplosion->setReferenceDistance(1000.f);
        mSfxShatter->setRolloffFactor(2.f);
        mSfxShatter->setReferenceDistance(1000.f);
        
        mLog->debugStream() << "created sound effect";
      
        fSfxCreated = true;
      //}    
      if (mShield == FIRE)
        mSfx = &mSfxExplosion;
      else
        mSfx = &mSfxShatter;

    //}    
	  //render();
	 
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
	      
    //mSceneNode->setVisible(false);
    fSfxCreated = false;
    
    setClass(CHASE);
    
	  fDead = true;
  };

	Obstacle::~Obstacle()
	{
    //mLog->infoStream() << "destructed";

		mMasterNode = NULL;

		GfxEngine::getSingletonPtr()->detachFromScene(this);	
		
		delete mPhyxBody->getMotionState();
    delete mPhyxBody;
    
    //if (fHasSfx && fSfxCreated) {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
        mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
      
      mSoundMgr->destroySound(mSfxExplosion);
      mSoundMgr->destroySound(mSfxShatter);
      
      mSoundMgr = 0;
      mSfxExplosion = 0;
      mSfxShatter = 0;
    //}
    
	  //if (fHasFx) {
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
	  //}
	  
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
	
	Vector3 Obstacle::randomPosition() {
	  int qualifier = rand();
	  int sign = (qualifier % 2 == 0) ? 1 : -1;

	  float z = mSphere->getPosition().z + 1200;

    // clamp to our portal position if it was further
	  float portalZ = Level::getSingleton().getTunnel()->getExitPortal()->getPosition().z;
	  if (z > portalZ) {
	    z = portalZ;
	    mMoveSpeed /= 2; // slow things down when we're this close to the player
	  }
	  
	  if (Level::getSingleton().currentZone()->getSettings().fFixedSpawnPosition)
	    return Vector3(0,20,z);
	  else
	    return Vector3( (qualifier % 30) * sign, (qualifier % 70), z); 
	  
	}
	void Obstacle::live() {
	  //if (!fDead)
	  //  return;
	  
    mSphere = Level::getSingleton().getSphere();
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
	  //mSceneNode->setVisible(true);
	  GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->addChild(mMasterNode);
	  
	  fDead = false;	  
	  //mLog->debugStream() << mName << idObject << " is alive";
	};
	
	void Obstacle::die() {
	  //if (fDead)
	  //  return;
	  
	  //mLog->debugStream() << mName << idObject << " is dead";
	  
	  // detach our physics body
	  mPhyxBody->activate(true);
	  mPhyxBody->clearForces();
	  mPhyxBody->setLinearVelocity(btVector3(0,0,0));
	  
	  PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  
	  // hide our mesh and particle systems
	  if (fHasFx || (mBlaze && mTide && mSteam && mMortar)) {
	    //if (mShield == FIRE) {
	      mBlaze->stop();
	      //mBlaze->setVisible(false);
	    //} else {
	      mTide->stop();
	      mMortar->stop();
	      mSteam->stop();
	      //mTide->setVisible(false);
	    //}
	  }
	  //mSceneNode->setVisible(false);
	  
	  
	  
	  GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->removeChild(mMasterNode);
	  
	  mUpdater = &Obstacle::updateNothing;
	  
	  fDead = true;
	};
	
	void Obstacle::render() {
	  // switch our materials and visual effects based on our shield
		if (mShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Obstacle/Fire");
		  
		  if (fHasFx) {
		    //if (mTide->isAttached()) {
		      ///mTide->setVisible(false);
		      if (fHasFullFx)
		        mSteam->stop();
		    //}

		    //mBlaze->setVisible(true);
		    if (fHasFullFx)
		      mMortar->start();
		    mBlaze->start();
		  }
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Obstacle/Ice");
		  if (fHasFx) {
		    //if (mBlaze->isAttached()) {
		      //mBlaze->setVisible(false);
		      mBlaze->stop();
		    //}

        //mTide->setVisible(true);
        mTide->start();
        if (fHasFullFx)
          mSteam->start();
		  }
		}
	};
	
	void Obstacle::copyFrom(const Obstacle& src) {
	  
	};
	
	void Obstacle::updateNothing(unsigned long lTimeElapsed) {
	
	};
	
	void Obstacle::update(unsigned long lTimeElapsed) {
	  (this->*mUpdater)(lTimeElapsed);
	};
	
	void Obstacle::updateCommon(unsigned long lTimeElapsed) {
    //if (fHasSfx)
    //  (*mSfx)->update(lTimeElapsed);
    	
	  // have i passed the player? then kill me
    if (mSphere->getPosition().z > mMasterNode->getPosition().z + 100) {
      die();
      return;
    }
    
    // colliding with the player?
    if (mSceneNode->_getWorldAABB().intersects(mSphere->getSceneNode()->_getWorldAABB())) {
      Event* evt = EventManager::getSingleton().createEvt("ObstacleCollided");
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
	
  void Obstacle::updateChase(unsigned long lTimeElapsed) {
    updateCommon(lTimeElapsed);
    
    mDirection = mSphere->getPosition() - mMasterNode->getPosition();
    mDirection.normalise();
    
		mPhyxBody->activate(true);
		mPhyxBody->setLinearVelocity(btVector3(
		  mDirection.x * 2, 
		  -1, 
		  mDirection.z * 2
		  ) * mMoveSpeed * lTimeElapsed);
		/*
		mPhyxBody->applyCentralForce(btVector3(
		  mDirection.x * mMoveSpeed * lTimeElapsed, 
		  mDirection.y * mMoveSpeed * lTimeElapsed, 
		  mDirection.z * mMoveSpeed * lTimeElapsed
		  )
		);*/

	};
	
	void Obstacle::updateDumb(unsigned long lTimeElapsed) {
	  updateCommon(lTimeElapsed);
	  
	  mPhyxBody->activate(true);
	  //mDirection = Vector3(0,-1,-1) * lTimeElapsed * mMoveSpeed;
	  mPhyxBody->setLinearVelocity(btVector3(
	    mDirection.x, mDirection.y, mDirection.z * 2
		) * mMoveSpeed * lTimeElapsed);
		/*mPhyxBody->applyCentralForce(btVector3(
		  mDirection.x, mDirection.y, mDirection.z
		) * lTimeElapsed * mMoveSpeed);*/
		
	};

	void Obstacle::updateStationary(unsigned long lTimeElapsed) {
	  updateCommon(lTimeElapsed);
	  
	  mPhyxBody->activate(true);
	  //mDirection = Vector3(mDirection.x * -1,0,0);// * lTimeElapsed * mMoveSpeed;
	  /*mPhyxBody->setLinearVelocity(btVector3(
	  mDirection.x, mDirection.y, mDirection.z
		  ));*/
		mPhyxBody->applyCentralForce(btVector3(
		  mDirection.x, mDirection.y, mDirection.z
		) * lTimeElapsed * mMoveSpeed);
		
	};	
	/*
	bool Obstacle::evtObstacleCollided(Event* inEvt) {
	  die();
	  
	  return true;
	};*/
	
	void Obstacle::setClass(OBSTACLE_CLASS inClass) {
	  mClass = inClass;
	  switch (mClass) {
	    case CHASE:
	      mUpdater = &Obstacle::updateChase;
	      //mMasterNode->setScale(1.0f, 1.0f, 1.0f);
	      break;
	    case DUMB:
	    case DUETTE:
	      mUpdater = &Obstacle::updateDumb;
	      //mMasterNode->setScale(0.5f, 0.5f, 0.5f);
	      break;
	    /*case STATIONARY:
        mUpdater = &Obstacle::updateStationary;
        // push me into a whirl
        if (!fDead)
          mPhyxBody->applyCentralForce(btVector3((rand() % 2 == 0) ? 700 : -700,0,0));
        break;*/
	  };
	  mDirection = Vector3(0,0,-1); // set some default direction
	};
	
	void Obstacle::setDirection(Vector3 inDirection) {
	  mDirection = inDirection;
	};
  SHIELD Obstacle::shield() { return mShield; }
	void Obstacle::setShield(const SHIELD inShield) { 
	  mShield = inShield; 
	  render();
	  mSfx = (mShield == FIRE) ? &mSfxExplosion : &mSfxShatter;
	};
	
	void Obstacle::setDuetteTwin(Obstacle* inObs) {
	  mDuetteTwin = inObs;
	};
	
	Obstacle* Obstacle::getDuetteTwin() { return mDuetteTwin; }; 
	
	OBSTACLE_CLASS Obstacle::getClass() { return mClass; };
} // end of namespace
