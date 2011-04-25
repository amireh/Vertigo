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
 
#include "GameState.h"

namespace Pixy
{

	void GameState::changeState( GameState *state ) {
		GameManager::getSingletonPtr()->changeState( state );
	}

	void GameState::pushState( GameState *state ) {
		GameManager::getSingletonPtr()->pushState( state );
	}

	void GameState::popState( void ) {
		GameManager::getSingletonPtr()->popState();
	}

	void GameState::requestShutdown( void ) {
		GameManager::getSingletonPtr()->requestShutdown();
	}
	
} // end of namespace Pixy
