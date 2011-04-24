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

#ifndef H_Obstacle_H
#define H_Obstacle_H

#ifndef H_Entity_H
#include "Entity.h"
#endif

#include "ParticleUniverseSystemManager.h"
#include <list>
#include <map>

using std::list;
using std::map;
using std::pair;
using std::make_pair;
using Ogre::Vector3;

namespace OgreOggSound {
  class OgreOggISound;
}

namespace Pixy
{
  
  typedef enum {
    CHASE,
    DUMB,
    DUETTE,
    SPINNER
  } OBSTACLE_CLASS;
  class Sphere;
    /*! \class Obstacle Obstacle.h "src/Obstacle.h"
     *  \brief 
     */
	class Obstacle : public Entity
	{
	public:
		
		Obstacle();
		Obstacle(const Obstacle& src);
		Obstacle& operator=(const Obstacle& rhs);
		
		virtual ~Obstacle();
		
		virtual void live();
		virtual void die();
		
		virtual void render();
		virtual void update(unsigned long lTimeElapsed);
	  
	  virtual void setDirection(Vector3 inDirection);
	  virtual void setClass(OBSTACLE_CLASS inClass);
	  
	  OBSTACLE_CLASS getClass();
	  
		SHIELD shield();
		void setShield(const SHIELD inShield);
	  
	  Obstacle* getDuetteTwin();
	  void setDuetteTwin(Obstacle* inObs);
	  
	protected:		
		void (Obstacle::*mUpdater)(unsigned long);
		
		void updateChase(unsigned long lTimeElapsed);
		void updateDumb(unsigned long lTimeElapsed);
		void updateStationary(unsigned long lTimeElapsed);
		// called when dead
		void updateNothing(unsigned long lTimeElapsed);
		
		// this is shared between all ALIVE obstacles regardless of class
		void updateCommon(unsigned long lTimeElapsed);
		
		//! helper method for copy ctor and assignment operator
		virtual void copyFrom(const Obstacle& src);
    SHIELD mShield;
    Sphere *mSphere;
    
    Vector3 randomPosition();

    OBSTACLE_CLASS mClass;
    Obstacle* mDuetteTwin;
    
    bool fDying;
    bool fHasFullFx;
    bool fSfxCreated;
    OgreOggSound::OgreOggISound *mSfxExplosion, *mSfxShatter, **mSfx;
    
    ParticleUniverse::ParticleSystem *mBlaze, *mSteam, *mTide, *mMortar;
    static bool ourMeshCreated;
    static btCollisionShape* ourShape;
	};
} // end of namespace
#endif
