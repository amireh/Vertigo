// Sphere.cpp
#include "Sphere.h"
#include "Utility.h"
#include "GfxEngine.h"
#include "Intro.h"
#include "PhyxEngine.h"
#include "Obstacle.h"
#include "Geometry.h"

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
		  mMaxSpeed = mMoveSpeed = 25.0f;
		  move = 0;
		  mDistance = 0;
		  mCurrentShield = FIRE;
		  mShields[FIRE] = 100;
		  mShields[ICE] = 100;
		
      currentStep = 0.0f;
      step = 0.05f;
		  
      mLog->infoStream() << "created";

      bindToName("PortalSighted", this, &Sphere::evtPortalSighted);
    };

	Sphere::~Sphere()
	{
	    mLog->infoStream() <<"destructed";
		GfxEngine::getSingletonPtr()->detachFromScene(this);
		
		mFireTrailNode = NULL;
		mIceSteamNode = NULL;
		mMasterNode = NULL;
		
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
    /*
    mFireTrail = GfxEngine::getSingletonPtr()->getSM()->createParticleSystem("SphereBlaze", "Vertigo/Effects/Player/Blaze");
    mIceSteam = GfxEngine::getSingletonPtr()->getSM()->createParticleSystem("SphereSteam", "Vertigo/Effects/Player/Steam");
    mFireTrail->setNonVisibleUpdateTimeout(0.5f);
    mIceSteam->setNonVisibleUpdateTimeout(0.5f);
    
    mFireTrail->setVisible(false);
    mIceSteam->setVisible(false);
    

    
    mFireTrailNode = mMasterNode->createChildSceneNode();
    mFireTrailNode->attachObject(mFireTrail);
    mIceSteamNode = mMasterNode->createChildSceneNode();
    mIceSteamNode->attachObject(mIceSteam);
    
    mFireTrailNode->setInheritOrientation(false);
    mIceSteamNode->setInheritOrientation(false);
    */
    //mSceneNode->detachObject(mSceneObject);
    
    /*
    mSphereNode = mSceneNode->createChildSceneNode();
    mSphereNode->attachObject(mSceneObject);
    mSphereNode->showBoundingBox(true);*/

		mLog->debugStream() << "sphere rendered";
		render();
		
		
		//mSceneNode->setScale(0.1f, 0.1f, 0.1f);
		//mSceneNode->pitch(Ogre::Degree(90));
		//mSceneNode->setPosition(Ogre::Vector3(0,0,0));
		
		btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0));
		
    mPhyxShape = new btSphereShape(14);
		mPhyxMS = new MotionState(trans, mMasterNode);
        btScalar mass = 1000;
        btVector3 fallInertia(0,0,0);
		
    mPhyxShape->calculateLocalInertia(mass,fallInertia);
        
		btRigidBody::btRigidBodyConstructionInfo
			mPhyxBodyCI(mass,mPhyxMS,mPhyxShape,fallInertia);
        
		mPhyxBody = new btRigidBody(mPhyxBodyCI);
    
    //mPhyxBody->setFlags(sphereCollidesWith);
    
/*
    setCollisionShape(mPhyxShape);
    setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);    
    */
    //Entity* me = static_cast<Entity*>(mObject->getUserPointer());
    //mLog->debugStream() << "collision object's ptr : " << me->getName();
    
    //btDiscreteDynamicsWorld* mWorld = PhyxEngine::getSingletonPtr()->world();
    //mWorld->addRigidBody(mPhyxBody);
		
		//mWorld->addCollisionObject(this);
		//setWorldTransform(trans);
		//setUserPointer(this);
		//mPhyxShape->setUserPointer(this);
		PhyxEngine::getSingletonPtr()->attachToWorld(this);
		mPhyxBody->proceedToTransform(trans);
		
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
	};
	
	void Sphere::render() {
		if (mCurrentShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Fire");
		  /*if (mIceSteam->isAttached())
		    mIceSteam->setVisible(false);
		    
		  //mSceneNode->attachObject(mFireTrail);
		  mFireTrail->setVisible(true);*/
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Ice");
		  /*if (mFireTrail->isAttached())
		    mFireTrail->setVisible(false);
		    //mSceneNode->detachObject(mFireTrail);

      mIceSteam->setVisible(true);
		  //mSceneNode->attachObject(mIceSteam);*/
		}
	};
	
	void Sphere::copyFrom(const Sphere& src) {
	};
	
	
	void Sphere::keyPressed( const OIS::KeyEvent &e )
	{
		
		switch (e.key) {
			case OIS::KC_W:
				mDirection.z = mMoveSpeed;
				break;
			case OIS::KC_A:
				//mPhyxBody->clearForces();
				mDirection.x = mMoveSpeed * 4;
				//mDirection.z = mMoveSpeed;
				break;
			case OIS::KC_D:
				//mPhyxBody->clearForces();
				mDirection.x = -mMoveSpeed * 4;
				//mDirection.z = mMoveSpeed;
				break;
			case OIS::KC_S:
				mPhyxBody->clearForces();
				break;
			case OIS::KC_Q:
				//mPhyxBody->clearForces();
				mDirection.y = mMoveSpeed;
				break;
			case OIS::KC_E:
				//mPhyxBody->clearForces();
				mDirection.y = -mMoveSpeed;
				break;			
			case OIS::KC_SPACE:
			  flipShields();
			  break;
		}
		
	}
	
	void Sphere::keyReleased( const OIS::KeyEvent &e ) {
		
		switch (e.key) {
			case OIS::KC_W:
				mDirection.z = 0;
				break;
			case OIS::KC_A:
				mDirection.x = (mDirection.x > 0) ? 0 : mDirection.x;
				break;
			case OIS::KC_D:
				mDirection.x = (mDirection.x < 0) ? 0 : mDirection.x;
				break;
			case OIS::KC_G:
			  GfxEngine::getSingletonPtr()->applyMotionBlur(0.5f);
			  break;
		}
		
	}
	

	void Sphere::update(unsigned long lTimeElapsed) {
	  processEvents();
	  
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
		//btTransform trans;
		//mPhyxBody->getMotionState()->getWorldTransform(trans);
		//mSceneNode->translate(mDirection * lTimeElapsed, Ogre::Node::TS_LOCAL);
		//mSceneNode->setPosition(Vector3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
	};
	
	void Sphere::flipShields() {
	  mCurrentShield = (mCurrentShield == FIRE) ? ICE : FIRE;
	  render();
	};
	
	void Sphere::collide(Obstacle* target) {
	  if (!target)
	    return;
	  
	  if (target->dead())
	    return;
	  
	  // hit our shields
	  if (mCurrentShield != target->shield()) {
	    mShields[mCurrentShield] -= 5;
	  } else {
	    mShields[mCurrentShield] += 5;
	    //GfxEngine::getSingletonPtr()->applyScreenShake(0);
	  }
	  //GfxEngine::getSingletonPtr()->applyMotionBlur(0.5f);
	  
	  mLog->debugStream() << "Sphere has collided with " << target->getName() << target->getObjectId();
	  //target->collide(this);
	}
	
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
	
	bool Sphere::evtPortalSighted(Event* inEvt) {
	  Vector3 dest = GfxEngine::getSingletonPtr()->getPortal()->getPosition();
	  mMaxSpeed = 100.0f;
	  mMoveSpeed = 50.0f;
	  
	  mDirection = Vector3(0, 100, 1) * mMoveSpeed;
	  return true;
	};
} // end of namespace
