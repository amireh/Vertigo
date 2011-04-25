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
#include "SfxEngine.h"

#include "Probe.h"
#include "Drone.h"
#include "Zone.h"

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
		
		inline GAME_STATE getId() const {
		  return STATE_GAME;
		};

		inline Probe* getProbe() const { 
		  return mProbe;
		};
    inline Tunnel* getTunnel() const {
      return mZone->currentTunnel();
    };
    inline Zone* currentZone() const {
      return mZone;
    };
    
    inline bool isGameOver() {
      return fGameOver;
    };
    
    // spawn rate will be increased by (mSpawnThreshold / inFactor) milliseconds
    // NOTE: called internally by Zone each time the player enters a portal
    void increaseSpawnRate(const int inFactor);
    inline int currentSpawnRate() const {
      return mSpawnRate;
    };
        
    void reset();
    
    void _hideEverything();
    void _showEverything();
    
	protected:
    void (Level::*mUpdater)(unsigned long);
    
    void updatePreparation(unsigned long lTimeElapsed);
    void updateGame(unsigned long lTimeElapsed);
    void updateGameOver(unsigned long lTimeElapsed);
    void updateNothing(unsigned long lTimeElapsed);
    
    bool evtZoneEntered(Event* inEvt);
    bool evtPlayerWon(Event* inEvt);
    bool evtProbeDied(Event* inEvt);
		bool evtPortalEntered(Event* inEvt);
		bool evtPortalSighted(Event* inEvt);
		bool evtTakingScreenshot(Event* inEvt);
		bool evtScreenshotTaken(Event* inEvt);
		
		//Tunnel *mTunnel;
		//std::list<Tunnel*> mTunnels;
		Zone* mZone;
		std::list<Engine*> mEngines;
		bool fScreenshotTaken;
		
		std::list<Drone*>::iterator _itrDrones;
		std::list<Engine*>::iterator _itrEngines;
	private:
		Level( void );
		Level( const Level& ) { }
		Level & operator = ( const Level& );
		
		EventManager  *mEvtMgr;
		GfxEngine		  *mGfxEngine;
		UIEngine		  *mUIEngine;
		PhyxEngine		*mPhyxEngine;
		SfxEngine     *mSfxEngine;
		Probe			    *mProbe;
		std::list<Drone*> mDrones;
		std::list<Drone*> mDronePool;
		
		long nrDrones;
		//int nrTunnels;
		
		static Level *mLevel;
		Ogre::Timer mSpawnTimer;
		
		bool fGameStarted;
		bool fGameOver;
		bool fSpawning; //! are we spawning drones?
		int mSpawnRate; //! how often in milliseconds do we spawn drones?
		int mSpawningThreshold; // spawn rate can't drop below this, original rate / 2
		
		bool fFirstZone;
		
		Drone* spawnDrone(DRONE_CLASS inClass);
		void spawnDuette();
		void releaseDrone(Drone* inObs);
	};
} // end of namespace
#endif
