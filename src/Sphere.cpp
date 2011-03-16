// Sphere.cpp
#include "Sphere.h"
#include "Utility.h"
#include "GfxEngine.h"
#include "Intro.h"
#include "PhyxEngine.h"

namespace Pixy
{
	
    Sphere::Sphere()
    {
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Sphere");
        mLog->infoStream() << "created";
		
		mName = "Sphere";
		mMesh = "SphereMesh";
		mMoveSpeed = 0.2;
		
		GfxEngine::getSingletonPtr()->createSphere(mMesh, 10, 64, 64);
		GfxEngine::getSingletonPtr()->attachToScene(this);
		/*
		mSceneObject = mSceneMgr->createEntity("mySphereEntity", mMesh);
		mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		mSceneNode->attachObject(mSceneObject);
		*/
		static_cast<Ogre::Entity*>(mSceneObject)->setMaterialName("Examples/RustySteel");
		mSceneNode->setPosition(Ogre::Vector3(0,10,0));
		
        mPhyxShape = new btSphereShape(1);
		mPhyxMS = new MotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,120,0)), mSceneNode);
        btScalar mass = 1;
        btVector3 fallInertia(0,0,0);
		
        mPhyxShape->calculateLocalInertia(mass,fallInertia);
        
		btRigidBody::btRigidBodyConstructionInfo
			mPhyxBodyCI(mass,mPhyxMS,mPhyxShape,fallInertia);
        
		mPhyxBody = new btRigidBody(mPhyxBodyCI);
		

		PhyxEngine::getSingletonPtr()->attachToWorld(this);
    };

	Sphere::~Sphere()
	{
	    mLog->infoStream() <<"destructed";
		GfxEngine::getSingletonPtr()->detachFromScene(this);
		
		delete mPhyxBody->getMotionState();
        delete mPhyxBody;

		delete mPhyxShape;

		if (mLog) {
			delete mLog;
			mLog = 0;
		}
	};
	
	void Sphere::live() {};
	void Sphere::die() {};
	
	void Sphere::render() {
		
	};
	
	void Sphere::copyFrom(const Sphere& src) {
	};
	
	
	void Sphere::keyPressed( const OIS::KeyEvent &e )
	{
		
		switch (e.key) {
			case OIS::KC_W:
				mDirection.z = +mMoveSpeed;
				break;
			case OIS::KC_A:
				//mPhyxBody->clearForces();
				mDirection.x = mMoveSpeed * 2;
				break;
			case OIS::KC_D:
				//mPhyxBody->clearForces();
				mDirection.x = -mMoveSpeed * 2;
				break;
				
		}
		
	}
	
	void Sphere::keyReleased( const OIS::KeyEvent &e ) {
		
		switch (e.key) {
			case OIS::KC_W:
				mDirection.z = 0;
				break;
			case OIS::KC_A:
				mDirection.x = 0;
				break;
			case OIS::KC_D:
				mDirection.x = 0;
				break;
				
		}
		
	}
	

	void Sphere::update(unsigned long lTimeElapsed) {
		mPhyxBody->activate(true);
		mPhyxBody->applyCentralForce(btVector3(mDirection.x, mDirection.y, mDirection.z));
		
		//btTransform trans;
		//mPhyxBody->getMotionState()->getWorldTransform(trans);
		//mSceneNode->translate(mDirection * lTimeElapsed, Ogre::Node::TS_LOCAL);
		//mSceneNode->setPosition(Vector3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
	};
	
} // end of namespace
