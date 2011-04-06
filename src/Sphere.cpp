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
		  mMoveSpeed = 0; // speed is set in evtPortalEntered
		  mMaxSpeed = 0;
      mScore = 0;
      
		  mCurrentShield = FIRE;
		  mShields[FIRE] = 1000;
		  mShields[ICE] = 1000;
		  
		  // a default direction
		  mDirection = Vector3(0,-1,1);
		  
      fDead = true;

      bindToName("ObstacleCollided", this, &Sphere::evtObstacleCollided);
      bindToName("PortalEntered", this, &Sphere::evtPortalEntered);
      bindToName("PortalSighted", this, &Sphere::evtPortalSighted);
      
      mLog->infoStream() << "created";
    };

	Sphere::~Sphere()
	{
	  mLog->infoStream() <<"destructed";
		
		mMasterNode = NULL;
		
		if (fHasFx) {
		  ParticleUniverse::ParticleSystemManager* fxMgr = 
      ParticleUniverse::ParticleSystemManager::getSingletonPtr();
      
      fxMgr->destroyParticleSystem(mFireEffect,GfxEngine::getSingletonPtr()->getSM());
      fxMgr->destroyParticleSystem(mIceEffect,GfxEngine::getSingletonPtr()->getSM());
      
		  mFireEffect = NULL;
		  mIceEffect = NULL;
		};
		if (fHasSfx) {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
      
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
	

	
	void Sphere::live() {

    using namespace Ogre;
		Geometry::createSphere(mMesh, 14, 32, 32);
		
		GfxEngine::getSingletonPtr()->attachToScene(this);
		//mSceneObject->setCastShadows(true);
    mMasterNode = GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->createChildSceneNode();
    mSceneNode->getParent()->removeChild(mSceneNode);
    mMasterNode->addChild(mSceneNode);
    //mMasterNode->setVisible(false);

    // preload our materials
    static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Ice");
    static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Fire");
    

		btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(0,35,30));
		
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
		
		// prepare our sound effects
    if (fHasSfx) {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
      mSfxBeep = mSoundMgr->createSound("SphereBeep", "beep.wav", false, false, true) ;
      mMasterNode->attachObject(mSfxBeep);
      
      mSfxBeep->setRolloffFactor(2.f);
      mSfxBeep->setReferenceDistance(1000.f);
      
      mLog->debugStream() << "created sound effect";
    }
    
    // prepare our visual effects
    if (fHasFx) {
	    ParticleUniverse::ParticleSystemManager* fxMgr = 
      ParticleUniverse::ParticleSystemManager::getSingletonPtr();
      
      mFireEffect = fxMgr->createParticleSystem(
        Ogre::String("FxSphereFireEffect"),
        "Vertigo/FX/Sphere/FireTrail",
        GfxEngine::getSingletonPtr()->getSM());
      mFireEffect->prepare();
               
      mIceEffect = fxMgr->createParticleSystem(
        Ogre::String("FxSphereIceEffect"),
        "Vertigo/FX/Sphere/IceSteam",
        GfxEngine::getSingletonPtr()->getSM());
      mIceEffect->prepare();
         
      mMasterNode->attachObject(mFireEffect);
      mMasterNode->attachObject(mIceEffect);
    }
    
    mScore = 0;
    fDead = false;
    
		render();
		mLog->debugStream() << "sphere alive & rendered";
	};
	
	void Sphere::die() {
	  PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  GfxEngine::getSingletonPtr()->detachFromScene(this);
	  mIceEffect->stop();
	  mFireEffect->stop();
	  mMasterNode->setVisible(false);
	  fDead = true;
	};
	
	void Sphere::render() {
		if (mCurrentShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Fire");
		  
		  if (fHasFx) {
		    if (mIceEffect->isAttached()) {
		      mIceEffect->setVisible(false);
		      mIceEffect->stop();
		    }

		    mFireEffect->setVisible(true);
		    mFireEffect->start();
		  }
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Ice");
		  
		  if (fHasFx) {
		    if (mFireEffect->isAttached()) {
		      mFireEffect->setVisible(false);
		      mFireEffect->stop();
		    }

		    mIceEffect->setVisible(true);
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
				//mPhyxBody->clearForces();
				mDirection.x = 1;
				//mDirection.z = mMoveSpeed;
				break;
			case OIS::KC_D:
				//mPhyxBody->clearForces();
				mDirection.x = -1;
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
				mDirection.x = (mDirection.x > 0) ? 0 : mDirection.x;
				break;
			case OIS::KC_D:
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
	

	void Sphere::update(unsigned long lTimeElapsed) {
	  processEvents();
	   
	  //mDirection.z += mMoveSpeed;
	  if (mMoveSpeed >= mMaxSpeed)
	    mMoveSpeed = mMaxSpeed;
	  
		mPhyxBody->activate(true);
		mPhyxBody->setLinearVelocity(
		  btVector3(mDirection.x * mMoveSpeed * lTimeElapsed, 
		            mDirection.y * mMoveSpeed * lTimeElapsed, 
		            mDirection.z * mMoveSpeed * lTimeElapsed));
		            
	  if (fHasSfx)
	    mSfxBeep->update(lTimeElapsed);
	    
		//mPhyxBody->applyCentralForce(btVector3(mDirection.x *lTimeElapsed, mDirection.y *lTimeElapsed, mDirection.z * mMoveSpeed * lTimeElapsed));
		
	};
	
	void Sphere::flipShields() {
	  mCurrentShield = (mCurrentShield == FIRE) ? ICE : FIRE;
	  render();
	};
	
	const Vector3& Sphere::getPosition() {
	  return mMasterNode->getPosition();
	};
	
	SHIELD Sphere::shield() { return mCurrentShield; };

	
	bool Sphere::evtObstacleCollided(Event* inEvt) {
	  //mLog->debugStream() << "collided with object at " << getPosition().z;
	  if (fPortalSighted)
	    return true;
	  
	  Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  
	    // hit our shields and calculate our new score
	    if (mCurrentShield == mObs->shield()) {
	      if (mShields[mCurrentShield] < 1000)
	        mShields[mCurrentShield] += 5;
	      
	        mPhyxBody->activate(true);
	        mPhyxBody->clearForces();
	        
	        mScore += mPhyxBody->getLinearVelocity().length() / 3;
	        
	        // speed the player up a bit
	        float step = mMoveSpeed / 2;
          setMaxSpeed(mMaxSpeed+step);
	        
	    } else {
	      // deteriorate the shield
	      mShields[mCurrentShield] -= 100;
	      // slow the player down
	      float step = mMoveSpeed / 3;
        setMaxSpeed(mMaxSpeed-step);
        
        mScore -= mPhyxBody->getLinearVelocity().length() / 2;
	      
	      if (fHasSfx) {
	        // play beep sound
	        mSfxBeep->stop();
	        mSfxBeep->play(true);
	      }
	    }
	    
	    if (mScore <= 0)
	      mScore = 0;
	    
	    UIEngine::getSingletonPtr()->_updateShields();
	    
	    if (mShields[mCurrentShield] <= 0) {
	      Event* mEvt = EventManager::getSingletonPtr()->createEvt("SphereDied");
	      EventManager::getSingletonPtr()->hook(mEvt);
	      mEvt = NULL;
	      die();
	      mLog->debugStream() << "oops! i'm dead";
	    };
	  
	  return true;
	};
	
	bool Sphere::evtPortalEntered(Event* inEvt) {
	  mLog->debugStream() << "entered portal";
	  //mDirection = Vector3(0,0,1);
	  //mPhyxBody->activate(true);
	  //mPhyxBody->applyCentralForce(btVector3(0,-10 * mMoveSpeed,100));
	  if (fHasFx)
	    GfxEngine::getSingletonPtr()->playEffect("Despawn", this);
	  
	  if (mMoveSpeed == 0)
	    mMoveSpeed = 6;
	  
	  setMoveSpeed(mMoveSpeed / 2);
	  
	  // default max speed
	  if (mMaxSpeed == 0)
	    mMaxSpeed = mMoveSpeed * 2;
	  
	  // increase max speed every time the player enters a new tunnel
	  setMaxSpeed(mMaxSpeed + mMaxSpeed * 0.25f);
	  
	  // __DEBUG__
	  //setMaxSpeed(0);
    
    fPortalSighted = false;
    
    mDirection = Vector3(0, -1, 1);
    mPhyxBody->activate(true);
    
	  return true;
	};
	
	bool Sphere::evtPortalSighted(Event* inEvt) {
	  Vector3 dest = Level::getSingletonPtr()->getTunnel()->getExitPortal()->getPosition();
	  Vector3 dir = dest - mMasterNode->getPosition();
	  dir.normalise();
	  //mMaxSpeed = 100.0f;
	  //mMoveSpeed = 50.0f;
	  
	  //mDirection = Vector3(0, 1, 5) * mMoveSpeed;
	  mDirection = dir;
	  
	  fPortalSighted = true;

	  return true;
	};
	
	int Sphere::getShieldState() {
	  return mShields[mCurrentShield];
	};
	
	const int Sphere::score() {
	  return mScore;
	};
} // end of namespace
