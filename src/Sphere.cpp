// Sphere.cpp
#include "Sphere.h"
#include "Utility.h"
#include "GfxEngine.h"
#include "Level.h"
#include "PhyxEngine.h"
#include "Obstacle.h"
#include "Geometry.h"
#include "AudioEngine.h"

namespace Pixy
{
	
    Sphere::Sphere()
    {
		
		  //if (!mLog)
		    mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Sphere");
		  
		  mLog->infoStream() << "creating";
		  
		  mName = "Sphere";
		  mType = SPHERE;
		  mMesh = "SphereMesh";
		  mMoveSpeed = 50.0f;
		  mMaxSpeed = 100.0f * mMoveSpeed;
		  move = 0;
		  mDistance = 0;
		  mCurrentShield = FIRE;
		  mShields[FIRE] = 100;
		  mShields[ICE] = 100;
		  mDirection = Vector3(0,0,mMoveSpeed);
		  
      mLog->infoStream() << "created";

      bindToName("ObstacleCollided", this, &Sphere::evtObstacleCollided);
      bindToName("PortalEntered", this, &Sphere::evtPortalEntered);
      bindToName("PortalSighted", this, &Sphere::evtPortalSighted);
    };

	Sphere::~Sphere()
	{
	  mLog->infoStream() <<"destructed";
		
		mFireTrailNode = NULL;
		mIceSteamNode = NULL;
		mMasterNode = NULL;
		
		if (fHasSfx) {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = AudioEngine::getSingletonPtr()->getSoundMgr();
      
      mSoundMgr->destroySound(mSfxBeep);
      
      mSoundMgr = NULL;
      mSfxBeep = NULL;
    }
    
		delete mPhyxBody->getMotionState();
    delete mPhyxBody;

		delete mPhyxShape;

		if (mLog) {
			delete mLog;
			mLog = 0;
		}
	};
	
	void Sphere::setMaxSpeed(float inSpeed) {
	  mMaxSpeed = inSpeed;
	};
	float Sphere::getMaxSpeed() const { return mMaxSpeed; };
	
	void Sphere::live() {

    using namespace Ogre;
		Geometry::createSphere(mMesh, 14, 32, 32);
		
		GfxEngine::getSingletonPtr()->attachToScene(this);
		mSceneObject->setCastShadows(true);
    mMasterNode = GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->createChildSceneNode();
    mSceneNode->getParent()->removeChild(mSceneNode);
    mMasterNode->addChild(mSceneNode);
    //mSceneNode->setVisible(false);


		mLog->debugStream() << "sphere rendered";
		render();
				
		btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(0,70,-10));
		
    mPhyxShape = new btSphereShape(14);
		mPhyxMS = new MotionState(trans, mMasterNode);
        btScalar mass = 1000;
        btVector3 fallInertia(0,0,0);
		
    mPhyxShape->calculateLocalInertia(mass,fallInertia);
        
		btRigidBody::btRigidBodyConstructionInfo
			mPhyxBodyCI(mass,mPhyxMS,mPhyxShape,fallInertia);
        
		mPhyxBody = new btRigidBody(mPhyxBodyCI);
    
		PhyxEngine::getSingletonPtr()->attachToWorld(this);
		mPhyxBody->proceedToTransform(trans);
		

    if (fHasSfx) {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = AudioEngine::getSingletonPtr()->getSoundMgr();
      mSfxBeep = mSoundMgr->createSound("SphereBeep", "beep.wav", false, false, true) ;
      mMasterNode->attachObject(mSfxBeep);
      
      mSfxBeep->setRolloffFactor(2.f);
      mSfxBeep->setReferenceDistance(1000.f);
      
      mLog->debugStream() << "created sound effect";
    }
	/*
	  mPath = new Ogre::SimpleSpline();
	  mPath->setAutoCalculate(false);
	  
	  mWaypoints.push_back(Vector3(0,0,0));
	  
	  mWaypoints.push_back(Vector3(0,0,-500));
	  mWaypoints.push_back(Vector3(-100,-300,-500));
	  mWaypoints.push_back(Vector3(-600,-300,-500));
	  
    //mWaypoints.push_back(Vector3(0,0,0));
    
    std::list<Vector3>::iterator itr;
    for (itr = mWaypoints.begin(); itr != mWaypoints.end(); ++itr) {
      mPath->addPoint(*itr);
    }
    mPath->recalcTangents();
    
    mWaypoints.clear();
    for (currentStep = 0.0f; currentStep <= 1.0f; currentStep += step) {
      mWaypoints.push_back(mPath->interpolate(currentStep));
    }
    
    mDirection = Ogre::Vector3::ZERO;
    mNextWaypoint = &mWaypoints.front();*/
	};
	void Sphere::die() {
	  PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  GfxEngine::getSingletonPtr()->detachFromScene(this);
	};
	
	void Sphere::render() {
		if (mCurrentShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Fire");
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Ice");
		}
	};
	
	void Sphere::copyFrom(const Sphere& src) {
	};
	
	
	void Sphere::keyPressed( const OIS::KeyEvent &e )
	{
		
		switch (e.key) {
			/*case OIS::KC_W:
				mDirection.z += mMoveSpeed;
				mMaxSpeed += mMoveSpeed;
				break;*/
			case OIS::KC_A:
				//mPhyxBody->clearForces();
				mDirection.x = mMoveSpeed * 12;
				//mDirection.z = mMoveSpeed;
				break;
			case OIS::KC_D:
				//mPhyxBody->clearForces();
				mDirection.x = -mMoveSpeed * 12;
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
		
		switch (e.key) {
			/*case OIS::KC_W:
				mDirection.z -= mMoveSpeed;
				mMaxSpeed -= mMoveSpeed;
				break;*/
			case OIS::KC_A:
				mDirection.x = (mDirection.x > 0) ? 0 : mDirection.x;
				break;
			case OIS::KC_D:
				mDirection.x = (mDirection.x < 0) ? 0 : mDirection.x;
				break;
		}
		
	}
	

	void Sphere::update(unsigned long lTimeElapsed) {
	  processEvents();
	  
	  if (fHasSfx)
	    mSfxBeep->update(lTimeElapsed);
	  
	  mDirection.z += mMoveSpeed;
	  if (mDirection.z >= mMaxSpeed)
	    mDirection.z = mMaxSpeed;
	  
		mPhyxBody->activate(true);
		//mPhyxBody->setLinearVelocity(btVector3(0,0,mMoveSpeed * lTimeElapsed));
		mPhyxBody->applyCentralForce(btVector3(mDirection.x *lTimeElapsed, mDirection.y *lTimeElapsed, mDirection.z *lTimeElapsed));
		
		
		//Ogre::Quaternion rot = mMasterNode->getPosition().getRotationTo(*mNextWaypoint);
		//mMasterNode->setOrientation( rot );
		//mMasterNode->translate(Vector3(rot.x + mMoveSpeed, 0, rot.z + mMoveSpeed));
		
		
		//mLog->debugStream() << "distance to next waypoint is " << mMasterNode->getPosition().distance( *mNextWaypoint );
    /*
		if (mDirection == Ogre::Vector3::ZERO ) {
		  locateNextWaypoint();
      mDirection = *mNextWaypoint - mMasterNode->getPosition();
      mDistance = mDirection.normalise();
      
      //mLog->debugStream() << "moving now to new waypoint: " << mNextWaypoint->x << ","<<mNextWaypoint->y << ","<<mNextWaypoint->z;
		} else {
       move = mMoveSpeed * lTimeElapsed;
       mDistance -= move;
       if (mDistance <= 0.0f)
       {
         mMasterNode->setPosition(*mNextWaypoint);
         mDirection = Ogre::Vector3::ZERO;
         
        Ogre::Vector3 src = mMasterNode->getOrientation() * Ogre::Vector3::UNIT_X;
        if ((1.0f + src.dotProduct(mDirection)) < 0.0001f) 
        {
            mMasterNode->yaw(Ogre::Degree(180));
        }
        else
        {
            Ogre::Quaternion quat = src.getRotationTo(mDirection);
            mMasterNode->rotate(quat);
        } // else
       } else {
         mMasterNode->translate(mDirection * move);
         mMasterNode->rotate(Vector3(1,0,0), Ogre::Degree(0.1f * lTimeElapsed));
       }
    }*/
	};
	
	void Sphere::flipShields() {
	  mCurrentShield = (mCurrentShield == FIRE) ? ICE : FIRE;
	  render();
	};
	
	const Vector3& Sphere::getPosition() {
	  return mMasterNode->getPosition();
	};
	
	SHIELD Sphere::shield() { return mCurrentShield; };
	/*
	void Sphere::locateNextWaypoint() {
	  if (mNextWaypoint == &mWaypoints.back()) {
	    mNextWaypoint = &mWaypoints.front();
	    return;
	  }
	  //currentStep += step;
	  
	  //if (currentStep > 1.0f)
	  //  currentStep = 0.0f;
	    
	  //mNextWaypoint = &mPath->interpolate(currentStep);
	  
	  list<Vector3>::iterator itr;
	  for (itr = mWaypoints.begin(); itr != mWaypoints.end(); ++itr) {
	    if (&(*itr) == mNextWaypoint) {
	      mNextWaypoint = &(*(++itr));
	      break;
	    } 
	  }
	};*/
	
	bool Sphere::evtObstacleCollided(Event* inEvt) {
	  Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  
	  // hit our shields
	  if (mCurrentShield != mObs->shield()) {
	    mShields[mCurrentShield] -= 5;
	    mPhyxBody->activate(true);
	    mPhyxBody->clearForces();
	    // push the player forward a little
	    mPhyxBody->applyCentralForce(btVector3(mDirection.x * mMoveSpeed,mDirection.y * mMoveSpeed, mDirection.z * mMoveSpeed));
	  } else {
	    mShields[mCurrentShield] += 5;
	    mPhyxBody->activate(true);
	    mPhyxBody->clearForces();
	    // push the player back
	    mPhyxBody->applyCentralForce(btVector3(mDirection.x * mMoveSpeed,mDirection.y * mMoveSpeed,-mMoveSpeed * 1000));
	    
	    if (fHasSfx) {
	      // play beep sound
	      mSfxBeep->stop();
	      mSfxBeep->play(true);
	    }
	  }
	  
	  return true;
	};
	
	bool Sphere::evtPortalEntered(Event* inEvt) {
	  mLog->debugStream() << "entered portal";
	  mDirection = Vector3(0,0,1);
	  mPhyxBody->activate(true);
	  mPhyxBody->applyCentralForce(btVector3(0,-10,100000));
	  if (fHasFx)
	    GfxEngine::getSingletonPtr()->playEffect("Despawn", this);
	  
	  //AudioEngine::getSingletonPtr()->playEffect(SFX_EXPLOSION, mMasterNode);
	  return true;
	};
	
	bool Sphere::evtPortalSighted(Event* inEvt) {
	  //Vector3 dest = Level::getSingletonPtr()->getTunnel()->getExitPortal()->getPosition();
	  mMaxSpeed = 100.0f;
	  mMoveSpeed = 50.0f;
	  
	  mDirection = Vector3(0, 8, 5) * mMoveSpeed;
	  return true;
	};
} // end of namespace
