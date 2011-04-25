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

#ifndef H_Drone_H
#define H_Drone_H

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
    CHASE, //! CHASE drones fix their direction onto the Probe's
    DUMB, //! DUMB drones move in a fixed direction
    DUETTE, //! duettes are actually DUMB drones with special behaviour
    SPINNER //! not functional currently
  } DRONE_CLASS;
  
  class Probe;
  /*! \class Drone Drone.h "src/Drone.h"
   *  \brief
   *  Drones are the obstacles that face the Probe and collide with it. 
   *
   *  \remark
   *  Drones normally spawn with a random shield and position. Their position
   *  can be fixed according to the Zone settings. However, they always spawn
   *  further than the Probe with a fixed value.
   */
	class Drone : public Entity
	{
	public:

		/*! \brief
		 *  Just like with the Probe:
		 *  prepares a shared mesh, particle systems, sound effects, 
		 *  and the physical component of the probe.
		 */		
		Drone();
		Drone(const Drone& src);
		Drone& operator=(const Drone& rhs);
		
		virtual ~Drone();
		
		/*! \brief
		 *  Similar to the Probe, here is where the Drone is rendered and attached
		 *  to the physical world.
		 */
		virtual void live();
		/*! \brief
		 *  Similar to the Probe, here is where the Drone is detached from scene
		 *  and the world.
		 *
		 *  \note
		 *  NO RESOURCES ARE FREED HERE.
		 */
		virtual void die();
		
		/*! \brief
		 *  Attaches the appropriate material and particle systems according to the shield.
		 */
		virtual void render();
		
		/*! \brief
		 *  See Drone::mUpdater
		 */
		virtual void update(unsigned long lTimeElapsed);
	  
	  
	  
	  /*! \brief
	   *  Changes the class of the Drone and thus its behaviour. See Drone::mUpdater
	   */
	  virtual void setClass(DRONE_CLASS inClass);
	  
	  inline DRONE_CLASS getClass() const {
	    return mClass;
	  };
	  
	  /*! \brief
	   *  The Drone's shield, note that drone shields do not have a state/HP.
	   */
		inline SHIELD getShield() const {
		  return mShield;
		};
		
		/*! \brief
		 *  Changes the shield and re-applies the material and other components accordingly.
		 */
		inline void setShield(const SHIELD inShield) {
		  mShield = inShield; 
	    this->render();
	    mSfx = (mShield == FIRE) ? &mSfxExplosion : &mSfxShatter;
		};
	  
	  inline Drone* getDuetteTwin() {
	    return mDuetteTwin;
	  };
	  inline void setDuetteTwin(Drone* inDrone) {
	    mDuetteTwin = inDrone;
	  };
	  
	protected:
	  /*! \brief
	   *  Sub-state controller; instead of evaluating conditions with 1 update
	   *  routine, we switch routines based on events and changes in states of
	   *  world objects.
	   *
	   *  \remark
	   *  This is done for performance reasons and a cleaner code.
	   */
		void (Drone::*mUpdater)(unsigned long);
		
		/*! \brief
		 *  Probe's position is fetched and a direction vector is calculated which
		 *  will be the path of the CHASE drone. Linear velocity is applied with
		 *  the resulting vector.
		 */
		void updateChase(unsigned long lTimeElapsed);
		/*! \brief
		 *  Applies linear velocity in a fixed direction.
		 */
		void updateDumb(unsigned long lTimeElapsed);
		/*! \brief
		 *  Stationary drones do not move but rotate around in a circle.
		 *
		 *  \warn
		 *  DEPRECATED and NOT used.
		 */
		void updateStationary(unsigned long lTimeElapsed);
		/*! \brief
		 *  Called when dead
		 */
		void updateNothing(unsigned long lTimeElapsed);
		
		/*! \brief
		 *  Check whether we passed the Probe, then we die. 
		 *  We also check for collisions here using simple Ogre AABB queries.
		 *
		 *  \note
		 *  This routine is called for any alive obstacle regardless of class.
		 */ 
		void updateCommon(unsigned long lTimeElapsed);
		
		//! helper method for copy ctor and assignment operator
		virtual void copyFrom(const Drone& src);
		
    SHIELD mShield;
    Probe *mProbe;
    
    /*! \brief
     *  Calculates a starting position based on the Probe's and zone settings.
     */
    Vector3 randomPosition();

    DRONE_CLASS mClass;
    Drone* mDuetteTwin; //! used by Probe to calculate the score, see Probe.cpp for more info
    bool fHasFullFx; //! Drones have 3 visual effects levels: low, medium and full
    OgreOggSound::OgreOggISound *mSfxExplosion, *mSfxShatter, **mSfx;
    
    ParticleUniverse::ParticleSystem *mBlaze, *mSteam, *mTide, *mMortar;
    
    static bool ourMeshCreated;
    static btCollisionShape* ourShape; // shapes can be shared across rigid bodies
	};
} // end of namespace
#endif
