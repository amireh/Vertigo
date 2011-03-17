/*
 *  MotionState.h
 *  Vertigo
 *
 *  Created by Ahmad Amireh on 3/16/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef H_MotionState_H
#define H_MotionState_H

#include <Ogre.h>
#include <btBulletDynamicsCommon.h>

namespace Pixy {
	
	class MotionState : public btMotionState {
	public:
		MotionState(const btTransform &initialpos, Ogre::SceneNode *node) {
			mVisibleobj = node;
			mPos1 = initialpos;
		}
		
		virtual ~MotionState() {
		}
		
		Ogre::SceneNode* getNode() { return mVisibleobj; };
		
		void setNode(Ogre::SceneNode *node) {
			mVisibleobj = node;
		}
		
		virtual void getWorldTransform(btTransform &worldTrans) const {
			worldTrans = mPos1;
		}
		
		virtual void setWorldTransform(const btTransform &worldTrans) {
			if(NULL == mVisibleobj) return; // silently return before we set a node
			btQuaternion rot = worldTrans.getRotation();
			mVisibleobj->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
			btVector3 pos = worldTrans.getOrigin();
			mVisibleobj->setPosition(pos.x(), pos.y(), pos.z());
		}
		
	protected:
		Ogre::SceneNode *mVisibleobj;
		btTransform mPos1;
	};
}

#endif
