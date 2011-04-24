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

#ifndef H_Sphere_H
#define H_Sphere_H

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

  class Obstacle;
    /*! \class Sphere Sphere.h "src/Sphere.h"
     *  \brief Represents Combat Sphere GameObject
     */
	class Sphere : public Entity, public EventListener
	{
	public:
		
		Sphere();
		Sphere(const Sphere& src);
		Sphere& operator=(const Sphere& rhs);
		
		virtual ~Sphere();
		
		virtual void live();
		virtual void die();
		
		virtual void render();
		virtual void update(unsigned long lTimeElapsed);
		
		void keyPressed( const OIS::KeyEvent &e );
		void keyReleased( const OIS::KeyEvent &e );
		void mouseMoved( const OIS::MouseEvent& e );
		
		SHIELD shield();
		void flipShields();
		
		int getShieldState();
		int getShieldState(SHIELD inShield);
		
		void reset();
		//float getMaxSpeed() const;
		
		//virtual void collide(Obstacle* inObj);
		
		const Vector3& getPosition();
		const int score();
		
		unsigned long getNrHits() const;
		unsigned long getNrMisses() const;
	protected:		
		void (Sphere::*mUpdate)(unsigned long);
		
		bool evtGameStarted(Event* inEvt);
		bool evtZoneEntered(Event* inEvt);
		bool evtPortalEntered(Event* inEvt);
		bool evtPortalSighted(Event* inEvt);
		bool evtObstacleCollided(Event* inEvt);
		bool evtSettingsChanged(Event* inEvt);
		
		void locateNextWaypoint();
		
		void updatePreparation(unsigned long lTimeElapsed);
		void updateGame(unsigned long lTimeElapsed);
		
		bool (Sphere::*mCollide)(Event*);
		bool collideCommon(Event* inEvt);
		bool collideNervebreak(Event* inEvt);
		
		//! helper method for copy ctor and assignment operator
		virtual void copyFrom(const Sphere& src);
		
		SHIELD mCurrentShield;
		map<SHIELD, int> mShields;
		Ogre::SimpleSpline *mPath;
		list<Vector3> mWaypoints;
		Vector3* mNextWaypoint;
		
		float mSpeedStep;
		
		int mScore;
		//Ogre::Vector3 mDirection;
		OgreOggSound::OgreOggISound *mSfxRumbling, *mSfxFlip, *mSfxBeep, *mSfxWarning;
		ParticleUniverse::ParticleSystem *mFireEffect, *mIceEffect;
		
		bool fDamped;
		bool fPortalSighted;
		bool fPortalReached;
		bool fStrafing;
		
		// score
		unsigned long mNrMisses;
		unsigned long mNrHits;
		
		int mRegenRate; // in survival mode, the regen rate is higher than the others
		
		Real mLastTunnelSegment;
		Real mTunnelLength;
	};
} // end of namespace
#endif
