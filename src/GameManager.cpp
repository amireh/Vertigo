#include "GameManager.h"
#include "GameState.h"
#include "Level.h"
//#include "Combat.h"
#include "log4cpp/PixyLogLayout.h"
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
		
		if( mInputMgr )
		    delete mInputMgr;
		

		if( mRoot )
		    delete mRoot;
		
		log4cpp::Category::shutdown();
		mRoot = NULL; mInputMgr = NULL;
	}
	
	void GameManager::startGame() {
		// init logger
		initLogger();
		using std::ostringstream;
		ostringstream lPathResources, lPathPlugins, lPathOgreCfg, lPathLog;
#if OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS
		lPathResources << PROJECT_ROOT << PROJECT_RESOURCES << "\\config\\resources_win32.cfg";
    lPathPlugins << PROJECT_ROOT << PROJECT_RESOURCES << "\\config\\plugins.cfg";
		lPathOgreCfg << PROJECT_ROOT << PROJECT_RESOURCES << "\\config\\ogre.cfg";	
		lPathLog << PROJECT_LOG_DIR << "\\Ogre.log";	
#elsif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		lPathResources << PROJECT_ROOT << PROJECT_RESOURCES << "/config/resources_osx.cfg";
    lPathPlugins << PROJECT_ROOT << PROJECT_RESOURCES << "/config/plugins.cfg";
		lPathOgreCfg << PROJECT_ROOT << PROJECT_RESOURCES << "/config/ogre.cfg";	
		lPathLog << PROJECT_LOG_DIR << "/Ogre.log";    
#else
		lPathResources << PROJECT_ROOT << PROJECT_RESOURCES << "/config/resources_linux.cfg";
    lPathPlugins << PROJECT_ROOT << PROJECT_RESOURCES << "/config/plugins.cfg";
		lPathOgreCfg << PROJECT_ROOT << PROJECT_RESOURCES << "/config/ogre.cfg";	
		lPathLog << PROJECT_LOG_DIR << "/Ogre.log";
#endif

		mRoot = OGRE_NEW Root(lPathPlugins.str(), lPathOgreCfg.str(), lPathLog.str());
		if (!mRoot) {
			throw Ogre::Exception( Ogre::Exception::ERR_INTERNAL_ERROR, 
								  "Error - Couldn't initalize OGRE!", 
								  "Vertigo - Error");
		}
		
	
		// Setup and configure game
		this->setupResources(lPathResources.str());
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
		
		mInputMgr->addKeyListener( this, "GameManager" );
		mInputMgr->addMouseListener( this, "GameManager" );
		
		
		// Change to first state
		this->changeState( Level::getSingletonPtr() );
		
		// lTimeLastFrame remembers the last time that it was checked
		// we use it to calculate the time since last frame
		unsigned long lTimeLastFrame, lTimeCurrentFrame, lTimeSinceLastFrame;
		
		lTimeLastFrame = 0;
		lTimeCurrentFrame = 0;
		lTimeSinceLastFrame = 0;

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
		    if( !mRoot->showConfigDialog() ) {
		        return false;
		    }
		}
		
		// Initialise and create a default rendering window
		mRenderWindow = mRoot->initialise( true, "Vertigo" );
		
		// Initialise resources
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		
		// Create needed scenemanagers
		mRoot->createSceneManager( ST_EXTERIOR_CLOSE, "ST_EXTERIOR_CLOSE" );
		
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
				
		        ResourceGroupManager::getSingleton().addResourceLocation( sArch, sType, sSection );
				
		        ++itSetting;
		    }
		}
	}
	
	void GameManager::changeState( GameState *inState ) {
		
		
		// Cleanup the current state
		if( !mStates.empty() ) {
		    mStates.back()->exit();
		    mStates.pop_back();
		}

		mCurrentState = inState->getId();
		// Store and init the new state
		mStates.push_back( inState );
		mStates.back()->enter();
	}
	
	void GameManager::pushState( GameState *inState ) {
		
		// Pause current state
		if( !mStates.empty() ) {
		    mStates.back()->pause();
		}
		
		// Store and init the new state
		mStates.push_back( inState );
		mStates.back()->enter();
	}
	
	void GameManager::popState() {
		// Cleanup the current state
		if( !mStates.empty() ) {
		    mStates.back()->exit();
		    mStates.pop_back();
		}
		
		// Resume previous state
		if( !mStates.empty() ) {
		    mStates.back()->resume();
		}
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
	
	GAME_STATE GameManager::gameState() const {
		return mCurrentState;
	}
	
	void GameManager::initLogger() {


		std::string lLogPath = PROJECT_LOG_DIR;
#if OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS || defined(_WIN32)
		lLogPath += "\\Pixy.log";
#else
		lLogPath += "/Pixy.log";
#endif	
		//std::cout << "| Initting log4cpp logger @ " << lLogPath << "!\n";
		
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
	}
	
} // end of namespace Pixy
