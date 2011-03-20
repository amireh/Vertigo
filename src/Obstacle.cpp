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
        mLog->infoStream() << "created";

    mName = "Obstacle";
	  mType = OBSTACLE;
	  mMesh = "ObstacleMesh";
	  mMoveSpeed = 1.0f;
	  mDeathDuration = 100;
	  fDying = false;
	  fHasFX = false;
	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
	  
	  mSphere = Intro::getSingleton().getSphere();
	  mPosition = randomPosition();
	  
	  GfxEngine::getSingletonPtr()->attachToScene(this);
	  
	  if (fHasFX) {
	    // create a fire trail particle system
	    std::ostringstream psName;
		  psName << mName << idObject << "Blaze";
      mFireTrail = GfxEngine::getSingletonPtr()->getSM()->createParticleSystem(psName.str(), "Vertigo/Effects/Blaze");
      mFireTrail->setNonVisibleUpdateTimeout(0.5f);
      psName.clear();
      psName << mName << idObject << "Steam";
      mIceSteam = GfxEngine::getSingletonPtr()->getSM()->createParticleSystem(psName.str(), "Vertigo/Effects/Steam");
      mIceSteam->setNonVisibleUpdateTimeout(0.5f);
    }
    
	  render();
	 
	  btTransform trans = btTransform(btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z));
	      
    mPhyxShape = PhyxEngine::getSingletonPtr()->obstaclesShape();
	  mPhyxMS = new MotionState(trans, mSceneNode);
    btScalar mass = 1;
    btVector3 fallInertia(0,0,1);
	
    mPhyxShape->calculateLocalInertia(mass,fallInertia);
        
	  btRigidBody::btRigidBodyConstructionInfo
		  mPhyxBodyCI(mass,mPhyxMS,mPhyxShape,fallInertia);
        
	  mPhyxBody = new btRigidBody(mPhyxBodyCI);
    
    mSceneNode->setVisible(false);
	  fDead = true;
  };

	Obstacle::~Obstacle()
	{
    mLog->infoStream() << "destructed";
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
	  return Vector3(
	    qualifier % 20, 
	    0, 
	    mSphere->getSceneNode()->getPosition().z + (qualifier % 250) + 1000);	   
	}
	void Obstacle::live() {
	  //if (!fDead)
	  //  return;
	  
	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
    render();
	  mSceneNode->setVisible(true);
	  mPosition = randomPosition(); 
    mPhyxBody->proceedToTransform(btTransform(btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z)));
	  
	  //PhyxEngine::getSingletonPtr()->attachToWorld(this);
	  
    //setCollisionShape(mPhyxShape);
    //setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);    
   // setUserPointer(this);
	  
    btDiscreteDynamicsWorld* mWorld = PhyxEngine::getSingletonPtr()->world();
    mWorld->addRigidBody(mPhyxBody);
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
	  
	  if (fHasFX) {
	    if (mShield == FIRE) {
	      mSceneNode->detachObject(mFireTrail);
	    } else {
	      mSceneNode->detachObject(mIceSteam);
	    }
	  }
	  
	  //mDirection = Vector3(0,0,0);
	  
	  btDiscreteDynamicsWorld* mWorld = PhyxEngine::getSingletonPtr()->world();
	  mPhyxBody->activate(false);
		//mWorld->removeCollisionObject(this);
		mWorld->removeRigidBody(mPhyxBody);
	  //PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  fDead = true;
	  fDying = false;
	};
	
	void Obstacle::render() {
		if (mShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Fire");
		  
		  if (fHasFX) {
			  if (mIceSteam->isAttached())
				  mSceneNode->detachObject(mIceSteam);

			  if (!mFireTrail->isAttached())
				  mSceneNode->attachObject(mFireTrail);
		  }
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Ice");
		  if (fHasFX) {
		    if (mFireTrail->isAttached())
			    mSceneNode->detachObject(mFireTrail);
			
			  if (!mIceSteam->isAttached())
				  mSceneNode->attachObject(mIceSteam);
		  }
		}
	};
	
	void Obstacle::copyFrom(const Obstacle& src) {
	  
	};
	
	
  void Obstacle::update(unsigned long lTimeElapsed) {
    if (fDying && mTimer.getMilliseconds() > mDeathDuration) {
      die();
    }
    
    if (fDead || fDying)
      return;
    
    if (mSphere->getSceneNode()->getPosition().z > mSceneNode->getPosition().z + 100) {
      die();
      return;
    }
    
    if (mSceneObject->getWorldBoundingBox().intersects(mSphere->getSceneObject()->getWorldBoundingBox())) {
      collide(mSphere);
      return;
    }
    
    mDirection = mSphere->getSceneNode()->getPosition() - mSceneNode->getPosition();
    mDirection.normalise();
    
		mPhyxBody->activate(true);
		mPhyxBody->applyCentralForce(btVector3(
		  mDirection.x * mMoveSpeed * lTimeElapsed * 6, 
		  mDirection.y * mMoveSpeed * lTimeElapsed, 
		  mDirection.z * mMoveSpeed * lTimeElapsed
		  )
		);

	};
	
	void Obstacle::collide(Entity* target) {
	  if (fDying || fDead) // prevent race conditions (if the obstacle gets hit twice by the player)
	    return;
	  
	  mLog->debugStream() << "Obstacle" << idObject << " has collided with " << target->getName() << target->getObjectId();
	  fDying = true;
	  mTimer.reset();
	  //die();
	}

  SHIELD Obstacle::shield() { return mShield; }
	
} // end of namespace
