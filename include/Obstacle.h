/* -----------------------------------------------
 *  Filename: Obstacle.h
 *  Date Created: ??/2009
 *
 *  Original Author:
 *      Ahmad Amireh ( ahmad.amireh@gmail.com )
 *
 *  Last Update:
 *      Date:   20/4/2009
 *      By:     Ahmad Amireh
 * ----------------------------------------------- */

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
