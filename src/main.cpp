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
 
#include "GameManager.h"

using namespace Pixy;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT ) {
#else
	int main( int argc, char **argv ) {
#endif
		
		GameManager *gameManager = GameManager::getSingletonPtr();
		
		try {
			// Initialise the game and switch to the first state
			gameManager->startGame();
			
		}
		catch ( Ogre::Exception& ex ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox( NULL, ex.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL );
#else
			std::cerr << "An exception has occured: " << ex.getFullDescription();
#endif
		} catch (std::exception& e) {
			Ogre::String errMsg = e.what();
			log4cpp::Category::getInstance(CLIENT_LOG_CATEGORY).errorStream() << errMsg;
		}
		
		delete gameManager;
		
		return 0;
	}
	
