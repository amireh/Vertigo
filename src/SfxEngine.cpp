/*
 *  This file is part of Vertigo.
 *
 *  Vertigo - a cross-platform arcade game powered by Ogre3D.
 *  Copyright (C) 2011  Ahmad Amireh <ahmad@amireh.net>
 * 
 *  Vertigo is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Vertigo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Vertigo.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
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
		

    mVolume = 0;
		idSound = 0;
		mGameTrack = mIntroTrack = NULL;
		mSoundMgr = NULL;
		
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
 
   	  if (!mGameTrack) {
        mGameTrack = mSoundMgr->createSound("MusicTrack", "music.ogg", true, false, true);
        mGameTrack->loop(true);
        mGameTrack->setVolume(0.5f);
      }
      if (!mIntroTrack) {
        mIntroTrack = mSoundMgr->createSound("IntroTrack", "intro.ogg", true, false, true);
        mIntroTrack->loop(true);
        mIntroTrack->setVolume(0.5f);
      }
      
      mVolume = 1.0f;
      mSoundMgr->setMasterVolume(mVolume);
      
      fAudioStopped = false;
      //toggleAudioState();
      
      mUpdater = &SfxEngine::updateIntro;
      
      fSetup = true;
    }

    bindToName("SettingsChanged", this, &SfxEngine::evtSettingsChanged);
    
    mLog->infoStream() << "set up!";
		return fSetup;
	}
	
	bool SfxEngine::setupIntro() {
	
	  mUpdater = &SfxEngine::updateIntro;
	  
	  //if (mIntroTrack)
	  //  mIntroTrack->play();
	  return true;
	};
	
	bool SfxEngine::setupGame() {
	  mLog->infoStream() << "loading sound track";

	  mProbe = Level::getSingletonPtr()->getProbe();
	  	  
    /*if (mIntroTrack)
      mIntroTrack->stop();*/
      
 	  //if (mGameTrack)
	  // mGameTrack->play();
	           
    //bindToName("DroneCollided", this, &SfxEngine::evtDroneCollided);
    //bindToName("PortalEntered", this, &SfxEngine::evtPortalEntered);
    
    mUpdater = &SfxEngine::updateGame;
    
    return true;	
	};
	
	bool SfxEngine::deferredSetup() {
	  if (!fSetup)
	    return false;

    GameState *currentState = (GameManager::getSingleton().currentState());
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
	  mSoundMgr->update(lTimeElapsed);
	};
	
	void SfxEngine::updateGame(unsigned long lTimeElapsed) {
		
		mSoundMgr->update(lTimeElapsed);
		//mSoundMgr->getListener()->setPosition(mProbe->getPosition());
		//mGameTrack->update(lTimeElapsed);
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
      if (mGameTrack) {
        mGameTrack->stop();
        
      }
    }
    
    fSetup = false;
    
		return true;
	}
	
	bool SfxEngine::evtDroneCollided(Event* inEvt) {
	  Drone* mObs = static_cast<Drone*>(inEvt->getAny());
	  
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
	    //mSoundMgr->pauseAllSounds();
	    //mSoundMgr->muteAllSounds();
	    mSoundMgr->setMasterVolume(mVolume);
	  } else {
	    //mSoundMgr->resumeAllPausedSounds();
	    //mSoundMgr->unmuteAllSounds(); 
	    mSoundMgr->setMasterVolume(0);
	  }
	  
	  fAudioStopped = !fAudioStopped;
	};
	
	void SfxEngine::playMusic() {
	  if (GameManager::getSingleton().getSettings()["Music Enabled"] == "No")
	    return;
	  
	  if (mGameTrack->isPlaying())
	    return;
	  
	  mGameTrack->loop(true);
    mGameTrack->play();
	  /*if (GameManager::getSingleton().currentState()->getId() == STATE_INTRO) {
	    if (mIntroTrack->isPlaying())
	      return;
	      
	    if (mGameTrack->isPlaying())
	      mGameTrack->stop();
	    
	    mIntroTrack->loop(true);
	    mIntroTrack->play();
	  } else {
	    if (mGameTrack->isPlaying())
	      return;
	  
	    if (mIntroTrack->isPlaying())
	      mIntroTrack->startFade(false, 1.0f);
	    
	    mGameTrack->loop(true);
	    mGameTrack->play();
	  }*/
	};
	
	void SfxEngine::stopMusic() {
	  if (mIntroTrack->isPlaying())
	    mIntroTrack->stop(true);
	  if (mGameTrack->isPlaying())
	    mGameTrack->stop(true);
	};
	
	bool SfxEngine::evtSettingsChanged(Event* inEvt) {
	  if (GameManager::getSingleton().getSettings()["Music Enabled"] == "No")
      this->stopMusic();
    else
      this->playMusic();
      
    return true;
	};
	
	void SfxEngine::raiseVolume() {
	  if (mVolume < 1.0f)
	    mVolume += 0.1f;
	  mSoundMgr->setMasterVolume(mVolume);
	};
	
	void SfxEngine::lowerVolume() {
	  if (mVolume > 0)
	    mVolume -= 0.1f;
	  mSoundMgr->setMasterVolume(mVolume);
	};
	
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
}
