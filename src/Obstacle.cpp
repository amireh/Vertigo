// Obstacle.cpp
#include "Obstacle.h"
#include "Utility.h"
#include "GfxEngine.h"
#include "Intro.h"
#include "PhyxEngine.h"

namespace Pixy
{
	
  Obstacle::Obstacle() {
	  mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Obstacle");
        mLog->infoStream() << "created";
	
	  mName = "Obstacle";
	  mType = OBSTACLE;
	  mMesh = "ObstacleMesh";
	  mMoveSpeed = 0.5;
	  mShield = ICE;
	  
	  mSphere = Intro::getSingleton().getSphere();
	  mPosition = Vector3(0, 0, mSphere->getSceneNode()->getPosition().z + 300); 
	  
	  GfxEngine::getSingletonPtr()->attachToScene(this);
	  render();
	  mSceneNode->setPosition(mPosition);
	  mSceneNode->pitch(Ogre::Degree(90));
	  	  
    mPhyxShape = new btSphereShape(8);
	  mPhyxMS = new MotionState(btTransform(btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z)),
	      mSceneNode);
        btScalar mass = 1;
        btVector3 fallInertia(0,0,100);
	
    mPhyxShape->calculateLocalInertia(mass,fallInertia);
        
	  btRigidBody::btRigidBodyConstructionInfo
		  mPhyxBodyCI(mass,mPhyxMS,mPhyxShape,fallInertia);
        
	  mPhyxBody = new btRigidBody(mPhyxBodyCI);

    mSceneNode->setVisible(false);
  };

	Obstacle::~Obstacle()
	{
    mLog->infoStream() << "destructed";
		GfxEngine::getSingletonPtr()->detachFromScene(this);
		
		delete mPhyxBody->getMotionState();
    delete mPhyxBody;
    
		delete mPhyxShape;
    
		if (mLog) {
			delete mLog;
			mLog = 0;
		}
	};
	
	void Obstacle::live() {
	  mSceneNode->setVisible(true);
	  mPosition = Vector3(0, 0, mSphere->getSceneNode()->getPosition().z + 500); 
    mPhyxBody->proceedToTransform(btTransform(btQuaternion(0,0,0,1),
	      btVector3(mPosition.x,mPosition.y,mPosition.z)));
	  PhyxEngine::getSingletonPtr()->attachToWorld(this);
	  mDirection = Vector3(0,0,-1);
	  fDead = false;
	  
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
      
    if (mSphere->getSceneNode()->getPosition().z > mSceneNode->getPosition().z) {
      fDead = true;
      return;
    }
		mPhyxBody->activate(true);
		mPhyxBody->applyCentralForce(btVector3(mDirection.x, mDirection.y, mDirection.z));
	};

	
} // end of namespace
