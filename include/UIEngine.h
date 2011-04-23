/*
 *  UIEngine.h
 *  Vertigo
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
#include "Zone.h"

#include "InputManager.h"
#include <Ogre.h>
#include "ogre/OgreSdkTrays.h"

#include "OgreOggSound.h"

using namespace OgreBites;
using namespace OgreOggSound;
namespace Pixy {
	
	typedef std::map<Ogre::String, Ogre::String> _tInfo;
	class UIZone {
	  public:
	    _tInfo& getInfo() { return mInfo; };
	    Zone* getZone() { return mZone; };
	    void setZone(Zone* inZone) { mZone = inZone; };
	    
	  protected:
	    _tInfo mInfo;
	    Zone* mZone;
	};
	
	/*	\class UIEngine
	 *	\brief
	 *	Manages all GUI and HUD elements of the game, primarily the Menu screen
	 *  and the game interface.
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
		 *  TODO: displays/removes a small label with the current FPS in the top section
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

    void showMenu();
    void hideMenu();
		
	protected:

    bool evtZoneEntered(Event* inEvt);
	  bool evtGameStarted(Event* inEvt);
	  bool evtPlayerWon(Event* inEvt);
	  bool evtSphereDied(Event* inEvt);
	  
	  // shifts from main menu to zone screen
	  void evtClickPlay();
	  void evtClickContinue();
	  // shows the video settings panel
	  void evtClickConfigure();
	  // saves the video settings
	  void evtClickApply();
	  void evtClickBackFromConfig();
	  // display a dialogue with some info on how to play the game
	  void evtClickHelp();
	  void evtClickQuit();
	  // back to main menu from zone screen
	  void evtClickBackFromZones();
	  // based on the selected zone, switches to Level state and starts the game
	  void evtClickEngage();
	  
	  // show the zones screen
	  void _showZones();
	  // navigate to the next zone in the list, and preview it
	  void _nextZone();
	  // display a sheet with the Zone's info and a rendering context with
	  // a live preview of it in action
	  void _previewZone();
	  // navigate to the previous zone in the list, and preview it
	  void _prevZone();
	  // hide zones screen
	  void _hideZones();
	  
	  OgreBites::Button* mCurrentButton;
	  
	  bool inMainMenu;
	  bool inZoneScreen;
	  UIZone *mSelectedZone;
	  
	  // prompts Ogre::OverlayManager for the overlays we're gonna use
	  // and assigns pointers to them
	  void assignHandles();
	  
	  // resizes shield bars to span 1/3 of the viewport each, and scales
	  // font size depending on the screen resolution 
	  void refitOverlays();
	  
	  void _hideHUDs();
	  void _showHUDs();

		void _hideMainMenu();
		void _showMainMenu();
		
		void _hideConfigMenu();
		void _showConfigMenu();
		
		void _showHelp();
		void _hideHelp();
	  
	  // creates widgets for our menu
	  virtual void setupWidgets();
	  bool loadZones();
	  
		EventManager *mEvtMgr;

    Ogre::Root *mRoot;
    Ogre::RenderWindow *mWindow;
    Ogre::Viewport* mViewport;
    SdkTrayManager *mTrayMgr;
		Ogre::OverlayManager *mOverlayMgr;
		//Sphere* mSphere;

    // this message is shown to the player in a dialogue
    Ogre::String mHelpMsg;
    
    // holds all of OGRE's video settings 
    SelectMenu* mRendererMenu, *mFxMenu, *mSfxMenu, *mMusicMenu;
    SelectMenu* mZoneMenu;
		Label* mTitleLabel;                            // zone title label
		std::vector<Ogre::OverlayContainer*> mThumbs;  // zone thumbnails
		Ogre::Real mCarouselPlace;                     // current state of carousel
    std::vector<UIZone*> mUIZones;
    OgreBites::TextBox* mTextBoxZoneInfo;
    bool inConfigMenu; // true when configuration menu is shown
    bool fShowingHelp; // when UISheetHelp is shown
    
    // these make for a shading effect over the game scene when the menu is open
		Ogre::OverlayContainer *mDialogShade;
		Ogre::Overlay *mShadeLayer;
		
		// these are the game UI sheets
		Ogre::Overlay *mUISheet;
		Ogre::OverlayContainer *mUIScore; // shows a label when the player wins
		Ogre::OverlayContainer *mUIPrepare; // this is a prompt, shown prior to starting the game
		Ogre::OverlayContainer *mUIHelp; // how to play overlay
		Ogre::OverlayContainer *mUILogo;
		
		Ogre::TextAreaOverlayElement *mTextScoreCaption;
		Ogre::TextAreaOverlayElement *mTextScoreStats;
		Ogre::TextAreaOverlayElement *mTextPrepare;
		Ogre::TextAreaOverlayElement *mTextHelp;
		Ogre::TextAreaOverlayElement *mTextVersion;
		
		
		// HUDs here
		Ogre::Overlay *mHUDSheet; // holds the shield bars
		Ogre::TextAreaOverlayElement *mHUDScore;
		Ogre::OverlayElement *mHUDFireShield, *mHUDIceShield;
		
		float mShieldBarWidth;
		float mShieldBarHeight;
		Ogre::FrameEvent evt;
		
		GameState *_currentState;    
    
    // menu event handlers
    virtual void itemSelected(SelectMenu* menu);
    virtual void buttonHit(Button* b);
    virtual void buttonOver(Button* b);
    virtual void okDialogClosed(const Ogre::DisplayString& message);
    
    OgreOggISound *mSfxButtonOver, *mSfxButtonHit;
    
    // saves the contents of mRendererMenu into Ogre's config file
    virtual void reconfigure(const Ogre::String& renderer, Ogre::NameValuePairList& options);
    	
	private:
		static UIEngine* _myUIEngine;
		UIEngine();
		UIEngine(const UIEngine& src);
		UIEngine& operator=(const UIEngine& rhs);
	};
}
#endif
