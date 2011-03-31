#include "Level.h"
#include "EventManager.h"
#include "StatePause.h"
#include <cstdlib>
#include <ctime> 

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
		mSfxEngine = AudioEngine::getSingletonPtr();
		
		mEngines.push_back(mGfxEngine);
		mEngines.back()->setup();
				
		//mUIEngine = UIEngine::getSingletonPtr();
		//mUIEngine->setup();
		
		mEngines.push_back(mPhyxEngine);
		mEngines.back()->setup();
		
		mEngines.push_back(mSfxEngine);
		mEngines.back()->setup();
		
		
		// grab CEGUI handle
		//mUISystem = &CEGUI::System::getSingleton();
 
		mSphere = new Sphere();
		mSphere->live();
		
		nrObstacles = 10;
		nrTunnels = 1;
		//createObstacle();
		
		for (int i =0; i < nrObstacles; ++i)
	    mObstaclePool.push_back(new Obstacle());
		
		fSpawning = true;
		

		
		bindToName("PortalEntered", this, &Level::evtPortalEntered);
		bindToName("PortalReached", this, &Level::evtPortalReached);
		bindToName("PortalSighted", this, &Level::evtPortalSighted);

    //for (int i =0; i < nrTunnels; ++i) {
      
    //};
    mTunnels.push_back(new Tunnel("Vertigo/Tunnel/Rose"));
    mTunnels.push_back(new Tunnel("Vertigo/Tunnel/Inferno"));
    mTunnels.push_back(new Tunnel("Vertigo/Tunnel/Lava/Translucent"));
    mTunnels.push_back(new Tunnel("Vertigo/Tunnel/Lava"));
    
    mTunnel = mTunnels.front();
    mTunnel->show();

		for (_itrEngines = mEngines.begin();
		     _itrEngines != mEngines.end();
		     ++_itrEngines)
		    (*_itrEngines)->deferredSetup();
		  		
		mLog->infoStream() << "Initialized successfully.";
		
	}


	
	void Level::exit( void ) {
	  
	  
	  
		for (std::list<Obstacle*>::iterator _itr = mObstaclePool.begin(); 
		     _itr != mObstaclePool.end();
		     ++_itr) {
		    //mLog->debugStream() << "updating objects";
		  delete *_itr;
		}

		for (std::list<Tunnel*>::iterator _itr = mTunnels.begin(); 
		     _itr != mTunnels.end();
		     ++_itr) {
		    //mLog->debugStream() << "updating objects";
		  delete *_itr;
		}
				
		mObstacles.clear();
		mTunnels.clear();
		
		delete mSphere;
		
		mEngines.clear();
		
		
		     
		mSfxEngine->cleanup();
		delete mSfxEngine;
		
		mPhyxEngine->cleanup();
		delete mPhyxEngine;
		
		//mUIEngine->cleanup();
		//delete mUIEngine;
		mGfxEngine->cleanup();
		delete mGfxEngine;
		
		
		EventManager::shutdown();
		
		mLog->infoStream() << "---- Exiting Level State ----";
		delete mLog;
		mLog = 0;
		 
	}
	
	void Level::keyPressed( const OIS::KeyEvent &e )
	{
	
		/*mUISystem->injectKeyDown(e.key);
		mUISystem->injectChar(e.text);*/
		mGfxEngine->keyPressed(e);
		mSphere->keyPressed(e);
		
		switch (e.key) {
		}
	}
	
	void Level::keyReleased( const OIS::KeyEvent &e ) {
		
		//mUISystem->injectKeyUp(e.key);
		mGfxEngine->keyReleased(e);
		mSphere->keyReleased(e);
		switch (e.key) {
			case OIS::KC_ESCAPE:
				this->requestShutdown();
				break;
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

	void Level::update( unsigned long lTimeElapsed ) {
		mEvtMgr->update();
		
		processEvents();
		
		for (_itrEngines = mEngines.begin();
		     _itrEngines != mEngines.end();
		     ++_itrEngines)
		    (*_itrEngines)->update(lTimeElapsed);
		    
		//mSfxEngine->update(lTimeElapsed);
		//mGfxEngine->update(lTimeElapsed);
		//mUIEngine->update(lTimeElapsed);
		//mPhyxEngine->update(lTimeElapsed);
		mSphere->update(lTimeElapsed);
		mTunnel->update(lTimeElapsed);
		
		
		//std::list<Obstacle*>::iterator _itr;
		for (_itrObstacles = mObstacles.begin(); 
		     _itrObstacles != mObstacles.end();
		     ) {
		    //mLog->debugStream() << "updating objects";
		    if ((*_itrObstacles)->dead()) {
		      
		      Obstacle *mObs = *_itrObstacles;
		      ++_itrObstacles;
		      releaseObstacle(mObs);
		      continue;
		    };	  
		  (*_itrObstacles)->update(lTimeElapsed);
		  ++_itrObstacles;
		}
		
		if (fSpawning && mTimer.getMilliseconds() > 600) {
		  spawnObstacle();
		  mTimer.reset();
		}
	}
	
  void Level::spawnObstacle() {
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
		  return;
		
	  mObs->live();
	  mObstacles.push_back(mObs);
	  
	  Event* evt = mEvtMgr->createEvt("ObstacleAlive");
	  evt->setAny((void*)mObs);
	  mEvtMgr->hook(evt);  
  }
  
  void Level::releaseObstacle(Obstacle* inObs) {
    //mLog->debugStream() << "obstacle is released into the pool";
    mObstacles.remove(inObs);
    //inObs->die();
    //mObstaclePool.push_back(inObs);
  }
  
  bool Level::evtPortalEntered(Event* inEvt) {
    mSphere->setMaxSpeed(30.0f);
    mSphere->setMoveSpeed(5.0f);
    
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
		
		// teleport the player to the second tunnel after 5 secs from sighting the portal
		//if (mTimer.getMilliseconds() > 5000) {
		  mTunnel->hide();
		  
		  if (mTunnel == mTunnels.back())
		    mTunnel = mTunnels.front();
		  else {
		    for (std::list<Tunnel*>::iterator _itr = mTunnels.begin();
		         _itr != mTunnels.end();
		         ++_itr) {
		      if ((*_itr) == mTunnel)
		        mTunnel = *(++_itr);   // this is our next tunnel  
		    }
		  }
		  
		  // relocate the sphere
		  
		  Vector3 pos = mTunnel->getNode()->getPosition();
		  btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(pos.x,pos.y,pos.z));
		  mSphere->getRigidBody()->proceedToTransform(trans);
		  mSphere->getMasterNode()->setPosition(pos);
		  mLog->debugStream() << "relocating sphere to " << pos.x << ", " << pos.y << ", " << pos.z;
		  // and show the shizzle
		  mTunnel->show();
		  
		  
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
    return mTunnel;
  };
  
  bool Level::areFxEnabled() { return false; }
  bool Level::areSfxEnabled() { return false; }
  
  void Level::dontUpdateMe(Engine* inEngine) {
    mEngines.remove(inEngine);
  };
} // end of namespace
