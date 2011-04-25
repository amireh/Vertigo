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
	
	class Probe;
	
	/*! \class PhyxEngine
	 *	\brief
	 *	Wrapper over BulletPhysics, handles the dynamics world and everything 
	 *  related to it.
	 */
  class PhyxEngine : public Engine
	{
	public:
		
		virtual ~PhyxEngine();
		static PhyxEngine* getSingletonPtr();
		
		/*! \brief
		 *  Creates the world and physical geometry.
		 */
		virtual bool setup();
		
		/*! \brief
		 *  Grabs a handle to the Probe so it can regulate its velocity.
		 */
		virtual bool deferredSetup();
		
		/*! \brief
		 *  Steps the world.
		 */
		virtual void update(unsigned long lTimeElapsed);
		
		/*! \brief
		 *  Destroys the world and cleans up all the resources.
		 */
		virtual bool cleanup();
		
		/*btDiscreteDynamicsWorld* getWorld() {
		  return mWorld;
		};*/
		
		void attachToWorld(Pixy::Entity* inEntity);
		void detachFromWorld(Pixy::Entity* inEntity);
		
		/*! \brief
		 *  Regulates the velocity of the Probe to not exceed Probe::mMaxSpeed.
		 *
		 *  \note
		 *  Adapted from BulletPhysics Wiki.
		 */
		static void myTickCallback(btDynamicsWorld *world, btScalar timeStep);
		
		/*! \brief
		 *  Convenience method for getting the Probe's mMaxSpeed;
		 *
		 *  \note
		 *  Called internally by myTickCallback();
		 */
		float _getMaxSpeed() const;
		
	protected:
	  
		btBroadphaseInterface* mBroadphase;
		btDefaultCollisionConfiguration* mCollisionConfig;
		btCollisionDispatcher* mDispatcher;
		btSequentialImpulseConstraintSolver* mSolver;
		btDiscreteDynamicsWorld *mWorld;
		btCollisionWorld* mCWorld;
		
		Ogre::Entity *mTunnelEntity;
    btRigidBody *mTunnelBody;
    btCollisionShape *mTunnelShape, *mObstacleShape;
    
    BtOgre::DebugDrawer *mDbgdraw;
		
		Probe* mProbe;
	private:
		static PhyxEngine* _myPhyxEngine;
		PhyxEngine();
		PhyxEngine(const PhyxEngine& src);
		PhyxEngine& operator=(const PhyxEngine& rhs);		
    };
};



#endif
