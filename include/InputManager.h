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

#ifndef H_InputManager_H
#define H_InputManager_H

#ifndef H_Pixy_H
#include "Pixy.h"
#endif

// OIS
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || defined(_WIN32)
#include <OIS.h>
#include <OISMouse.h>
#include <OISKeyboard.h>
#include <OISInputManager.h>
#else
#include <OIS/OIS.h>
#include <OIS/OISMouse.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISInputManager.h>
#endif
#include <Ogre.h>
namespace Pixy
{
    /*! \class InputManager
     *  \brief
     *  OIS Key & Mouse Listener, input handler and dispatcher for GameManager.
     */
	class InputManager : public OIS::KeyListener, OIS::MouseListener {
		public:
			virtual ~InputManager( void );

			//! Initializes OIS input system and sets Ogre window as the event capturing context.
			void initialise( Ogre::RenderWindow *renderWindow );
		
			//! Grabs the input & dispatches events
			void capture( void );

			void addKeyListener( OIS::KeyListener *keyListener, const std::string& instanceName );
			void addMouseListener( OIS::MouseListener *mouseListener, const std::string& instanceName );

			void removeKeyListener( const std::string& instanceName );
			void removeMouseListener( const std::string& instanceName );

			void removeKeyListener( OIS::KeyListener *keyListener );
			void removeMouseListener( OIS::MouseListener *mouseListener );

			void removeAllListeners( void );
			void removeAllKeyListeners( void );
			void removeAllMouseListeners( void );

			void setWindowExtents( int width, int height );

			OIS::Mouse*    getMouse( void );
			OIS::Keyboard* getKeyboard( void );

			static InputManager* getSingletonPtr( void );
		private:
			InputManager( void );
			InputManager( const InputManager& ) { }
			InputManager & operator = ( const InputManager& );

			bool keyPressed( const OIS::KeyEvent &e );
			bool keyReleased( const OIS::KeyEvent &e );

			bool mouseMoved( const OIS::MouseEvent &e );
			bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
			bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );

			OIS::InputManager *mInputSystem;
			OIS::Mouse        *mMouse;
			OIS::Keyboard     *mKeyboard;

			std::map<std::string, OIS::KeyListener*> mKeyListeners;
			std::map<std::string, OIS::MouseListener*> mMouseListeners;


			std::map<std::string, OIS::KeyListener*>::iterator itKeyListener;
			std::map<std::string, OIS::MouseListener*>::iterator itMouseListener;


			std::map<std::string, OIS::KeyListener*>::iterator itKeyListenerEnd;
			std::map<std::string, OIS::MouseListener*>::iterator itMouseListenerEnd;


			static InputManager *mInputManager;
	};
} // end of namespace
#endif

