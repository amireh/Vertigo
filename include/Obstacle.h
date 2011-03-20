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

#include <list>
#include <map>
#include "InputManager.h"

using std::list;
using std::map;
using std::pair;
using std::make_pair;
using Ogre::Vector3;
namespace Pixy
{
  
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
				
		SHIELD shield();
		
		virtual void collide(Entity* inObj);
		
	protected:		
		
		//! helper method for copy ctor and assignment operator
		virtual void copyFrom(const Obstacle& src);
    SHIELD mShield;
    Sphere *mSphere;
    
    Vector3 randomPosition();
    Ogre::Timer mTimer;
    bool fDying;
    int mDeathDuration;
	};
} // end of namespace
#endif
