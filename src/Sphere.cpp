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
		  mMoveSpeed = 0.2f;
		  move = 0;
		  mDistance = 0;
		  mCurrentShield = FIRE;
		  mShields[FIRE] = 100;
		  mShields[ICE] = 100;
		
      currentStep = 0.0f;
      step = 0.05f;
		  
      mLog->infoStream() << "created";

    };

	Sphere::~Sphere()
	{
	    mLog->infoStream() <<"destructed";
		GfxEngine::getSingletonPtr()->detachFromScene(this);
		
		mFireTrailNode = NULL;
		mMasterNode = NULL;
		
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
		Geometry::createSphere(mMesh, 10, 64, 64);
		

		
		GfxEngine::getSingletonPtr()->attachToScene(this);
		
    mFireTrail = GfxEngine::getSingletonPtr()->getSM()->createParticleSystem("SphereBlaze", "Vertigo/Effects/Blaze");
    mIceSteam = GfxEngine::getSingletonPtr()->getSM()->createParticleSystem("SphereSteam", "Vertigo/Effects/Steam");
    mFireTrail->setNonVisibleUpdateTimeout(0.5f);
    mIceSteam->setNonVisibleUpdateTimeout(0.5f);
    
    mMasterNode = GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->createChildSceneNode();
    mSceneNode->getParent()->removeChild(mSceneNode);
    mMasterNode->addChild(mSceneNode);
    mFireTrailNode = mMasterNode->createChildSceneNode();
    mFireTrailNode->attachObject(mFireTrail);
    
    //mSceneNode->detachObject(mSceneObject);
    
    /*
    mSphereNode = mSceneNode->createChildSceneNode();
    mSphereNode->attachObject(mSceneObject);
    mSphereNode->showBoundingBox(true);*/
    
    //mFireTrail->setVisible(false);
    //mIceSteam->setVisible(false);
		mLog->debugStream() << "sphere rendered";
		render();
		
		
		//mSceneNode->setScale(0.1f, 0.1f, 0.1f);
		//mSceneNode->pitch(Ogre::Degree(90));
		//mSceneNode->setPosition(Ogre::Vector3(0,0,0));
		
		btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0));
		
    mPhyxShape = new btSphereShape(10);
		mPhyxMS = new MotionState(trans, mMasterNode);
        btScalar mass = 100;
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
		//PhyxEngine::getSingletonPtr()->attachToWorld(this);
		//mPhyxBody->proceedToTransform(trans);
	
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
    mNextWaypoint = &mWaypoints.front();
	};
	void Sphere::die() {
	  //PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	};
	
	void Sphere::render() {
		if (mCurrentShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Fire");
		  /*if (mIceSteam->isAttached())
		    mSceneNode->detachObject(mIceSteam);*/
		    
		  //mSceneNode->attachObject(mFireTrail);
		  mFireTrail->setVisible(true);
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Ice");
		  /*if (mFireTrail->isAttached())
		    mSceneNode->detachObject(mFireTrail);*/

		  //mSceneNode->attachObject(mIceSteam);
		}
	};
	
	void Sphere::copyFrom(const Sphere& src) {
	};
	
	
	void Sphere::keyPressed( const OIS::KeyEvent &e )
	{
		
		switch (e.key) {
			/*case OIS::KC_W:
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
				break;			*/	
			case OIS::KC_SPACE:
			  flipShields();
			  break;
		}
		
	}
	
	void Sphere::keyReleased( const OIS::KeyEvent &e ) {
		
		/*switch (e.key) {
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
		}*/
		
	}
	

	void Sphere::update(unsigned long lTimeElapsed) {
		//mPhyxBody->activate(true);
		//Ogre::Quaternion rot = mMasterNode->getPosition().getRotationTo(*mNextWaypoint);
		//mMasterNode->setOrientation( rot );
		//mMasterNode->translate(Vector3(rot.x + mMoveSpeed, 0, rot.z + mMoveSpeed));
		//mPhyxBody->applyCentralForce(btVector3(mDirection.x *lTimeElapsed, mDirection.y *lTimeElapsed, mDirection.z *lTimeElapsed));
		
		//mLog->debugStream() << "distance to next waypoint is " << mMasterNode->getPosition().distance( *mNextWaypoint );

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
    }
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
	    GfxEngine::getSingletonPtr()->applyScreenShake(0);
	  }
	  //GfxEngine::getSingletonPtr()->applyMotionBlur(0.5f);
	  
	  mLog->debugStream() << "Sphere has collided with " << target->getName() << target->getObjectId();
	  target->collide(this);
	}
	
	const Vector3& Sphere::getPosition() {
	  return mMasterNode->getPosition();
	};
	
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
	};
} // end of namespace
