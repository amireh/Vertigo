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

#ifndef H_Probe_H
#define H_Probe_H

#ifndef H_Entity_H
#include "Entity.h"
#endif

#include <list>
#include <map>
#include "InputManager.h"
#include "EventListener.h"
#include "ParticleUniverseSystemManager.h"

using std::list;
using std::map;
using std::pair;
using std::make_pair;

namespace OgreOggSound {
  class OgreOggISound;
};

using Ogre::Real;
namespace Pixy
{

  class Drone;
    /*! \class Probe Probe.h "src/Probe.h"
     *  \brief Represents Combat Probe GameObject
     */
	class Probe : public Entity, public EventListener
	{
	public:
		
		/*! \brief
		 *  Prepares the mesh, particle systems, sound effects, and the physical component
		 *  of the probe.
		 */
		Probe();
		Probe(const Probe& src);
		Probe& operator=(const Probe& rhs);
		
		virtual ~Probe();
		
		/*! \brief
		 *  Shows the MasterNode of the probe and attaches it to the physical world.
		 */
		virtual void live();
		
		/*! \brief
		 *  Detaches the MasterNode from the scene and the probe from the physical world.
		 *
		 *  \note
		 *  NO RESOURCES ARE FREED HERE.
		 */
		virtual void die();
		
		/*! \brief
		 *  Respective to mCurrentShield, applies a material and starts/stops the
		 *  particle systems. 
		 */
		virtual void render();
		
		/*! \brief
		 *  See updatePreparation() and updateGame()
		 */
		virtual void update(unsigned long lTimeElapsed);

		void reset();
		
		/*! \brief
		 *  Bindings for moving left, right, a bit forward, and flipping shields. 
		 */
		void keyPressed( const OIS::KeyEvent &e );
		/*! \brief
		 *  Since moving left and right is buffered, we need to track it here.
		 */
		void keyReleased( const OIS::KeyEvent &e );
		
		inline SHIELD getShield() const {
		  return mCurrentShield; 
	  };
		
		inline int getShieldState() { 
		  return mShields[mCurrentShield];
	  };
	  
		inline int getShieldState(SHIELD inShield) {
		  return mShields[inShield];
		};
		
		inline const Vector3& getPosition() {
		  return mMasterNode->getPosition();
		};
		inline int getScore() const {
		  return mScore;
		};
		
		inline unsigned long getNrHits() const { 
		  return mNrHits;
		};
		inline unsigned long getNrMisses() const { 
		  return mNrMisses;
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
		void (Probe::*mUpdate)(unsigned long);
		
		/*! \brief
		 *  This is the starting update method, used after engaging a zone and before
		 *  playing.
		 */
		void updatePreparation(unsigned long lTimeElapsed);
		/*! \brief
		 *  Moves the probe, and checks whether we reached the last portal, or we're near it.
		 */
		void updateGame(unsigned long lTimeElapsed);
				
		/*! \brief
		 *  Resets the probe in preparation for playing the new zone.
		 */
		bool evtGameStarted(Event* inEvt);
		/*! \brief
		 *  Attaches the probe to the world and begins updating it using updateGame().
		 */
		bool evtZoneEntered(Event* inEvt);
		/*! \brief
		 *  Updates the movement settings according to the zone.
		 */
		bool evtPortalEntered(Event* inEvt);
		/*! \brief
		 *  When we're near a portal, movement and controls are disabled, and collisions
		 *  don't affect our shields anymore.
		 */
		bool evtPortalSighted(Event* inEvt);
		/*! \brief
		 *  Handles the logic behind colliding with drones.
		 *
		 *  \remark
		 *  There are two different routines for collision that depend on the game mode;
		 *  but commonly, the score is updated, the shield state is modified, and
		 *  sound effects are played accordingly.
		 */
		bool evtDroneCollided(Event* inEvt);
		/*! \brief
		 *  Stops/plays music and sound effects based on the settings the user applies.
		 */
		bool evtSettingsChanged(Event* inEvt);
		
		bool (Probe::*mCollide)(Event*);
		/*! \brief
		 *  Applies to GameModes: Arcade, Dodgy, Survival.
		 *
		 *  \remark
		 *  A miss is the result of colliding with a drone with the opposite shield,
		 *  and a hit is the opposite.
		 */
		bool collideCommon(Event* inEvt);
		/*! \brief
		 *  Applies to GameModes: Nervebreak.
		 *
		 *  \remark
		 *  A hit is the result of colliding with a drone with the opposite shield,
		 *  and a miss is the opposite. This is directly the negation of the collideCommon.
		 */
		bool collideNervebreak(Event* inEvt);
		
		void flipShields();
		
		//! helper method for copy ctor and assignment operator
		virtual void copyFrom(const Probe& src);
		
		SHIELD mCurrentShield;
		map<SHIELD, int> mShields;
		
		float mSpeedStep;
		int mScore;

		OgreOggSound::OgreOggISound *mSfxRumbling, *mSfxFlip, *mSfxBeep, *mSfxWarning;
		ParticleUniverse::ParticleSystem *mFireEffect, *mIceEffect;
		
		bool fPortalSighted;
		bool fPortalReached;
		
		// score
		unsigned long mNrMisses;
		unsigned long mNrHits;
		
		int mRegenRate; // in survival mode, the regen rate is higher than the others
		
		Real mLastTunnelSegment;
		Real mTunnelLength;
	};
} // end of namespace
#endif
