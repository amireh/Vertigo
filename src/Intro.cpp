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
	}
	
	void Intro::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
    //mUIEngine->mousePressed(e, id);
		//mGfxEngine->mousePressed(e, id);
	}
	
	void Intro::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
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
} // end of namespace
