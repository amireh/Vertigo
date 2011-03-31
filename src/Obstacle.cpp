// Obstacle.cpp
#include "Obstacle.h"
#include "Utility.h"
#include "GfxEngine.h"
#include "Level.h"
#include "PhyxEngine.h"
#include <cstdlib>
#include "AudioEngine.h"

namespace Pixy
{
	
  Obstacle::Obstacle() {
	  mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Obstacle");
    //mLog->infoStream() << "created";

    mName = "Obstacle";
	  mType = OBSTACLE;
	  mMesh = "ObstacleMesh";
	  mMoveSpeed = 1.2f;
	  fDying = false;

	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
	  
	  mSphere = Level::getSingleton().getSphere();
	  mPosition = Vector3(0,0, -1000);
	  
	  GfxEngine::getSingletonPtr()->attachToScene(this);
	  
	  //mSceneNode->setPosition(mPosition);

    mMasterNode = GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->createChildSceneNode();
    mSceneNode->getParent()->removeChild(mSceneNode);
    mMasterNode->addChild(mSceneNode);
    mMasterNode->setPosition(mPosition);
	  
	  if (fHasFx) {
	    ParticleUniverse::ParticleSystemManager* fxMgr = 
      ParticleUniverse::ParticleSystemManager::getSingletonPtr();
      
      mBlaze = fxMgr->createParticleSystem(
        Ogre::String("FxBlaze" + stringify(idObject)),
        "Vertigo/FX/Blaze",
        GfxEngine::getSingletonPtr()->getSM());
      mBlaze->prepare();
      
      mSteam = fxMgr->createParticleSystem(
        Ogre::String("FxSteam" + stringify(idObject)),
        "Vertigo/FX/Steam",
        GfxEngine::getSingletonPtr()->getSM());
      mSteam->prepare();
         
      mMasterNode->attachObject(mBlaze);
      mMasterNode->attachObject(mSteam);      
    }
    
	  render();
	 
	  btTransform trans = btTransform(btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z));
	      
    mPhyxShape = PhyxEngine::getSingletonPtr()->obstaclesShape();
	  mPhyxMS = new MotionState(trans, mMasterNode);
    btScalar mass = 1;
    btVector3 fallInertia(0,0,1);
	
    mPhyxShape->calculateLocalInertia(mass,fallInertia);
        
	  btRigidBody::btRigidBodyConstructionInfo
		  mPhyxBodyCI(mass,mPhyxMS,mPhyxShape,fallInertia);
        
	  mPhyxBody = new btRigidBody(mPhyxBodyCI);
    mPhyxBody->proceedToTransform(trans);
	      
    mSceneNode->setVisible(false);
    fSfxCreated = false;
	  fDead = true;
  };

	Obstacle::~Obstacle()
	{
    //mLog->infoStream() << "destructed";

		mMasterNode = NULL;

		GfxEngine::getSingletonPtr()->detachFromScene(this);	
		
		delete mPhyxBody->getMotionState();
    delete mPhyxBody;
    
    if (fHasSfx && fSfxCreated) {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = AudioEngine::getSingletonPtr()->getSoundMgr();
      mSoundMgr->destroySound(mSfxExplosion);
      mSoundMgr->destroySound(mSfxShatter);
      mSoundMgr = NULL;
      mSfxExplosion = mSfxShatter = NULL;
    }
		//delete mPhyxShape;
    
		if (mLog) {
			delete mLog;
			mLog = 0;
		}
	};
	
	Vector3 Obstacle::randomPosition() {
	  int qualifier = rand();
	  int sign = (qualifier % 2 == 0) ? 1 : -1;
	  float z = mSphere->getPosition().z + 1200;
	  float portalZ = Level::getSingletonPtr()->getTunnel()->getExitPortal()->getPosition().z;
	  if (z > portalZ)
	    z = portalZ;
	  return Vector3(
	    (qualifier % 20) * sign, 
	    0,
	    z); 
	    //mSphere->getPosition().z + 1200);
	    
	}
	void Obstacle::live() {
	  //if (!fDead)
	  //  return;
	  
	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
    render();
	  mSceneNode->setVisible(true);
	  mPosition = randomPosition();
	  mMasterNode->setPosition(mPosition); 
    mPhyxBody->proceedToTransform(btTransform(btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z)));
	  
	  PhyxEngine::getSingletonPtr()->attachToWorld(this);
	  
	  mPhyxBody->activate(true);
	  mPhyxBody->setLinearVelocity(btVector3(0,0,-mMoveSpeed));

    if (fHasSfx) {
      if (!fSfxCreated) {
        OgreOggSound::OgreOggSoundManager *mSoundMgr;
        mSoundMgr = AudioEngine::getSingletonPtr()->getSoundMgr();
        mSfxExplosion = mSoundMgr->createSound(Ogre::String("Explosion" + stringify(idObject)), "explosion.wav", false, false, true) ;
        mSfxShatter = mSoundMgr->createSound(Ogre::String("Shatter" + stringify(idObject)), "shatter3.wav", false, false, true) ;
        mMasterNode->attachObject(mSfxExplosion);
        mMasterNode->attachObject(mSfxShatter);
        
        mSfxExplosion->setRolloffFactor(2.f);
        mSfxExplosion->setReferenceDistance(1000.f);
        mSfxShatter->setRolloffFactor(2.f);
        mSfxShatter->setReferenceDistance(1000.f);
        
        mLog->debugStream() << "created sound effect";
      
        fSfxCreated = true;
      }

      if (mShield == FIRE)
        mSfx = &mSfxExplosion;
      else
        mSfx = &mSfxShatter;

    }
       
	  fDead = false;
	  
	  //mLog->debugStream() << mName << idObject << " is alive";
	};
	void Obstacle::die() {
	  //if (fDead)
	  //  return;
	  
	  //mLog->debugStream() << mName << idObject << " is dead";
	  
	  
	  
	  if (fHasFx) {
	    if (mShield == FIRE) {
	      mBlaze->stop();
	      mBlaze->setVisible(false);
	    } else {
	      mSteam->stop();
	      mSteam->setVisible(false);
	    }
	  }
	  mSceneNode->setVisible(false);
	  PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  
	  fDead = true;

	};
	
	void Obstacle::render() {
	  // switch our materials and visual effects based on our shield
		if (mShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Obstacle/Fire");
		  
		  if (fHasFx) {
		    if (mSteam->isAttached()) {
		      mSteam->setVisible(false);
		      mSteam->stop();
		    }

		    mBlaze->setVisible(true);
		    mBlaze->start();
		  }
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Obstacle/Ice");
		  if (fHasFx) {
		    if (mBlaze->isAttached()) {
		      mBlaze->setVisible(false);
		      mBlaze->stop();
		    }

        mSteam->setVisible(true);
        mSteam->start();
		  }
		}
	};
	
	void Obstacle::copyFrom(const Obstacle& src) {
	  
	};
	
	
  void Obstacle::update(unsigned long lTimeElapsed) {
    /*if (fDying && mTimer.getMilliseconds() > mDeathDuration) {
      die();
    }*/
    
    if (fDead || fDying)
      return;
    
    /*if (mSphere->getPosition().z > mMasterNode->getPosition().z + 100) {
      die();
      return;
    }*/
    if (fHasSfx)
      (*mSfx)->update(lTimeElapsed);
    
    //if (mSceneObject->getWorldBoundingBox().intersects(mSphere->getSceneObject()->getWorldBoundingBox())) {
    if (mSceneNode->_getWorldAABB().intersects(mSphere->getSceneNode()->_getWorldAABB())) {
      Event* evt = EventManager::getSingleton().createEvt("ObstacleCollided");
      evt->setAny((void*)this);
      EventManager::getSingleton().hook(evt);
      evt = NULL;
      
      //OgreOggSound::OgreOggSoundManager *mSoundMgr;
      //mSoundMgr = AudioEngine::getSingletonPtr()->getSoundMgr();
      //mSoundMgr->getSound(Ogre::String("Explosion" + stringify(idObject)))->play();
      //mSfxExplosion->setPosition(mMasterNode->getPosition());
      if (fHasSfx) {
        (*mSfx)->stop();
        (*mSfx)->play(true);
      }
      //mSoundMgr = NULL;
      return die();
    }
	  
    mDirection = mSphere->getPosition() - mMasterNode->getPosition();
    mDirection.normalise();
    
		mPhyxBody->activate(true);
		/*mPhyxBody->setLinearVelocity(btVector3(
		  mDirection.x * mMoveSpeed * lTimeElapsed * 10, 
		  mDirection.y * mMoveSpeed * lTimeElapsed * 10, 
		  mDirection.z * mMoveSpeed * lTimeElapsed * 10
		  ));*/
		mPhyxBody->applyCentralForce(btVector3(
		  mDirection.x * mMoveSpeed * lTimeElapsed, 
		  mDirection.y * mMoveSpeed * lTimeElapsed, 
		  mDirection.z * mMoveSpeed * lTimeElapsed
		  )
		);

	};
	/*
	bool Obstacle::evtObstacleCollided(Event* inEvt) {
	  die();
	  
	  return true;
	};*/
	
  SHIELD Obstacle::shield() { return mShield; }
	
} // end of namespace
