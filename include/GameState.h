/*
 *  This file is part of Vertigo.
 *
 *  Vertigo - a cross-platform arcade game powered by Ogre3D.
 *  Copyright (C) 2011  spacegaier, Ahmad Amireh <ahmad@amireh.net>
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

#ifndef H_GameState_H
#define H_GameState_H

#ifndef H_GameManager_H
#include "GameManager.h"
#endif
#ifndef H_InputManager_H
#include "InputManager.h"
#endif

namespace Pixy
{
    /*! \class GameState
     *  \brief
     *  Defines general behaviour of game states such as the Main Menu, and Combat.
     */
  class Engine;
	class GameState {
		
	public:
		~GameState(){ };
		
		virtual void enter( void )  = 0;
		virtual void exit( void )   = 0;
		
		virtual void pause( void )  = 0;
		virtual void resume( void ) = 0;
		virtual void update( unsigned long lTimeElapsed ) = 0;
		
		virtual void keyPressed( const OIS::KeyEvent &e ) = 0;
		virtual void keyReleased( const OIS::KeyEvent &e ) = 0;
		
		virtual void mouseMoved( const OIS::MouseEvent &e ) = 0;
		virtual void mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id )  = 0;
		virtual void mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) = 0;
		
		void changeState( GameState *state );
		void pushState( GameState *state );
		void popState( void );
		void requestShutdown( void );

		virtual GAME_STATE getId() const=0;
		inline virtual bool running() {
		  return fRunning;
		};
		
		inline virtual bool areFxEnabled() { 
		  return GameManager::getSingleton().getSettings()["Visual Detail"] != "Low";
		};
    inline virtual bool areSfxEnabled() { 
      return GameManager::getSingleton().getSettings()["Sound Enabled"] == "Yes";
    };
    
	protected:
		GameState(){ };
		log4cpp::Category* mLog;
		bool fRunning;
	private:
		
		GameState(const GameState&) { fRunning = false; mLog = 0; }
		GameState& operator=(const GameState&);
    };
} // end of namespace
#endif
