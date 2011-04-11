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
	  mClass = CHASE;
	  
	  if (!ourMeshCreated)
	    Geometry::createSphere("ObstacleMesh", 12, 16, 16);
    if (!ourShape)
      ourShape = new btSphereShape(12);

	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
	  
	  mSphere = Level::getSingleton().getSphere();
	  mPosition = randomPosition(); //ector3(0,10, -1000);
	  
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
        "Vertigo/FX/Sphere/FireTrail",
        GfxEngine::getSingletonPtr()->getSM());
      mBlaze->prepare();
      
      mSteam = fxMgr->createParticleSystem(
        Ogre::String("FxSteam" + stringify(idObject)),
        "Vertigo/FX/Steam",
        GfxEngine::getSingletonPtr()->getSM());
      mSteam->prepare();
      
      //mSteam->stop();
      //mBlaze->stop();
         
      mMasterNode->attachObject(mBlaze);
      mMasterNode->attachObject(mSteam);      
    }
    
	  //render();
	 
	  btTransform trans = btTransform(btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z));
	      
    mPhyxShape = ourShape;
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
    
    if (fHasSfx && fSfxCreated) {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
      mSoundMgr->destroySound(mSfxExplosion);
      mSoundMgr->destroySound(mSfxShatter);
      mSoundMgr = NULL;
      mSfxExplosion = mSfxShatter = NULL;
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
	
	Vector3 Obstacle::randomPosition() {
	  int qualifier = rand();
	  int sign = (qualifier % 2 == 0) ? 1 : -1;
	  //float z = mSphere->getPosition().z + 1200;
	  float z;
	  /*Obstacle *tmpObs = Level::getSingletonPtr()->lastObstacleAlive();
	  if (tmpObs)
	    z = tmpObs->getMasterNode()->getPosition().z;
	  else*/
	    z = mSphere->getPosition().z;
	  z += 1200;
	  //float z = Level::getSingletonPtr()->lastObstacleAlive()->getPosition().z;
	  float portalZ = Level::getSingletonPtr()->getTunnel()->getExitPortal()->getPosition().z;
	  if (z > portalZ)
	    z = portalZ;
	  return Vector3(
	    0,
	    16, 
	    //(qualifier % 50),
	    //20,
	    z); 
	    //mSphere->getPosition().z + 1200);
	    
	}
	void Obstacle::live() {
	  //if (!fDead)
	  //  return;
	  
	  mMoveSpeed = Level::getSingleton().currentZone()->getSettings().mMoveSpeed;
	  mMaxSpeed = mMoveSpeed * Level::getSingleton().currentZone()->getSettings().mMaxSpeedFactor;
	  
	  mDirection = Vector3::ZERO;
	  
	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
    render();
	  mSceneNode->setVisible(true);
	  mPosition = randomPosition();
	  mMasterNode->setPosition(mPosition); 
	  
	  PhyxEngine::getSingletonPtr()->attachToWorld(this);
	  
	  mPhyxBody->activate(true);
	  mPhyxBody->clearForces();
    mPhyxBody->proceedToTransform(btTransform(btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z)));
	  //mPhyxBody->setLinearVelocity(btVector3(0,0,0));
	  //mPhyxBody->setLinearVelocity(btVector3(0,0,-mMoveSpeed));

    if (fHasSfx) {
      if (!fSfxCreated) {
        OgreOggSound::OgreOggSoundManager *mSoundMgr;
        mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
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
    
    //mClass = (qualifier % 2 == 0) ? CHASE : DUMB;// : STATIONARY;
    
	  fDead = false;	  
	  //mLog->debugStream() << mName << idObject << " is alive";
	};
	
	void Obstacle::die() {
	  //if (fDead)
	  //  return;
	  
	  //mLog->debugStream() << mName << idObject << " is dead";
	  
	  mPhyxBody->activate(true);
	  mPhyxBody->clearForces();
	  mPhyxBody->setLinearVelocity(btVector3(0,0,0));
	  
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

    if (fDead || fDying)
      return;
    
    if (mSphere->getPosition().z > mMasterNode->getPosition().z + 100) {
      die();
      return;
    }
    if (fHasSfx)
      (*mSfx)->update(lTimeElapsed);
    
    //if (mSceneObject->getWorldBoundingBox().intersects(mSphere->getSceneObject()->getWorldBoundingBox())) {
    if (mSceneNode->_getWorldAABB().intersects(mSphere->getSceneNode()->_getWorldAABB())) {
      Event* evt = EventManager::getSingleton().createEvt("ObstacleCollided");
      evt->setAny((void*)this);
      EventManager::getSingleton().hook(evt);
      evt = NULL;
      
      //OgreOggSound::OgreOggSoundManager *mSoundMgr;
      //mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
      //mSoundMgr->getSound(Ogre::String("Explosion" + stringify(idObject)))->play();
      //mSfxExplosion->setPosition(mMasterNode->getPosition());
      if (fHasSfx) {
        (*mSfx)->stop();
        (*mSfx)->play(true);
      }
      //mSoundMgr = NULL;
      return die();
    }

	  (this->*mUpdater)(lTimeElapsed);
	};
	
  void Obstacle::updateChase(unsigned long lTimeElapsed) {

    mDirection = mSphere->getPosition() - mMasterNode->getPosition();
    mDirection.normalise();
    
		mPhyxBody->activate(true);
		mPhyxBody->setLinearVelocity(btVector3(
		  mDirection.x * 10, 
		  -1, 
		  mDirection.z * 5
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
	  mPhyxBody->activate(true);
	  //mDirection = Vector3(0,-1,-1) * lTimeElapsed * mMoveSpeed;
	  mPhyxBody->setLinearVelocity(btVector3(
	    mDirection.x, mDirection.y, mDirection.z
		) * mMoveSpeed * lTimeElapsed * 7);
		/*mPhyxBody->applyCentralForce(btVector3(
		  mDirection.x, mDirection.y, mDirection.z
		) * lTimeElapsed * mMoveSpeed);*/
		
	};

	void Obstacle::updateStationary(unsigned long lTimeElapsed) {
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
	void Obstacle::setShield(const SHIELD inShield) { mShield = inShield; render(); };
} // end of namespace
