#include "SfxEngine.h"
#include "Level.h"
#include "GfxEngine.h"

using std::map;
namespace Pixy {
	SfxEngine* SfxEngine::_mySfxEngine = NULL;
	
	SfxEngine* SfxEngine::getSingletonPtr() {
		if( !_mySfxEngine ) {
		    _mySfxEngine = new SfxEngine();
		}
		
		return _mySfxEngine;
	}
	
	SfxEngine::SfxEngine() {
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "SfxEngine");
		mLog->infoStream() << "firing up";
		
		idSound = 0;
		mMusicTrack = NULL;
		mSoundMgr = NULL;
		
		fSetup = false;		
	}
	
	SfxEngine::~SfxEngine() {
		mLog->infoStream() << "shutting down";
		if (fSetup) {
			delete mLog;
			mLog = 0;
			
			//delete mSoundMgr;
			fSetup = false;
		}
		
		if (_mySfxEngine)
		  _mySfxEngine = NULL;
	}

  
	bool SfxEngine::setup() {
    
    
		if (!fSetup) {
      
      
      if (!mSoundMgr) {
        mSoundMgr = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
        mSoundMgr->init();
        mSoundMgr->setSceneManager(GfxEngine::getSingletonPtr()->getSM());
      }
            
      fAudioStopped = false;
      //toggleAudioState();
      
      fSetup = true;
    }


    
    mLog->infoStream() << "set up!";
		return fSetup;
	}
	
	bool SfxEngine::setupIntro() {
	
	  mUpdater = &SfxEngine::updateIntro;
	  
	  return true;
	};
	
	bool SfxEngine::setupGame() {
	  mLog->infoStream() << "loading sound track";

	  mSphere = Level::getSingletonPtr()->getSphere();
	  	  
	  if (!mMusicTrack) {
      mMusicTrack = mSoundMgr->createSound("MusicTrack", "music.ogg", false, false, true);
      mMusicTrack->setVolume(0.5f);
    }
 	  if (mMusicTrack)
	    mMusicTrack->play();
	           
    bindToName("ObstacleCollided", this, &SfxEngine::evtObstacleCollided);
    bindToName("PortalEntered", this, &SfxEngine::evtPortalEntered);
    
    mUpdater = &SfxEngine::updateGame;
    
    return true;	
	};
	
	bool SfxEngine::deferredSetup() {
	  if (!fSetup)
	    return false;

    GameState *currentState = (GameManager::getSingleton().currentState());
    if ( !currentState->areSfxEnabled() ) {
      currentState->dontUpdateMe(this);
      return true;
    }    
    if (currentState->getId() == STATE_INTRO)
      fSetup = setupIntro();
    else 
      fSetup = setupGame();
	  
	  return true;
	};
	
	void SfxEngine::update(unsigned long lTimeElapsed) {
	  processEvents();
	  
	  (this->*mUpdater)(lTimeElapsed);
	};
	
	void SfxEngine::updateIntro(unsigned long lTimeElapsed) {
	
	};
	
	void SfxEngine::updateGame(unsigned long lTimeElapsed) {
		
		mSoundMgr->update(lTimeElapsed);
		mSoundMgr->getListener()->setPosition(mSphere->getPosition());
		//mMusicTrack->update(lTimeElapsed);
    /*mSoundMgr->setListenerPosition( mListener->getMasterNode()->getPosition(),
                                    Ogre::Vector3::ZERO, 
                                    mListener->getMasterNode()->getOrientation() );	*/	
	}

  void SfxEngine::attachListener(Entity* inEntity) {
    /*SceneNode* mNode = inEntity->getMasterNode();
    // Set our LISTENER Location (i.e. the ears)
    // The listener is what hears the sounds emitted by audio sources.
    // Note: you only have ONE set of ears - i.e. there is only ONE listener.
    mSoundMgr->setListenerPosition( mNode->getPosition(),
                                    Ogre::Vector3::ZERO, 
                                    mNode->getOrientation() );

    mListener = inEntity;*/
  };
  
	bool SfxEngine::cleanup() {
	  if (!fSetup)
	    return true;

    mLog->debugStream() << "cleaning up";
    
    GameState *currentState = (GameManager::getSingleton().currentState());
    if (currentState->getId() == STATE_GAME) {
      if (mMusicTrack) {
        mMusicTrack->stop();
        mSoundMgr->destroySound(mMusicTrack);
        mMusicTrack = NULL;
      }
    }
        
		return true;
	}
	
	bool SfxEngine::evtObstacleCollided(Event* inEvt) {
	  Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  
	  /*if (mObs->shield() == FIRE)
	    mSoundMgr->getSound("Explosion")->play();
	    //playEffect(SFX_EXPLOSION, mObs->getMasterNode());
	  else
	    mSoundMgr->getSound("Shatter")->play();
	    //playEffect(SFX_SHATTER, mObs->getMasterNode());*/
	  
	  return true;
	};
	
	bool SfxEngine::evtPortalEntered(Event* inEvt) {
	
	  //playEffect(SFX_SHATTER, Level::getSingletonPtr()->getTunnel()->getEntrancePortal());
	  
	  return true;
	};
	
	OgreOggSound::OgreOggSoundManager* SfxEngine::getSoundMgr() { return mSoundMgr; };
	
	void SfxEngine::toggleAudioState() {
	  if (!fAudioStopped) {
	    mSoundMgr->pauseAllSounds();
	    mSoundMgr->muteAllSounds();
	  } else {
	    mSoundMgr->resumeAllPausedSounds();
	    mSoundMgr->unmuteAllSounds(); 
	  }
	  
	  fAudioStopped = !fAudioStopped;
	};
}
