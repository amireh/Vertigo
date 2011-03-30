#include "AudioEngine.h"
#include "StateGame.h"
#include "GfxEngine.h"

using std::map;
namespace Pixy {
	AudioEngine* AudioEngine::_myAudioEngine = NULL;
	
	AudioEngine* AudioEngine::getSingletonPtr() {
		if( !_myAudioEngine ) {
		    _myAudioEngine = new AudioEngine();
		}
		
		return _myAudioEngine;
	}
	
	AudioEngine::AudioEngine() {
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "AudioEngine");
		mLog->infoStream() << "firing up";
		
		idSound = 0;
		
		fSetup = false;		
	}
	
	AudioEngine::~AudioEngine() {
		mLog->infoStream() << "shutting down";
		if (fSetup) {
			delete mLog;
			mLog = 0;
			
			//delete mSoundMgr;
			fSetup = false;
		}
	}

  
	bool AudioEngine::setup() {
		if (fSetup)
			return true;

		/*
    mSoundMgr = SoundManager::createManager();
    mSoundMgr->init();
    std::string tmpPath = "";
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || defined(_WIN32)
    tmpPath = "..\\resources\\media\\sounds\\";
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    tmpPath = "../Resources/media/sounds/";
#else
    tmpPath = "../resources/media/sounds/";
#endif

    mSoundMgr->setAudioPath( (char*)tmpPath.c_str() );
    // We loop to be able to test the pause function
    ++idSound;
    mSoundMgr->loadAudio( "explosion.wav", &(idSound), true);
    mSounds.insert(make_pair<SOUND_EFFECT, unsigned long>(SFX_EXPLOSION, idSound));
    ++idSound;
    mSoundMgr->loadAudio( "shatter2.wav", &(idSound), true);
    mSounds.insert(make_pair<SOUND_EFFECT, unsigned long>(SFX_SHATTER, idSound));
    */
    
    mSoundMgr = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    mSoundMgr->init();
    mSoundMgr->setSceneManager(GfxEngine::getSingletonPtr()->getSM());
    mSoundMgr->createSound("Explosion", "explosion.wav", false, false, true) ;
    mSoundMgr->createSound("Shatter", "shatter2.wav", false, false, true) ;

    mMusicTrack = mSoundMgr->createSound("MusicTrack", "music.ogg", false, false, true);
    mMusicTrack->setVolume(0.5f);
    
    bindToName("ObstacleCollided", this, &AudioEngine::evtObstacleCollided);
    bindToName("PortalEntered", this, &AudioEngine::evtPortalEntered);
    
    mLog->infoStream() << "set up!";
    
    fAudioStopped = false;
    toggleAudioState();
    
		fSetup = true;
		return fSetup;
	}
	
	bool AudioEngine::deferredSetup() {
	  mSphere = StateGame::getSingletonPtr()->getSphere();
	  mMusicTrack->play();
	  /*if (!mListener && mSphere)
	    attachListener(mSphere);*/
	  
	  //playEffect(SFX_EXPLOSION, mSphere->getMasterNode());
	  
	  return true;
	};
	
	void AudioEngine::update(unsigned long lTimeElapsed) {
		processEvents();
		
		mSoundMgr->update(lTimeElapsed);
		mSoundMgr->getListener()->setPosition(mSphere->getPosition());
		//mMusicTrack->update(lTimeElapsed);
    /*mSoundMgr->setListenerPosition( mListener->getMasterNode()->getPosition(),
                                    Ogre::Vector3::ZERO, 
                                    mListener->getMasterNode()->getOrientation() );	*/	
	}

  void AudioEngine::attachListener(Entity* inEntity) {
    /*SceneNode* mNode = inEntity->getMasterNode();
    // Set our LISTENER Location (i.e. the ears)
    // The listener is what hears the sounds emitted by audio sources.
    // Note: you only have ONE set of ears - i.e. there is only ONE listener.
    mSoundMgr->setListenerPosition( mNode->getPosition(),
                                    Ogre::Vector3::ZERO, 
                                    mNode->getOrientation() );

    mListener = inEntity;*/

  };
  
  void AudioEngine::playEffect(SOUND_EFFECT inEffect, SceneNode* inEmitter) {
/*
    if (!mListener) {
      mLog->errorStream() << "attempting to play effect with no listener attached, aborting";
      return;
    }
    
    mLog->debugStream() << "playing sound effect with id: " << inEffect;
    
    // Set the Audio SOURCE location/position AND by default PLAY the audio.
    // Note how we reference the correct audio source by using 'mAudioSource'.
    mSoundMgr->setSound( mSounds[inEffect], 
                         inEmitter->getPosition(),
                         Ogre::Vector3::ZERO, 
                         Ogre::Vector3::ZERO, 
                         1000.0f, true, true, 1.0f );  
*/
    //mSoundMgr->playAudio( mSounds[inEffect], false );
  };
  
	bool AudioEngine::cleanup() {
		return true;
	}
	
	bool AudioEngine::evtObstacleCollided(Event* inEvt) {
	  Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  
	  /*if (mObs->shield() == FIRE)
	    mSoundMgr->getSound("Explosion")->play();
	    //playEffect(SFX_EXPLOSION, mObs->getMasterNode());
	  else
	    mSoundMgr->getSound("Shatter")->play();
	    //playEffect(SFX_SHATTER, mObs->getMasterNode());*/
	  
	  return true;
	};
	
	bool AudioEngine::evtPortalEntered(Event* inEvt) {
	
	  playEffect(SFX_SHATTER, StateGame::getSingletonPtr()->getTunnel()->getEntrancePortal());
	  
	  return true;
	};
	
	OgreOggSound::OgreOggSoundManager* AudioEngine::getSoundMgr() { return mSoundMgr; };
	
	void AudioEngine::toggleAudioState() {
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
