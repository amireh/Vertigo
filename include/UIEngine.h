/*
 *  UIEngine.h
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/11/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */

#ifndef H_UIEngine_H
#define H_UIEngine_H

#include "GameState.h"
#include "Engine.h"
#include "Event.h"
#include "EventManager.h"
#include "EventListener.h"
#include "Sphere.h"

#include "InputManager.h"
#include <Ogre.h>
#include "OgreSdkTrays.h"

// CEGUI
/*#ifdef _WIN32
#include <CEGUI.h>
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include "RendererModules/Ogre/CEGUIOgreRenderer.h"
#else
#include <CEGUI/CEGUI.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUISchemeManager.h>

//#include "CEGUI/ScriptingModules/LuaScriptModule/CEGUILua.h"
#include "CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h"
#endif
*/
using namespace OgreBites;
namespace Pixy {
	
	/*	\class UIEngine
	 *	\brief
	 *	Loads and unloads UISheets, manages CEGUI system, and handles UI related operations
	 *
	 *	\remark
	 *	At the moment, the UIEngine acts as a manager for UISheets, however,
	 *	the sheets are ought to be handled from within the LUA subsystem.
	 */
	class UIEngine : public Engine, public EventListener, public SdkTrayListener {
		
	public:
		virtual ~UIEngine();
		static UIEngine* getSingletonPtr();
		
		/* \brief
		 *  prepares shared resources across all gamestates; namely, the SdkTrayManager,
		 *  the Menu widgets and event bindings 
		 */
		virtual bool setup();
		
		/* \brief
		 *  prepares GameState-exclusive resources, in the case of Level state,
		 *  the overlays are refitted, see refitOverlays();
		 */
		virtual bool deferredSetup();
		virtual void update(unsigned long lTimeElapsed);
		
		/* \brief
		 *  Not much happens here since the SdkTrayMgr has to live through the whole
		 *  engine life, otherwise it goes bananas and raises segfaults.
		 */
		virtual bool cleanup();
		
		void keyPressed( const OIS::KeyEvent &e );
		void keyReleased( const OIS::KeyEvent &e );		
		void mouseMoved( const OIS::MouseEvent &e );
		void mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		void mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		
		/* \brief
		 *  updates HUDs according to the Sphere's state of shields and score
		 *
		 *  NOTE:
		 *  This is called internally by Sphere on collision events.
		 */
		void _updateShields();
		
		/* \brief
		 *  displays a small label with the current FPS in the top section
		 */
		void toggleFPS();
		
		/* \brief
		 *  change the sheets displayed according to the given GameState
		 *
		 *  NOTE: 
		 *  This is called internally by Level and Intro in their exit
		 *  routines to toggle the HUDs / Menu respectively.
		 */
		void _refit(GameState *inState);

		
	protected:

	  bool evtGameStarted(Event* inEvt);
	  bool evtPlayerWon(Event* inEvt);
	  bool evtSphereDied(Event* inEvt);
	  
	  // prompts Ogre::OverlayManager for the overlays we're gonna use
	  // and assigns pointers to them
	  void assignHandles();
	  
	  // toggles visibility of overlays appropriately
	  void setupOverlays();
	  // resizes shield bars to span 1/3 of the viewport each, and scales
	  // font size depending on the screen resolution 
	  void refitOverlays();
	  
	  void _hideHUDs();
	  void _showHUDs();

		void _hideMenu();
		void _showMenu();
	  
	  // creates widgets for our menu
	  virtual void setupWidgets();
	  
		EventManager *mEvtMgr;

    Ogre::Root *mRoot;
    Ogre::RenderWindow *mWindow;
    SdkTrayManager *mTrayMgr;
		Ogre::OverlayManager *mOverlayMgr;
		Sphere* mSphere;

    // this message is shown to the player in a dialogue
    Ogre::String mHelpMsg;
    
    // holds all of OGRE's video settings 
    SelectMenu* mRendererMenu;

    // these make for a shading effect over the game scene when the menu is open
		Ogre::OverlayContainer *mDialogShade;
		Ogre::Overlay *mShadeLayer;
		
		// these are the game UI sheets
		Ogre::Overlay *mUISheet; // holds the shield bars
		Ogre::Overlay *mUISheetLoss; // shows a label when the player loses
		Ogre::Overlay *mUISheetWin; // shows a label when the player wins
		Ogre::Overlay *mUISheetPrepare; // this is a prompt, shown prior to starting the game
		// HUDs here
		Ogre::OverlayElement *mUIScore;
		Ogre::OverlayElement *mFireShield, *mIceShield;
		
		float mShieldBarWidth;
		Ogre::FrameEvent evt;
		
		GameState *_currentState;    
    
    
    virtual void itemSelected(SelectMenu* menu);
    virtual void buttonHit(Button* b);
    virtual void reconfigure(const Ogre::String& renderer, Ogre::NameValuePairList& options);
    	
	private:
		static UIEngine* _myUIEngine;
		UIEngine();
		UIEngine(const UIEngine& src);
		UIEngine& operator=(const UIEngine& rhs);
	};
}
#endif
