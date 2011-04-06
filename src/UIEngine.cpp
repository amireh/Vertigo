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
		mZoneMenu = 0;
    mTitleLabel = 0;
    mCarouselPlace = 0.0f;
    mRendererMenu = 0;
	}
	
	UIEngine::~UIEngine() {
		mLog->infoStream() << "shutting down";

		if (fSetup) {
			
			mLog->debugStream() << "destroying system";

      delete mTrayMgr;
	    mOverlayMgr->destroyOverlayElement(mDialogShade);
	    mOverlayMgr->destroy("ShadeLayer");
	    
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
    _hideMenu();
    _hideHUDs();
	  
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
		mDialogShade->show();
		mShadeLayer->show();
		
		mTrayMgr = new SdkTrayManager("BrowserControls", mWindow, InputManager::getSingletonPtr()->getMouse(), this);
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

    mLog->debugStream() << "binding handlers";
    
    bindToName("ZoneEntered", this, &UIEngine::evtZoneEntered);
    bindToName("GameStarted", this, &UIEngine::evtGameStarted);
    bindToName("PlayerWon", this, &UIEngine::evtPlayerWon);
    bindToName("SphereDied", this, &UIEngine::evtSphereDied);
        
    mLog->debugStream() << "set up!";
		fSetup = true;
		return true;
	}
	
	bool UIEngine::loadZones() { 
		
		UIZone* mUIZone = new UIZone();
		mUIZone->getInfo()["Thumbnail"] = "Lava_Cracks.jpg";
		mUIZone->getInfo()["Title"] = "Inferno";
		mUIZone->getInfo()["Path"] = "inferno.vzs";
		mUIZones.push_back(mUIZone);
		
		mUIZone = new UIZone();
		mUIZone->getInfo()["Thumbnail"] = "Volcanic_Rose_Glass.jpg";
		mUIZone->getInfo()["Title"] = "Twilight Meadow";
		mUIZone->getInfo()["Path"] = "twilight_meadow.vzs";
		mUIZones.push_back(mUIZone);
		
		mUIZone = new UIZone();
		mUIZone->getInfo()["Thumbnail"] = "Slime.jpg";
		mUIZone->getInfo()["Title"] = "Toxicity";
		mUIZone->getInfo()["Path"] = "toxicity.vzs";
		mUIZones.push_back(mUIZone);

    //for (int i =0; i < 6; ++i) {
		mUIZone = new UIZone();
    mUIZone->getInfo()["Thumbnail"] = "Dante_Afterlife.jpg";
		mUIZone->getInfo()["Title"] = "Dante's Afterlife";
		mUIZone->getInfo()["Path"] = "dante_afterlife.vzs";
		mUIZones.push_back(mUIZone);
		//}
		
		return true;
	}
	
	bool UIEngine::deferredSetup() {
		
		
		_currentState = GameManager::getSingleton().currentState();
		
		if (_currentState->getId() == STATE_GAME) {
		  mSphere = Level::getSingletonPtr()->getSphere();
		  
		  _hideMenu();
		  _showHUDs();
		  setupOverlays();
		  refitOverlays();
		  
		  mUISheetPrepare->show();
		} else {
		  _hideHUDs();
		  _showMenu();
		}
		return true;
	}
	
	
	void UIEngine::update( unsigned long lTimeElapsed ) {
		processEvents();
		
		// update our good tray manager
    evt.timeSinceLastFrame = evt.timeSinceLastEvent = lTimeElapsed;		
    mTrayMgr->frameRenderingQueued(evt);
    
    Ogre::OverlayContainer* selected;
    // don't do all these calculations when sample's running or when in configuration screen or when no samples loaded
    if (mTitleLabel->getTrayLocation() != TL_NONE)
    {
	    // makes the carousel spin smoothly toward its right position
	    Ogre::Real carouselOffset = mZoneMenu->getSelectionIndex() - mCarouselPlace;
	    if ((carouselOffset <= 0.001) && (carouselOffset >= -0.001)) mCarouselPlace = mZoneMenu->getSelectionIndex();
	    else mCarouselPlace += carouselOffset * Ogre::Math::Clamp<Ogre::Real>(evt.timeSinceLastFrame * 1.0, -1.0, 1.0);

	    // update the thumbnail positions based on carousel state
	    for (int i = 0; i < (int)mThumbs.size(); i++)
	    {
		    Ogre::Real thumbOffset = mCarouselPlace - i;
		    Ogre::Real phase = (thumbOffset / 2.0) - 2.8;

		    if (thumbOffset < -2 || thumbOffset > 4)    // prevent thumbnails from wrapping around in a circle
		    {
			    mThumbs[i]->hide();
			    continue;
		    }
		    else mThumbs[i]->show();

		    Ogre::Real scale = 1.0 / Ogre::Math::Pow((Ogre::Math::Abs(thumbOffset) + 1.0), 0.75);
		    Ogre::Real left = Ogre::Math::Cos(phase) * 200.0;
		    Ogre::Real top = Ogre::Math::Sin(phase) * 200.0;
		

		    Ogre::BorderPanelOverlayElement* frame =
			    (Ogre::BorderPanelOverlayElement*)mThumbs[i]->getChildIterator().getNext();

		    mThumbs[i]->setDimensions(256.0 * scale, 192.0 * scale);
		    frame->setDimensions(mThumbs[i]->getWidth() + 16.0, mThumbs[i]->getHeight() + 16.0);
		    mThumbs[i]->setPosition((int)(left - 80.0 - (mThumbs[i]->getWidth() / 2.0)),
			    (int)(top - 5.0 - (mThumbs[i]->getHeight() / 2.0)));

		    if (i == mZoneMenu->getSelectionIndex()) {
		      frame->setBorderMaterialName("SdkTrays/Frame/Over");
		      //frame->hide();
		      selected = mThumbs[i];
		      //frame->getParent()->setZOrder(1000);
		    }
		    else {
		      //frame->getParent()->setZOrder(10);
		      frame->setBorderMaterialName("SdkTrays/Frame");
		    }
	    }
	    //mThumbs[mZoneMenu->getSelectionIndex()]->hide();
	    // move the selected thumb to the front
	    mTrayMgr->getTraysLayer()->remove2D(selected);
	    mTrayMgr->getTraysLayer()->add2D(selected);
	    //mThumbs[mZoneMenu->getSelectionIndex()]->show();
    }    
	}
	
	void UIEngine::mouseMoved( const OIS::MouseEvent &e )
	{
		OIS::MouseState state = e.state;
    OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
		if (mTrayMgr->injectMouseMove(orientedEvt)) return;
		
		if (_currentState->getId() != STATE_INTRO)
		  return;
		 
		if (mTitleLabel->getTrayLocation() != TL_NONE &&
			  orientedEvt.state.Z.rel != 0 && mZoneMenu->getNumItems() != 0)
		{
			int newIndex = mZoneMenu->getSelectionIndex() - orientedEvt.state.Z.rel / Ogre::Math::Abs(orientedEvt.state.Z.rel);
			mZoneMenu->selectItem(Ogre::Math::Clamp<int>(newIndex, 0, mZoneMenu->getNumItems() - 1));
		}   	
	}
	
	void UIEngine::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  if (mTrayMgr->injectMouseDown(orientedEvt, id)) return;
	  
	  if (_currentState->getId() != STATE_INTRO)
		  return;
		  
		if (mTitleLabel->getTrayLocation() != TL_NONE)
			for (unsigned int i = 0; i < mThumbs.size(); i++)
				if (mThumbs[i]->isVisible() && Widget::isCursorOver(mThumbs[i],
                          Ogre::Vector2(mTrayMgr->getCursorContainer()->getLeft(),
                                        mTrayMgr->getCursorContainer()->getTop()), 0))
				{
					mZoneMenu->selectItem(i);
					break;
				}
	}
	
	void UIEngine::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  
	  if (mTrayMgr->injectMouseUp(orientedEvt, id)) return;
	  
	}
	
  void UIEngine::setupWidgets()
	{
	
	  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
		Ogre::MaterialPtr thumbMat = Ogre::MaterialManager::getSingleton().create("ZoneThumbnail", "Bootstrap");
		thumbMat->getTechnique(0)->getPass(0)->createTextureUnitState();
			
		mTrayMgr->destroyAllWidgets();

    mTitleLabel = mTrayMgr->createLabel(TL_CENTER, "ZoneTitle", "", 200);
    mZoneMenu = mTrayMgr->createThickSelectMenu(TL_NONE, "ZoneMenu", "Choose Zone", 250, 10);
    
		// create main navigation tray
		mTrayMgr->showLogo(TL_BOTTOM);
		mTrayMgr->createSeparator(TL_BOTTOM, "LogoSep");
		mTrayMgr->createButton(TL_BOTTOM, "PlayResume", Level::getSingleton().running() ? "Resume" : "Play");
		mTrayMgr->createButton(TL_BOTTOM, "Configure", "Configure");
		mTrayMgr->createButton(TL_BOTTOM, "Help", "Help");
		mTrayMgr->createButton(TL_BOTTOM, "Quit", "Quit");

		// create configuration screen button tray
		mTrayMgr->createButton(TL_NONE, "Apply", "Apply Changes");
		mTrayMgr->createButton(TL_NONE, "Back", "Go Back");

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

		populateZoneMenu();
	}
	
	void UIEngine::populateZoneMenu() {
	  
		for (unsigned int i = 0; i < mThumbs.size(); i++)    // destroy all thumbnails in carousel
		{
			Ogre::MaterialManager::getSingleton().remove(mThumbs[i]->getName());
			Widget::nukeOverlayElement(mThumbs[i]);
		}
		mThumbs.clear();

		Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
		Ogre::StringVector zoneTitles;
		Ogre::MaterialPtr templateMat = Ogre::MaterialManager::getSingleton().getByName("ZoneThumbnail");
		
		// populate the sample menu and carousel with filtered samples
		for (int i=0; i<mUIZones.size(); ++i)
		{
			
			UIZone* mUIZone = mUIZones[i];
			_tInfo info = mUIZone->getInfo();
			
			Ogre::String name = "ZoneThumb" + Ogre::StringConverter::toString(i);

			// clone a new material for sample thumbnail
			Ogre::MaterialPtr newMat = templateMat->clone(name);

			Ogre::TextureUnitState* tus = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
			if (Ogre::ResourceGroupManager::getSingleton().resourceExists("General", info["Thumbnail"]))
				tus->setTextureName(info["Thumbnail"]);
			else 
        tus->setTextureName("thumb_error.png");

			// create sample thumbnail overlay
			Ogre::BorderPanelOverlayElement* bp = (Ogre::BorderPanelOverlayElement*)
				mOverlayMgr->createOverlayElementFromTemplate("SdkTrays/Picture", "BorderPanel", name);
			bp->setHorizontalAlignment(Ogre::GHA_CENTER);
			bp->setVerticalAlignment(Ogre::GVA_CENTER);
			bp->setMaterialName(name);
			bp->setUserAny(Ogre::Any(mUIZone));
			mTrayMgr->getTraysLayer()->add2D(bp);

			// add sample thumbnail and title
			mThumbs.push_back(bp);
			zoneTitles.push_back(info["Title"]);
		}

	  mCarouselPlace = 0;  // reset carousel

	  mZoneMenu->setItems(zoneTitles);
	  if (mZoneMenu->getNumItems() != 0) 
	    itemSelected(mZoneMenu);
    
    //mZoneMenu->selectItem(mZoneMenu->getNumItems()-1); // select the last one
	};
		
  void UIEngine::buttonHit(Button* b) {
		if (b->getName() == "Configure")   // enter configuration screen
		{
			mTrayMgr->removeWidgetFromTray("PlayResume");
			mTrayMgr->removeWidgetFromTray("Configure");
			mTrayMgr->removeWidgetFromTray("Help");
			mTrayMgr->removeWidgetFromTray("Quit");
			mTrayMgr->moveWidgetToTray("Apply", TL_BOTTOM);
			mTrayMgr->moveWidgetToTray("Back", TL_BOTTOM);

			for (unsigned int i = 0; i < mThumbs.size(); i++)
			{
				mThumbs[i]->hide();
			}

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

			//windowResized(mWindow);
		}
		else if (b->getName() == "Back")   // leave configuration screen
		{
			while (mTrayMgr->getNumWidgets(mRendererMenu->getTrayLocation()) > 3)
			{
				mTrayMgr->destroyWidget(mRendererMenu->getTrayLocation(), 3);
			}

			while (mTrayMgr->getNumWidgets(TL_NONE) != 0)
			{
				mTrayMgr->moveWidgetToTray(TL_NONE, 0, TL_CENTER);
			}

      mTrayMgr->removeWidgetFromTray("ZoneMenu");
			mTrayMgr->removeWidgetFromTray("Apply");
			mTrayMgr->removeWidgetFromTray("Back");
			mTrayMgr->removeWidgetFromTray("ConfigLabel");
			mTrayMgr->removeWidgetFromTray(mRendererMenu);
			mTrayMgr->removeWidgetFromTray("ConfigSeparator");

			mTrayMgr->moveWidgetToTray("PlayResume", TL_BOTTOM);
			mTrayMgr->moveWidgetToTray("Configure", TL_BOTTOM);
			mTrayMgr->moveWidgetToTray("Help", TL_BOTTOM);
			mTrayMgr->moveWidgetToTray("Quit", TL_BOTTOM);

			//windowResized(mWindow);
		}
		else if (b->getName() == "Apply")   // apply any changes made in the configuration screen
		{
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
		}
		else if (b->getName() == "PlayResume") {
		  // if the Level state is running, then we resume it
		  // if it's not, then we're launching a new game
		  UIZone* zone = Ogre::any_cast<UIZone*>(mThumbs[mZoneMenu->getSelectionIndex()]->getUserAny());
		  
		  // if this is the first zone the player wants to play, just switch to it
		  if (!Level::getSingleton().running()) {
		    Intro::getSingleton().setSelectedZone(zone->getInfo()["Path"]);
		    return GameManager::getSingleton().changeState(Level::getSingletonPtr());
		  } else {
		    // if we're engaged in a zone, and the selected zone in menu is not the same
		    // then we have to reload the game
		    if (Intro::getSingleton().getSelectedZone() != zone->getInfo()["Path"]) {
		      // reset and load new level
		      Intro::getSingleton().setSelectedZone(zone->getInfo()["Path"]);
		      Level::getSingleton().reset();
		      Event* mEvt = mEvtMgr->createEvt("ZoneEntered");
		      mEvt->setProperty("Path", zone->getInfo()["Path"]);
		      mEvtMgr->hook(mEvt);    
		      
		      return GameManager::getSingleton().popState();
		    } else {
		      // just resume the current level
		      _hideMenu();
		      return GameManager::getSingleton().popState();
		    }
			}
			
		  if (Level::getSingleton().running()) {
		    
		  } else {
		    
		  }
		} else if (b->getName() == "Quit") {
		  return GameManager::getSingleton().requestShutdown();
		} else if(b->getName() == "Help") {
		  mTrayMgr->showOkDialog("How to play", mHelpMsg, 520);
		};
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
    } else if (menu == mZoneMenu)    // sample changed, so update slider, label and description
		{

			UIZone* zone = Ogre::any_cast<UIZone*>(mThumbs[menu->getSelectionIndex()]->getUserAny());
			mTitleLabel->setCaption(menu->getSelectedItem());
			//Intro::getSingleton().setSelectedZone(zone->getInfo()["Path"]);
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
	
	void UIEngine::_hideMenu() {
	  mTrayMgr->hideAll();
	  mShadeLayer->hide();
	};
	
	void UIEngine::_showMenu() {
	  mTrayMgr->showAll();
	  mShadeLayer->show();
	};
	
	void UIEngine::assignHandles() {
		mUISheet = mOverlayMgr->getByName("Vertigo/UI");
		mUIScore = mUISheet->getChild("UI/TimerContainer")->getChild("UI/Timer");
		
		mUISheetLoss = mOverlayMgr->getByName("Vertigo/UI/Loss");
		mUISheetWin = mOverlayMgr->getByName("Vertigo/UI/Win");
		mUISheetPrepare = mOverlayMgr->getByName("Vertigo/UI/Prepare");
		
		mFireShield = mUISheet->getChild("UI/FireShieldContainer")->getChild("UI/FireShield");
		mIceShield = mUISheet->getChild("UI/IceShieldContainer")->getChild("UI/IceShield");
	};
	
	void UIEngine::setupOverlays() {
	  mUISheet->show();
		mUISheetLoss->hide();
		mUISheetWin->hide();
		mUISheetPrepare->show();	
	};
	
	void UIEngine::refitOverlays() {
	  Ogre::Viewport* mViewport = mWindow->getViewport(0);
		float aspect_ratio = mViewport->getActualWidth() / mViewport->getActualHeight();
		
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
    using Ogre::TextAreaOverlayElement;
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
	
	void UIEngine::_updateShields() {
    using namespace Ogre;
    OverlayElement* mElement = (mSphere->shield() == FIRE) 
      ? mFireShield
      : mIceShield;
    
    mElement->setWidth(mShieldBarWidth * (mSphere->getShieldState() / 1000.0f));
    
    if (mElement->getHorizontalAlignment() == Ogre::GHA_RIGHT)
      mElement->setLeft(-1 * mElement->getWidth());
    
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
	  mUISheetPrepare->hide();
	  mUISheetWin->hide();
	  mUISheetLoss->hide();
	  return true;
	};
	
	bool UIEngine::evtZoneEntered(Event* inEvt) {
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
	
	void UIEngine::_refit(GameState* inState) {
	  if (inState->getId() == STATE_INTRO) {
	    _hideHUDs();
	    _showMenu();
	  } else {
	    _hideMenu();
	    _showHUDs();
	  }
	  
	  _currentState = inState;
	};

  void UIEngine::showDialog(const std::string& inCaption, const std::string& inMessage) {
    mTrayMgr->showOkDialog(inCaption, inMessage, 450);
  }	
}
