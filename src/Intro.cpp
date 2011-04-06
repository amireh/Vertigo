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
		fRunning = true;
		
		srand((unsigned)time(0));
		
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Intro");

    mEvtMgr = EventManager::getSingletonPtr();
    //GfxEngine::getSingletonPtr()->setup();
    //GfxEngine::getSingletonPtr()->hideUI();
    mUIEngine = UIEngine::getSingletonPtr();
    mUIEngine->setup();
    mUIEngine->deferredSetup();
		//mUIEngine->show();
		
		mLog->infoStream() << "Initialized successfully.";
		
	}


	
	void Intro::exit( void ) {
	  
	  
	  
		
		bool fShuttingDown = GameManager::getSingleton().shuttingDown();
		
		//mUIEngine->cleanup();
		//mUIEngine->hide();
		/*mSfxEngine->cleanup();
		mPhyxEngine->cleanup();
		mGfxEngine->cleanup();*/
		
		fRunning = false;
		
		mUIEngine->_refit(Level::getSingletonPtr());
		
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

		/*if (e.key == OIS::KC_Q) {
		  return this->requestShutdown();
		} else */if (e.key == OIS::KC_ESCAPE) {
		  if (Level::getSingleton().running())
		    return GameManager::getSingleton().popState();
		  else
		    return this->requestShutdown();
		  
		}

		
	}
	
	void Intro::mouseMoved( const OIS::MouseEvent &e )
	{
		mUIEngine->mouseMoved(e);
		//mSphere->mouseMoved(e);
		//mGfxEngine->mouseMoved(e);
		
	}
	
	void Intro::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  mUIEngine->mousePressed(e, id);
		//mGfxEngine->mousePressed(e, id);
	}
	
	void Intro::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  mUIEngine->mouseReleased(e, id);
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
		
    //mUIEngine->update(lTimeElapsed);
	}

  
  bool Intro::areFxEnabled() { return true; }
  bool Intro::areSfxEnabled() { return true; }
  void Intro::dontUpdateMe(Engine* inEngine) {
    mEngines.remove(inEngine);
  };
  
} // end of namespace
