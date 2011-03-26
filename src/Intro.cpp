#include "Intro.h"
#include "EventManager.h"
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
		
		// init engines
		mGfxEngine = GfxEngine::getSingletonPtr();
		mGfxEngine->setup();
				
		mUIEngine = UIEngine::getSingletonPtr();
		mUIEngine->setup();
		
		mPhyxEngine = PhyxEngine::getSingletonPtr();
		mPhyxEngine->setup();
			
		// grab CEGUI handle
		mUISystem = &CEGUI::System::getSingleton();
 
		mSphere = new Sphere();
		mSphere->live();
		
		nrObstacles = 0;
		//createObstacle();
		
		for (int i =0; i < nrObstacles; ++i)
	    mObstaclePool.push_back(new Obstacle());
		
		
		mGfxEngine->deferredSetup();
		mPhyxEngine->deferredSetup();
		
		mLog->infoStream() << "Initialized successfully.";
		
	}


	
	void Intro::exit( void ) {
	  
		std::list<Obstacle*>::iterator _itr;
		for (_itr = mObstaclePool.begin(); 
		     _itr != mObstaclePool.end();
		     ++_itr) {
		    //mLog->debugStream() << "updating objects";
		  delete *_itr;
		}
		
		mObstacles.clear();
		
		delete mSphere;
		delete mPhyxEngine;
		delete mUIEngine;
		delete mGfxEngine;
		
		mLog->infoStream() << "---- Exiting Intro State ----";
		delete mLog;
		mLog = 0;
		 
	}
	
	void Intro::keyPressed( const OIS::KeyEvent &e )
	{
	
		mUISystem->injectKeyDown(e.key);
		mUISystem->injectChar(e.text);
		mGfxEngine->keyPressed(e);
		mSphere->keyPressed(e);
		
		switch (e.key) {
		}
	}
	
	void Intro::keyReleased( const OIS::KeyEvent &e ) {
		
		mUISystem->injectKeyUp(e.key);
		mGfxEngine->keyReleased(e);
		mSphere->keyReleased(e);
		switch (e.key) {
			case OIS::KC_ESCAPE:
				this->requestShutdown();
				break;
			case OIS::KC_SPACE:
				//fireLoginEvt();
				break;			

		}
		
	}
	
	void Intro::mouseMoved( const OIS::MouseEvent &e )
	{
		// Update CEGUI with the mouse motion
		mUISystem->injectMouseMove(e.state.X.rel, e.state.Y.rel);
		mGfxEngine->mouseMoved(e);
	}
	
	void Intro::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		mUISystem->injectMouseButtonDown(convertButton(id));
		mGfxEngine->mousePressed(e, id);
	}
	
	void Intro::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		mUISystem->injectMouseButtonUp(convertButton(id));
		mGfxEngine->mouseReleased(e, id);
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
	
	Sphere* Intro::getSphere() { return mSphere; };

	void Intro::update( unsigned long lTimeElapsed ) {
		
		mGfxEngine->update(lTimeElapsed);
		mUIEngine->update(lTimeElapsed);
		mPhyxEngine->update(lTimeElapsed);
		mSphere->update(lTimeElapsed);
		
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
		
		if (mTimer.getMilliseconds() > 500) {
		  spawnObstacle();
		  mTimer.reset();
		}
	}
	
  void Intro::spawnObstacle() {
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
  }
  
  void Intro::releaseObstacle(Obstacle* inObs) {
    //mLog->debugStream() << "obstacle is released into the pool";
    mObstacles.remove(inObs);
    //inObs->die();
    //mObstaclePool.push_back(inObs);
  }
} // end of namespace
