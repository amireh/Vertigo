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

using std::list;
using std::map;
using std::pair;
using std::make_pair;
namespace Pixy
{

    /*! \class Sphere Sphere.h "src/Sphere.h"
     *  \brief Represents Combat Sphere GameObject
     */
	class Sphere : public Entity
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
		
		SHIELD currentShield();
		void flipShields();
		
	protected:		
		
		//! helper method for copy ctor and assignment operator
		virtual void copyFrom(const Sphere& src);
		
		SHIELD mCurrentShield;
		map<SHIELD, int> mShields;  
	};
} // end of namespace
#endif
