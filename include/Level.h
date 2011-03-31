/* -----------------------------------------------
 *  Filename: Level.h
 *  Date Created: ??/2009
 *
 *  Original Author:
 *      IMPORTANT : GET AUTHOR NAME FROM OGRE WIKI
 *
 *  Last Update:
 *      Date:   20/4/2009
 *      By:     Ahmad Amireh
 * ----------------------------------------------- */

#ifndef H_Level_H
#define H_Level_H

#include <list>
#include <map>
#include "GameState.h"
#include "EventManager.h"
#include "EventListener.h"

#include "UIEngine.h"
#include "GfxEngine.h"
#include "PhyxEngine.h"
#include "AudioEngine.h"

#include "Sphere.h"
#include "Obstacle.h"
#include "Tunnel.h"

using std::list;
namespace Pixy
{
    /*! \class Level
     *  \brief
     *  Main Menu state, handles non-Combat game tasks.
     */
	
	
	class Level : public GameState, public EventListener {
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
		
		static Level* getSingletonPtr( void );
		static Level& getSingleton();
		
		virtual GAME_STATE getId() const;

		Sphere* getSphere();
    Tunnel* getTunnel();
    
    bool areFxEnabled();
    bool areSfxEnabled();
    
    void dontUpdateMe(Engine* inEngine);
    
	protected:

		bool evtPortalEntered(Event* inEvt);
		bool evtPortalReached(Event* inEvt);
		bool evtPortalSighted(Event* inEvt);
		
		Tunnel *mTunnel;
		std::list<Tunnel*> mTunnels;
		std::list<Engine*> mEngines;
		
		std::list<Obstacle*>::iterator _itrObstacles;
		std::list<Engine*>::iterator _itrEngines;
	private:
		Level( void ) { }
		Level( const Level& ) { }
		Level & operator = ( const Level& );
		
		EventManager *mEvtMgr;
		GfxEngine		*mGfxEngine;
		UIEngine		*mUIEngine;
		PhyxEngine		*mPhyxEngine;
		AudioEngine *mSfxEngine;
		CEGUI::System	*mUISystem;
		Sphere			  *mSphere;
		std::list<Obstacle*> mObstacles;
		std::list<Obstacle*> mObstaclePool;
		std::list<Obstacle*> mDeadObstacles;
		
		
		
		long nrObstacles;
		int nrTunnels;
		
		static Level    *mLevel;
		Ogre::Timer mTimer;
		bool fSpawning; // are we spawning obstacles?
		void spawnObstacle();
		void releaseObstacle(Obstacle* inObs);
	};
} // end of namespace
#endif
