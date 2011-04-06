#include "Intro.h"
#include "EventManager.h"
#include "Level.h"
#include <cstdlib>
#include <ctime> 

using namespace Ogre;
namespace Pixy
{
	
	Intro* Intro::mIntro;
	
	GAME_STATE Intro::getId() const { return STATE_INTRO; }
	
	void Intro::enter( void ) {
		
		srand((unsigned)time(0));
		
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Intro");

    GfxEngine::getSingletonPtr()->hideUI();
    
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
	  
	  setupWidgets();
	  
	  mTrayMgr->setTrayPadding(10);
		//mEvtMgr = EventManager::getSingletonPtr();
		
		//mEngines.clear();
		
		// init engines
		//mGfxEngine = GfxEngine::getSingletonPtr();
		//mPhyxEngine = PhyxEngine::getSingletonPtr();
		//mSfxEngine = SfxEngine::getSingletonPtr();
		//mUIEngine = UIEngine::getSingletonPtr();
		
		//mEngines.push_back(mGfxEngine);
		//mEngines.back()->setup();
		/*
		//mEngines.push_back(mPhyxEngine);
		//mEngines.back()->setup();
		
		mEngines.push_back(mSfxEngine);
		mEngines.back()->setup();		*/
		
		//mEngines.push_back(mUIEngine);
		//mEngines.back()->setup();

    // prepare our sphere
		//mSphere = new Sphere();
		//mSphere->live();
    /*
		for (_itrEngines = mEngines.begin();
		     _itrEngines != mEngines.end();
		     ++_itrEngines)
		    (*_itrEngines)->deferredSetup();*/
		
		    		
		mLog->infoStream() << "Initialized successfully.";
		
	}


	
	void Intro::exit( void ) {
	  
	  
	  delete mTrayMgr;
	  mOverlayMgr->destroyOverlayElement(mDialogShade);
	  mOverlayMgr->destroy("ShadeLayer");
		//mObstacles.clear();
		
		//delete mSphere;
		
		//mEngines.clear();
		
		//bool fShuttingDown = GameManager::getSingleton().shuttingDown();
		
		//mUIEngine->cleanup();
		//if (fShuttingDown)
		//  delete mUIEngine;
				     
		//mSfxEngine->cleanup();
		//if (fShuttingDown)
		  //delete mSfxEngine;
		
		//mPhyxEngine->cleanup();
		//delete mPhyxEngine;
		
		//mGfxEngine->cleanup();
		//if (fShuttingDown)
		//  delete mGfxEngine;
		
		
		//EventManager::shutdown();
		GfxEngine::getSingletonPtr()->showUI();
		
		mLog->infoStream() << "---- Exiting Intro State ----";
		delete mLog;
		mLog = 0;
		 
	}
	
	void Intro::keyPressed( const OIS::KeyEvent &e )
	{
	
	  //mUIEngine->keyPressed( e );
	
	}
	
	void Intro::keyReleased( const OIS::KeyEvent &e ) {
		
		//mUIEngine->keyReleased( e );

		if (e.key == OIS::KC_Q) {
		  return this->requestShutdown();
		} else if (e.key == OIS::KC_RETURN || e.key == OIS::KC_ESCAPE)
		  //GameManager::getSingleton().changeState(Level::getSingletonPtr());
		  GameManager::getSingleton().popState();

		
	}
	
	void Intro::mouseMoved( const OIS::MouseEvent &e )
	{
		//mUIEngine->mouseMoved(e);
		//mSphere->mouseMoved(e);
		//mGfxEngine->mouseMoved(e);
		
		OIS::MouseState state = e.state;
    OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
		if (mTrayMgr->injectMouseMove(orientedEvt)) return;
	}
	
	void Intro::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  if (mTrayMgr->injectMouseDown(orientedEvt, id)) return;
	  
    //mUIEngine->mousePressed(e, id);
		//mGfxEngine->mousePressed(e, id);
	}
	
	void Intro::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  
	  if (mTrayMgr->injectMouseUp(orientedEvt, id)) return;
	  
		//mUIEngine->mouseReleased(e, id);
		//mGfxEngine->mouseReleased(e, id);
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
	  //mLog->debugStream() << " LTE : " << lTimeElapsed;
		//mEvtMgr->update();
		//processEvents();
		
		/*for (_itrEngines = mEngines.begin();
		     _itrEngines != mEngines.end();
		     ++_itrEngines)
		    (*_itrEngines)->update(lTimeElapsed);*/
    
	}

  
  bool Intro::areFxEnabled() { return true; }
  bool Intro::areSfxEnabled() { return true; }
  void Intro::dontUpdateMe(Engine* inEngine) {
    mEngines.remove(inEngine);
  };
  
  void Intro::setupWidgets()
	{
		mTrayMgr->destroyAllWidgets();

		// create main navigation tray
		mTrayMgr->showLogo(TL_BOTTOM);
		mTrayMgr->createSeparator(TL_BOTTOM, "LogoSep");
		mTrayMgr->createButton(TL_BOTTOM, "Resume", "Resume");
		//mTrayMgr->createButton(TL_RIGHT, "UnloadReload", mLoadedSamples.empty() ? "Reload Samples" : "Unload Samples");
		mTrayMgr->createButton(TL_BOTTOM, "Configure", "Configure");
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
		
  void Intro::buttonHit(Button* b) {
		if (b->getName() == "Configure")   // enter configuration screen
		{
			mTrayMgr->removeWidgetFromTray("Resume");
			//mTrayMgr->removeWidgetFromTray("UnloadReload");
			mTrayMgr->removeWidgetFromTray("Configure");
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

			mTrayMgr->moveWidgetToTray("Resume", TL_BOTTOM);
			//mTrayMgr->moveWidgetToTray("UnloadReload", TL_RIGHT);
			mTrayMgr->moveWidgetToTray("Configure", TL_BOTTOM);
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
		else if (b->getName() == "Resume") {
		  return GameManager::getSingleton().popState();
		} else if (b->getName() == "Quit") {
		  return this->requestShutdown();
		}
  };
  
  void Intro::itemSelected(SelectMenu* menu) {
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
	void Intro::reconfigure(const Ogre::String& renderer, Ogre::NameValuePairList& options)
	{

		//SampleContext::reconfigure(renderer, options);
	}  
} // end of namespace
