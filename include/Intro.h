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
     *  Main Menu state, handles non-Combat game tasks.
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
		
		virtual GAME_STATE getId() const;
    
    virtual bool areFxEnabled();
    virtual bool areSfxEnabled();
    virtual void dontUpdateMe(Engine* inEngine);
    
    Zone* getSelectedZone();
    void setSelectedZone(Zone* inZone);
    
	protected:
	  bool evtSettingsChanged(Event* inEvt);
	  // contains the path of the Zone sheet, this is set by the UIEngine
	  // when a player chooses a zone thumb from the menu
	  Zone* mSelectedZone;
    /*
    virtual void setupWidgets();
    //virtual bool keyPressed(const OIS::KeyEvent& evt);
    virtual void itemSelected(SelectMenu* menu);
    virtual void buttonHit(Button* b);
    virtual void reconfigure(const Ogre::String& renderer, Ogre::NameValuePairList& options);
    
    Ogre::String mHelpMsg;
    SelectMenu* mRendererMenu;                     // render system selection menu
    Ogre::Root *mRoot;
    Ogre::RenderWindow *mWindow;
    SdkTrayManager *mTrayMgr;
    */
    
		std::list<Engine*> mEngines;
		
		std::list<Engine*>::iterator _itrEngines;
	private:
		Intro( void ) { fRunning = false; }
		Intro( const Intro& ) { }
		Intro & operator = ( const Intro& );
		
		EventManager *mEvtMgr;
		GfxEngine		*mGfxEngine;
		UIEngine		*mUIEngine;
		PhyxEngine		*mPhyxEngine;
		SfxEngine *mSfxEngine;
		Sphere			  *mFireSphere, *mIceSphere;
		std::list<Obstacle*> mObstacles;
		Ogre::OverlayContainer *mDialogShade;
		Ogre::OverlayManager *mOverlayMgr;
		Ogre::Overlay *mShadeLayer;
		
		
		static Intro   *mIntro;
		
	};
} // end of namespace
#endif
