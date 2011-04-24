#include "Level.h"
#include "EventManager.h"
#include "Pause.h"
#include <cstdlib>
#include <ctime> 
#include "Intro.h"

using namespace Ogre;
namespace Pixy
{
	
	Level* Level::mLevel;
	
	GAME_STATE Level::getId() const { return STATE_GAME; }
	
	Level::Level() {
	  fRunning = false;
	  fScreenshotTaken = false; // see Level::updateNothing() for info on this
	};
	
	void Level::enter( void ) {
		
		fRunning = true;
		
		srand((unsigned)time(0));
		
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Level");
		
		mEvtMgr = EventManager::getSingletonPtr();
		
		//mEngines.clear();
		
		// init engines
		mGfxEngine = GfxEngine::getSingletonPtr();
		mPhyxEngine = PhyxEngine::getSingletonPtr();
		mSfxEngine = SfxEngine::getSingletonPtr();
		mUIEngine = UIEngine::getSingletonPtr();
		
		mLog->debugStream() << "igniting engines";
		
		mEngines.push_back(mGfxEngine);
		mEngines.back()->setup();
		
		mEngines.push_back(mPhyxEngine);
		mEngines.back()->setup();
		
		mEngines.push_back(mSfxEngine);
		mEngines.back()->setup();		
		
		mEngines.push_back(mUIEngine);
		mUIEngine->setup();
		//mUIEngine->hide();
		
		/*mEngines.push_back(mUIEngine);
		mEngines.back()->setup();*/

    fFirstZone = true;
    
    mZone = new Zone(Intro::getSingleton().getSelectedZone()->filePath());
    
    mLog->debugStream() << "creating sphere";
    // prepare our sphere
		mSphere = new Sphere();
		mSphere->live();

    // create our level
    //mZone = new Zone(Intro::getSingleton().getSelectedZone());
    
    
    // now prepare our obstacles
		nrObstacles = 12;
		for (int i =0; i < nrObstacles; ++i)
	    mObstaclePool.push_back(new Obstacle());
		
    // event handlers
    bindToName("ZoneEntered", this, &Level::evtZoneEntered);
    bindToName("PlayerWon", this, &Level::evtPlayerWon);
		bindToName("SphereDied", this, &Level::evtSphereDied);
		bindToName("PortalEntered", this, &Level::evtPortalEntered);
		bindToName("PortalReached", this, &Level::evtPortalReached);
		bindToName("PortalSighted", this, &Level::evtPortalSighted);
		bindToName("TakingScreenshot", this, &Level::evtTakingScreenshot);
		bindToName("ScreenshotTaken", this, &Level::evtScreenshotTaken);

		for (_itrEngines = mEngines.begin();
		     _itrEngines != mEngines.end();
		     ++_itrEngines)
		    (*_itrEngines)->deferredSetup();
		
		
		
		//mTimer.reset();

    mUpdater = &Level::updatePreparation;
    
        		
		mLog->infoStream() << "Initialized successfully.";
		reset();
	}


	
	void Level::exit( void ) {
	  
	  
	  fRunning = false;
	  
		for (std::list<Obstacle*>::iterator _itr = mObstaclePool.begin(); 
		     _itr != mObstaclePool.end();
		     ++_itr) {
		    //mLog->debugStream() << "updating objects";
		  delete *_itr;
		}

		/*for (std::list<Tunnel*>::iterator _itr = mTunnels.begin(); 
		     _itr != mTunnels.end();
		     ++_itr) {
		    //mLog->debugStream() << "updating objects";
		  delete *_itr;
		}*/
				
		mObstacles.clear();
		//mTunnels.clear();
		
		mLog->infoStream() << "---- Exiting Level State ----";
		
		delete mZone;
		delete mSphere;
		
		mEngines.clear();
		
		bool fShuttingDown = GameManager::getSingleton().shuttingDown();
		
		mUIEngine->cleanup();
		//if (fShuttingDown)
		//  delete mUIEngine;
				     
		mSfxEngine->cleanup();
		//if (fShuttingDown)
		//  delete mSfxEngine;
		
		mPhyxEngine->cleanup();
		//if (fShuttingDown)
  	//	delete mPhyxEngine;
		
		mGfxEngine->cleanup();
		//if (fShuttingDown)
  	//	delete mGfxEngine;
		
		//EventManager::shutdown();
		
		
		
		mUIEngine->_refit(Intro::getSingletonPtr());
		
		mGfxEngine = 0;
		mPhyxEngine = 0;
		mSfxEngine = 0;
		mUIEngine = 0;
		
		mLog->infoStream() << "---- Exiting Level State ----";
		delete mLog;
		mLog = 0;
		 
	}
	
	void Level::keyPressed( const OIS::KeyEvent &e )
	{
	
	  mUIEngine->keyPressed( e );
	
		if (!fGameStarted)
		  return;	
		/*mUISystem->injectKeyDown(e.key);
		mUISystem->injectChar(e.text);*/
		mGfxEngine->keyPressed(e);
		mSphere->keyPressed(e);
		
		switch (e.key) {
		}
	}
	
	void Level::keyReleased( const OIS::KeyEvent &e ) {
		
		mSfxEngine->keyReleased(e);
		mUIEngine->keyReleased( e );
		// start the game when a key is released
		if (!fGameStarted) {
		  if ( e.key == OIS::KC_RETURN) {
		    fGameStarted = true;
		    mEvtMgr->hook(mEvtMgr->createEvt("GameStarted"));
		  }
		  return;
		}
		
		if (e.key == OIS::KC_ESCAPE) {
		  //return this->requestShutdown();
		  return GameManager::getSingleton().pushState(Intro::getSingletonPtr());
		}
		
		//mUISystem->injectKeyUp(e.key);
		mGfxEngine->keyReleased(e);
		mSphere->keyReleased(e);
		switch (e.key) {
		  case OIS::KC_P:
		    GameManager::getSingleton().pushState(Pause::getSingletonPtr());
		    break;
			case OIS::KC_EQUALS:
				
				break;	
			/*case OIS::KC_G:
			  mEvtMgr->hook(mEvtMgr->createEvt("SphereDied"));
			  mSphere->die();
			  break;
			case OIS::KC_H:
			  mEvtMgr->hook(mEvtMgr->createEvt("PlayerWon"));
			  break;		*/

		}
		
	}
	
	void Level::mouseMoved( const OIS::MouseEvent &e )
	{
		//mUIEngine->mouseMoved(e);
		//mSphere->mouseMoved(e);
		mGfxEngine->mouseMoved(e);
	}
	
	void Level::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
    //mUIEngine->mousePressed(e, id);
		mGfxEngine->mousePressed(e, id);
	}
	
	void Level::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		//mUIEngine->mouseReleased(e, id);
		mGfxEngine->mouseReleased(e, id);
	}
	
	void Level::pause( void ) {
	  mLog->infoStream() << "---- Pausing ----";
	  this->_hideEverything();
	}
	
	void Level::resume( void ) {
	  mLog->infoStream() << "---- Resuming ----";
	  if (!fGameOver)
	    this->_showEverything();
	}
		
	Level* Level::getSingletonPtr( void ) {
		if( !mLevel )
		    mLevel = new Level();
		
		return mLevel;
	}
	
	Level& Level::getSingleton( void ) {
		return *getSingletonPtr();
	}
	
	Sphere* Level::getSphere() { return mSphere; };

  void Level::updatePreparation(unsigned long lTimeElapsed) {
    if (fGameStarted) {
      mUpdater = &Level::updateGame;
      fSpawning = true;
    }
    
    mUIEngine->update(lTimeElapsed);
    mGfxEngine->update(lTimeElapsed);
    mPhyxEngine->update(lTimeElapsed);
    mSfxEngine->update(lTimeElapsed);
    mSphere->update(lTimeElapsed);
    //mZone->update(lTimeElapsed);
  };

  void Level::updateGameOver(unsigned long lTimeElapsed) {
		/*for (_itrEngines = mEngines.begin();
		     _itrEngines != mEngines.end();
		     ++_itrEngines)
		    (*_itrEngines)->update(lTimeElapsed);  */
		mUIEngine->update(lTimeElapsed);
		mSfxEngine->update(lTimeElapsed);
		mGfxEngine->update(lTimeElapsed);
		mSphere->update(lTimeElapsed);
		mZone->update(lTimeElapsed);
  };
  
  void Level::updateGame(unsigned long lTimeElapsed) {

		/*for (_itrEngines = mEngines.begin();
		     _itrEngines != mEngines.end();
		     ++_itrEngines)
		    (*_itrEngines)->update(lTimeElapsed);*/
		
		mSfxEngine->update(lTimeElapsed);
		mGfxEngine->update(lTimeElapsed);
		mUIEngine->update(lTimeElapsed);
		mPhyxEngine->update(lTimeElapsed);

    mZone->update(lTimeElapsed);
		mSphere->update(lTimeElapsed);
		
		//std::list<Obstacle*>::iterator _itr;
		for (_itrObstacles = mObstacles.begin(); 
		     _itrObstacles != mObstacles.end();
		     ) {
		    //mLog->debugStream() << "updating objects";
		    // release the dead obstacles
		    if ((*_itrObstacles)->dead()) {
		      
		      Obstacle *mObs = *_itrObstacles;
		      ++_itrObstacles;
		      releaseObstacle(mObs);
		      continue;
		    };
		  // and update the living ones	  
		  (*_itrObstacles)->update(lTimeElapsed);
		  ++_itrObstacles;
		}
		
		if (fSpawning && mTimer.getMilliseconds() > mSpawnTimer) {
		  //
		  spawnObstacle(mZone->getSettings().mRegisteredObstacleClasses[rand() % mZone->getSettings().mRegisteredObstacleClasses.size()]);
		  /*if (rand() % 3 == 0)
		    spawnDuette();
		  else
		    spawnObstacle(CHASE);*/
		    
		  mTimer.reset();
		}  
  };
  
  void Level::updateNothing(unsigned long lTimeElapsed) {
    /* WELCOME TO THE UGLIEST HACK OF MY LIFE
     * i'm gonna write this so i can remember how to refactor it later:
     *
     * basically, taking a screenshot will lock the game out since we're not threaded
     * and will cause a huge frame burst causing everything to go out of sync;
     * resetting the frame timer in GameManager seems too much interference for good design
     * and it's a very costly operation
     *
     * so what happens is: 
     *  1) GfxEngine gets key event to take screenie
     *  2) GfxEngine emits event "TakingScreenshot"
     *  3) Level handles the event and:
     *    a) sets the updater to &Level::updateNothing
     *    b) sets fScreenshotTaken = false
     *    c) manually updates GfxEngine with 0 time elapsed, to make it handle the event
     *  4) GfxEngine handles the event and actually takes the screenshot
     *  5) GfxEngine emits event "ScreenshotTaken"
     *  6) Level handles the event and sets fScreenshotTaken = true
     *  NOTE: we do NOT set back the updater immediately, because that would defeat the whole point
     *  7) INSIDE Level::updateNothing(), it will be updated with a huge lTimeElapsed
     *     and will redirect the updater back to Level::updateGame()
     *  8) Screenie taken + world consistent = PROFIT
    */
    if (fScreenshotTaken)
      mUpdater = &Level::updateGame;
  };
  
	void Level::update( unsigned long lTimeElapsed ) {
	  //mLog->debugStream() << " LTE : " << lTimeElapsed;
		mEvtMgr->update();
		processEvents();
		
    (this->*mUpdater)(lTimeElapsed);
    
	}
	
  Obstacle* Level::spawnObstacle(OBSTACLE_CLASS inClass) {
    //mLog->debugStream() << "obstacle is pulled from the pool";
    
    if (inClass == DUETTE) {
      spawnDuette();
      return NULL;
    }
      
    Obstacle* mObs = NULL;// = mObstaclePool.front();
    
		std::list<Obstacle*>::iterator _itr;
		for (_itr = mObstaclePool.begin(); 
		     _itr != mObstaclePool.end();
		     ++_itr) {
		    //mLog->debugStream() << "updating objects";
		    if ((*_itr)->dead()) {
		      mObs = *_itr;
		      break;
		    };    
		}
		
		if (!mObs)
		  return NULL;
		
		mObs->live();
		mObs->setClass(inClass);
	  mObstacles.push_back(mObs);
	  
	  /*Event* evt = mEvtMgr->createEvt("ObstacleAlive");
	  evt->setAny((void*)mObs);
	  mEvtMgr->hook(evt);*/
	  
	  return mObs;  
  }
  
  void Level::spawnDuette() {
    Obstacle *mObs[2];
    SHIELD lastShield = (rand() % 2 == 0) ? FIRE : ICE;
    for (int i=0;i<2;++i) {
      mObs[i] = spawnObstacle(DUMB);
      mObs[i]->setClass(DUETTE);
      if (!mObs[i])
        break;

      // set one of them to go left and the other to the right
      mObs[i]->setDirection(Vector3((i == 0) ? 0.6f : -0.6f,-1,-1));
      mObs[i]->setShield(lastShield);
      
      // swap shields
      lastShield = (lastShield == FIRE) ? ICE : FIRE;
    }
    mObs[0]->setDuetteTwin(mObs[1]);
    mObs[1]->setDuetteTwin(mObs[0]);
  };
  
  void Level::releaseObstacle(Obstacle* inObs) {
    //mLog->debugStream() << "obstacle is released into the pool";
    mObstacles.remove(inObs);
    //inObs->die();
    //mObstaclePool.push_back(inObs);
  }
  
  bool Level::evtPortalEntered(Event* inEvt) {
    
    
    mTimer.reset();
	  fSpawning = true;
		
    return true;
  };
  
  bool Level::evtPortalReached(Event* inEvt) {
    
    //if (mZone->getSettings().fResetVelocity)
      
      //mSphere->setMaxSpeed(mSphere->getMoveSpeed());
    //mLog->infoStream() << "Portal is reached, reducing velocity";
    
		return true;

  };
  
  bool Level::evtPortalSighted(Event* inEvt) {
    fSpawning = false;
    //mSphere->getRigidBody()->clearForces();
    //mSphere->getRigidBody()->setLinearVelocity(btVector3(0,0,0));
    //mSphere->setMaxSpeed(0.0f);
    
    // clean up obstacles
		std::list<Obstacle*>::iterator _itr;
		/*for (_itr = mObstacles.begin(); 
		     _itr != mObstacles.end();)
		{
      Obstacle *mObs = *_itr;
      ++_itr;
      mObs->die();
      releaseObstacle(mObs);
      continue;
		}*/
		
		// raise the speed of all obstacles
		for (_itr = mObstaclePool.begin(); 
		     _itr != mObstaclePool.end();
		     ++_itr)
		{ 
		  (*_itr)->setMaxSpeed((*_itr)->getMaxSpeed() + (*_itr)->getMaxSpeed() * mZone->getSettings().mMaxSpeedStep);
	  }    
    
    mLog->infoStream() << "Portal is in sight";
    
    return true;
  };
  
  Tunnel* Level::getTunnel() {
    //return mTunnel;
    return mZone->currentTunnel();
  };
  
  bool Level::areFxEnabled() { return GameManager::getSingleton().getSettings()["Visual Detail"] != "Low"; }
  bool Level::areSfxEnabled() { return GameManager::getSingleton().getSettings()["Sound Enabled"] == "Yes"; }
  
  void Level::dontUpdateMe(Engine* inEngine) {
    mEngines.remove(inEngine);
  };
  
  Obstacle* Level::lastObstacleAlive() {
    
    return (!mObstacles.empty()) ? mObstacles.back() : NULL;
  };
  
  const std::list<Obstacle*>& Level::getObstacles() {
    return mObstacles;
  };
  
  bool Level::evtSphereDied(Event* inEvt) {
    //if (!fGameOver) {
    //  mTimer.reset();
    fGameOver = true;
    mUpdater = &Level::updateGameOver;
    return true;     
    //}
    /*
    if (mTimer.getMilliseconds() < 1000)
      return false;
    else {
      mLog->debugStream() << "game is over!";
      
      //GameManager::getSingleton().pushState(Pause::getSingletonPtr());
      return true;
    }*/
  };
  
  bool Level::evtPlayerWon(Event* inEvt) {
    mUpdater = &Level::updateGameOver;
    fGameOver = true;
    
    _hideEverything();
    
    return true;
  };
 
  bool Level::isGameOver() { return fGameOver; };
  
  // TODO: FIX THIS, completely wrong!!
  void Level::increaseSpawnRate(const int inFactor) {
    // increase the rate of obstacle spawning
    mSpawnTimer -= mSpawningThreshold / inFactor;
    if (mSpawnTimer <= mSpawningThreshold)
      mSpawnTimer = mSpawningThreshold; // shouldn't be faster than this, really  
  };
  int Level::currentSpawnRate() const {
    return mSpawnTimer;
  };
  
  bool Level::evtZoneEntered(Event* inEvt) {
    
    mLog->infoStream() << " ----- entered " << mZone->name() << " zone ----- ";
    mSfxEngine->playMusic();
    mZone->engage();
    mUIEngine->_refit(this);
    //reset();
    
    return true;
  };
  void Level::reset() {
    if (!fRunning)
      return;
    
    mLog->infoStream() << "loading a new zone, resetting the game";
    //mEvtMgr->_clearQueue();
    
    
    // this class is the one who emits the first "ZoneEntered" event and thus
    // does not have to handle it
    if (!fFirstZone) {

      mZone->disengage();
      delete mZone;
      
      //mSphere->live();
      
      // load the new zone
      mZone = new Zone(Intro::getSingleton().getSelectedZone()->filePath());
      
      
      
      // kill our obstacles
      mLog->debugStream() << "I have " << mObstacles.size() << " obstacles alive, killng em";
		  std::list<Obstacle*>::iterator _itr;
		  for (_itr = mObstaclePool.begin(); 
		       _itr != mObstaclePool.end();)
		  {      
        Obstacle *mObs = *_itr;
        //++_itr;
        mObs->die();
        ++_itr;
        //releaseObstacle(mObs);
		  }
		  
		  mObstacles.clear();
    }
    
    nrMaxAliveObstacles = mZone->getSettings().mObstacleCap;
    mSpawnTimer = mZone->getSettings().mSpawnRate;
		mSpawningThreshold = mSpawnTimer / 2;
		
    mUpdater = &Level::updatePreparation;
    fSpawning = false;
    fGameStarted = false;
    fGameOver = false;
    
    mLog->infoStream() << "engaging zone " << mZone->name();
    //mZone->engage();
    
    //mTimer.reset();
    
    Event* mEvt = mEvtMgr->createEvt("ZoneEntered");
    //mEvt->setProperty("Path", Intro::getSingleton().getSelectedZone());
    //mEvt->setProperty("FirstZone", "True");
    mEvtMgr->hook(mEvt);  
    
    fFirstZone = false;  
  };
  
  Zone* Level::currentZone() {
    return mZone;
  };
  
  void Level::_hideEverything() {
    // hide our tunnel, obstacles, and sphere
    mSphere->getMasterNode()->setVisible(false);
	  std::list<Obstacle*>::iterator _itr;
	  for (_itr = mObstaclePool.begin(); 
	       _itr != mObstaclePool.end();
	       ++_itr)
      (*_itr)->getMasterNode()->setVisible(false);
      
    mZone->currentTunnel()->getNode()->setVisible(false);
  };
  
  void Level::_showEverything() {
    mSphere->getMasterNode()->setVisible(true);
	  std::list<Obstacle*>::iterator _itr;
	  for (_itr = mObstaclePool.begin(); 
	       _itr != mObstaclePool.end();
	       ++_itr)
      (*_itr)->getMasterNode()->setVisible(true);
    
    // in the case where the player is switching to a new zone, and was already playing one
    // (Level is running), _showEverything() will be called on GameState::resume()
    // and since our zone will not be engaged yet, the tunnel will never be shown anyway
    // that's why we chekc if the tunnel exists.. otherwise, this applies to pausing / resuming the game
    // which happens by toggling the menu
    if (mZone->currentTunnel())
      mZone->currentTunnel()->getNode()->setVisible(true);  
  };
  
  bool Level::evtTakingScreenshot(Event* inEvt) {
    mLog->debugStream() << "gfxengine says it's taking a screenie";
    
    mUpdater = &Level::updateNothing;
    
    mGfxEngine->update(0);
    
    fScreenshotTaken = false;
    
    return true;
  };
  
  bool Level::evtScreenshotTaken(Event* inEvt) {
    mLog->debugStream() << "gfxengine says it's done taking a screenie";
    
    fScreenshotTaken = true;
    //mUpdater = &Level::updateGame;
    
    return true;
  };
} // end of namespace
