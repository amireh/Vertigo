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

#include "GameState.h"

using std::list;
namespace Pixy
{
  /*! \class Pause
   *  \brief
   *  Pause state; does literally nothing. Returns flow back to Level state
   *  when ESCAPE or P is pressed.
   */
	class Pause : public GameState {
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
				
		inline virtual GAME_STATE getId() const {
		  return STATE_PAUSE;
		};

	protected:
		
	private:
	  static Pause* mPause;
		Pause( void ) { }
		Pause( const Pause& ) { }
		Pause & operator = ( const Pause& );
	};
} // end of namespace
#endif
