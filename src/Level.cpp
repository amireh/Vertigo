#include "Level.h"
#include "EventManager.h"
#include "StatePause.h"
#include <cstdlib>
#include <ctime> 
#include "Intro.h"

using namespace Ogre;
namespace Pixy
{
	
	Level* Level::mLevel;
	
	GAME_STATE Level::getId() const { return STATE_GAME; }
	
	void Level::enter( void ) {
		
		srand((unsigned)time(0));
		
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Level");
		
		mEvtMgr = EventManager::getSingletonPtr();
		
		mEngines.clear();
		
		// init engines
		mGfxEngine = GfxEngine::getSingletonPtr();
		mPhyxEngine = PhyxEngine::getSingletonPtr();
		mSfxEngine = SfxEngine::getSingletonPtr();
		//mUIEngine = UIEngine::getSingletonPtr();
		
		mLog->debugStream() << "igniting engines";
		
		mEngines.push_back(mGfxEngine);
		mEngines.back()->setup();
		
		mEngines.push_back(mPhyxEngine);
		mEngines.back()->setup();
		
		mEngines.push_back(mSfxEngine);
		mEngines.back()->setup();		
		
		/*mEngines.push_back(mUIEngine);
		mEngines.back()->setup();*/

    mLog->debugStream() << "creating sphere";
    // prepare our sphere
		mSphere = new Sphere();
		mSphere->live();

    // create our level
    mZone = new Zone("dante_afterlife.vzs");
    mZone->engage();
    /*nrTunnels = 4;
    mTunnels.push_back(new Tunnel("Vertigo/Tunnel/Rose"));
    mTunnels.push_back(new Tunnel("Vertigo/Tunnel/Inferno",30));
    mTunnels.push_back(new Tunnel("Vertigo/Tunnel/Lava/Translucent",40));
    mTunnels.push_back(new Tunnel("Vertigo/Tunnel/Lava",20));*/

    //mTunnel = mTunnels.front();
    //mTunnel->show();
  
    // now prepare our obstacles
		nrObstacles = 12;
		nrMaxAliveObstacles = 3;
		
		mSpawnTimer = 900;
		mSpawningThreshold = mSpawnTimer / 2;
		
		for (int i =0; i < nrObstacles; ++i)
	    mObstaclePool.push_back(new Obstacle());
		
		fSpawning = true;
		fGameOver = fGameStarted = false;
    fGameStarted = true;
    
    // event handlers
    bindToName("PlayerWon", this, &Level::evtPlayerWon);
		bindToName("SphereDied", this, &Level::evtSphereDied);
		bindToName("PortalEntered", this, &Level::evtPortalEntered);
		bindToName("PortalReached", this, &Level::evtPortalReached);
		bindToName("PortalSighted", this, &Level::evtPortalSighted);

		for (_itrEngines = mEngines.begin();
		     _itrEngines != mEngines.end();
		     ++_itrEngines)
		    (*_itrEngines)->deferredSetup();
		
		mTimer.reset();

    mUpdater = &Level::updateGame;
    		
		mLog->infoStream() << "Initialized successfully.";
		
	}


	
	void Level::exit( void ) {
	  
	  
	  
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
		
		delete mZone;
		delete mSphere;
		
		mEngines.clear();
		
		bool fShuttingDown = GameManager::getSingleton().shuttingDown();
		
		/*mUIEngine->cleanup();
		//if (fShuttingDown)
		  delete mUIEngine;*/
				     
		mSfxEngine->cleanup();
		//if (fShuttingDown)
		  delete mSfxEngine;
		
		mPhyxEngine->cleanup();
		//if (fShuttingDown)
  		delete mPhyxEngine;
		
		mGfxEngine->cleanup();
		//if (fShuttingDown)
  		delete mGfxEngine;
		
		//EventManager::shutdown();
		
		mLog->infoStream() << "---- Exiting Level State ----";
		delete mLog;
		mLog = 0;
		 
	}
	
	void Level::keyPressed( const OIS::KeyEvent &e )
	{
	
	  //mUIEngine->keyPressed( e );
	
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
		
		//mUIEngine->keyReleased( e );
		// start the game when a key is released
		/*if (!fGameStarted) {
		  fGameStarted = true;
		  mEvtMgr->hook(mEvtMgr->createEvt("GameStarted"));
		  return;
		}*/
		if (e.key == OIS::KC_ESCAPE) {
		  //return this->requestShutdown();
		  return GameManager::getSingleton().changeState(Intro::getSingletonPtr());
		}
		if (!fGameStarted)
		  return;
		
		//mUISystem->injectKeyUp(e.key);
		mGfxEngine->keyReleased(e);
		mSphere->keyReleased(e);
		switch (e.key) {
		  case OIS::KC_P:
		    GameManager::getSingleton().pushState(StatePause::getSingletonPtr());
		    break;
		  case OIS::KC_M:
		    mSfxEngine->toggleAudioState();
		    break;
			case OIS::KC_EQUALS:
				
				break;			

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
	}
	
	void Level::resume( void ) {
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
    }
    
    //mUIEngine->update(lTimeElapsed);
  };

  void Level::updateGameOver(unsigned long lTimeElapsed) {
		for (_itrEngines = mEngines.begin();
		     _itrEngines != mEngines.end();
		     ++_itrEngines)
		    (*_itrEngines)->update(lTimeElapsed);  
  };
  
  void Level::updateGame(unsigned long lTimeElapsed) {

		/*for (_itrEngines = mEngines.begin();
		     _itrEngines != mEngines.end();
		     ++_itrEngines)
		    (*_itrEngines)->update(lTimeElapsed);*/
		
		//mLog->debugStream() << "updating sfx";
		mSfxEngine->update(lTimeElapsed);
		//mLog->debugStream() << "updating gfx";
		mGfxEngine->update(lTimeElapsed);
		//mLog->debugStream() << "updating ui";
		//mUIEngine->update(lTimeElapsed);
		//mLog->debugStream() << "updating phyx";
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
		
		if (fSpawning && mTimer.getMilliseconds() > mSpawnTimer && mObstacles.size() <= nrMaxAliveObstacles) {
		  //
		  if (rand() % 3 == 0)
		    spawnDuette();
		  else
		    spawnObstacle(CHASE);
		    
		  mTimer.reset();
		}  
  };
  
	void Level::update( unsigned long lTimeElapsed ) {
	  //mLog->debugStream() << " LTE : " << lTimeElapsed;
		mEvtMgr->update();
		processEvents();
		
		updateGame(lTimeElapsed);
    //(this->*mUpdater)(lTimeElapsed);
    
	}
	
  Obstacle* Level::spawnObstacle(OBSTACLE_CLASS inClass) {
    //mLog->debugStream() << "obstacle is pulled from the pool";
    
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
	  
	  Event* evt = mEvtMgr->createEvt("ObstacleAlive");
	  evt->setAny((void*)mObs);
	  mEvtMgr->hook(evt);
	  
	  return mObs;  
  }
  
  void Level::spawnDuette() {
    Obstacle *mObs = NULL;
    SHIELD lastShield = (rand() % 2 == 0) ? FIRE : ICE;
    for (int i=0;i<2;++i) {
      mObs = spawnObstacle(DUMB);
      if (!mObs)
        break;

      // set one of them to go left and the other to the right
      mObs->setDirection(Vector3((i == 0) ? 0.5f : -0.5f,-1,-1));
      mObs->setShield(lastShield);
      
      // swap shields
      lastShield = (lastShield == FIRE) ? ICE : FIRE;
    }
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
    

    
    //fSpawning = false;
    //mSphere->getRigidBody()->clearForces();
    //mSphere->getRigidBody()->setLinearVelocity(btVector3(0,0,0));
    mSphere->setMaxSpeed(0.0f);
    //mLog->infoStream() << "Portal is reached, reducing velocity";
    
		std::list<Obstacle*>::iterator _itr;
		for (_itr = mObstacles.begin(); 
		     _itr != mObstacles.end();)
		{      
      Obstacle *mObs = *_itr;
      ++_itr;
      mObs->die();
      releaseObstacle(mObs);
      continue;
		}
		
		// raise the speed of all obstacles
		for (_itr = mObstaclePool.begin(); 
		     _itr != mObstaclePool.end();
		     ++_itr)
		{ 
		  (*_itr)->setMaxSpeed((*_itr)->getMaxSpeed() + (*_itr)->getMaxSpeed() * 0.25f);
	  }
		// teleport the player to the second tunnel after 5 secs from sighting the portal
		//if (mTimer.getMilliseconds() > 5000) {
		  
		  
/*		  if (mTunnel == mTunnels.back()) {
		    // the player has won
		    mEvtMgr->hook(mEvtMgr->createEvt("PlayerWon"));
		    fGameOver = true;
		    return true;
		    //mTunnel = mTunnels.front();
		  }
		  else {
		    mTunnel->hide();
		    for (std::list<Tunnel*>::iterator _itr = mTunnels.begin();
		         _itr != mTunnels.end();
		         ++_itr) {
		      if ((*_itr) == mTunnel)
		        mTunnel = *(++_itr);   // this is our next tunnel  
		    }
		  }
		  
		  // relocate the sphere
		  
		  Vector3 pos = mTunnel->getNode()->getPosition();
		  btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(pos.x,pos.y+35,pos.z));
		  mSphere->getRigidBody()->proceedToTransform(trans);
		  mSphere->getMasterNode()->setPosition(pos);
		  mLog->debugStream() << "relocating sphere to " << pos.x << ", " << pos.y << ", " << pos.z;
		  // and show the shizzle
		  mTunnel->show();*/
		  
		  
		  return true;
		//};
    return false;
  };
  
  bool Level::evtPortalSighted(Event* inEvt) {
    fSpawning = false;
    //mSphere->getRigidBody()->clearForces();
    //mSphere->getRigidBody()->setLinearVelocity(btVector3(0,0,0));
    //mSphere->setMaxSpeed(0.0f);
    mTimer.reset();
    mLog->infoStream() << "Portal is in sight";
    
    return true;
  };
  
  Tunnel* Level::getTunnel() {
    //return mTunnel;
    return mZone->currentTunnel();
  };
  
  bool Level::areFxEnabled() { return true; }
  bool Level::areSfxEnabled() { return true; }
  
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
    mUpdater = &Level::updateGameOver;
    //if (!fGameOver) {
    //  mTimer.reset();
      fGameOver = true;
    return true;     
    //}
    /*
    if (mTimer.getMilliseconds() < 1000)
      return false;
    else {
      mLog->debugStream() << "game is over!";
      
      //GameManager::getSingleton().pushState(StatePause::getSingletonPtr());
      return true;
    }*/
  };
  
  bool Level::evtPlayerWon(Event* inEvt) {
    fGameOver = true;
    return true;
  };
 
  bool Level::isGameOver() { return fGameOver; };
  void Level::increaseSpawnRate(const int inFactor) {
    // increase the rate of obstacle spawning
    mSpawnTimer -= mSpawningThreshold / inFactor;
    if (mSpawnTimer <= mSpawningThreshold)
      mSpawnTimer = mSpawningThreshold; // shouldn't be faster than this, really  
  };
  int Level::currentSpawnRate() const {
    return mSpawnTimer;
  };
} // end of namespace
