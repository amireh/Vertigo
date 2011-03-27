// Obstacle.cpp
#include "Obstacle.h"
#include "Utility.h"
#include "GfxEngine.h"
#include "Intro.h"
#include "PhyxEngine.h"
#include <cstdlib>

namespace Pixy
{
	
  Obstacle::Obstacle() {
	  mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Obstacle");
    //mLog->infoStream() << "created";

    mName = "Obstacle";
	  mType = OBSTACLE;
	  mMesh = "ObstacleMesh";
	  mMoveSpeed = 1.2f;
	  mDeathDuration = 100;
	  fDying = false;
	  fHasFX = true;
	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
	  
	  mSphere = Intro::getSingleton().getSphere();
	  mPosition = Vector3(0,0, -1000);
	  
	  GfxEngine::getSingletonPtr()->attachToScene(this);
	  
	  //mSceneNode->setPosition(mPosition);

    mMasterNode = GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->createChildSceneNode();
    mSceneNode->getParent()->removeChild(mSceneNode);
    mMasterNode->addChild(mSceneNode);
    mMasterNode->setPosition(mPosition);
	  
	  if (fHasFX) {
	    ParticleUniverse::ParticleSystemManager* fxMgr = 
      ParticleUniverse::ParticleSystemManager::getSingletonPtr();
      
      mBlaze = fxMgr->createParticleSystem(
        Ogre::String("FxBlaze" + stringify(rand())),
        "Vertigo/FX/Blaze",
        GfxEngine::getSingletonPtr()->getSM());
      mBlaze->prepare();
      
      mSteam = fxMgr->createParticleSystem(
        Ogre::String("FxSteam" + stringify(rand())),
        "Vertigo/FX/Steam",
        GfxEngine::getSingletonPtr()->getSM());
      mSteam->prepare();
      
	    // create a fire trail particle system
	    std::ostringstream psName;
		  psName << mName << idObject << "Blaze";
      //mFireTrail = GfxEngine::getSingletonPtr()->getSM()->createParticleSystem(psName.str(), "Vertigo/FX/Blaze");
      //mFireTrail->setNonVisibleUpdateTimeout(0.5f);
      psName.clear();
      psName << mName << idObject << "Steam";
      //mIceSteam = GfxEngine::getSingletonPtr()->getSM()->createParticleSystem(psName.str(), "Vertigo/Effects/Steam");
      //mIceSteam->setNonVisibleUpdateTimeout(0.5f);
      
      mFireTrailNode = mMasterNode->createChildSceneNode();
      mMasterNode->attachObject(mBlaze);
      mIceSteamNode = mMasterNode->createChildSceneNode();
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
	  fDead = true;
  };

	Obstacle::~Obstacle()
	{
    //mLog->infoStream() << "destructed";

		mFireTrailNode = NULL;
		mIceSteamNode = NULL;
		mMasterNode = NULL;

		GfxEngine::getSingletonPtr()->detachFromScene(this);	
		
		delete mPhyxBody->getMotionState();
    delete mPhyxBody;
    
		//delete mPhyxShape;
    
		if (mLog) {
			delete mLog;
			mLog = 0;
		}
	};
	
	Vector3 Obstacle::randomPosition() {
	  int qualifier = rand();
	  int sign = (qualifier % 2 == 0) ? 1 : -1;
	  return Vector3(
	    (qualifier % 20) * sign, 
	    0, 
	    mSphere->getPosition().z + 1200);	   
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
    //setCollisionShape(mPhyxShape);
    //setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);    
   // setUserPointer(this);
	  
    //btDiscreteDynamicsWorld* mWorld = PhyxEngine::getSingletonPtr()->world();
    //mWorld->addRigidBody(mPhyxBody);
		//mWorld->addCollisionObject(this);

	  //mDirection = Vector3(0,0,-1);
	  fDead = false;
	  
	  mLog->debugStream() << mName << idObject << " is alive";
	};
	void Obstacle::die() {
	  //if (fDead)
	  //  return;
	  
	  mLog->debugStream() << mName << idObject << " is dead";
	  
	  mSceneNode->setVisible(false);
	  /*
	  if (fHasFX) {
	    if (mShield == FIRE) {
	      mSceneNode->detachObject(mFireTrail);
	    } else {
	      mSceneNode->detachObject(mIceSteam);
	    }
	  }
	  */
	  //mDirection = Vector3(0,0,0);
	  
	  //btDiscreteDynamicsWorld* mWorld = PhyxEngine::getSingletonPtr()->world();
	  //mPhyxBody->activate(false);
		//mWorld->removeCollisionObject(this);
		//mWorld->removeRigidBody(mPhyxBody);
	  PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  fDead = true;
	  fDying = false;
	};
	
	void Obstacle::render() {
		if (mShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Obstacle/Fire");
		  
		  if (fHasFX) {
		    if (mSteam->isAttached()) {
		      mSteam->setVisible(false);
		      mSteam->stop();
		    }

		    mBlaze->setVisible(true);
		    mBlaze->start();
		  }
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Obstacle/Ice");
		  if (fHasFX) {
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
    
    //if (mSceneObject->getWorldBoundingBox().intersects(mSphere->getSceneObject()->getWorldBoundingBox())) {
    if (mSceneNode->_getWorldAABB().intersects(mSphere->getSceneNode()->_getWorldAABB())) {
      Event* evt = EventManager::getSingleton().createEvt("ObstacleCollided");
      evt->setAny((void*)this);
      EventManager::getSingleton().hook(evt);
      evt = NULL;
      collide(mSphere);
      return;
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
	
	void Obstacle::collide(Entity* target) {
	  if (fDying || fDead) // prevent race conditions (if the obstacle gets hit twice by the player)
	    return;
	  
	  mLog->debugStream() << "Obstacle" << idObject << " has collided with " << target->getName() << target->getObjectId();
	  //fDying = true;
	  //mTimer.reset();
	  die();
	}

  SHIELD Obstacle::shield() { return mShield; }
	
} // end of namespace
