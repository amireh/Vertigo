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
	  
	  setupWidgets();
	  
	  mHelpMsg = "The sphere you're controlling is equipped with 2 shields: ";
	  mHelpMsg += "a fiery one to protect against fire obstacles, and an icy one ";
	  mHelpMsg += "to protect against ice obstacles. ";
	  mHelpMsg += "\nYour mission is to reach the last portal with your shields intact.";
	  mHelpMsg += "\nPress space to flip shields.";
	  mHelpMsg += "\nYou may use the arrows or A and D buttons to steer your direction.";

    mLog->debugStream() << "set up!";
		fSetup = true;
		return true;
	}
	
	bool UIEngine::deferredSetup() {
		
		return true;
	}
	
	
	void UIEngine::update( unsigned long lTimeElapsed ) {
		processEvents();
	}
	
	void UIEngine::mouseMoved( const OIS::MouseEvent &e )
	{
		OIS::MouseState state = e.state;
    OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
		if (mTrayMgr->injectMouseMove(orientedEvt)) return;
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
	
  void UIEngine::setupWidgets()
	{
		mTrayMgr->destroyAllWidgets();

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

		//populateSampleMenus();
	}
		
  void UIEngine::buttonHit(Button* b) {
		if (b->getName() == "Configure")   // enter configuration screen
		{
			mTrayMgr->removeWidgetFromTray("PlayResume");
			mTrayMgr->removeWidgetFromTray("Configure");
			mTrayMgr->removeWidgetFromTray("Help");
			mTrayMgr->removeWidgetFromTray("Quit");
			mTrayMgr->moveWidgetToTray("Apply", TL_BOTTOM);
			mTrayMgr->moveWidgetToTray("Back", TL_BOTTOM);

			/*for (unsigned int i = 0; i < mThumbs.size(); i++)
			{
				mThumbs[i]->hide();
			}*/

			/*while (mTrayMgr->getTrayContainer(TL_CENTER)->isVisible())
			{
				mTrayMgr->removeWidgetFromTray(TL_CENTER, 0);
			}

			while (mTrayMgr->getTrayContainer(TL_LEFT)->isVisible())
			{
				mTrayMgr->removeWidgetFromTray(TL_LEFT, 0);
			}*/

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

			/*while (mTrayMgr->getNumWidgets(TL_NONE) != 0)
			{
				mTrayMgr->moveWidgetToTray(TL_NONE, 0, TL_LEFT);
			}*/

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
		  if (Level::getSingleton().running())
		    return GameManager::getSingleton().popState();
		  else
		    return GameManager::getSingleton().changeState(Level::getSingletonPtr());
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
	
	void UIEngine::hide() {
	  mTrayMgr->hideAll();
	  mShadeLayer->hide();
	};
	
	void UIEngine::show() {
	  mTrayMgr->showAll();
	  mShadeLayer->show();
	};
}
