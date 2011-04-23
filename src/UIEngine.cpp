/*
 *  UIEngine.cpp
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/11/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */

#include "UIEngine.h"
#include "GameManager.h"
#include "Level.h"
#include "Intro.h"

namespace Pixy {
	UIEngine* UIEngine::_myUIEngine = NULL;
	
	UIEngine* UIEngine::getSingletonPtr() {
		if( !_myUIEngine ) {
		    _myUIEngine = new UIEngine();
		}
		
		return _myUIEngine;
	}
	
	UIEngine::UIEngine()  {
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "UIEngine");
		mLog->infoStream() << "firing up";
		fSetup = false;
		//mZoneMenu = 0;
    mTitleLabel = 0;
    mCarouselPlace = 0.0f;
    mRendererMenu = 0;
    mCurrentButton = 0;
    inConfigMenu = false;
	}
	
	UIEngine::~UIEngine() {
		mLog->infoStream() << "shutting down";

		if (fSetup) {
			
			mLog->debugStream() << "destroying system";

      delete mTrayMgr;
	    mOverlayMgr->destroyOverlayElement(mDialogShade);
	    mOverlayMgr->destroy("ShadeLayer");
	    
	    while (!mUIZones.empty()) {
	      delete mUIZones.back()->getZone();
	      delete mUIZones.back();
	      mUIZones.pop_back();
	    }
	    
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
        mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
      
      mSoundMgr->destroySound(mSfxButtonHit);
      mSoundMgr->destroySound(mSfxButtonOver);
      
      mSoundMgr = 0;
      mSfxButtonHit = 0;
      mSfxButtonOver = 0;
      	    
	    //mUIZones.clear();
	    
			delete mLog; 
			mLog = 0;
			fSetup = false;
		}
		
		_myUIEngine = NULL;
	}
	
	bool UIEngine::cleanup() {
	  if (!fSetup)
	    return true;
      
    mLog->debugStream() << "cleaning up";	    
    hideMenu();
    _hideHUDs();
	  
	  mLog->debugStream() << "cleaned";
		return true;
	}
	
	
	bool UIEngine::setup() {
		if (fSetup)
			return true;
		
		mEvtMgr = EventManager::getSingletonPtr();
		_currentState = GameManager::getSingleton().currentState();
		
		mLog->infoStream() << "initting CEGUI system & CEGUIOgreRenderer.";
		
    mRoot = Ogre::Root::getSingletonPtr();
    mWindow = mRoot->getAutoCreatedWindow();
    mViewport = mWindow->getViewport(0);
    mOverlayMgr = Ogre::OverlayManager::getSingletonPtr();
    
		mDialogShade = (Ogre::OverlayContainer*)mOverlayMgr->createOverlayElement("Panel", "DialogShade");
		mDialogShade->setMaterialName("SdkTrays/Shade");
		
		mShadeLayer = mOverlayMgr->create("ShadeLayer");
		mShadeLayer->setZOrder(100);
		mShadeLayer->add2D(mDialogShade);
		mDialogShade->hide();
		mShadeLayer->hide();
		
		mTrayMgr = new SdkTrayManager("Vertigo/Intro", mWindow, InputManager::getSingletonPtr()->getMouse(), this);
		//mTrayMgr->showBackdrop("SdkTrays/Bands");
		mTrayMgr->getTrayContainer(TL_NONE)->hide();
	  mTrayMgr->setTrayPadding(10);
	  
	  loadZones();
	  setupWidgets();
	  
	  mHelpMsg = "Gameplay:";
	  mHelpMsg += "\n The probe you're controlling is equipped with 2 shields: ";
	  mHelpMsg += "\n one to protect against fiery drones, and another to\n protect against icy drones.";
	  mHelpMsg += "\n\n You will be warped through zones, and your \nobjective will be reported there.";
	  mHelpMsg += "\n\nControls:";
	  mHelpMsg += "\n - Press the space bar to flip shields";
	  mHelpMsg += "\n - You may use the arrows or A and D buttons to strafe";
	  mHelpMsg += "\n - Press 1-4 to change camera mode";
	  mHelpMsg += "\n\nMisc. Controls:";
	  mHelpMsg += "\n - Press M to toggle all sounds on and off";
	  mHelpMsg += "\n - Press K to take a screenshot";
	  mHelpMsg += "\n - Press U to toggle the interface";
	  mHelpMsg += "\n - Press F to toggle frame stats";
	  mHelpMsg += "\n - Press - or = to control sound volume";
	  mHelpMsg += "\n\nIf you think the game is running too slow\ntry adjusting some settings in the\nconfiguration menu.";
	  mHelpMsg += "\n\nMay the force be with you.";

    assignHandles();
    
    refitOverlays();
    
    // set up sounds
    mLog->debugStream() << "creating sound effects";
    
    OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
    mSfxButtonHit = mSoundMgr->createSound("UIButtonHit", "button_hit.wav", false, false, true);
    mSfxButtonOver = mSoundMgr->createSound("UIButtonOver", "button_over.wav", false, false, true);

    mSfxButtonHit->disable3D(true);
    mSfxButtonHit->setVolume(1);
    
    mSfxButtonOver->disable3D(true);
    mSfxButtonOver->setVolume(1);
    

    mLog->debugStream() << "binding handlers";
    
    bindToName("ZoneEntered", this, &UIEngine::evtZoneEntered);
    bindToName("GameStarted", this, &UIEngine::evtGameStarted);
    bindToName("PlayerWon", this, &UIEngine::evtPlayerWon);
    bindToName("SphereDied", this, &UIEngine::evtSphereDied);
    
    inZoneScreen = false;
    inMainMenu = false;
    inConfigMenu = false;
    fShowingHelp = false;
    
    mLog->debugStream() << "set up!";
		fSetup = true;
		
		
		_showMainMenu();
		return true;
	}
	
	bool UIEngine::loadZones() { 
		
		Ogre::ResourceGroupManager& mRGM = Ogre::ResourceGroupManager::getSingleton();
		Ogre::StringVectorPtr tZones = mRGM.findResourceNames("General", "*.vzs", false);

		for (Ogre::StringVector::iterator itrZones = tZones->begin(); 
		     itrZones != tZones->end(); 
		     ++itrZones)
		{
		  // skip this we don't want to parse it
		  if (*itrZones == "example.vzs")
		    continue;
		    
		  mLog->debugStream() << "found a zone named " << (*itrZones);
		  
		  UIZone* mUIZone = new UIZone();
		  mUIZone->getInfo()["Path"] = *itrZones;
		  mUIZone->setZone(new Zone(mUIZone->getInfo()["Path"]));
		  mUIZones.push_back(mUIZone);
		}

		return true;
	}
	
	bool UIEngine::deferredSetup() {

    if (Level::getSingleton().running())
      static_cast<OgreBites::Button*>(mTrayMgr->getWidget("Play"))->setCaption("Continue");
    
		return true;
	}
	
	void UIEngine::_refit(GameState* inState) {
	  if (inState->getId() == STATE_INTRO) {
	    _hideHUDs();
	    showMenu();
	  } else {

		  hideMenu();
		  if (!Level::getSingleton().isGameOver())
		    _showHUDs();
		  else {
		    // show score
		    mUISheet->show();
		    mUIScore->show(); 
		  }
	  }
	  
	  _currentState = inState;
	};
	
	void UIEngine::update( unsigned long lTimeElapsed ) {
		processEvents();
		
		// update our good tray manager
    evt.timeSinceLastFrame = evt.timeSinceLastEvent = lTimeElapsed;		
    mTrayMgr->frameRenderingQueued(evt);
    
	}
	
	void UIEngine::mouseMoved( const OIS::MouseEvent &e )
	{
		OIS::MouseState state = e.state;
    OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
		if (mTrayMgr->injectMouseMove(orientedEvt)) return;
		
		if (_currentState->getId() != STATE_INTRO)
		  return;

	}
	
	void UIEngine::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  if (mTrayMgr->injectMouseDown(orientedEvt, id)) return;

		  
	}
	
	void UIEngine::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  
	  if (mTrayMgr->injectMouseUp(orientedEvt, id)) return;
	  
	}
	
	void UIEngine::keyPressed( const OIS::KeyEvent &e ) {

	};
	void UIEngine::keyReleased( const OIS::KeyEvent &e ) {

    // Level state specific bindings
    if (GameManager::getSingleton().currentState()->getId() == STATE_GAME) {
      switch (e.key) {
        case OIS::KC_F:
          if (mTrayMgr->areFrameStatsVisible())
            mTrayMgr->hideFrameStats();
          else
            mTrayMgr->showFrameStats(TL_TOPRIGHT);
          break; 
        case OIS::KC_U:
          // toggle HUDs
          (mHUDSheet->isVisible()) ? mHUDSheet->hide() : mHUDSheet->show();;
          break;
      }
      return;
    }

    if (mTrayMgr->isDialogVisible()) {
      mTrayMgr->closeDialog();
	    mDialogShade->hide();
	    mShadeLayer->hide();
    }
    
    // Intro state / menu specific bindings
	  if (fShowingHelp) {

      _hideHelp();
	    _showMainMenu();
	    
	    return;
	  };

    TrayLocation trayIdx;
    int widgIdx;
    switch (e.key) {
      /* -- keyboard menu navigation -- */
      case OIS::KC_UP:
        // mark the previous button in the current menu, if it's on the top, get the last one
        trayIdx = mCurrentButton->getTrayLocation();
        widgIdx = mTrayMgr->locateWidgetInTray(mCurrentButton);
        mCurrentButton->_stopFlashing();
        if (widgIdx == 0) {
          mCurrentButton = static_cast<OgreBites::Button*>(mTrayMgr->getWidget(trayIdx, mTrayMgr->getNumWidgets(trayIdx)-1));
        } else {
          mCurrentButton = static_cast<OgreBites::Button*>(mTrayMgr->getWidget(trayIdx, widgIdx -1));
        }
        mCurrentButton->_doFlash();
        break;
      case OIS::KC_DOWN:
        // mark the next button, if it's the last, get the first one
        trayIdx = mCurrentButton->getTrayLocation();
        widgIdx = mTrayMgr->locateWidgetInTray(mCurrentButton);
        mCurrentButton->_stopFlashing();
        if (widgIdx == mTrayMgr->getNumWidgets(trayIdx)-1) {
          mCurrentButton = static_cast<OgreBites::Button*>(mTrayMgr->getWidget(trayIdx, 0));
        } else {
          mCurrentButton = static_cast<OgreBites::Button*>(mTrayMgr->getWidget(trayIdx, widgIdx+1));
        }
        mCurrentButton->_doFlash();
        break;
      case OIS::KC_RETURN:
        buttonHit(mCurrentButton);
        break;
      // this only applies when we're in the zone screen
      case OIS::KC_RIGHT:
        if (inZoneScreen)
          _nextZone();
        break;
      case OIS::KC_LEFT:
        if (inZoneScreen)
          _prevZone();
        break;
      // pressing ESC has different behaviour based on the current screen
      case OIS::KC_ESCAPE:
        if (inZoneScreen) {
          // going back from zones screen to the main menu
          _hideZones();
          _showMainMenu();
        } else if (inMainMenu) {
          if (!Level::getSingleton().running())
            // we're quitting
            return GameManager::getSingleton().requestShutdown();
          else {
            // we're going back to the game
            // show HUDs unless the game is over
            this->_refit(Level::getSingletonPtr());
            return GameManager::getSingleton().popState();
          }		        
        } else if (inConfigMenu) {
          // going back from config screen to main menu
          _hideConfigMenu();
          _showMainMenu();
        }
        break;
      // TODO: this is redundant, merge it with the level state's (common bindings)
      case OIS::KC_F:
        if (mTrayMgr->areFrameStatsVisible())
          mTrayMgr->hideFrameStats();
        else
          mTrayMgr->showFrameStats(TL_TOPRIGHT);
        break;
    };
    
	};	
	
	void UIEngine::_showHelp() {
	  mTextHelp->setCaption(mHelpMsg);
	  mDialogShade->show();
	  mShadeLayer->show();
	  
	  mUIHelp->show();
	  mTextHelp->show();
	  fShowingHelp = true;

	};
	void UIEngine::_hideHelp() {
    mUIHelp->hide();
    mDialogShade->hide();
    mShadeLayer->hide();
    fShowingHelp = false;	
	};
  void UIEngine::setupWidgets()
	{
	
	  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
		Ogre::MaterialPtr thumbMat = Ogre::MaterialManager::getSingleton().create("ZoneThumbnail", "Bootstrap");
		thumbMat->getTechnique(0)->getPass(0)->createTextureUnitState();
			
		mTrayMgr->destroyAllWidgets();


		int button_width = mViewport->getActualWidth() / 5;
		
		// create main navigation tray
		mTrayMgr->createButton(TL_BOTTOM, "Play", "Play", button_width);
		mTrayMgr->createButton(TL_NONE, "Continue", "Continue", button_width);
		mTrayMgr->createButton(TL_NONE, "ChangeZone", "Change Zone", button_width);
		mTrayMgr->createButton(TL_BOTTOM, "Configure", "Settings", button_width);
		mTrayMgr->createButton(TL_BOTTOM, "Help", "Help", button_width);
		mTrayMgr->createButton(TL_BOTTOM, "Quit", "Quit", button_width);
		
		// create zone screen tray
		mTextBoxZoneInfo = mTrayMgr->createTextBox(TL_NONE, "ZoneInfo", "Zone Information", 450, 400);
		mTrayMgr->createButton(TL_NONE, "Engage", "Engage", button_width);
		mTrayMgr->createLabel(TL_NONE, "ZoneTitle", "Choose a Zone", mViewport->getActualWidth()/2);
		mTrayMgr->createButton(TL_NONE, "NextZone", "next");
		mTrayMgr->createButton(TL_NONE, "PrevZone", "previous");
		mTrayMgr->createButton(TL_NONE, "BackFromZones", "Go Back", button_width);
		mTextBoxZoneInfo->hide();

		// create configuration screen button tray
		mTrayMgr->createButton(TL_NONE, "Apply", "Apply Changes", button_width);
		mTrayMgr->createButton(TL_NONE, "BackFromConfig", "Go Back", button_width);

		// create configuration screen and renderer menu
		mTrayMgr->createLabel(TL_NONE, "ConfigLabel", "Video Settings");
		mFxMenu = mTrayMgr->createLongSelectMenu(TL_NONE, "ConfigFxMenu", "Visual Effects", 340, 140, 3);
		mFxMenu->addItem("Full");
		mFxMenu->addItem("Medium");
		mFxMenu->addItem("Low");
		int tFxLevel = GameManager::getSingleton().getSettings().FX_LEVEL;
		mFxMenu->selectItem((tFxLevel == FX_LEVEL_LOW) ? "Low" : (tFxLevel == FX_LEVEL_MEDIUM) ? "Medium" : "Full");
		
		mMusicMenu = mTrayMgr->createLongSelectMenu(TL_NONE, "ConfigMusicMenu", "Music", 340, 140, 2);
		mMusicMenu->addItem("On");
		mMusicMenu->addItem("Off");
		mMusicMenu->selectItem(GameManager::getSingleton().getSettings().MUSIC_ENABLED ? "On" : "Off");
		
		mSfxMenu = mTrayMgr->createLongSelectMenu(TL_NONE, "ConfigSfxMenu", "Sound Effects", 340, 140, 2);
		mSfxMenu->addItem("On");
		mSfxMenu->addItem("Off");
		mSfxMenu->selectItem(GameManager::getSingleton().getSettings().SOUND_ENABLED ? "On" : "Off");
		
		mRendererMenu = mTrayMgr->createLongSelectMenu(TL_NONE, "RendererMenu", "Render System", 450, 240, 10);
		mTrayMgr->createSeparator(TL_NONE, "ConfigSeparator");
		mTrayMgr->createSeparator(TL_NONE, "ConfigSeparator2");

		// populate render system names
		Ogre::StringVector rsNames;
		Ogre::RenderSystemList rsList = mRoot->getAvailableRenderers();
		for (unsigned int i = 0; i < rsList.size(); i++)
		{
			rsNames.push_back(rsList[i]->getName());
		}
		mRendererMenu->setItems(rsNames);

	}

		
  void UIEngine::buttonHit(Button* b) {
    if (GameManager::getSingleton().getSettings().SOUND_ENABLED) {
      if (mSfxButtonHit->isPlaying())
        mSfxButtonHit->stop();
      
      mSfxButtonHit->play(true);
    }
      
		if (b->getName() == "Configure")   // enter configuration screen
		{
		  evtClickConfigure();
		}
		else if (b->getName() == "BackFromConfig")   // leave configuration screen
		{
		  evtClickBackFromConfig();
		}
		else if (b->getName() == "Apply")   // apply any changes made in the configuration screen
		{
		  evtClickApply();
		}
		else if (b->getName() == "Play" || b->getName() == "ChangeZone")
		  evtClickPlay();
		else if (b->getName() == "Continue")
		  evtClickContinue();
		else if (b->getName() == "Quit") {
		  evtClickQuit();
		  
		} else if(b->getName() == "Help") {
		  evtClickHelp();
		  
		} else if (b->getName() == "Engage") {
		  evtClickEngage();
		} else if (b->getName() == "BackFromZones") {
		  evtClickBackFromZones();
		} else if (b->getName() == "PrevZone")
		  _prevZone();
		  else if (b->getName() == "NextZone")
		  _nextZone();
  };
  
  void UIEngine::buttonOver(Button* b) {
    if (!GameManager::getSingleton().getSettings().SOUND_ENABLED)
      return;
      
    if (mSfxButtonOver->isPlaying())
      mSfxButtonOver->stop();
    
    mSfxButtonOver->startFade(true, 0.2f);
  }
  
  void UIEngine::itemSelected(SelectMenu* menu) {
    if (menu == mRendererMenu)    // renderer selected, so update all settings
    {
	    while (mTrayMgr->getNumWidgets(mRendererMenu->getTrayLocation()) > 3)
	    {
		    mTrayMgr->destroyWidget(mRendererMenu->getTrayLocation(), 3);
	    }

	    Ogre::ConfigOptionMap& options = mRoot->getRenderSystemByName(menu->getSelectedItem())->getConfigOptions();
	    unsigned int i = 0;

	    // create all the config option select menus
	    for (Ogre::ConfigOptionMap::iterator it = options.begin(); it != options.end(); it++)
	    {
		    i++;
		    SelectMenu* optionMenu = mTrayMgr->createLongSelectMenu
			    (TL_CENTER, "ConfigOption" + Ogre::StringConverter::toString(i), it->first, 450, 240, 10);
		    optionMenu->setItems(it->second.possibleValues);
		
		    // if the current config value is not in the menu, add it
		    try
		    {
			    optionMenu->selectItem(it->second.currentValue);
		    }
		    catch (Ogre::Exception e)
		    {
			    optionMenu->addItem(it->second.currentValue);
			    optionMenu->selectItem(it->second.currentValue);
		    }
	    }

		  mTrayMgr->moveWidgetToTray("ConfigSeparator2", TL_CENTER);
		  mTrayMgr->moveWidgetToTray(mFxMenu, TL_CENTER);
		  mTrayMgr->moveWidgetToTray(mMusicMenu, TL_CENTER);
		  mTrayMgr->moveWidgetToTray(mSfxMenu, TL_CENTER);
    }
  }
  
  void UIEngine::okDialogClosed(const Ogre::DisplayString& message) {
    mShadeLayer->hide();
    mDialogShade->hide();
  }
  
	/*-----------------------------------------------------------------------------
	| Extends reconfigure to save the view and the index of last sample run.
	-----------------------------------------------------------------------------*/
	void UIEngine::reconfigure(const Ogre::String& renderer, Ogre::NameValuePairList& options)
	{
		Ogre::RenderSystem* rs = mRoot->getRenderSystemByName(renderer);
		
		// set all given render system options
		for (Ogre::NameValuePairList::iterator it = options.begin(); it != options.end(); it++)
		{
			rs->setConfigOption(it->first, it->second);
              
		}
		mRoot->saveConfig();
    //mRoot->setRenderSystem(rs);
    mShadeLayer->show();
    mDialogShade->show();
    mTrayMgr->showOkDialog("Notice!", "Your new settings will be applied the next time you run Vertigo.");
	}
	
	void UIEngine::hideMenu() {

	  mTrayMgr->hideCursor();
	  _hideMainMenu();
	  _hideZones();
	  if (inConfigMenu)
	    _hideConfigMenu();
    
    mUISheet->hide();
    
    // return everything to what it was
    /*if (Level::getSingleton().running() && !Level::getSingleton().isGameOver()) {
	    Level::getSingleton()._showEverything();
	  }*/
	  
	  // and tell who's interested that we resumed the game
	  EventManager::getSingleton().hook(EventManager::getSingleton().createEvt("GameShown"));
	};
	
	void UIEngine::showMenu() {
    
    // tell who's interested that we paused the game
    EventManager::getSingleton().hook(EventManager::getSingleton().createEvt("MenuShown"));
    
    // hide the scene
    /*if (Level::getSingleton().running()) {
	    Level::getSingleton()._hideEverything();
	  }*/
	  
    mTrayMgr->showCursor();
	  _showMainMenu();
	  
	  mDialogShade->hide();
	  mShadeLayer->hide();
	  mUISheet->show();
	  mUIScore->hide();
	  mUIPrepare->hide();
	  mUIHelp->hide();
	};
	
	void UIEngine::_hideMainMenu() {

		
		if (Level::getSingleton().running()) {
		  mTrayMgr->removeWidgetFromTray("ChangeZone");
		  mTrayMgr->removeWidgetFromTray("Continue");
		} else
		  mTrayMgr->removeWidgetFromTray("Play");
		  
		mTrayMgr->removeWidgetFromTray("Configure");
		mTrayMgr->removeWidgetFromTray("Help");
		mTrayMgr->removeWidgetFromTray("Quit");
	  mUILogo->hide();
	  mTextVersion->hide();
	  
		inMainMenu = false;
	};
	
	void UIEngine::_showMainMenu() {
	  inMainMenu = true;
	  
    mUILogo->show();
    mTextVersion->show();
		
		if (Level::getSingleton().running()) {
		  mTrayMgr->moveWidgetToTray("Continue", TL_BOTTOM);
		  mTrayMgr->moveWidgetToTray("ChangeZone", TL_BOTTOM);
		} else 
		  mTrayMgr->moveWidgetToTray("Play", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Configure", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Help", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Quit", TL_BOTTOM);
	  
	  // assign a default button
	  if (mCurrentButton)
      mCurrentButton->_stopFlashing();
    // flash Continue if a game is running, otherwise flash Play
    if (Level::getSingleton().running())
	    mCurrentButton = static_cast<OgreBites::Button*>(mTrayMgr->getWidget("Continue"));
	  else
	    mCurrentButton = static_cast<OgreBites::Button*>(mTrayMgr->getWidget("Play"));
	  mCurrentButton->_doFlash();

	};
	
	void UIEngine::assignHandles() {
	  using Ogre::TextAreaOverlayElement;
	  
		mHUDSheet = mOverlayMgr->getByName("Vertigo/HUD");
		mHUDScore = static_cast<TextAreaOverlayElement*>
		  (mHUDSheet->getChild("HUD/Containers/Score")->getChild("HUD/Elements/Score"));

		mHUDFireShield = mHUDSheet->getChild("HUD/Containers/FireShield")->getChild("HUD/Elements/FireShield");
		mHUDIceShield = mHUDSheet->getChild("HUD/Containers/IceShield")->getChild("HUD/Elements/IceShield");
		
		mUISheet = mOverlayMgr->getByName("Vertigo/UI");
		mUIPrepare = mUISheet->getChild("UI/Containers/Prepare");
		mUIHelp = mUISheet->getChild("UI/Containers/Help");
		mUIScore = mUISheet->getChild("UI/Containers/Score");
		mUILogo = mUISheet->getChild("UI/Containers/Logo");
		
		
		mTextPrepare = static_cast<TextAreaOverlayElement*>
		  (mUIPrepare->getChild("UI/Text/Prepare"));
		mTextHelp = static_cast<TextAreaOverlayElement*>
		  (mUIHelp->getChild("UI/Text/Help"));
		mTextScoreCaption = static_cast<TextAreaOverlayElement*>
		  (mUIScore->getChild("UI/Text/Score/Caption"));  
		mTextScoreStats = static_cast<TextAreaOverlayElement*>
		  (mUIScore->getChild("UI/Text/Score/Stats"));
		mTextVersion = static_cast<TextAreaOverlayElement*>
		  (mUISheet->getChild("UI/Containers/Version")->getChild("UI/Text/Version"));
    
	};
	
	void UIEngine::refitOverlays() {
	  using Ogre::TextAreaOverlayElement;
	  
		int width = mViewport->getActualWidth();
		int height = mViewport->getActualHeight();
		float aspect_ratio = width / height;
		
		/* --------- UI / Intro sheets section --------- */
		
		// resize our Zone title label, center it and set it to span 1/12 of the height of the viewport
	  Ogre::TextAreaOverlayElement* label = static_cast<Ogre::TextAreaOverlayElement*>(((Ogre::OverlayContainer*)mTrayMgr->getWidget("ZoneTitle")->getOverlayElement())->getChild("ZoneTitle/LabelCaption")); 
	  label->setCharHeight(mViewport->getActualHeight() / 12);
	  label->setHorizontalAlignment(Ogre::GHA_CENTER);
	  label->setAlignment(Ogre::TextAreaOverlayElement::Center);
	
	  // resize the logo to span a 1/2 of the viewport's height, since it's a regular
	  // rectangle, then we use the same dimension for both width and height
	  mUILogo->setWidth(height / 2);
	  mUILogo->setHeight(height / 2);
	  mUILogo->setTop(height / 5);
	  mUILogo->setLeft(-1 * (height / 4) - (height / 18));
	
	
	  // resize all our buttons to 1/16 of viewport height
	  mTrayMgr->getWidget("Play")->getOverlayElement()->setHeight(height / 16);
	  mTrayMgr->getWidget("Continue")->getOverlayElement()->setHeight(height / 16);
	  mTrayMgr->getWidget("ChangeZone")->getOverlayElement()->setHeight(height / 16);
	  mTrayMgr->getWidget("Configure")->getOverlayElement()->setHeight(height / 16);
	  mTrayMgr->getWidget("Help")->getOverlayElement()->setHeight(height / 16);
	  mTrayMgr->getWidget("Quit")->getOverlayElement()->setHeight(height / 16);
	  mTrayMgr->getWidget("Apply")->getOverlayElement()->setHeight(height / 16);
	  mTrayMgr->getWidget("BackFromConfig")->getOverlayElement()->setHeight(height / 16);
	  mTrayMgr->getWidget("Engage")->getOverlayElement()->setHeight(height / 16);
	  mTrayMgr->getWidget("BackFromZones")->getOverlayElement()->setHeight(height / 16);
	
	  mTrayMgr->adjustTrays();
	  
	  // scale overlays' font size
    //float font_size = 32 / aspect_ratio;
    mTextPrepare->setCharHeight(mTextPrepare->getCharHeight() / aspect_ratio);
    mTextHelp->setCharHeight(mTextHelp->getCharHeight() / aspect_ratio);
    mTextVersion->setCharHeight(mTextVersion->getCharHeight() / aspect_ratio);
    mTextScoreCaption->setCharHeight(mTextScoreCaption->getCharHeight() / aspect_ratio);
    mTextScoreStats->setCharHeight(mTextScoreStats->getCharHeight() / aspect_ratio);
    mHUDScore->setCharHeight(mHUDScore->getCharHeight() / aspect_ratio);
    
    // move the Version label to right bottom corner
    mTextVersion->setLeft(-1 * (mTextVersion->getSpaceWidth() * 10 )); // 10 is nr of chars -1
    mTextVersion->setTop(-1 * (mTextVersion->getCharHeight()));
    
    mLog->debugStream() << "Version text's dimensions: " 
      << mTextVersion->_getRelativeWidth() 
      << ", " << mTextVersion->_getRelativeHeight();

		/* --------- HUDs section --------- */
		
		// our shield bars should span 1/3 of the width of the viewport
	  mShieldBarWidth = mViewport->getActualWidth() / 3;
	  mShieldBarHeight = mViewport->getActualHeight() / 16;
	  
	  mHUDFireShield->getParent()->setWidth(mShieldBarWidth);
	  mHUDFireShield->setWidth(mShieldBarWidth);
	  
	  mHUDFireShield->getParent()->setHeight(mShieldBarHeight);
	  mHUDFireShield->getParent()->setTop(-1*mShieldBarHeight);
	  mHUDFireShield->setHeight(mShieldBarHeight);
	  mHUDFireShield->setTop(-1*mShieldBarHeight);
	
	  // since our ice shield is on the right side of the screen, and we shrink
	  // it inwards, we have to tune its left position
	  mHUDIceShield->getParent()->setWidth(mShieldBarWidth);
	  mHUDIceShield->getParent()->setLeft(-1*mShieldBarWidth);
	  mHUDIceShield->setWidth(mShieldBarWidth);
	  mHUDIceShield->setLeft(-1*mShieldBarWidth);

	  mHUDIceShield->getParent()->setHeight(mShieldBarHeight);
	  mHUDIceShield->getParent()->setTop(-1*mShieldBarHeight);
	  mHUDIceShield->setHeight(mShieldBarHeight);
	  mHUDIceShield->setTop(-1*mShieldBarHeight);
	};
	
	void UIEngine::_updateShields() {
	
    using namespace Ogre;
    Sphere* mSphere = Level::getSingleton().getSphere();
    
    // in the Dodgy game mode, there is only 1 bar and it's centered
    // so we don't need to tune the left positioning of the ice shield
    // nor update them both
    // TODO: why am i updating both shields??
    if (Level::getSingleton().currentZone()->getSettings().mMode == DODGY) {
      OverlayElement* mElement = (mSphere->shield() == FIRE) 
        ? mHUDFireShield
        : mHUDIceShield;
      
      mElement->setWidth(mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));

    } else {
      for (int i=0; i < 2; ++i) {
        SHIELD tShield = (i == 0) ? FIRE : ICE;
        
        OverlayElement* mElement = (tShield == FIRE) 
          ? mHUDFireShield
          : mHUDIceShield;
        
        mElement->setWidth(mShieldBarWidth * (mSphere->getShieldState(tShield) / 1000.0f));
        
        // adjust the iceshield's left position
        if (mElement->getHorizontalAlignment() == Ogre::GHA_RIGHT)
          mElement->setLeft(-1 * mElement->getWidth());
      }
    }
    
    // update the player's score
    mHUDScore->setCaption(stringify(mSphere->score()));
	};
	
	void UIEngine::_hideHUDs() {
	  mHUDSheet->hide();
	};
	
	void UIEngine::_showHUDs() {
	  mHUDSheet->show();
	};
	
	bool UIEngine::evtSphereDied(Event* inEvt) {
	  mTextScoreCaption->setCaption("You have LOST");

	  Sphere *mSphere = Level::getSingleton().getSphere();
	  double accuracy = (mSphere->getNrHits() * 1.0f / (mSphere->getNrMisses() + mSphere->getNrHits()) ) * 100.f;
	  std::ostringstream msg;
	  msg << "\n\nScore: " << stringify(mSphere->score())
	      << "\nAccuracy: " << stringify( accuracy ) << "%"
	      << "\nTime survived: "
	      << stringify( Level::getSingleton().currentZone()->getTimeElapsed() )
	      << " seconds";
	  mTextScoreStats->setCaption(msg.str());
    
    mLog->debugStream() << "sphere misses: " << mSphere->getNrMisses()
    << ", hits " << mSphere->getNrHits()
    << ", accuracy: " << accuracy;

	  mDialogShade->show();
	  mShadeLayer->show();    
    mUISheet->show();
	  mUIScore->show();
	  return true;
	};
	
	// TODO: refactor these two methods into 1 helper
	bool UIEngine::evtPlayerWon(Event* inEvt) {
	  mTextScoreCaption->setCaption("Congratulations! You have WON");
	  
	  Sphere *mSphere = Level::getSingleton().getSphere();
	  double accuracy = (mSphere->getNrHits() * 1.0f / (mSphere->getNrMisses() + mSphere->getNrHits()) ) * 100.f;
	  
	  std::ostringstream msg;
	  msg << "\n\nScore: " << stringify(mSphere->score())
	      << "\nAccuracy: " << stringify( accuracy ) << "%"
	      << "\nTime survived: "
	      << stringify( Level::getSingleton().currentZone()->getTimeElapsed() )
	      << " seconds";
	  mTextScoreStats->setCaption(msg.str());
	  
	  mLog->debugStream() << "sphere misses: " << mSphere->getNrMisses()
    << ", hits " << mSphere->getNrHits()
    << ", accuracy: " << accuracy;

	  //mDialogShade->show();
	  //mShadeLayer->show();
	  mUISheet->show();
	  mUIScore->show();
	  _hideHUDs();
	  
	  return true;
	};
	
	bool UIEngine::evtGameStarted(Event* inEvt) {  
	  mDialogShade->hide();
	  mShadeLayer->hide();
	  mUIPrepare->hide();
	  mUIScore->hide();
	  
	  
	  
	  // if it's dodgy mode, we have to hide one of the shield HUDs since
	  // the player will be using only one of them
	  Sphere* mSphere = Level::getSingleton().getSphere();

    // we divide by 1000 because the shield's normal starting value is 1000
    mHUDFireShield->getParent()->setWidth(mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));
    mHUDIceShield->getParent()->setWidth(mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));

    // if it's dodgy mode, we have to hide one of the bars, and center the other
    // keep in mind that each bar has 2 elements: the background and the foreground
	  if (Level::getSingleton().currentZone()->getSettings().mMode == DODGY) {
	    
	    if (mSphere->shield() == FIRE) {
	      // hide the ice shield HUD
	      mHUDIceShield->getParent()->hide();
	      mHUDIceShield->hide();
	      
	      mHUDFireShield->show();
	      mHUDFireShield->getParent()->show();
	      
	      // resize the background of the shield hud
	      float pos = (mViewport->getActualWidth() - mHUDFireShield->getParent()->getWidth()) / 2;
	      
	      // center them both
	      // TODO: why not use Ogre's overlay alignment scheme?
        mHUDFireShield->getParent()->setLeft(pos);
        mHUDFireShield->setLeft(0);
	      
	    } else {
	      // hide the fire one
	      mHUDFireShield->hide();
	      mHUDFireShield->getParent()->hide();
	      
	      mHUDIceShield->getParent()->show();
	      mHUDIceShield->show();
	      
	      // and set the ice ones into their new positions
	      // NOTE: this is weird, i myself don't get why it's working this way
	      float pos = (mViewport->getActualWidth() - mHUDIceShield->getParent()->getWidth()) / 2;
	      pos *= -1;
	      pos -= mHUDIceShield->getParent()->getWidth();
        mHUDIceShield->getParent()->setLeft(pos);
        mHUDIceShield->setLeft(-1 * mHUDIceShield->getParent()->getWidth());
	    }
	  } else {
	    // reset positions of the shields
	    mHUDFireShield->getParent()->setLeft(0);
	    mHUDFireShield->setLeft(0);
	    
	    mHUDIceShield->getParent()->setLeft(-1 * mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));
	    mHUDIceShield->setLeft(-1 * mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));
	    
	    // and show them
	    mHUDFireShield->getParent()->show();
	    mHUDFireShield->show();
	    
	    mHUDIceShield->getParent()->show();
	    mHUDIceShield->show();
	  }

    _showHUDs();
	  _updateShields();
	  return true;
	};
	
	bool UIEngine::evtZoneEntered(Event* inEvt) {
	  //this->_refit(Level::getSingletonPtr());
	  
	  std::ostringstream msg;
	  msg << "Zone Mode: ";
	  switch (mSelectedZone->getZone()->getSettings().mMode) {
	    case ARCADE:
	      msg << "Arcade"
	          << "\nDifficulty: Easy"
	          << "\n\nObjective:"
	          << "\n Reach the last portal with both shields functional."
	          << "\n\nControls:"
	          << "\n - Flip shields by pressing SPACE BAR."
	          << "\n - You can move to the left and right"
	          << "\n   by pushing A and D."
	          << "\n - You can adjust the camera mode by pressing 1-4.";
	      break;
	    case DODGY:
	      msg << "Dodgy"
	          << "\nDifficulty: Moderate"
	          << "\n\nObjective:"
	          << "\n Only one of your shields is functional,"
	          << "\n you MUST dodge one kind of drones."
	          << "\n\nControls:"
	          << "\n - You can move to the left and right"
	          << "\n   by pushing A and D."
	          << "\n - You can adjust the camera mode by pressing 1-4.";
	      break;
	    case SURVIVAL:
	      msg << "Survival"
	          << "\nDifficulty: Moderate"
	          << "\n\nObjective:"
	          << "\n Both of your shields are active and"
	          << "\n your missionis to explore as much"
	          << "\n as you can by surviving."
	          << "\n\nControls:"
	          << "\n - Flip shields by pressing SPACE BAR."
	          << "\n - You can move to the left and right"
	          << "\n   by pushing A and D."
	          << "\n - You can adjust the camera mode by pressing 1-4.";
	      break;
	    case NERVEBREAK:
	      msg << "Nervebreak"
	          << "\nDifficulty: Hard"
	          << "\n\nObjective:"
	          << "\n The drones have adapted to your shields,"
	          << "\n you must hit each drone with the shield"
	          << "\n opposite to its element."
	          << "\n\nControls:"
	          << "\n - Flip shields by pressing SPACE BAR."
	          << "\n - You can move as usual."
	          << "\n - You can adjust the camera mode by pressing 1-4."
	          << "\n\n** WARNING: brain stability might deteriorate **";
	  }
	  msg << "\n\nPress ENTER when ready!";
	  
	  
	  
	  mTextPrepare->setCaption(msg.str());
	  mDialogShade->show();
	  mShadeLayer->show();
    mUISheet->show();
	  mUIPrepare->show();
	  //mTextPrepare->show();
	  
	  _hideHUDs();
	  //_updateShields();
	  
	  return true;
	};
	
	void UIEngine::toggleFPS() {
	  if (mTrayMgr->areFrameStatsVisible())
	    mTrayMgr->hideFrameStats();
	  else
	    mTrayMgr->showFrameStats(OgreBites::TL_TOPRIGHT);
	};
	
  
  // shifts from main menu to zones screen
  void UIEngine::evtClickPlay() {
    // remove main screen menu
	  _hideMainMenu();
    _showZones(); 
  };
  
  void UIEngine::evtClickContinue() {
    if (!Level::getSingleton().running())
      return;
      
    this->_refit(Level::getSingletonPtr());
    return GameManager::getSingleton().popState();
  };
  
  // shows the video settings panel
  void UIEngine::evtClickConfigure() {
    inConfigMenu = true;
    
		_hideMainMenu();
		// if the viewport's height is any less than 600, we have to move these
		// buttons to the bottom right corner
		mTrayMgr->moveWidgetToTray("Apply", (mViewport->getActualHeight() <= 600) ? TL_BOTTOMRIGHT : TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("BackFromConfig", (mViewport->getActualHeight() <= 600) ? TL_BOTTOMRIGHT : TL_BOTTOM);

		while (mTrayMgr->getTrayContainer(TL_CENTER)->isVisible())
		{
			mTrayMgr->removeWidgetFromTray(TL_CENTER, 0);
		}

		while (mTrayMgr->getTrayContainer(TL_LEFT)->isVisible())
		{
			mTrayMgr->removeWidgetFromTray(TL_LEFT, 0);
		}

		mTrayMgr->moveWidgetToTray("ConfigLabel", TL_CENTER);
		mTrayMgr->moveWidgetToTray(mRendererMenu, TL_CENTER);
		mTrayMgr->moveWidgetToTray("ConfigSeparator", TL_CENTER);

		mRendererMenu->selectItem(mRoot->getRenderSystem()->getName());
		
		mTrayMgr->moveWidgetToTray("ConfigSeparator2", TL_CENTER);
		mTrayMgr->moveWidgetToTray(mFxMenu, TL_CENTER);
		mTrayMgr->moveWidgetToTray(mMusicMenu, TL_CENTER);
		mTrayMgr->moveWidgetToTray(mSfxMenu, TL_CENTER);

    if (mCurrentButton)
      mCurrentButton->_stopFlashing();
    mCurrentButton = static_cast<OgreBites::Button*>(mTrayMgr->getWidget("Apply"));
	  mCurrentButton->_doFlash();
		//windowResized(mWindow);  
  };
  void UIEngine::evtClickApply() {
		bool reset = false;

		Ogre::ConfigOptionMap& options =
			mRoot->getRenderSystemByName(mRendererMenu->getSelectedItem())->getConfigOptions();

		Ogre::NameValuePairList newOptions;

		// collect new settings and decide if a reset is needed

		if (mRendererMenu->getSelectedItem() != mRoot->getRenderSystem()->getName()) reset = true;

		for (unsigned int i = 3; i < mTrayMgr->getNumWidgets(mRendererMenu->getTrayLocation()) - 4; i++)
		{
			SelectMenu* menu = (SelectMenu*)mTrayMgr->getWidget(mRendererMenu->getTrayLocation(), i);
			if (menu->getSelectedItem() != options[menu->getCaption()].currentValue) reset = true;
			newOptions[menu->getCaption()] = menu->getSelectedItem();
		}

		// reset with new settings if necessary
		if (reset) reconfigure(mRendererMenu->getSelectedItem(), newOptions);
		
		t_PixySettings settings;
		std::string tFxLevel = mFxMenu->getSelectedItem();
		settings.FX_LEVEL = (tFxLevel == "Low") ? FX_LEVEL_LOW : (tFxLevel == "Medium") ? FX_LEVEL_MEDIUM : FX_LEVEL_FULL;
		settings.MUSIC_ENABLED = (mMusicMenu->getSelectedItem() == "On");
		settings.SOUND_ENABLED = (mSfxMenu->getSelectedItem() == "On");
		
		GameManager::getSingleton().applyNewSettings(settings);
  };
  
  void UIEngine::_hideConfigMenu() {
		mTrayMgr->removeWidgetFromTray(mSfxMenu);
		mTrayMgr->removeWidgetFromTray(mMusicMenu);
		mTrayMgr->removeWidgetFromTray(mFxMenu);
		mTrayMgr->removeWidgetFromTray("ConfigSeparator2");
		  
		while (mTrayMgr->getNumWidgets(mRendererMenu->getTrayLocation()) > 3)
		{
			mTrayMgr->destroyWidget(mRendererMenu->getTrayLocation(), 3);
		}

		/*while (mTrayMgr->getNumWidgets(TL_NONE) != 0)
		{
			mTrayMgr->moveWidgetToTray(TL_NONE, 0, TL_CENTER);
		}*/

		mTrayMgr->removeWidgetFromTray("Apply");
		mTrayMgr->removeWidgetFromTray("BackFromConfig");
		mTrayMgr->removeWidgetFromTray("ConfigLabel");
		mTrayMgr->removeWidgetFromTray(mRendererMenu);

		mTrayMgr->removeWidgetFromTray("ConfigSeparator");
		
		
		inConfigMenu = false;
  };
  void UIEngine::evtClickBackFromConfig() {

    _hideConfigMenu();
    _showMainMenu();
		/*mTrayMgr->moveWidgetToTray("Play", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Configure", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Help", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Quit", TL_BOTTOM);*/

		//windowResized(mWindow);  
  };

  // display an overlay with some info on how to play the game
  void UIEngine::evtClickHelp() { 
	  
	  _showHelp();
	  _hideMainMenu();
	  
  };
  
  void UIEngine::evtClickQuit() {
    return GameManager::getSingleton().requestShutdown();
  };
  
  // based on the selected zone, switches to Level state and starts the game
  void UIEngine::evtClickEngage() {
	  
	  if (!mSelectedZone) // this shouldn't happen
	    return;

    Intro::getSingleton().setSelectedZone(mSelectedZone->getZone());

    // a zone was selected, engage it
    // Level will check for the selected zone in reset() and handle it from there
    //Level::getSingleton()._showEverything(); // we be nice and return everything to what it was
    Level::getSingleton().reset();
    	  
	  // if this is the first zone the player wants to play, just switch to it
	  if (!Level::getSingleton().running()) {
	    return GameManager::getSingleton().changeState(Level::getSingletonPtr());
	  } else {
      return GameManager::getSingleton().popState();
		}
		
  };
  
  void UIEngine::evtClickBackFromZones() {
    _hideZones();
    _showMainMenu();
    
  }
  
  // show the zones screen
  void UIEngine::_showZones() {
		//mUILogo->hide();
		
		GfxEngine::getSingletonPtr()->getCamera()->setPosition(Vector3(0,75, -300));
	  GfxEngine::getSingletonPtr()->getCamera()->lookAt(Vector3(0,75, 100));
		  
		if (Level::getSingleton().running()) {
		  //Level::getSingleton().getTunnel()->getNode()->setVisible(false);
		  
		}
		mTrayMgr->moveWidgetToTray("Engage", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("BackFromZones", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("PrevZone", TL_LEFT);
		mTrayMgr->moveWidgetToTray("NextZone", TL_RIGHT);
		mTrayMgr->moveWidgetToTray("ZoneTitle", TL_TOP);
    //mTrayMgr->moveWidgetToTray("ZoneInfo", TL_LEFT);
    //mTextBoxZoneInfo->show();		

    inZoneScreen = true;
    
    mSelectedZone = mUIZones.front();
    _previewZone();
    
    if (mCurrentButton)
      mCurrentButton->_stopFlashing();
    mCurrentButton = static_cast<OgreBites::Button*>(mTrayMgr->getWidget("Engage"));
	  mCurrentButton->_doFlash();
  };
  // navigate to the next zone in the list, and preview it
  void UIEngine::_nextZone() {
    
    // let's find the next zone
    UIZone* mNextZone = 0;
    if (mSelectedZone == mUIZones.back()) // if it's the last zone, just choose the first one
      mNextZone = mUIZones.front();
    else {
      std::vector<UIZone*>::const_iterator _itr;
      for (_itr = mUIZones.begin(); _itr != mUIZones.end(); ++_itr)
        if ((*_itr) == mSelectedZone) {
          mNextZone = *(++_itr);
          break;
        }
    }
    
    //mSelectedZone->getZone()->disengage();
    GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->removeChild(mSelectedZone->getZone()->firstTunnel()->getNode());
    mSelectedZone = mNextZone;
    
    _previewZone();
  };
  // display a sheet with the Zone's info and a rendering context with
  // a live preview of it in action
  void UIEngine::_previewZone() {
    //mSelectedZone->getZone()->engage();
    GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->addChild(mSelectedZone->getZone()->firstTunnel()->getNode());
    //mTextBoxZoneInfo->setText(mSelectedZone->getInfo()["Description"]);
    static_cast<OgreBites::Label*>(mTrayMgr->getWidget("ZoneTitle"))->setCaption(mSelectedZone->getZone()->name());
  };
  // navigate to the previous zone in the list, and preview it
  void UIEngine::_prevZone() {
  
    // let's find the previous zone
    UIZone* mPrevZone = 0;
    if (mSelectedZone == mUIZones.front()) // if it's the first zone, just choose the last one
      mPrevZone = mUIZones.back();
    else {
      std::vector<UIZone*>::const_iterator _itr;
      for (_itr = mUIZones.begin(); _itr != mUIZones.end(); ++_itr)
        if ((*_itr) == mSelectedZone) {
          mPrevZone = *(--_itr);
          break;
        }
    }
    
    //mSelectedZone->getZone()->disengage();
    GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->removeChild(mSelectedZone->getZone()->firstTunnel()->getNode());
    mSelectedZone = mPrevZone;
    
    _previewZone();
  };
  // hide zones screen
  void UIEngine::_hideZones() {
    
    /*if (Level::getSingleton().running()) {
		  Level::getSingleton().getTunnel()->getNode()->setVisible(true);
		}*/

		mTrayMgr->removeWidgetFromTray("ZoneTitle");
		mTrayMgr->removeWidgetFromTray("Engage");
		mTrayMgr->removeWidgetFromTray("BackFromZones");
		mTrayMgr->removeWidgetFromTray("PrevZone");
		mTrayMgr->removeWidgetFromTray("NextZone");
    //mTrayMgr->removeWidgetFromTray("ZoneInfo");
    //mTextBoxZoneInfo->hide();	
		
		if (mSelectedZone->getZone()->firstTunnel()->getNode()->getParentSceneNode())
      GfxEngine::getSingletonPtr()->getSM()->getRootSceneNode()->removeChild(mSelectedZone->getZone()->firstTunnel()->getNode());
    
    inZoneScreen = false;

  };
}
