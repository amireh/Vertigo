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
 
#include "Entity.h"
#include "Utility.h"
#include "Level.h"

namespace Pixy
{

  long Entity::idCounter = 0;
  
	Entity::~Entity()
	{
	};

  Entity::Entity()
  {
		
		idObject = ++idCounter;
		mName = "Unnamed";
		mMesh = "";
		mSceneNode = mMasterNode = 0;
		mSceneObject = 0;
		
		mMoveSpeed = mMaxSpeed = 0;
		mDirection = Vector3::ZERO;
		fDead = true;
		
		fHasFx = GameManager::getSingleton().currentState()->areFxEnabled();
		fHasSfx = GameManager::getSingleton().currentState()->areSfxEnabled();
	};

  Entity::Entity(const Entity& src)
  {
      copyFrom(src);
  };

  Entity& Entity::operator=(const Entity& rhs)
  {
      // check for self-assignment
      if (this == &rhs)
          return (*this);

      copyFrom(rhs);

      return (*this);
  };

  void Entity::copyFrom(const Entity& src)
  {
      idObject = src.idObject;
      mName = src.mName;
      mMesh = src.mMesh;
      
      mDirection = src.mDirection;
      mMoveSpeed = src.mMoveSpeed;
      mMaxSpeed = src.mMaxSpeed;
      
      fDead = src.fDead;
      fHasFx = src.fHasFx;
      fHasSfx = src.fHasSfx;
      
      mSceneNode = src.mSceneNode;
      mMasterNode = src.mMasterNode;
      mSceneObject = src.mSceneObject;
      
      mPhyxBody = src.mPhyxBody;
      mPhyxShape = src.mPhyxShape;
      mPhyxMS = src.mPhyxMS;
      
      mLog = src.mLog;
  };
	
	void Entity::collide(Entity* target) {
	  if (mLog)
	    mLog->warnStream() << "parent collide() called, children MUST override";
	}
	
} // end of namespace
