/* -----------------------------------------------
 *  Filename: Pause.h
 *  Date Created: ??/2009
 *
 *  Original Author:
 *      IMPORTANT : GET AUTHOR NAME FROM OGRE WIKI
 *
 *  Last Update:
 *      Date:   20/4/2009
 *      By:     Ahmad Amireh
 * ----------------------------------------------- */

#ifndef H_Pause_H
#define H_Pause_H

#include <list>
#include "GameState.h"
#include "EventManager.h"
#include "EventListener.h"
//#include "UIEngine.h"
#include "GfxEngine.h"
#include "PhyxEngine.h"
#include <map>
#include "Sphere.h"
#include "Obstacle.h"


using std::list;
namespace Pixy
{
    /*! \class Pause
     *  \brief
     *  Main Menu state, handles non-Combat game tasks.
     */
	
	
	class Pause : public GameState, public EventListener {
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
		
		static Pause* getSingletonPtr( void );
		static Pause& getSingleton();
		
    virtual bool areFxEnabled();
    virtual bool areSfxEnabled();		
		virtual GAME_STATE getId() const;
    virtual void dontUpdateMe(Engine* inEngine);
		Sphere* getSphere();

	protected:
		void fireLoginEvt();
		bool evtPortalReached(Event* inEvt);
		bool evtPortalSighted(Event* inEvt);
		
	private:
		Pause( void ) { }
		Pause( const Pause& ) { }
		Pause & operator = ( const Pause& );
		
		EventManager *mEvtMgr;
		GfxEngine		*mGfxEngine;
		//UIEngine		*mUIEngine;
		PhyxEngine		*mPhyxEngine;
		//CEGUI::System	*mUISystem;
		Sphere			  *mSphere;
		std::list<Obstacle*> mObstacles;
		std::list<Obstacle*> mObstaclePool;
		std::list<Obstacle*> mDeadObstacles;
		long nrObstacles;
		static Pause    *mPause;
		Ogre::Timer mTimer;
		bool fSpawning; // are we spawning obstacles?
		void spawnObstacle();
		void releaseObstacle(Obstacle* inObs);
	};
} // end of namespace
#endif
