#include "SfxEngine.h"
#include "Level.h"
#include "GfxEngine.h"

using std::map;
namespace Pixy {
	SfxEngine* SfxEngine::_instance = NULL;
	
	SfxEngine* SfxEngine::getSingletonPtr() {
		if( !_instance ) {
		    _instance = new SfxEngine();
		}
		
		return _instance;
	}
	
	SfxEngine::SfxEngine() {
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "SfxEngine");
		mLog->infoStream() << "firing up";

		idSound = 0;
		mGameTrack = mIntroTrack = 0;
		mSoundMgr = 0;
		
		fSetup = false;		
	}
	
	SfxEngine::~SfxEngine() {
		mLog->infoStream() << "shutting down";
		if (fSetup) {
			delete mLog;
			mLog = 0;

		  if (mGameTrack) {
        mSoundMgr->destroySound(mGameTrack);
        mGameTrack = NULL;
      }
      if (mIntroTrack) {
        mSoundMgr->destroySound(mIntroTrack);
        mIntroTrack = NULL;
      }		
			//delete mSoundMgr;
			fSetup = false;
		}
		
		if (_instance)
		  _instance = NULL;
	}

  
	bool SfxEngine::setup() {
    
    
		if (!fSetup) {
      
      mVolume = 1.0f;
      
      if (!mSoundMgr) {
        mSoundMgr = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
        mSoundMgr->init();
        mSoundMgr->setSceneManager(GfxEngine::getSingletonPtr()->getSM());
      }

   	  if (!mGameTrack) {
   	    // we can't use OGG on mac, we use WAV instead
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
        mGameTrack = mSoundMgr->createSound("MusicTrack", "music.wav", true, false, true);
#else
        mGameTrack = mSoundMgr->createSound("MusicTrack", "music.ogg", true, false, true);
#endif
        mGameTrack->loop(true);
        mGameTrack->setVolume(mVolume / 2.0f);
      }
      
    }

    mSoundMgr->setMasterVolume(mVolume);
    
    fAudioStopped = false;
    //toggleAudioState();
    
    
    bindToName("SettingsChanged", this, &SfxEngine::evtSettingsChanged);
    
    mLog->infoStream() << "set up!";
    
    fSetup = true;
		return fSetup;
	}
	
	bool SfxEngine::deferredSetup() {
	  if (!fSetup)
	    return false;
    
	  return fSetup;
	};
	
	void SfxEngine::update(unsigned long lTimeElapsed) {
	  processEvents();
	  
	  mSoundMgr->update(lTimeElapsed);
	};
  
	bool SfxEngine::cleanup() {
	  if (!fSetup)
	    return true;

    mLog->infoStream() << "cleaning up";
    fSetup = false;
    
		return true;
	}
	
	OgreOggSoundManager* SfxEngine::getSoundMgr() { return mSoundMgr; };

	void SfxEngine::keyReleased( const OIS::KeyEvent &e ) {
	  switch (e.key) {
	    case OIS::KC_MINUS:
	      lowerVolume();
	      break;
	    case OIS::KC_EQUALS:
	      raiseVolume();
	      break;
	    case OIS::KC_M:
	      toggleAudioState();
	      break;
	  }
	}
		
	void SfxEngine::toggleAudioState() {
	  if (!fAudioStopped) {
	    mSoundMgr->pauseAllSounds();
	    mSoundMgr->muteAllSounds();
	    mSoundMgr->setMasterVolume(0);
	  } else {
	    mSoundMgr->resumeAllPausedSounds();
	    mSoundMgr->unmuteAllSounds();
	     mSoundMgr->setMasterVolume(mVolume);
	  }
	  
	  fAudioStopped = !fAudioStopped;
	};
	
	void SfxEngine::playMusic() {
	  if (!GameManager::getSingleton().getSettings().MUSIC_ENABLED)
	    return;
	  
	  if (mGameTrack->isPlaying())
	    return;
	  
	  mGameTrack->loop(true);
    mGameTrack->play();
	};
	
	void SfxEngine::stopMusic() {
	  
	  if (mGameTrack->isPlaying())
	    mGameTrack->stop(true);
	};
	
	bool SfxEngine::evtSettingsChanged(Event* inEvt) {
	  if (!GameManager::getSingleton().getSettings().MUSIC_ENABLED)
      this->stopMusic();
    else
      this->playMusic();
      
    return true;
	};
	
	void SfxEngine::raiseVolume() {
	  if (mVolume != 1.0f)
	    mVolume += 0.1f;
	    
	  mSoundMgr->setMasterVolume(mVolume);
	};
	
	void SfxEngine::lowerVolume() {
	  if (mVolume != 0)
	    mVolume -= 0.1f;
	    
	  mSoundMgr->setMasterVolume(mVolume);
	};
	

}
