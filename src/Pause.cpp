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
  
#include "Pause.h"
#include "GameManager.h"

namespace Pixy
{
	
	Pause* Pause::mPause;
	
	GAME_STATE Pause::getId() const { return STATE_PAUSE; }
	
	void Pause::enter( void ) {
	}
	
	void Pause::exit( void ) {
	}
	
	void Pause::keyPressed( const OIS::KeyEvent &e )	{
	}
	
	void Pause::keyReleased( const OIS::KeyEvent &e ) {
		switch (e.key) {	 
			case OIS::KC_P:
				  GameManager::getSingleton().popState();
				break;
		}
	}
	
	void Pause::mouseMoved( const OIS::MouseEvent &e )	{
	}
	
	void Pause::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	}
	
	void Pause::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	}
	
	void Pause::pause( void ) {
	}
	
	void Pause::resume( void ) {
	}
	
	Pause* Pause::getSingletonPtr( void ) {
		if( !mPause )
		    mPause = new Pause();
		
		return mPause;
	}
	
	Pause& Pause::getSingleton( void ) {
		return *getSingletonPtr();
	}
	
	void Pause::update( unsigned long lTimeElapsed ) {
	}
	
	bool Pause::areFxEnabled() { return true; }
  bool Pause::areSfxEnabled() { return true; }
  
  void Pause::dontUpdateMe(Engine* inEngine) { };
} // end of namespace
