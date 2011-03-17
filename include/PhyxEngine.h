/*
 *  PhyxEngine.h
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/15/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */


#ifndef H_PhyxEngine_H
#define H_PhyxEngine_H

#include "Pixy.h"
#include "Engine.h"
#include "Entity.h"
#include <btBulletDynamicsCommon.h>

namespace Pixy
{
	
	class Sphere;
	
	/*! \class PhyxEngine
	 *	\brief
	 *	Binds physics functionality in game Entity objects, handles collision 
	 *	events, manages all Physics-related resources, and acts as a wrapper
	 *	over nVidia's PhyX system.
	 */
  class PhyxEngine : public Engine
	{
	public:
		
		virtual ~PhyxEngine();
		static PhyxEngine* getSingletonPtr();
		
		
		virtual bool setup();
		virtual bool deferredSetup();
		virtual void update(unsigned long lTimeElapsed);
		virtual bool cleanup();

		/*
		void mouseMoved( const OIS::MouseEvent &e );
		void mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		void mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		*/
		
		btDiscreteDynamicsWorld* world();
		
		void attachToWorld(Pixy::Entity* inEntity);
		void detachFromWorld(Pixy::Entity* inEntity);
		
		btCollisionShape* obstaclesShape();
		
	protected:
		btBroadphaseInterface* mBroadphase;
		btDefaultCollisionConfiguration* mCollisionConfig;
		btCollisionDispatcher* mDispatcher;
		btSequentialImpulseConstraintSolver* mSolver;
		btDiscreteDynamicsWorld *mWorld;
		btCollisionWorld* mCWorld;
		
		btCollisionObject *mFloor, *mCeiling, *mLWall, *mRWall;
		btCollisionShape *mFloorShape, *mCeilingShape, *mLWallShape, *mRWallShape, *mObstacleShape;
		btDefaultMotionState *mFloorMS, *mCeilingMS, *mLWallMS, *mRWallMS;
		btRigidBody *mFloorBody, *mCeilingBody, *mLWallBody, *mRWallBody;
		
		Sphere* mSphere;
	private:
		static PhyxEngine* _myPhyxEngine;
		PhyxEngine();
		PhyxEngine(const PhyxEngine& src);
		PhyxEngine& operator=(const PhyxEngine& rhs);		
    };
};



#endif
