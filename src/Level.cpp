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
 
#include "Level.h"
#include "EventManager.h"
#include "Pause.h"
#include <cstdlib>
#include <ctime> 
#include "Intro.h"

using namespace Ogre;
namespace Pixy
{
	
	Level* Level::mLevel;
	
	Level::Level() {
	  fRunning = false;
	  fScreenshotTaken = false; // see Level::updateNothing() for info on this
	};
	
	void Level::enter( void ) {
		
		fRunning = true;
		
		srand((unsigned)time(0));
		
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Level");
		
		mEvtMgr = EventManager::getSingletonPtr();
		
		
		// init engines
		mGfxEngine = GfxEngine::getSingletonPtr();
		mPhyxEngine = PhyxEngine::getSingletonPtr();
		mSfxEngine = SfxEngine::getSingletonPtr();
		mUIEngine = UIEngine::getSingletonPtr();
		
		mLog->debugStream() << "igniting engines";
		
		mGfxEngine->setup();
		mPhyxEngine->setup();
		mSfxEngine->setup();
		mUIEngine->setup();

    fFirstZone = true;
    
    mZone = new Zone(Intro::getSingleton().getSelectedZone()->filePath());
    
    // prepare our probe
    mLog->debugStream() << "creating sphere";
		mProbe = new Probe();
		mProbe->live();    
    
    // now prepare our obstacles
		nrDrones = 12;
		for (int i =0; i < nrDrones; ++i)
	    mDronePool.push_back(new Drone());
		
    // event handlers
    bindToName("ZoneEntered", this, &Level::evtZoneEntered);
    bindToName("PlayerWon", this, &Level::evtPlayerWon);
		bindToName("ProbeDied", this, &Level::evtProbeDied);
		bindToName("PortalEntered", this, &Level::evtPortalEntered);
		bindToName("PortalSighted", this, &Level::evtPortalSighted);
		bindToName("TakingScreenshot", this, &Level::evtTakingScreenshot);
		bindToName("ScreenshotTaken", this, &Level::evtScreenshotTaken);

		mGfxEngine->deferredSetup();
		mPhyxEngine->deferredSetup();
		mSfxEngine->deferredSetup();
		mUIEngine->deferredSetup();

    mUpdater = &Level::updatePreparation;
    
		mLog->infoStream() << "initialized successfully.";
		reset();
	}

	void Level::exit( void ) {
	  fRunning = false;
	  
		for (std::list<Drone*>::iterator _itr = mDronePool.begin(); 
		     _itr != mDronePool.end();
		     ++_itr) {
		  delete *_itr;
		}
				
		mDrones.clear();
		
		mLog->infoStream() << "---- exiting Level State ----";
		
		delete mZone;
		delete mProbe;
		
		mEngines.clear();
		
		mUIEngine->cleanup();
		mSfxEngine->cleanup();
		mPhyxEngine->cleanup();
		mGfxEngine->cleanup();
		
		mGfxEngine = 0;
		mPhyxEngine = 0;
		mSfxEngine = 0;
		mUIEngine = 0;
		
		mLog->infoStream() << "---- Exiting Level State ----";
		delete mLog;
		mLog = 0;
		 
	}
	
	void Level::keyPressed( const OIS::KeyEvent &e )
	{
	
	  mUIEngine->keyPressed(e);
	
		if (!fGameStarted)
		  return;
		
		mGfxEngine->keyPressed(e);
		mProbe->keyPressed(e);
		
		switch (e.key) {
		}
	}
	
	void Level::keyReleased( const OIS::KeyEvent &e ) {
		
		mSfxEngine->keyReleased(e);
		mUIEngine->keyReleased( e );
		
		// start the game when the ENTER key is released
		if (!fGameStarted) {
		  if ( e.key == OIS::KC_RETURN) {
		    fGameStarted = true;
		    mEvtMgr->hook(mEvtMgr->createEvt("GameStarted"));
		  }
		  return;
		}
		
		// show the menu
		if (e.key == OIS::KC_ESCAPE) {
		  return GameManager::getSingleton().pushState(Intro::getSingletonPtr());
		}
		
		mProbe->keyReleased(e);
		switch (e.key) {
		  case OIS::KC_P:
		    GameManager::getSingleton().pushState(Pause::getSingletonPtr());
		    break;
			case OIS::KC_G:
			  mEvtMgr->hook(mEvtMgr->createEvt("ProbeDied"));
			  mProbe->die();
			  break;
			case OIS::KC_H:
			  mEvtMgr->hook(mEvtMgr->createEvt("PlayerWon"));
			  break;
		}
	}
	
	void Level::mouseMoved( const OIS::MouseEvent &e )
	{
	}
	
	void Level::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	}
	
	void Level::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	}
	
	void Level::pause( void ) {
	  mLog->infoStream() << "---- Pausing ----";
	  this->_hideEverything();
	}
	
	void Level::resume( void ) {
	  mLog->infoStream() << "---- Resuming ----";
	  if (!fGameOver)
	    this->_showEverything();
	}
		
	Level* Level::getSingletonPtr( void ) {
		if( !mLevel )
		    mLevel = new Level();
		
		return mLevel;
	}
	
	Level& Level::getSingleton( void ) {
		return *getSingletonPtr();
	}
	
  void Level::updatePreparation(unsigned long lTimeElapsed) {
    if (fGameStarted) {
      mUpdater = &Level::updateGame;
      fSpawning = true;
    }
    
    mUIEngine->update(lTimeElapsed);
    mGfxEngine->update(lTimeElapsed);
    mPhyxEngine->update(lTimeElapsed);
    mSfxEngine->update(lTimeElapsed);
    mProbe->update(lTimeElapsed);
  };

  void Level::updateGameOver(unsigned long lTimeElapsed) {
    // we don't update the Physics engine here since we want
    // everything to stop
    
		mUIEngine->update(lTimeElapsed);
		mSfxEngine->update(lTimeElapsed);
		mGfxEngine->update(lTimeElapsed);
		mProbe->update(lTimeElapsed);
		mZone->update(lTimeElapsed);
  };
  
  void Level::updateGame(unsigned long lTimeElapsed) {
	
		mSfxEngine->update(lTimeElapsed);
		mGfxEngine->update(lTimeElapsed);
		mUIEngine->update(lTimeElapsed);
		mPhyxEngine->update(lTimeElapsed);

    mZone->update(lTimeElapsed);
		mProbe->update(lTimeElapsed);
		
		for (_itrDrones = mDrones.begin(); 
		     _itrDrones != mDrones.end();
		     ) {
		    // release the dead obstacles
		    if ((*_itrDrones)->dead()) {
		      
		      Drone *mDrone = *_itrDrones;
		      ++_itrDrones;
		      releaseDrone(mDrone);
		      continue;
		    };
		  // and update the living ones	  
		  (*_itrDrones)->update(lTimeElapsed);
		  ++_itrDrones;
		}
		
		if (fSpawning && mSpawnTimer.getMilliseconds() > mSpawnRate) {
		  // spawn a random class of obstacles per the registered ones
		  spawnDrone(mZone->getSettings().mRegisteredDroneClasses[rand() % mZone->getSettings().mRegisteredDroneClasses.size()]);
		  mSpawnTimer.reset();
		}
  };
  
  void Level::updateNothing(unsigned long lTimeElapsed) {
    /* WELCOME TO THE UGLIEST HACK OF MY LIFE
     * i'm gonna write this so i can remember how to refactor it later:
     *
     * basically, taking a screenshot will lock the game out since we're not threaded
     * and will cause a huge frame burst causing everything to go out of sync;
     * resetting the frame timer in GameManager seems too much interference for good design
     * and it's a very costly operation
     *
     * so what happens is: 
     *  1) GfxEngine gets key event to take screenie
     *  2) GfxEngine emits event "TakingScreenshot"
     *  3) Level handles the event and:
     *    a) sets the updater to &Level::updateNothing
     *    b) sets fScreenshotTaken = false
     *    c) manually updates GfxEngine with 0 time elapsed, to make it handle the event
     *  4) GfxEngine handles the event and actually takes the screenshot
     *  5) GfxEngine emits event "ScreenshotTaken"
     *  6) Level handles the event and sets fScreenshotTaken = true
     *  NOTE: we do NOT set back the updater immediately, because that would defeat the whole point
     *  7) INSIDE Level::updateNothing(), it will be updated with a huge lTimeElapsed
     *     and will redirect the updater back to Level::updateGame()
     *  8) Screenie taken + world consistent = PROFIT
    */
    if (fScreenshotTaken)
      mUpdater = &Level::updateGame;
  };
  
	void Level::update( unsigned long lTimeElapsed ) {
		mEvtMgr->update();
		processEvents();
		
    (this->*mUpdater)(lTimeElapsed);
    
	}
	
  Drone* Level::spawnDrone(DRONE_CLASS inClass) {
    
    if (inClass == DUETTE) {
      spawnDuette();
      return NULL;
    }
      
    Drone* mDrone = NULL;
    
    // find a dead drone
		std::list<Drone*>::iterator _itr;
		for (_itr = mDronePool.begin(); 
		     _itr != mDronePool.end();
		     ++_itr) {
		    if ((*_itr)->dead()) {
		      mDrone = *_itr;
		      break;
		    };    
		}
		
		if (!mDrone)
		  return NULL;
		
		mDrone->live();
		mDrone->setClass(inClass);
	  mDrones.push_back(mDrone);
	  	  
	  return mDrone;  
  }
  
  void Level::spawnDuette() {
    // spawn two obstacles of opposing shields, and opposing directions
    Drone *mDrone[2];
    SHIELD lastShield = (rand() % 2 == 0) ? FIRE : ICE;
    for (int i=0;i<2;++i) {
      mDrone[i] = spawnDrone(DUMB);
      mDrone[i]->setClass(DUETTE);
      if (!mDrone[i]) {
        mLog->errorStream() << "could not spawn duettes!! aborting";
        break;
      }

      // set one of them to go left and the other to the right
      mLog->debugStream() << "setting duette drone direction";
      mDrone[i]->setDirection(Vector3((i == 0) ? 0.6f : -0.6f,-1,-1));
      mDrone[i]->setShield(lastShield);
      
      // swap shields
      lastShield = (lastShield == FIRE) ? ICE : FIRE;
    }
    // we need to link them for some score counting, see Probe.cpp
    mDrone[0]->setDuetteTwin(mDrone[1]);
    mDrone[1]->setDuetteTwin(mDrone[0]);
  };
  
  void Level::releaseDrone(Drone* inDrone) {
    mDrones.remove(inDrone);
  }
  
  bool Level::evtPortalEntered(Event* inEvt) {
    // resume spawning drones
    
    mSpawnTimer.reset();
	  fSpawning = true;
		
    return true;
  };

  bool Level::evtPortalSighted(Event* inEvt) {
    fSpawning = false; // stop spawning

		// raise the speed of all obstacles
		std::list<Drone*>::iterator _itr;
		for (_itr = mDronePool.begin(); 
		     _itr != mDronePool.end();
		     ++_itr)
		  (*_itr)->setMaxSpeed((*_itr)->getMaxSpeed() + (*_itr)->getMaxSpeed() * mZone->getSettings().mMaxSpeedStep);
    
    return true;
  };
  
  bool Level::evtProbeDied(Event* inEvt) {
    fGameOver = true;
    mUpdater = &Level::updateGameOver;

    return true;     
  };
  
  bool Level::evtPlayerWon(Event* inEvt) {
    mUpdater = &Level::updateGameOver;
    fGameOver = true;
    
    _hideEverything();
    
    return true;
  };
  
  // TODO: FIX THIS, completely wrong!!
  void Level::increaseSpawnRate(const int inFactor) {
    // increase the rate of obstacle spawning
    mSpawnRate -= mSpawningThreshold / inFactor;
    if (mSpawnRate <= mSpawningThreshold)
      mSpawnRate = mSpawningThreshold; // shouldn't be faster than this, really  
  };
  
  bool Level::evtZoneEntered(Event* inEvt) {
    mLog->infoStream() << " ----- entered " << mZone->name() << " zone ----- ";

    mSfxEngine->playMusic();
    mZone->engage();
    mUIEngine->_refit(this);
    
    return true;
  };

  void Level::reset() {
    if (!fRunning)
      return;
    
    mLog->infoStream() << "loading a new zone, resetting the game";

    // this class is the one who emits the first "ZoneEntered" event and thus
    // does not have to handle it
    if (!fFirstZone) {

      mZone->disengage();
      delete mZone;
            
      // load the new zone
      mZone = new Zone(Intro::getSingleton().getSelectedZone()->filePath());

      // kill our obstacles and clear the queue
      //mLog->debugStream() << "I have " << mDrones.size() << " obstacles alive, killng em";
		  std::list<Drone*>::iterator _itr;
		  for (_itr = mDronePool.begin(); 
		       _itr != mDronePool.end();
		       ++_itr)
        (*_itr)->die();
		  
		  mDrones.clear();
    } else
      fFirstZone = false;
    
    mSpawnRate = mZone->getSettings().mSpawnRate;
		mSpawningThreshold = mSpawnRate / 2;
	  
    fSpawning = false;
    fGameStarted = false;
    fGameOver = false;

    mUpdater = &Level::updatePreparation;

    mEvtMgr->hook(mEvtMgr->createEvt("ZoneEntered"));
    
  };
  
  void Level::_hideEverything() {
    // hide our tunnel, drones, and probe
    mProbe->getMasterNode()->setVisible(false);
	  std::list<Drone*>::iterator _itr;
	  for (_itr = mDronePool.begin(); 
	       _itr != mDronePool.end();
	       ++_itr)
      (*_itr)->getMasterNode()->setVisible(false);
      
    mZone->currentTunnel()->getNode()->setVisible(false);
  };
  
  void Level::_showEverything() {
    mProbe->getMasterNode()->setVisible(true);
	  std::list<Drone*>::iterator _itr;
	  for (_itr = mDronePool.begin(); 
	       _itr != mDronePool.end();
	       ++_itr)
      (*_itr)->getMasterNode()->setVisible(true);
    
    // in the case where the player is switching to a new zone, and was already playing one
    // (Level is running), _showEverything() will be called on GameState::resume()
    // and since our zone will not be engaged yet, the tunnel will never be shown anyway
    // that's why we chekc if the tunnel exists.. otherwise, this applies to pausing / resuming the game
    // which happens by toggling the menu
    if (mZone->currentTunnel())
      mZone->currentTunnel()->getNode()->setVisible(true);  
  };
  
  bool Level::evtTakingScreenshot(Event* inEvt) {
    mLog->debugStream() << "gfxengine says it's taking a screenie";
    
    mUpdater = &Level::updateNothing;
    
    // step the gfxengine so it could process the event
    mGfxEngine->update(0);
    
    fScreenshotTaken = false;
    
    return true;
  };
  
  bool Level::evtScreenshotTaken(Event* inEvt) {
    mLog->debugStream() << "gfxengine says it's done taking a screenie";
    
    fScreenshotTaken = true;
    
    return true;
  };
} // end of namespace
