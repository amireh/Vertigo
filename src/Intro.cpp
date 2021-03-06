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
 
#include "Intro.h"
#include "EventManager.h"
#include "Level.h"
#include <cstdlib>
#include <ctime> 

using namespace Ogre;
namespace Pixy
{
	
	Intro* Intro::mIntro;
	
	void Intro::enter( void ) {
		fRunning = true;
		
		srand((unsigned)time(0));
		
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Intro");

    mSelectedZone = 0;
    
    mEvtMgr = EventManager::getSingletonPtr();
    mGfxEngine = GfxEngine::getSingletonPtr();
    mSfxEngine = SfxEngine::getSingletonPtr();
    mUIEngine = UIEngine::getSingletonPtr();
    
    mGfxEngine->setup();
    mSfxEngine->setup();
    mUIEngine->setup();
    
    mUIEngine->deferredSetup();
    mSfxEngine->deferredSetup();
    mGfxEngine->deferredSetup();

		mUIEngine->_refit(this);

    mSfxEngine->playMusic();
    
		mLog->infoStream() << "Initialized successfully.";
	}


	
	void Intro::exit( void ) {
	  
		fRunning = false;
		bool fShuttingDown = GameManager::getSingleton().shuttingDown();
		
		mLog->infoStream() << "---- Exiting Intro State ----";
		
    mEvtMgr = 0;
    mGfxEngine = 0;
    mSfxEngine = 0;
    mUIEngine = 0;
		
		delete mLog;
		mLog = 0;
		
		 
	}
	
	void Intro::keyPressed( const OIS::KeyEvent &e )
	{
	  mUIEngine->keyPressed( e );
	}
	
	void Intro::keyReleased( const OIS::KeyEvent &e ) {
		
		mUIEngine->keyReleased( e );
    mSfxEngine->keyReleased(e);
    
	}
	
	void Intro::mouseMoved( const OIS::MouseEvent &e )
	{
		mUIEngine->mouseMoved(e);		
	}
	
	void Intro::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  mUIEngine->mousePressed(e, id);
	}
	
	void Intro::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  mUIEngine->mouseReleased(e, id);
	}
	
	void Intro::pause( void ) {
	}
	
	void Intro::resume( void ) {
	}
		
	Intro* Intro::getSingletonPtr( void ) {
		if( !mIntro )
		    mIntro = new Intro();
		
		return mIntro;
	}
	
	Intro& Intro::getSingleton( void ) {
		return *getSingletonPtr();
	}
	
	void Intro::update( unsigned long lTimeElapsed ) {
		mEvtMgr->update();
		processEvents();
		
    mUIEngine->update(lTimeElapsed);
    mSfxEngine->update(lTimeElapsed);
    mGfxEngine->update(lTimeElapsed);
	}
} // end of namespace
