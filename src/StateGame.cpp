#include "StateGame.h"
#include "EventManager.h"
#include "StatePause.h"
#include <cstdlib>
#include <ctime> 

using namespace Ogre;
namespace Pixy
{
	
	StateGame* StateGame::mStateGame;
	
	GAME_STATE StateGame::getId() const { return STATE_GAME; }
	
	void StateGame::enter( void ) {
		
		srand((unsigned)time(0));
		
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "StateGame");
		
		mEvtMgr = EventManager::getSingletonPtr();
		
		// init engines
		mGfxEngine = GfxEngine::getSingletonPtr();
		mGfxEngine->setup();
				
		//mUIEngine = UIEngine::getSingletonPtr();
		//mUIEngine->setup();
		
		mPhyxEngine = PhyxEngine::getSingletonPtr();
		mPhyxEngine->setup();
			
		// grab CEGUI handle
		//mUISystem = &CEGUI::System::getSingleton();
 
		mSphere = new Sphere();
		mSphere->live();
		
		nrObstacles = 20;
		nrTunnels = 2;
		//createObstacle();
		
		for (int i =0; i < nrObstacles; ++i)
	    mObstaclePool.push_back(new Obstacle());
		
		fSpawning = true;
		
		mGfxEngine->deferredSetup();
		mPhyxEngine->deferredSetup();
		
		bindToName("PortalEntered", this, &StateGame::evtPortalEntered);
		bindToName("PortalReached", this, &StateGame::evtPortalReached);
		bindToName("PortalSighted", this, &StateGame::evtPortalSighted);

    //for (int i =0; i < nrTunnels; ++i) {
      
    //};
    mTunnels.push_back(new Tunnel("Vertigo/Tunnel/Lava"));
    mTunnels.push_back(new Tunnel("Vertigo/Tunnel/Lava/Translucent"));
    mTunnel = mTunnels.back();
    mTunnel->show();
    		
		mLog->infoStream() << "Initialized successfully.";
		
	}


	
	void StateGame::exit( void ) {
	  
	  
	  
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
		delete mPhyxEngine;
		
		//mUIEngine->cleanup();
		//delete mUIEngine;
		mGfxEngine->cleanup();
		delete mGfxEngine;
		
		EventManager::shutdown();
		
		mLog->infoStream() << "---- Exiting StateGame State ----";
		delete mLog;
		mLog = 0;
		 
	}
	
	void StateGame::keyPressed( const OIS::KeyEvent &e )
	{
	
		/*mUISystem->injectKeyDown(e.key);
		mUISystem->injectChar(e.text);*/
		mGfxEngine->keyPressed(e);
		mSphere->keyPressed(e);
		
		switch (e.key) {
		}
	}
	
	void StateGame::keyReleased( const OIS::KeyEvent &e ) {
		
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
			case OIS::KC_EQUALS:
				
				break;			

		}
		
	}
	
	void StateGame::mouseMoved( const OIS::MouseEvent &e )
	{
		//mUIEngine->mouseMoved(e);
		mGfxEngine->mouseMoved(e);
	}
	
	void StateGame::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
    //mUIEngine->mousePressed(e, id);
		mGfxEngine->mousePressed(e, id);
	}
	
	void StateGame::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		//mUIEngine->mouseReleased(e, id);
		mGfxEngine->mouseReleased(e, id);
	}
	
	void StateGame::pause( void ) {
	}
	
	void StateGame::resume( void ) {
	}
		
	StateGame* StateGame::getSingletonPtr( void ) {
		if( !mStateGame )
		    mStateGame = new StateGame();
		
		return mStateGame;
	}
	
	StateGame& StateGame::getSingleton( void ) {
		return *getSingletonPtr();
	}
	
	Sphere* StateGame::getSphere() { return mSphere; };

	void StateGame::update( unsigned long lTimeElapsed ) {
		mEvtMgr->update();
		
		processEvents();
		
		mGfxEngine->update(lTimeElapsed);
		//mUIEngine->update(lTimeElapsed);
		mPhyxEngine->update(lTimeElapsed);
		mSphere->update(lTimeElapsed);
		mTunnel->update(lTimeElapsed);
		
		std::list<Obstacle*>::iterator _itr;
		for (_itr = mObstacles.begin(); 
		     _itr != mObstacles.end();
		     ) {
		    //mLog->debugStream() << "updating objects";
		    if ((*_itr)->dead()) {
		      
		      Obstacle *mObs = *_itr;
		      ++_itr;
		      releaseObstacle(mObs);
		      continue;
		    };	  
		  (*_itr)->update(lTimeElapsed);
		  ++_itr;     
		}
		
		if (fSpawning && mTimer.getMilliseconds() > 600) {
		  spawnObstacle();
		  mTimer.reset();
		}
	}
	
  void StateGame::spawnObstacle() {
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
  
  void StateGame::releaseObstacle(Obstacle* inObs) {
    //mLog->debugStream() << "obstacle is released into the pool";
    mObstacles.remove(inObs);
    //inObs->die();
    //mObstaclePool.push_back(inObs);
  }
  
  bool StateGame::evtPortalEntered(Event* inEvt) {
    mSphere->setMaxSpeed(25.0f);
    mSphere->setMoveSpeed(25.0f);
    
    mTimer.reset();
	  fSpawning = true;
		
    return true;
  };
  
  bool StateGame::evtPortalReached(Event* inEvt) {
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
		  mLog->debugStream() << "relocating sphere to " << pos.x << ", " << pos.y << ", " << pos.z;
		  // and show the shizzle
		  mTunnel->show();
		  
		  
		  return true;
		//};
    return false;
  };
  
  bool StateGame::evtPortalSighted(Event* inEvt) {
    fSpawning = false;
    //mSphere->getRigidBody()->clearForces();
    //mSphere->getRigidBody()->setLinearVelocity(btVector3(0,0,0));
    //mSphere->setMaxSpeed(0.0f);
    mTimer.reset();
    mLog->infoStream() << "Portal is in sight";
    
    return true;
  };
  
  Tunnel* StateGame::getTunnel() {
    return mTunnel;
  };
} // end of namespace
