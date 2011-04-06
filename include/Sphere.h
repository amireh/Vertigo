/* -----------------------------------------------
 *  Filename: Sphere.h
 *  Date Created: ??/2009
 *
 *  Original Author:
 *      Ahmad Amireh ( ahmad.amireh@gmail.com )
 *
 *  Last Update:
 *      Date:   20/4/2009
 *      By:     Ahmad Amireh
 * ----------------------------------------------- */

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
		
		void reset();
		//float getMaxSpeed() const;
		
		//virtual void collide(Obstacle* inObj);
		
		const Vector3& getPosition();
		const int score();
	protected:		
		void (Sphere::*mUpdate)(unsigned long);
		
		bool evtGameStarted(Event* inEvt);
		bool evtZoneEntered(Event* inEvt);
		bool evtPortalEntered(Event* inEvt);
		bool evtPortalSighted(Event* inEvt);
		bool evtObstacleCollided(Event* inEvt);
		
		void locateNextWaypoint();
		
		void updatePreparation(unsigned long lTimeElapsed);
		void updateGame(unsigned long lTimeElapsed);
		
		//! helper method for copy ctor and assignment operator
		virtual void copyFrom(const Sphere& src);
		
		SHIELD mCurrentShield;
		map<SHIELD, int> mShields;
		Ogre::SimpleSpline *mPath;
		list<Vector3> mWaypoints;
		Vector3* mNextWaypoint;
		
		int mScore;
		//Ogre::Vector3 mDirection;
		OgreOggSound::OgreOggISound *mSfxRumbling, *mSfxFlipShield, *mSfxBeep;
		ParticleUniverse::ParticleSystem *mFireEffect, *mIceEffect;
		
		bool fDamped;
		bool fPortalSighted;
	};
} // end of namespace
#endif
