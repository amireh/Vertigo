/* -----------------------------------------------
 *  Filename: Intro.h
 *  Date Created: ??/2009
 *
 *  Original Author:
 *      IMPORTANT : GET AUTHOR NAME FROM OGRE WIKI
 *
 *  Last Update:
 *      Date:   20/4/2009
 *      By:     Ahmad Amireh
 * ----------------------------------------------- */

#ifndef H_Intro_H
#define H_Intro_H

#include <list>
#include <map>
#include "GameState.h"
#include "EventManager.h"
#include "EventListener.h"

#include "UIEngine.h"
#include "GfxEngine.h"
#include "PhyxEngine.h"
#include "SfxEngine.h"

#include "Sphere.h"
#include "Obstacle.h"
#include "Tunnel.h"

using std::list;
namespace Pixy
{
    /*! \class Intro
     *  \brief
     *  Main Menu state, handles non-Combat game tasks.
     */
	
	
	class Intro : public GameState, public EventListener {
	public:
		
		void enter( void );
		void exit( void );
		
		void pause( void );
		void resume( void );
		void update( unsigned long lTimeElapsed );
		
		void keyPressed( const OIS::KeyEvent &e );
		void keyReleased( const OIS::KeyEvent &e );
		
		void mouseMoved( const OIS::MouseEvent &e );
		void mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		void mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		
		static Intro* getSingletonPtr( void );
		static Intro& getSingleton();
		
		virtual GAME_STATE getId() const;
    
    virtual bool areFxEnabled();
    virtual bool areSfxEnabled();
    virtual void dontUpdateMe(Engine* inEngine);
    
	protected:

		std::list<Engine*> mEngines;
		
		std::list<Engine*>::iterator _itrEngines;
	private:
		Intro( void ) { }
		Intro( const Intro& ) { }
		Intro & operator = ( const Intro& );
		
		EventManager *mEvtMgr;
		GfxEngine		*mGfxEngine;
		UIEngine		*mUIEngine;
		PhyxEngine		*mPhyxEngine;
		SfxEngine *mSfxEngine;
		Sphere			  *mSphere;
		std::list<Obstacle*> mObstacles;
		
		static Intro   *mIntro;
		
	};
} // end of namespace
#endif
