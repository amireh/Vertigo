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
		  mMoveSpeed = 4;
		  mCurrentShield = FIRE;
		  mShields[FIRE] = 100;
		  mShields[ICE] = 100;
		

		  
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
		PhyxEngine::getSingletonPtr()->attachToWorld(this);
		mPhyxBody->proceedToTransform(trans);
	

	};
	void Sphere::die() {
	  PhyxEngine::getSingletonPtr()->detachFromWorld(this);
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
		mPhyxBody->activate(true);
		mPhyxBody->applyCentralForce(btVector3(mDirection.x *lTimeElapsed, mDirection.y *lTimeElapsed, mDirection.z *lTimeElapsed));
		
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
} // end of namespace
