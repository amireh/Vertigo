/*
 *  This file is part of Vertigo.
 *
 *  Vertigo - a cross-platform arcade game powered by Ogre3D.
 *  Copyright (C) 2011 
 *    Philip Allgaier <spacegaier@ogre3d.org>, 
 *    Ahmad Amireh <ahmad@amireh.net>
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
 
#include "GameManager.h"
#include "GameState.h"
#include "Intro.h"
#include "Level.h"
#include "log4cpp/PixyLogLayout.h"

#include "UIEngine.h"
#include "GfxEngine.h"
#include "SfxEngine.h"
#include "PhyxEngine.h"
using namespace Ogre;

namespace Pixy
{
	GameManager* GameManager::mGameManager;
	
	GameManager::GameManager() :
	mRoot(NULL),
	mInputMgr(NULL),
	fShutdown(false) {
	}
	
	GameManager::~GameManager() {

		// Clean up all the states
		while( !mStates.empty() ) {
		    mStates.back()->exit();
		    mStates.pop_back();
		}

    mLog->infoStream() << "tearing down all engines";
    
    delete GfxEngine::getSingletonPtr();
    delete SfxEngine::getSingletonPtr();
    delete PhyxEngine::getSingletonPtr();
    delete UIEngine::getSingletonPtr();
    
		if( mInputMgr )
		    delete mInputMgr;
		

		if( mRoot )
		    delete mRoot;
		
		mLog->infoStream() << "++++++ Vertigo cleaned up successfully ++++++";
		if (mLog)
		  delete mLog;
		  
		log4cpp::Category::shutdown();
		mRoot = NULL; mInputMgr = NULL;
	}

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>
	
	// This function will locate the path to our application on OS X,
	// unlike windows you cannot rely on the current working directory
	// for locating your configuration files and resources.
	std::string macBundlePath()
	{
		char path[1024];
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		assert(mainBundle);
		
		CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
		assert(mainBundleURL);
		
		CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
		assert(cfStringRef);
		
		CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);
		
		CFRelease(mainBundleURL);
		CFRelease(cfStringRef);
		
		return std::string(path);
	}
#endif

  void GameManager::loadRenderSystems() 
  { 
    //Root* ogreRoot = Root::getSingletonPtr(); 
    bool rendererInstalled = false;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
/*    HRESULT hr; 
    DWORD dwDirectXVersion = 0; 
    TCHAR strDirectXVersion[10]; 

    hr = GetDXVersion( &dwDirectXVersion, strDirectXVersion, 10 ); 
    if( SUCCEEDED(hr) ) { 
      ostringstream dxinfoStream; 
      dxinfoStream << "DirectX version: " << strDirectXVersion; 
      //LogManager::getSingleton().logMessage(dxinfoStream.str());
      mLog->infoStream() << dxinfoStream.str();  
*/
      //if(dwDirectXVersion >= 0x00090000) { 
        try { 
          mRoot->loadPlugin("RenderSystem_Direct3D9"); 
          mRoot->setRenderSystem(mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem"));
          rendererInstalled = true;
        } catch (Ogre::Exception& e) {
          mLog->errorStream() << "Unable to create D3D9 RenderSystem: " << e.getFullDescription();
        } catch(std::exception& e) { 
          mLog->errorStream() << "Unable to create D3D9 RenderSystem: " << e.what(); 
        }
      //} 
//    } 
#endif
    try {
      std::string lPluginsPath;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
      lPluginsPath = macBundlePath() + "/Contents/Plugins/";
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      lPluginsPath = ".\\";
#else
      lPluginsPath = "./";
#endif
      mRoot->loadPlugin(lPluginsPath + "RenderSystem_GL");
      if (!rendererInstalled) {
        mRoot->setRenderSystem(mRoot->getRenderSystemByName("OpenGL Rendering Subsystem"));
        rendererInstalled = true;
      }
    } 
    catch(Ogre::Exception& e) { 
      mLog->errorStream() << "Unable to create OpenGL RenderSystem: " << e.getFullDescription(); 
    } 

    try { 
      mRoot->loadPlugin("./Plugin_CgProgramManager");  
    } 
    catch(Ogre::Exception& e) { 
      mLog->errorStream() << "Unable to create CG Program manager RenderSystem: " << e.getFullDescription(); 
    } 
  }
	void GameManager::startGame() {
		// init logger
		initLogger();
		using std::ostringstream;
		ostringstream lPathResources, lPathPlugins, lPathCfg, lPathOgreCfg, lPathLog;
#if OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS
		lPathResources << PROJECT_ROOT << PROJECT_RESOURCES << "\\config\\resources_win32.cfg";
    lPathPlugins << PROJECT_ROOT << PROJECT_RESOURCES << "\\config\\plugins.cfg";
    lPathCfg << PROJECT_ROOT << PROJECT_RESOURCES << "\\config\\";
		lPathOgreCfg << lPathCfg.str() << "ogre.cfg";	
		lPathLog << PROJECT_LOG_DIR << "\\Ogre.log";	
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		lPathResources << macBundlePath() << "/Contents/Resources/config/resources_osx.cfg";
		lPathPlugins << macBundlePath() << "/Contents/Resources/config/plugins.cfg";
		lPathCfg << macBundlePath() << "/Contents/Resources/config/";
		lPathOgreCfg << lPathCfg.str() << "ogre.cfg";	
		lPathLog << macBundlePath() << "/Contents/Logs/Ogre.log";    
#else
		lPathResources << PROJECT_ROOT << PROJECT_RESOURCES << "/config/resources_linux.cfg";
    lPathPlugins << PROJECT_ROOT << PROJECT_RESOURCES << "/config/plugins.cfg";
    lPathCfg << PROJECT_ROOT << PROJECT_RESOURCES << "/config/";
		lPathOgreCfg << lPathCfg.str() << "ogre.cfg";	
		lPathLog << PROJECT_LOG_DIR << "/Ogre.log";
#endif
    
    mConfigPath = lPathCfg.str();
    
    
		mRoot = OGRE_NEW Root(lPathPlugins.str(), lPathOgreCfg.str(), lPathLog.str());
		if (!mRoot) {
			throw Ogre::Exception( Ogre::Exception::ERR_INTERNAL_ERROR, 
								  "Error - Couldn't initalize OGRE!", 
								  "Vertigo - Error");
		}
		loadRenderSystems();
	  

		
		// Setup and configure game
		
		if( !this->configureGame() ) {
		    // If game can't be configured, throw exception and quit application
		    throw Ogre::Exception( Ogre::Exception::ERR_INTERNAL_ERROR,
								  "Error - Couldn't Configure Renderwindow",
								  "Vertigo - Error" );
		    return;
		}

	  // Setup input & register as listener
		mInputMgr = InputManager::getSingletonPtr();
		mRenderWindow = mRoot->getAutoCreatedWindow();
		mInputMgr->initialise( mRenderWindow );
		WindowEventUtilities::addWindowEventListener( mRenderWindow, this );
				
		this->setupResources(lPathResources.str());
		
		mInputMgr->addKeyListener( this, "GameManager" );
		mInputMgr->addMouseListener( this, "GameManager" );
		
		// Change to first state
		this->changeState( Intro::getSingletonPtr() );
		
		// lTimeLastFrame remembers the last time that it was checked
		// we use it to calculate the time since last frame
		
		lTimeLastFrame = 0;
		lTimeCurrentFrame = 0;
		lTimeSinceLastFrame = 0;

    mRoot->getTimer()->reset();
    
		// main game loop
		while( !fShutdown ) {
			
	    // calculate time since last frame and remember current time for next frame
	    lTimeCurrentFrame = mRoot->getTimer()->getMilliseconds();
	    lTimeSinceLastFrame = lTimeCurrentFrame - lTimeLastFrame;
	    lTimeLastFrame = lTimeCurrentFrame;
		
	    // update input manager
	    mInputMgr->capture();
		
	    // cpdate current state
	    mStates.back()->update( lTimeSinceLastFrame );

			WindowEventUtilities::messagePump();

			// render next frame
		  mRoot->renderOneFrame();

		}
		
		
	}
	
	
	bool GameManager::configureGame() {
				
		// Load config settings from ogre.cfg
		if( !mRoot->restoreConfig() ) {
		    // If there is no config file, show the configuration dialog
		    //if( !mRoot->showConfigDialog() ) {
		    //    return false;
		    //}
		}
		
		// Initialise and create a default rendering window
		mRenderWindow = mRoot->initialise( true, "Vertigo" );
		
		// Initialise resources
		//ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		
		// Create needed scenemanagers
		Ogre::SceneManager *mSceneMgr = mRoot->createSceneManager( ST_EXTERIOR_CLOSE, "LoadingScene" );
		Ogre::Camera *mCamera = mSceneMgr->createCamera("LoadingCamera");
		mRenderWindow->addViewport(mCamera, -1);
		
		return true;
	}
	
	void GameManager::setupResources(std::string inPath) {

		// Load resource paths from config file
		ConfigFile cf;
		cf.load( inPath );
		
		// Go through all settings in the file
		ConfigFile::SectionIterator itSection = cf.getSectionIterator();
		
		String sSection, sType, sArch;
		while( itSection.hasMoreElements() ) {
		    sSection = itSection.peekNextKey();
			
		    ConfigFile::SettingsMultiMap *mapSettings = itSection.getNext();
		    ConfigFile::SettingsMultiMap::iterator itSetting = mapSettings->begin();
		    while( itSetting != mapSettings->end() ) {
		        sType = itSetting->first;
		        sArch = itSetting->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
				ResourceGroupManager::getSingleton().addResourceLocation( String(macBundlePath() + "/" + sArch), sType, sSection);
#else
				ResourceGroupManager::getSingleton().addResourceLocation( sArch, sType, sSection);
#endif
				
		        ++itSetting;
		    }
		}
		// Initialise resources
		ResourceGroupManager::getSingleton().initialiseResourceGroup("Bootstrap");
		OgreBites::SdkTrayManager *mTrayMgr = 
		  new OgreBites::SdkTrayManager("Vertigo/UI/Loader", mRenderWindow, InputManager::getSingletonPtr()->getMouse(), 0);
		//mTrayMgr->showAll();
		mTrayMgr->showLoadingBar(1,1, 1);
		//ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
		this->loadConfig();
		mTrayMgr->hideLoadingBar();
		delete mTrayMgr;
		
		//mRenderWindow->removeViewport(-1);
		//mRoot->getSceneManager("LoadingScene")->destroyCamera("LoadingCamera");
		//mRoot->destroySceneManager(mRoot->getSceneManager("LoadingScene"));

	}
	
	void GameManager::changeState( GameState *inState ) {
		
		
		// Cleanup the current state
		if( !mStates.empty() ) {
		    mStates.back()->exit();
		    mStates.pop_back();
		}

		// Store and init the new state
		mCurrentState = inState;
		mStates.push_back( inState );
		mStates.back()->enter();
		
		// reset our frame timer to eliminate any frame bursts
		//mRoot->getTimer()->reset();
		//lTimeLastFrame = 0;
	}
	
	void GameManager::pushState( GameState *inState ) {
		
		// Pause current state
		if( !mStates.empty() ) {
		    mStates.back()->pause();
		}
		
		mCurrentState = inState;
		
		// Store and init the new state
		mStates.push_back( inState );
		mStates.back()->enter();
		
		// reset our frame timer to eliminate any frame bursts
		//mRoot->getTimer()->reset();
		//lTimeLastFrame = 0;
	}
	
	void GameManager::popState() {
		// Cleanup the current state
		if( !mStates.empty() ) {
		    mStates.back()->exit();
		    mStates.pop_back();
		}
		
		// Resume previous state
		if( !mStates.empty() ) {
		    mCurrentState = mStates.back();
		    mStates.back()->resume();
		}
		
		// reset our frame timer to eliminate any frame bursts
		//mRoot->getTimer()->reset();
		//lTimeLastFrame = 0;
	}
	
	void GameManager::requestShutdown() {
		fShutdown = true;
	}
	
	bool GameManager::keyPressed( const OIS::KeyEvent &e ) {
		// Call keyPressed of current state
		mStates.back()->keyPressed( e );
		
		return true;
	}
	
	bool GameManager::keyReleased( const OIS::KeyEvent &e ) {
		// Call keyReleased of current state
		mStates.back()->keyReleased( e );
		
		return true;
	}
	
	bool GameManager::mouseMoved( const OIS::MouseEvent &e ) {
		// Call mouseMoved of current state
		mStates.back()->mouseMoved( e );
		
		return true;
	}
	
	bool GameManager::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		// Call mousePressed of current state
		mStates.back()->mousePressed( e, id );
		
		return true;
	}
	
	bool GameManager::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		// Call mouseReleased of current state
		mStates.back()->mouseReleased( e, id );
		
		return true;
	}
	
	GameManager* GameManager::getSingletonPtr() {
		if( !mGameManager ) {
		    mGameManager = new GameManager();
		}
		
		return mGameManager;
	}
	
	GameManager& GameManager::getSingleton() {
		return *getSingletonPtr();
	}
	
	GameState* GameManager::currentState() {
		return mCurrentState;
	}
	
	void GameManager::initLogger() {


		std::string lLogPath = PROJECT_LOG_DIR;
#if OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS || defined(_WIN32)
		lLogPath += "\\Pixy.log";
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		lLogPath = macBundlePath() + "/Contents/Logs/Pixy.log";
#else
		lLogPath += "/Pixy.log";		
#endif	
		std::cout << "| Initting log4cpp logger @ " << lLogPath << "!\n";
		
		log4cpp::Appender* lApp = new 
		log4cpp::FileAppender("FileAppender",
							  lLogPath, false);
		
        log4cpp::Layout* lLayout = new Pixy::PixyLogLayout();
		/* used for header logging */
		PixyLogLayout* lHeaderLayout = new PixyLogLayout();
		lHeaderLayout->setVanilla(true);
		
		lApp->setLayout(lHeaderLayout);
		
		std::string lCatName = CLIENT_LOG_CATEGORY;
		log4cpp::Category* lCat = &log4cpp::Category::getInstance(lCatName);
		
        lCat->setAdditivity(false);
		lCat->setAppender(lApp);
		lCat->setPriority(log4cpp::Priority::DEBUG);

		lCat->infoStream() << "\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+";
		lCat->infoStream() << "\n+                                Vertigo v" << VERTIGO_VERSION << "                                +";
		lCat->infoStream() << "\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
		
		lApp->setLayout(lLayout);
		
		lApp = 0;
		lCat = 0;
		lLayout = 0;
		lHeaderLayout = 0;
		
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "GameManager");
	}
	
	bool GameManager::shuttingDown() { return fShutdown; };
	
	tPixySettings& GameManager::getSettings() {
	  return mSettings;
	};
	
	void GameManager::loadConfig() {
	  mLog->infoStream() << "loading Vertigo config";
	  
	  Ogre::ResourceGroupManager& mRGM = Ogre::ResourceGroupManager::getSingleton();
	  
	  if (mRGM.resourceExists("General", "vertigo.cfg")) {
	    mLog->infoStream() << "found existing configuration, parsing...";
	    
	    Ogre::ConfigFile *mCfg = new Ogre::ConfigFile();
	    mCfg->loadFromResourceSystem("vertigo.cfg", "General");
	    mSettings.insert(std::make_pair<std::string,std::string>(
	      "Visual Detail", 
	      mCfg->getSetting("Visual Detail", Ogre::StringUtil::BLANK, "Medium")
	    ));
	    
	    mSettings.insert(std::make_pair<std::string,std::string>(
	      "Music Enabled", 
	      mCfg->getSetting("Music Enabled", Ogre::StringUtil::BLANK, "Yes")
	    ));
	    
	    mSettings.insert(std::make_pair<std::string,std::string>(
	      "Sound Enabled", 
	      mCfg->getSetting("Sound Enabled", Ogre::StringUtil::BLANK, "Yes")
	    ));
	    
	    delete mCfg;
	    
	  } else {
	    // default values
	    mSettings.insert(std::make_pair<std::string,std::string>("Visual Detail", "Medium"));
	    mSettings.insert(std::make_pair<std::string,std::string>("Sound Enabled", "Yes"));
	    mSettings.insert(std::make_pair<std::string,std::string>("Music Enabled", "Yes"));
	  }
	  
    this->saveConfig();
	};
	
	void GameManager::saveConfig() {
	  mLog->infoStream() << "saving Vertigo config";
	  
	  std::string tConfigFilePath = mConfigPath + "vertigo.cfg";
	  std::ofstream of(tConfigFilePath.c_str());
	  if (!of) {
	    mLog->errorStream() << "could not write settings to " << tConfigFilePath << "!! aborting";
	    return;
	  }
	  
	  of << "Visual Detail=" << mSettings["Visual Detail"] << std::endl;
	  of << "Music Enabled=" << mSettings["Music Enabled"] << std::endl;
	  of << "Sound Enabled=" << mSettings["Sound Enabled"] << std::endl;
	  of.close();
	  
	};
	
	void GameManager::applyNewSettings(tPixySettings& inSettings) {
	  mSettings = inSettings;
	  this->saveConfig();
	  EventManager* mEvtMgr = EventManager::getSingletonPtr();
	  mEvtMgr->hook(mEvtMgr->createEvt("SettingsChanged"));
	};
} // end of namespace Pixy
