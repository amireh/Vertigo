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
