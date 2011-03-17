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
	  mMoveSpeed = 8;
	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
	  
	  mSphere = Intro::getSingleton().getSphere();
	  mPosition = randomPosition();
	  
	  GfxEngine::getSingletonPtr()->attachToScene(this);
	  render();
	  mSceneNode->setPosition(mPosition);
	  mSceneNode->pitch(Ogre::Degree(90));
	 
	  btTransform trans = btTransform(btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z));
	      
    mPhyxShape = PhyxEngine::getSingletonPtr()->obstaclesShape();
    //mPhyxShape = new btSphereShape(12);
    //mPhyxShape->setUserPointer(this);
	  mPhyxMS = new MotionState(trans, mSceneNode);
    btScalar mass = 1;
    btVector3 fallInertia(0,0,1);
	
    mPhyxShape->calculateLocalInertia(mass,fallInertia);
        
	  btRigidBody::btRigidBodyConstructionInfo
		  mPhyxBodyCI(mass,mPhyxMS,mPhyxShape,fallInertia);
        
	  mPhyxBody = new btRigidBody(mPhyxBodyCI);
    //mPhyxBody->setFlags(obstacleCollidesWith);
    /*
    mObject = new btCollisionObject();
    mObject->setCollisionShape(mPhyxShape);
    mObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);    
    mObject->setUserPointer(this);*/
    
    //mObject->setWorldTransform(trans);
    
    mSceneNode->setVisible(false);
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
	    qualifier % 30, 
	    0, 
	    mSphere->getSceneNode()->getPosition().z + qualifier % 250 + 1000);	   
	}
	void Obstacle::live() {
	  int qualifier = rand();
	  mShield = (qualifier % 2 == 0) ? ICE : FIRE;
    render();
	  mSceneNode->setVisible(true);
	  mPosition = randomPosition(); 
    mPhyxBody->proceedToTransform(btTransform(btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z)));
	  
	  //PhyxEngine::getSingletonPtr()->attachToWorld(this);
	  
    setCollisionShape(mPhyxShape);
    setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);    
    setUserPointer(this);
	  
    btDiscreteDynamicsWorld* mWorld = PhyxEngine::getSingletonPtr()->world();
    mWorld->addRigidBody(mPhyxBody, COL_OBSTACLES, obstacleCollidesWith);
		mWorld->addCollisionObject(this);

	  mDirection = Vector3(0,0,-1);
	  fDead = false;
	  
	  mLog->debugStream() << mName << " is alive";
	};
	void Obstacle::die() {
	  mSceneNode->setVisible(false);
	  mDirection = Vector3(0,0,0);
	  PhyxEngine::getSingletonPtr()->detachFromWorld(this);
	  fDead = true;
	  
	};
	
	void Obstacle::render() {
		if (mShield == FIRE) {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Fire");
		} else {
		  static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Sphere/Ice");
		}
	};
	
	void Obstacle::copyFrom(const Obstacle& src) {
	  
	};
	
	
  void Obstacle::update(unsigned long lTimeElapsed) {
    if (fDead)
      return;
      
    if (mSphere->getSceneNode()->getPosition().z > mSceneNode->getPosition().z + 100) {
      fDead = true;
      return;
    }
		mPhyxBody->activate(true);
		mPhyxBody->applyCentralForce(btVector3(mDirection.x * mMoveSpeed, mDirection.y * mMoveSpeed, mDirection.z * mMoveSpeed));

	};
	
	void Obstacle::collide(Entity* target) {
	  mLog->debugStream() << "Obstacle" << idObject << " has collided with " << target->getName() << target->getObjectId();
	  die();
	}

  SHIELD Obstacle::shield() { return mShield; }
	
} // end of namespace
