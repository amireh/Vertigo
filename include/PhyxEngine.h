/*
 *  This file is part of Vertigo.
 *
 *  Vertigo - a cross-platform arcade game powered by Ogre3D.
 *  Copyright (C) 2011  Ahmad Amireh <ahmad@amireh.net>
 * 
 *  Vertigo is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Vertigo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Vertigo.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef H_PhyxEngine_H
#define H_PhyxEngine_H

#include "Pixy.h"
#include "Engine.h"
#include "Entity.h"
#include <btBulletDynamicsCommon.h>
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"

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
		
		static void myTickCallback(btDynamicsWorld *world, btScalar timeStep);
		
		float getMaxSpeed() const;
		
	protected:
	  
		btBroadphaseInterface* mBroadphase;
		btDefaultCollisionConfiguration* mCollisionConfig;
		btCollisionDispatcher* mDispatcher;
		btSequentialImpulseConstraintSolver* mSolver;
		btDiscreteDynamicsWorld *mWorld;
		btCollisionWorld* mCWorld;
		
		/*btCollisionObject *mFloor, *mCeiling, *mLWall, *mRWall, *mBWall;
		btCollisionShape *mFloorShape, *mCeilingShape, *mLWallShape, *mRWallShape, *mBWallShape;
		btDefaultMotionState *mFloorMS, *mCeilingMS, *mLWallMS, *mRWallMS, *mBWallMS;
		btRigidBody *mFloorBody, *mCeilingBody, *mLWallBody, *mRWallBody, *mBWallBody;*/
		Ogre::Entity *mTunnelEntity;
    btRigidBody *mTunnelBody;
    btCollisionShape *mTunnelShape, *mObstacleShape;
    
    BtOgre::DebugDrawer *mDbgdraw;
		
		Sphere* mSphere;
		
		//btScalar mMaxSpeed;
	private:
		static PhyxEngine* _myPhyxEngine;
		PhyxEngine();
		PhyxEngine(const PhyxEngine& src);
		PhyxEngine& operator=(const PhyxEngine& rhs);		
    };
};



#endif
