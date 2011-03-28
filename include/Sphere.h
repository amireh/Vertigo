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

using std::list;
using std::map;
using std::pair;
using std::make_pair;
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
		
		SHIELD shield();
		void flipShields();
		
		void setMaxSpeed(float inSpeed);
		float getMaxSpeed() const;
		
		//virtual void collide(Obstacle* inObj);
		
		const Vector3& getPosition();
	protected:		
		
		bool evtPortalSighted(Event* inEvt);
		bool evtObstacleCollided(Event* inEvt);
		
		void locateNextWaypoint();
		
		//! helper method for copy ctor and assignment operator
		virtual void copyFrom(const Sphere& src);
		
		SHIELD mCurrentShield;
		map<SHIELD, int> mShields;
		Ogre::SimpleSpline *mPath;
		list<Vector3> mWaypoints;
		Vector3* mNextWaypoint;
		
		Ogre::Real mDistance;
		Ogre::Real move;
		float mMaxSpeed;
		float step;
		float currentStep;
		//Ogre::Vector3 mDirection;
		Ogre::SceneNode* mFireTrailNode, *mIceSteamNode;
	};
} // end of namespace
#endif
