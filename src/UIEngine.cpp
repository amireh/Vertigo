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
    fConfiguring = false;
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
	  
	  mHelpMsg = "The sphere you're controlling is equipped with 2 shields: ";
	  mHelpMsg += "a fiery one to protect against fire obstacles, and an icy one ";
	  mHelpMsg += "to protect against ice obstacles. ";
	  mHelpMsg += "\nYour mission is to reach the last portal with your shields intact.";
	  mHelpMsg += "\nPress space to flip shields.";
	  mHelpMsg += "\nYou may use the arrows or A and D buttons to steer your direction.";

    assignHandles();
    
    refitOverlays();
    
    mLog->debugStream() << "binding handlers";
    
    bindToName("ZoneEntered", this, &UIEngine::evtZoneEntered);
    bindToName("GameStarted", this, &UIEngine::evtGameStarted);
    bindToName("PlayerWon", this, &UIEngine::evtPlayerWon);
    bindToName("SphereDied", this, &UIEngine::evtSphereDied);
    
    inZoneScreen = false;
    inMainMenu = false;
    fConfiguring = false;
        
    mLog->debugStream() << "set up!";
		fSetup = true;
		
		
		_showMainMenu();
		return true;
	}
	
	bool UIEngine::loadZones() { 
		
		UIZone* mUIZone = new UIZone();
		mUIZone->getInfo()["Title"] = "Inferno";
		mUIZone->getInfo()["Path"] = "inferno.vzs";
		mUIZone->getInfo()["Description"] = "Difficulty: Rookie\nFoo: bar.";
		mUIZone->setZone(new Zone(mUIZone->getInfo()["Path"]));
		mUIZones.push_back(mUIZone);
		
		mUIZone = new UIZone();
		mUIZone->getInfo()["Title"] = "Twilight Meadow";
		mUIZone->getInfo()["Path"] = "twilight_meadow.vzs";
		mUIZone->getInfo()["Description"] = "Difficulty: Rookie\nFoo: bar.";
		mUIZone->setZone(new Zone(mUIZone->getInfo()["Path"]));
		mUIZones.push_back(mUIZone);
		
		mUIZone = new UIZone();
		mUIZone->getInfo()["Title"] = "Toxicity";
		mUIZone->getInfo()["Path"] = "toxicity.vzs";
		mUIZone->getInfo()["Description"] = "Difficulty: Rookie\nFoo: bar.";
		mUIZone->setZone(new Zone(mUIZone->getInfo()["Path"]));
		mUIZones.push_back(mUIZone);

    //for (int i =0; i < 6; ++i) {
		mUIZone = new UIZone();
		mUIZone->getInfo()["Title"] = "Dante's Afterlife";
		mUIZone->getInfo()["Path"] = "dante_afterlife.vzs";
		mUIZone->getInfo()["Description"] = "Difficulty: Rookie\nFoo: bar.";
		mUIZone->setZone(new Zone(mUIZone->getInfo()["Path"]));
		mUIZones.push_back(mUIZone);
		//}
		
		return true;
	}
	
	bool UIEngine::deferredSetup() {
		
		
		_currentState = GameManager::getSingleton().currentState();
		//_refit(_currentState);
		refitOverlays();
		
		if (_currentState->getId() == STATE_GAME) {
		  //mSphere = Level::getSingletonPtr()->getSphere();
		  
		  
		  	
		}
		
		return true;
	}
	
	void UIEngine::_refit(GameState* inState) {
	  if (inState->getId() == STATE_INTRO) {
	    _hideHUDs();
	    showMenu();
	  } else {

		  hideMenu();
		  _showHUDs();
		  
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
		 
		/*if (mTitleLabel->getTrayLocation() != TL_NONE &&
			  orientedEvt.state.Z.rel != 0 && mZoneMenu->getNumItems() != 0)
		{
			int newIndex = mZoneMenu->getSelectionIndex() - orientedEvt.state.Z.rel / Ogre::Math::Abs(orientedEvt.state.Z.rel);
			mZoneMenu->selectItem(Ogre::Math::Clamp<int>(newIndex, 0, mZoneMenu->getNumItems() - 1));
		} */  	
	}
	
	void UIEngine::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  if (mTrayMgr->injectMouseDown(orientedEvt, id)) return;
	  
	  if (_currentState->getId() != STATE_INTRO)
		  return;
		  
	}
	
	void UIEngine::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  
	  if (mTrayMgr->injectMouseUp(orientedEvt, id)) return;
	  
	}
	
	void UIEngine::keyPressed( const OIS::KeyEvent &e ) {
	

	  //}
	};
	void UIEngine::keyReleased( const OIS::KeyEvent &e ) {

    if (GameManager::getSingleton().currentState()->getId() == STATE_GAME) {
      switch (e.key) {
        case OIS::KC_F:
          if (mTrayMgr->areFrameStatsVisible())
            mTrayMgr->hideFrameStats();
          else
            mTrayMgr->showFrameStats(TL_TOPLEFT);
          break;      
      }
      return;
    }
	  if (mTrayMgr->isDialogVisible() && (e.key == OIS::KC_ESCAPE || e.key == OIS::KC_RETURN)) {
	    mTrayMgr->closeDialog();
	    return;
	  };

    TrayLocation trayIdx;
    int widgIdx;
    switch (e.key) {
      case OIS::KC_UP:
        trayIdx = mCurrentButton->getTrayLocation();
        widgIdx = mTrayMgr->locateWidgetInTray(mCurrentButton);
        mCurrentButton->_stopFlashing();
        if (widgIdx == 0) {
          mCurrentButton = static_cast<Button*>(mTrayMgr->getWidget(trayIdx, mTrayMgr->getNumWidgets(trayIdx)-1));
        } else {
          mCurrentButton = static_cast<Button*>(mTrayMgr->getWidget(trayIdx, widgIdx -1));
        }
        mCurrentButton->_doFlash();
        break;
      case OIS::KC_DOWN:
        trayIdx = mCurrentButton->getTrayLocation();
        widgIdx = mTrayMgr->locateWidgetInTray(mCurrentButton);
        mCurrentButton->_stopFlashing();
        if (widgIdx == mTrayMgr->getNumWidgets(trayIdx)-1) {
          mCurrentButton = static_cast<Button*>(mTrayMgr->getWidget(trayIdx, 0));
        } else {
          mCurrentButton = static_cast<Button*>(mTrayMgr->getWidget(trayIdx, widgIdx+1));
        }
        mCurrentButton->_doFlash();
        break;
      case OIS::KC_RETURN:
        buttonHit(mCurrentButton);
        break;
      case OIS::KC_RIGHT:
        if (inZoneScreen)
          _nextZone();
        break;
      case OIS::KC_LEFT:
        if (inZoneScreen)
          _prevZone();
        break;
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
            // re-engage in case the user went to the zones screen
            //Level::getSingleton().currentZone()->engage();
            this->_refit(Level::getSingletonPtr());
            return GameManager::getSingleton().popState();
          }
        } else if (fConfiguring) {
          // going back from config screen to main menu
          _hideConfigMenu();
          _showMainMenu();
        }
        break;
      case OIS::KC_F:
        if (mTrayMgr->areFrameStatsVisible())
          mTrayMgr->hideFrameStats();
        else
          mTrayMgr->showFrameStats(TL_TOPLEFT);
        break;
    };
    
	};	
	
  void UIEngine::setupWidgets()
	{
	
	  Ogre::Viewport* mViewport = mWindow->getViewport(0);
	  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
		Ogre::MaterialPtr thumbMat = Ogre::MaterialManager::getSingleton().create("ZoneThumbnail", "Bootstrap");
		thumbMat->getTechnique(0)->getPass(0)->createTextureUnitState();
			
		mTrayMgr->destroyAllWidgets();


		int button_width = mViewport->getActualWidth() / 5;
		
		// create main navigation tray
		//mTrayMgr->showLogo(TL_BOTTOM);
		//mTrayMgr->createSeparator(TL_BOTTOM, "LogoSep");
		mTrayMgr->createButton(TL_BOTTOM, "PlayResume", Level::getSingleton().running() ? "Resume" : "Play", button_width);
		mTrayMgr->createButton(TL_BOTTOM, "Configure", "Configure", button_width);
		mTrayMgr->createButton(TL_BOTTOM, "Help", "Help", button_width);
		mTrayMgr->createButton(TL_BOTTOM, "Quit", "Quit", button_width);
		
		// create zone screen tray
		mTextBoxZoneInfo = mTrayMgr->createTextBox(TL_NONE, "ZoneInfo", "Zone Information", 450, 400);
		mTrayMgr->createButton(TL_NONE, "Engage", "Engage", button_width);
		mTrayMgr->createLabel(TL_NONE, "ChooseZoneLabel", "Choose a Zone", mViewport->getActualWidth()/2);
		mTrayMgr->createButton(TL_NONE, "NextZone", ">");
		mTrayMgr->createButton(TL_NONE, "PrevZone", "<");
		mTrayMgr->createButton(TL_NONE, "BackFromZones", "Go Back", button_width);
		mTextBoxZoneInfo->hide();

		// create configuration screen button tray
		mTrayMgr->createButton(TL_NONE, "Apply", "Apply Changes", button_width);
		mTrayMgr->createButton(TL_NONE, "BackFromConfig", "Go Back", button_width);

		// create configuration screen label and renderer menu
		mTrayMgr->createLabel(TL_NONE, "ConfigLabel", "Configuration");
		mRendererMenu = mTrayMgr->createLongSelectMenu(TL_NONE, "RendererMenu", "Render System", 450, 240, 10);
		mTrayMgr->createSeparator(TL_NONE, "ConfigSeparator");

		// populate render system names
		Ogre::StringVector rsNames;
		Ogre::RenderSystemList rsList = mRoot->getAvailableRenderers();
		for (unsigned int i = 0; i < rsList.size(); i++)
		{
			rsNames.push_back(rsList[i]->getName());
		}
		mRendererMenu->setItems(rsNames);

		//populateZoneMenu();
	}
	
	void UIEngine::populateZoneMenu() {
	};
		
  void UIEngine::buttonHit(Button* b) {
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
		else if (b->getName() == "PlayResume") {
		  evtClickPlay();
		} else if (b->getName() == "Quit") {
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

	    //windowResized(mWindow);
    }
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
    mRoot->setRenderSystem(rs);
    
    mTrayMgr->showOkDialog("Notice!", "Your new settings will be applied the next time you run Vertigo.");
	}
	
	void UIEngine::hideMenu() {
	  mUILogo->hide();
	  //mTrayMgr->hideLogo();
	  mTrayMgr->hideCursor();
	  //mTrayMgr->removeWidgetFromTray("LogoSep");
	  
	  _hideMainMenu();
	  
	  _hideZones();
	  //mTrayMgr->clearAllTrays();
	  if (fConfiguring)
	    _hideConfigMenu();
    //_hideConfigMenu();
    
	  /*GfxEngine::getSingletonPtr()->getViewport()->setAutoUpdated(true);
	  GfxEngine::getSingletonPtr()->getViewport()->clear();*/	  
	};
	
	void UIEngine::showMenu() {
	  if (!Level::getSingleton().running())
      mUILogo->show();
    
    mTrayMgr->showCursor();
	  //mTrayMgr->showLogo(TL_BOTTOM);
	  //mTrayMgr->moveWidgetToTray("LogoSep", TL_BOTTOM);
	  _showMainMenu();
	  
	  mUISheetWin->hide();
	  mUISheetLoss->hide();
	  
	  /*GfxEngine::getSingletonPtr()->getViewport()->setAutoUpdated(false);
	  GfxEngine::getSingletonPtr()->getViewport()->clear();*/
	};
	void UIEngine::_hideMainMenu() {
	  //mTrayMgr->hideAll();
		mTrayMgr->removeWidgetFromTray("PlayResume");
		mTrayMgr->removeWidgetFromTray("Configure");
		mTrayMgr->removeWidgetFromTray("Help");
		mTrayMgr->removeWidgetFromTray("Quit");
		inMainMenu = false;
	  //mShadeLayer->hide();
	  //mDialogShade->hide();
	  //mUILogo->hide();
	  

	};
	
	void UIEngine::_showMainMenu() {
	  //mTrayMgr->showAll();

	  inMainMenu = true;
	  
		mTrayMgr->moveWidgetToTray("PlayResume", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Configure", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Help", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Quit", TL_BOTTOM);
	
	  
	  if (mCurrentButton)
      mCurrentButton->_stopFlashing();
	  mCurrentButton = static_cast<OgreBites::Button*>(mTrayMgr->getWidget("PlayResume"));
	  mCurrentButton->_doFlash();
	  //mDialogShade->show();
	  //mShadeLayer->show();
	  //mUILogo->show();
	  

	};
	
	void UIEngine::assignHandles() {
		mUISheet = mOverlayMgr->getByName("Vertigo/UI");
		mUIScore = mUISheet->getChild("UI/TimerContainer")->getChild("UI/Timer");
		
		mUISheetLoss = mOverlayMgr->getByName("Vertigo/UI/Loss");
		mUISheetWin = mOverlayMgr->getByName("Vertigo/UI/Win");
		mUISheetPrepare = mOverlayMgr->getByName("Vertigo/UI/Prepare");
		
		mFireShield = mUISheet->getChild("UI/FireShieldContainer")->getChild("UI/FireShield");
		mIceShield = mUISheet->getChild("UI/IceShieldContainer")->getChild("UI/IceShield");
		
		mUILogo = mOverlayMgr->getByName("Vertigo/UI/Intro/Logo");
	};
	
	void UIEngine::setupOverlays() {
	  mUISheet->show();
		mUISheetLoss->hide();
		mUISheetWin->hide();
		mUISheetPrepare->show();
		
		
	};
	
	void UIEngine::refitOverlays() {
	  using Ogre::TextAreaOverlayElement;
	  
	  Ogre::Viewport* mViewport = mWindow->getViewport(0);
		int width = mViewport->getActualWidth();
		int height = mViewport->getActualHeight();
		float aspect_ratio = width / height;
		
		if (_currentState->getId() == STATE_INTRO) {
		  Ogre::TextAreaOverlayElement* label = static_cast<Ogre::TextAreaOverlayElement*>(((Ogre::OverlayContainer*)mTrayMgr->getWidget("ChooseZoneLabel")->getOverlayElement())->getChild("ChooseZoneLabel/LabelCaption")); 
		  label->setCharHeight(mViewport->getActualHeight() / 12);
		  label->setHorizontalAlignment(Ogre::GHA_LEFT);
		  label->setAlignment(Ogre::TextAreaOverlayElement::Left);
		
		  // resize the logo to span a 1/2 of the viewport's height, since it's a regular
		  // rectangle, then we use the same dimension for both width and height
		  mUILogo->getChild("UI/Intro/Logo")->setWidth(height / 2);
		  mUILogo->getChild("UI/Intro/Logo")->setHeight(height / 2);
		
		  //mTrayMgr->getWidget("Vertigo/Intro/Logo")->getOverlayElement()->setHeight(height / 12);
		  //mTrayMgr->getWidget("Vertigo/Intro/Logo")->getOverlayElement()->setWidth(height / 12);
		
		  // resize all our buttons to 1/16 of viewport height
		  mTrayMgr->getWidget("PlayResume")->getOverlayElement()->setHeight(height / 16);
		  mTrayMgr->getWidget("Configure")->getOverlayElement()->setHeight(height / 16);
		  mTrayMgr->getWidget("Help")->getOverlayElement()->setHeight(height / 16);
		  mTrayMgr->getWidget("Quit")->getOverlayElement()->setHeight(height / 16);
		  mTrayMgr->getWidget("Apply")->getOverlayElement()->setHeight(height / 16);
		  mTrayMgr->getWidget("BackFromConfig")->getOverlayElement()->setHeight(height / 16);
		  mTrayMgr->getWidget("Engage")->getOverlayElement()->setHeight(height / 16);
		  mTrayMgr->getWidget("BackFromZones")->getOverlayElement()->setHeight(height / 16);
		
		  mTrayMgr->adjustTrays();
		} else {
		
		  //mShieldBarWidth = mUISheet->getChild("UI/FireShieldContainer")->getChild("UI/FireShield")->getWidth();
		  mShieldBarWidth = mViewport->getActualWidth() / 3;
		  mUISheet->getChild("UI/FireShieldContainer")->setWidth(mShieldBarWidth);
		  mUISheet->getChild("UI/FireShieldContainer")->getChild("UI/FireShield")->setWidth(mShieldBarWidth);
		
		  /*mUISheet->getChild("UI/FireShieldContainer")->setHeight(64.0f / aspect_ratio);
		  mUISheet->getChild("UI/FireShieldContainer")->getChild("UI/FireShield")->setHeight(64.0f / aspect_ratio);*/
		
		  mUISheet->getChild("UI/IceShieldContainer")->setWidth(mShieldBarWidth);
		  mUISheet->getChild("UI/IceShieldContainer")->setLeft(-1*mShieldBarWidth);
		  mUISheet->getChild("UI/IceShieldContainer")->getChild("UI/IceShield")->setWidth(mShieldBarWidth);
		  mUISheet->getChild("UI/IceShieldContainer")->getChild("UI/IceShield")->setLeft(-1*mShieldBarWidth);
		
      // scale overlays' font size
      
      TextAreaOverlayElement* mTOverlay;
      float font_size = 32 / aspect_ratio;
      static_cast<TextAreaOverlayElement*>(
        mOverlayMgr->getByName("Vertigo/UI/Prepare")->
                     getChild("UI/Containers/Prepare")->
                     getChild("UI/Text/Prepare"))->setCharHeight(font_size);
      static_cast<TextAreaOverlayElement*>(
        mOverlayMgr->getByName("Vertigo/UI/Loading")->
                     getChild("UI/Containers/Loading")->
                     getChild("UI/Text/Loading"))->setCharHeight(font_size);
      static_cast<TextAreaOverlayElement*>(
        mOverlayMgr->getByName("Vertigo/UI/Loss")->
                     getChild("UI/Containers/Loss")->
                     getChild("UI/Text/Loss"))->setCharHeight(font_size);
      static_cast<TextAreaOverlayElement*>(
        mOverlayMgr->getByName("Vertigo/UI/Win")->
                     getChild("UI/Containers/Win")->
                     getChild("UI/Text/Win"))->setCharHeight(font_size);
                     
    };
	};
	
	void UIEngine::_updateShields() {
    using namespace Ogre;
    Sphere* mSphere = Level::getSingleton().getSphere();
    if (Level::getSingleton().currentZone()->getSettings().mMode == DODGY) {
      OverlayElement* mElement = (mSphere->shield() == FIRE) 
        ? mFireShield
        : mIceShield;
        
      mElement->setWidth(mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));
      
      if (mElement->getHorizontalAlignment() == Ogre::GHA_RIGHT)
        mElement->setLeft(-1 * mElement->getWidth());
    } else {
    for (int i=0; i < 2; ++i) {
      SHIELD tShield = (i == 0) ? FIRE : ICE;
      
      OverlayElement* mElement = (tShield == FIRE) 
        ? mFireShield
        : mIceShield;
      
      mElement->setWidth(mShieldBarWidth * (mSphere->getShieldState(tShield) / 1000.0f));
      
      if (mElement->getHorizontalAlignment() == Ogre::GHA_RIGHT)
        mElement->setLeft(-1 * mElement->getWidth());
    }
    }
    // update the player's score
    mUIScore->setCaption(stringify(mSphere->score()));
    
    //mLog->debugStream() << "updating UI, element's new width: " << mElement->getWidth();  
    //mElement->setCaption(String(stringify(mSphere->getShieldState())));	
	};
	
	void UIEngine::_hideHUDs() {
	  mUISheet->hide();
	};
	
	void UIEngine::_showHUDs() {
	  mUISheet->show();
	};
	
	bool UIEngine::evtSphereDied(Event* inEvt) {
	  mUISheetLoss->show();
	  return true;
	};
	
	
	bool UIEngine::evtPlayerWon(Event* inEvt) {
	  mUISheetWin->show();
	  return true;
	};
	
	bool UIEngine::evtGameStarted(Event* inEvt) {  
	  mDialogShade->hide();
	  mShadeLayer->hide();
	  mUISheetPrepare->hide();
	  mUISheetWin->hide();
	  mUISheetLoss->hide();
	  
	  // if it's dodgy mode, we have to hide one of the shield HUDs since
	  // the player will be using only one of them
	  Sphere* mSphere = Level::getSingleton().getSphere();
	  if (Level::getSingleton().currentZone()->getSettings().mMode == DODGY) {
	    
	    if (mSphere->shield() == FIRE) {
	      // hide the ice shield HUD
	      mIceShield->hide();
	      mUISheet->getChild("UI/IceShieldContainer")->hide();
	      //mUISheet->getChild("UI/FireShieldContainer")->setWidth(mShieldBarWidth);
	      
	      // resize the background of the shield hud
	      mFireShield->show();
	      mUISheet->getChild("UI/FireShieldContainer")->show();
	      
	      // and center them both
	      //mFireShield->setHorizontalAlignment(Ogre::GHA_CENTER);
	      //mUISheet->getChild("UI/FireShieldContainer")->setHorizontalAlignment(Ogre::GHA_CENTER);
	      
	    } else {
	      // hide the fire one
	      mFireShield->hide();
	      mUISheet->getChild("UI/FireShieldContainer")->hide();
	      
	      mIceShield->show();
	      mUISheet->getChild("UI/IceShieldContainer")->show();
	    }
	  } else {
	    mUISheet->getChild("UI/FireShieldContainer")->show();
	    mUISheet->getChild("UI/IceShieldContainer")->show();
	    
	    mIceShield->show();
	    mFireShield->show();
	  }
	  
    mUISheet->getChild("UI/FireShieldContainer")->setWidth(mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));
    mUISheet->getChild("UI/FireShieldContainer")->setWidth(mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));
    mUISheet->getChild("UI/IceShieldContainer")->setWidth(mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));
	  mUISheet->getChild("UI/IceShieldContainer")->setLeft(-1 * mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));
	  
	  _updateShields();
	  return true;
	};
	
	bool UIEngine::evtZoneEntered(Event* inEvt) {
	  //this->_refit(Level::getSingletonPtr());
	  
	  std::ostringstream msg;
	  msg << "Zone Mode: ";
	  switch (mSelectedZone->getZone()->getSettings().mMode) {
	    case ARCADE:
	      msg << "Arcade\n"
	          << "\nYour objective is to reach the last \nportal with both shields functional."
	          << "\n\nControls:"
	          << "\nFlip shields by pressing SPACE BAR."
	          << "\nYou can also move to the left and right\nby pushing A and D.";
	      break;
	    case DODGY:
	      msg << "Dodgy\n"
	          << "\nOnly one of your shields is functional,\n you MUST dodge one kind of hazards."
	          << "\n\nControls:"
	          << "\nYou can move to the left and right\nby pushing A and D.";
	      break;
	  }
	  msg << "\n\nPress ENTER when ready!";
	  
	  Ogre::OverlayElement* tText = static_cast<Ogre::OverlayElement*>(mUISheetPrepare->getChild("UI/Containers/Prepare")->getChild("UI/Text/Prepare"));
	  tText->setCaption(msg.str());
	  //tText->setMetricsMode(Ogre::GMM_PIXELS);
	  /*Ogre::Viewport* mViewport = mWindow->getViewport(0);
	  tText->setTop(mViewport->getActualHeight() / 2 - ((tText->getHeight() * mViewport->getActualHeight()) / 2));
	  tText->setLeft(mViewport->getActualWidth() / 2 - (tText->getWidth() * mViewport->getActualWidth() / 2));*/
	  mDialogShade->show();
	  mShadeLayer->show();
	  mUISheet->show();
	  mUISheetPrepare->show();
	  mUISheetWin->hide();
	  mUISheetLoss->hide();
	  return true;
	};
	
	void UIEngine::toggleFPS() {
	  if (mTrayMgr->areFrameStatsVisible())
	    mTrayMgr->hideFrameStats();
	  else
	    mTrayMgr->showFrameStats(OgreBites::TL_TOPLEFT);
	};
	
  void UIEngine::showDialog(const std::string& inCaption, const std::string& inMessage) {
    
    mTrayMgr->showOkDialog(inCaption, inMessage, 450);

  }
  
  // shifts from main menu to zones screen
  void UIEngine::evtClickPlay() {
    // remove main screen menu
	  _hideMainMenu();
    _showZones(); 
  };
  
  // shows the video settings panel
  void UIEngine::evtClickConfigure() {
    fConfiguring = true;
    
		_hideMainMenu();
		mTrayMgr->moveWidgetToTray("Apply", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("BackFromConfig", TL_BOTTOM);

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

		for (unsigned int i = 3; i < mTrayMgr->getNumWidgets(mRendererMenu->getTrayLocation()); i++)
		{
			SelectMenu* menu = (SelectMenu*)mTrayMgr->getWidget(mRendererMenu->getTrayLocation(), i);
			if (menu->getSelectedItem() != options[menu->getCaption()].currentValue) reset = true;
			newOptions[menu->getCaption()] = menu->getSelectedItem();
		}

		// reset with new settings if necessary
		if (reset) reconfigure(mRendererMenu->getSelectedItem(), newOptions);  
  };
  
  void UIEngine::_hideConfigMenu() {
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
		
		fConfiguring = false;
  };
  void UIEngine::evtClickBackFromConfig() {

    _hideConfigMenu();
    _showMainMenu();
		/*mTrayMgr->moveWidgetToTray("PlayResume", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Configure", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Help", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("Quit", TL_BOTTOM);*/

		//windowResized(mWindow);  
  };
  // display a dialogue with some info on how to play the game
  void UIEngine::evtClickHelp() { 
    mTrayMgr->showOkDialog("How to play", mHelpMsg, 520);
    //if (mCurrentButton)
      //mCurrentButton->_stopFlashing();
    //mCurrentButton = static_cast<Button*>(mTrayMgr->getWidget(mTrayMgr->getName() + "/OkButton"));
    //mCurrentButton->_doFlash();
  };
  void UIEngine::evtClickQuit() {
    return GameManager::getSingleton().requestShutdown();
  };
  // based on the selected zone, switches to Level state and starts the game
  void UIEngine::evtClickEngage() {
	  // if the Level state is running, then we resume it
	  // if it's not, then we're launching a new game
	  UIZone* zone = mSelectedZone;//Ogre::any_cast<UIZone*>(mThumbs[mZoneMenu->getSelectionIndex()]->getUserAny());
	  
	  if (!zone)
	    return;
	  
	  // if this is the first zone the player wants to play, just switch to it
	  if (!Level::getSingleton().running()) {
	    Intro::getSingleton().setSelectedZone(zone->getZone());
	    return GameManager::getSingleton().changeState(Level::getSingletonPtr());
	  } else {
	    // if we're engaged in a zone, and the selected zone in menu is not the same
	    // then we have to reload the game
	    //if (Intro::getSingleton().getSelectedZone()->name() != zone->getZone()->name()) {
	      // reset and load new level
	      //EventManager::getSingletonPtr()->_clearQueue();
	      Intro::getSingleton().setSelectedZone(zone->getZone());
	      Level::getSingleton().reset();
	      //Event* mEvt = mEvtMgr->createEvt("ZoneEntered");
	      //mEvtMgr->hook(mEvt);
	      //this->_refit(Level::getSingletonPtr());
	      return GameManager::getSingleton().popState();
	   /* } else {
	      // just resume the current level
	      //hideMenu();
	      this->_refit(Level::getSingletonPtr());
	      return GameManager::getSingleton().popState();
	    }*/
		}
		
	  if (Level::getSingleton().running()) {
	    
	  } else {
	    
	  }
  };
  
  void UIEngine::evtClickBackFromZones() {
    _hideZones();
    _showMainMenu();
    
  }
  
  // show the zones screen
  void UIEngine::_showZones() {
		mUILogo->hide();
		
		if (Level::getSingleton().running()) {
		  Level::getSingleton().getTunnel()->getNode()->setVisible(false);
		  GfxEngine::getSingletonPtr()->getCamera()->setPosition(Vector3(0,75, -200));
		  GfxEngine::getSingletonPtr()->getCamera()->lookAt(Vector3(0,75, 100));
		}
		mTrayMgr->moveWidgetToTray("Engage", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("BackFromZones", TL_BOTTOM);
		mTrayMgr->moveWidgetToTray("PrevZone", TL_LEFT);
		mTrayMgr->moveWidgetToTray("NextZone", TL_RIGHT);
		mTrayMgr->moveWidgetToTray("ChooseZoneLabel", TL_TOPLEFT);
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
    
    mSelectedZone->getZone()->disengage();
    mSelectedZone = mNextZone;
    
    _previewZone();
  };
  // display a sheet with the Zone's info and a rendering context with
  // a live preview of it in action
  void UIEngine::_previewZone() {
    mSelectedZone->getZone()->engage();
    //mTextBoxZoneInfo->setText(mSelectedZone->getInfo()["Description"]);
    static_cast<OgreBites::Label*>(mTrayMgr->getWidget("ChooseZoneLabel"))->setCaption(mSelectedZone->getInfo()["Title"]);
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
    
    mSelectedZone->getZone()->disengage();
    mSelectedZone = mPrevZone;
    
    _previewZone();
  };
  // hide zones screen
  void UIEngine::_hideZones() {
  
    if (Level::getSingleton().running()) {
		  Level::getSingleton().getTunnel()->getNode()->setVisible(true);
		}

		mTrayMgr->removeWidgetFromTray("ChooseZoneLabel");
		mTrayMgr->removeWidgetFromTray("Engage");
		mTrayMgr->removeWidgetFromTray("BackFromZones");
		mTrayMgr->removeWidgetFromTray("PrevZone");
		mTrayMgr->removeWidgetFromTray("NextZone");
    //mTrayMgr->removeWidgetFromTray("ZoneInfo");
    //mTextBoxZoneInfo->hide();	
		  
    mSelectedZone->getZone()->disengage();
    inZoneScreen = false;

  };
}
