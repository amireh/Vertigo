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

#include "Zone.h"

using std::list;
namespace Pixy
{
  /*! \class Intro
   *  \brief
   *  Main Menu state, handles the UI for configuring the game and starting the game.
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
		
		inline virtual GAME_STATE getId() const {
		  return STATE_INTRO;
		};
    
    /*! \brief
     *  Returns handle to the zone selected by the player to engage.
     *
     *  \note
     *  This is cloned by Level whenever a zone is requested to be played.
     */
    inline Zone* getSelectedZone() {
      return mSelectedZone;
    };
    
    /*! \brief
     *  Assigns the zone to be played. This is called by the UIEngine whenever
     *  the player previews a zone in the Zone screen.
     */
    inline void setSelectedZone(Zone* inZone) {
      mSelectedZone = inZone;
    };
    
	protected:
	  bool evtSettingsChanged(Event* inEvt);
	  Zone* mSelectedZone;

	private:
		Intro( void ) { fRunning = false; }
		Intro( const Intro& ) { }
		Intro & operator = ( const Intro& );
		
		EventManager  *mEvtMgr;
		GfxEngine		  *mGfxEngine;
		UIEngine		  *mUIEngine;
		PhyxEngine		*mPhyxEngine;
		SfxEngine     *mSfxEngine;

		static Intro   *mIntro;
		
	};
} // end of namespace
#endif
