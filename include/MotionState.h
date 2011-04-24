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
