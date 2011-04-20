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

    mSelectedZone = 0;
    
    mEvtMgr = EventManager::getSingletonPtr();
    mGfxEngine = GfxEngine::getSingletonPtr();
    mSfxEngine = SfxEngine::getSingletonPtr();
    mUIEngine = UIEngine::getSingletonPtr();
    
    mGfxEngine->setup();
    mSfxEngine->setup();
    mUIEngine->setup();
    
    mUIEngine->deferredSetup();
    mSfxEngine->deferredSetup();
    mGfxEngine->deferredSetup();

		//mUIEngine->show();
		mUIEngine->_refit(this);

		mLog->infoStream() << "Initialized successfully.";
		
	}


	
	void Intro::exit( void ) {
	  
	  
	  
		fRunning = false;
		bool fShuttingDown = GameManager::getSingleton().shuttingDown();
		
		//mUIEngine->cleanup();
		//mUIEngine->hide();
		/*mSfxEngine->cleanup();
		mPhyxEngine->cleanup();
		mGfxEngine->cleanup();*/
		
		
		
		if (!fShuttingDown);
		  //mUIEngine->_refit(Level::getSingletonPtr());
		else {
		  mSelectedZone = 0;
		}
		mLog->infoStream() << "---- Exiting Intro State ----";
		
    mEvtMgr = 0;
    mGfxEngine = 0;
    mSfxEngine = 0;
    mUIEngine = 0;
		
		delete mLog;
		mLog = 0;
		
		 
	}
	
	void Intro::keyPressed( const OIS::KeyEvent &e )
	{
	
	  mUIEngine->keyPressed( e );
	
	}
	
	void Intro::keyReleased( const OIS::KeyEvent &e ) {
		
		mUIEngine->keyReleased( e );
    mSfxEngine->keyReleased(e);
    
		//if (e.key == OIS::KC_ESCAPE) 
  
		
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
		mEvtMgr->update();
		processEvents();
		
    mUIEngine->update(lTimeElapsed);
    mSfxEngine->update(lTimeElapsed);
    mGfxEngine->update(lTimeElapsed);
	}

  
  bool Intro::areFxEnabled() { return GameManager::getSingleton().getSettings().FX_LEVEL != FX_LEVEL_LOW; }
  bool Intro::areSfxEnabled() { return GameManager::getSingleton().getSettings().SOUND_ENABLED; }
  void Intro::dontUpdateMe(Engine* inEngine) {
    mEngines.remove(inEngine);
  };
  
  Zone* Intro::getSelectedZone() { return mSelectedZone; };
  
  void Intro::setSelectedZone(Zone* inZone) { mSelectedZone = inZone; };
  
} // end of namespace
