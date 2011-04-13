#include "Tunnel.h"
#include "GfxEngine.h"
#include "Level.h"
#include "SfxEngine.h"
#include "GameManager.h"

namespace Pixy {

  int Tunnel::nrTunnels = 0;
  bool Tunnel::fMeshGenerated = false;
  bool Tunnel::fEffectPrepared = false;
  ParticleUniverse::ParticleSystem* Tunnel::mPortalEffect = NULL;
  OgreOggSound::OgreOggISound* Tunnel::mSfxPortal = NULL;
    
  Tunnel::Tunnel(const String inMaterial, 
                 const int inNrSegments, 
                 const Real inSegmentLength, 
                 const Real inRadius, 
                 const Vector3& inPosition,
                 bool inAutoShow) {
    
    idObject = ++nrTunnels;
    mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Tunnel" + stringify(idObject));
    
    mNrSegments = inNrSegments;
    mSegmentLength = inSegmentLength;
    mRadius = inRadius;
    mMaterial = inMaterial;
    
    mEvtMgr = EventManager::getSingletonPtr();
    mGfxEngine = GfxEngine::getSingletonPtr();
    mSceneMgr = mGfxEngine->getSM();
    mFxMgr = ParticleUniverse::ParticleSystemManager::getSingletonPtr();
    

    
    // create our master node and set its starting position
    /*mNode = mSceneMgr->getRootSceneNode()->
      createChildSceneNode(String("Terrain/Tunnel/" + stringify(idObject)));*/
    mNode = mSceneMgr->createSceneNode(String("Terrain/Tunnel/" + stringify(idObject)));
    mNode->setPosition(inPosition);
    
    
    mEntrance = mExit = NULL;
    fHasSfx = GameManager::getSingleton().currentState()->areSfxEnabled();
    //fHasFx = GameManager::getSingleton().currentState()->areFxEnabled();
    fHasFx = true; // tunnel always has to have particles, for portals
    fPassedEntrance = false;
    fPortalSighted = false;
    fPortalReached = false;
    
    //mLog->debugStream() << "created";
    
    generateSegments();
    generatePortals();
    
    // prepare sound effect
    if (!mSfxPortal) {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
      mSfxPortal = mSoundMgr->createSound("Teleport", "teleport.wav", false, false, true) ;
      mSfxPortal->setRolloffFactor(2.f);
      mSfxPortal->setReferenceDistance(1000.f);
    };
    
    //mNode->setVisible(false);
    
    if (inAutoShow)
      show();
  };
  
  Tunnel::~Tunnel() {
    /*if (mPortalEffect) {
      mPortalEffect->stop();
      mFxMgr->destroyParticleSystem(mPortalEffect, mSceneMgr);
      mPortalEffect = NULL;
      fEffectPrepared = false;
    }*/

        
    /*if (fHasSfx && mSfxPortal) {
      OgreOggSound::OgreOggSoundManager *mSoundMgr;
      mSoundMgr = SfxEngine::getSingletonPtr()->getSoundMgr();
      mSoundMgr->destroySound(mSfxPortal);
      mSoundMgr = NULL;
      mSfxPortal = NULL;
    };*/
    
    // TODO remove scene nodes
    if (mNode->getParentSceneNode())
      mNode->getParentSceneNode()->removeAndDestroyChild(mNode->getName());
    
    mLog->debugStream() << "destructed " << idObject;
    if (mLog)
      delete mLog;
    
    //mSphere = NULL;
    //mSphereNode = NULL;
    mLog = NULL;
    mGfxEngine = NULL;
    mFxMgr = NULL;
    mSceneMgr = NULL;
    mEntrance = mExit = NULL;
  };
  
  void Tunnel::generateSegments() {

    //mLog->debugStream() << "generating segments";
    
    if (!fMeshGenerated) {
      Procedural::Root::getInstance()->sceneManager = mSceneMgr;
            
	    Procedural::TubeGenerator()
	    .setOuterRadius(mRadius)
	    .setInnerRadius(mRadius - 2.0f)
	    .setHeight(mSegmentLength)
	    .setNumSegBase(32)
	    .setNumSegHeight(1)
	    .realizeMesh("TunnelMesh");
			
			fMeshGenerated = true;
	  }

	  Vector3 offset = mNode->getPosition();
	  Ogre::Entity* tmpEntity;
	  Ogre::SceneNode* tmpNode;
	  Ogre::String tmpName = "";
	  for (int i =0; i < mNrSegments; ++i) {
		  tmpName = String("Tunnel" + stringify(idObject) + "/Segment" + stringify(i));

  		tmpEntity = mSceneMgr->createEntity(tmpName, "TunnelMesh");
		  tmpNode = mNode->createChildSceneNode();
		  tmpEntity->setMaterialName(mMaterial);
		  tmpNode->attachObject(tmpEntity);
		  tmpNode->setPosition(
		    Vector3(
		      offset.x + 0, 
		      offset.y + 70, 
		      offset.z + (i * mSegmentLength)
		    )
		  );
		  //tmpNode->roll(Ogre::Degree(90));
		  tmpNode->pitch(Ogre::Degree(90));
		  //tmpNode->yaw(Ogre::Degree(30));
		  //tmpNode->showBoundingBox(true);			
		  mSegments.push_back(tmpNode);
	  }
	  
	  mLength = mNrSegments * mSegmentLength;
	  
	  tmpNode = NULL;
	  tmpEntity = NULL;
  };
  
  void Tunnel::generatePortals() {
  
    //mLog->debugStream() << "generating portals";
    
    mPortalEffect = mFxMgr->getParticleSystem("FxPortal");
    
    if (fHasFx && !mPortalEffect) {
      mPortalEffect = mFxMgr->createParticleSystem(
        "FxPortal",
        "Vertigo/FX/Portal", 
        mSceneMgr);
      mPortalEffect->prepare();
      fEffectPrepared = true;
    }

    Portal** mPortal;
    SceneNode* mSegment;
    Ogre::String tmpName;
    Vector3 offset;
    int i;
    for (i = 0; i < 2; ++i) {
      tmpName = String("Tunnel" + stringify(idObject) + "/Portal/" + stringify(i));
      
      if (i % 2 == 0) {
        mPortal = &mEntrance;
        mSegment = mSegments.front();
        offset.z = -5;
      } else {
        mPortal = &mExit;
        mSegment = mSegments.back();
        offset.z = mSegmentLength + 5;
      }
      
      // create our node
      (*mPortal) = mNode->createChildSceneNode(tmpName);
      
      // place it at the end of the tunnel or at the beginning
      Vector3 posPortal = mSegment->getPosition();
      posPortal.y = 70;
      posPortal.z += offset.z; 
      (*mPortal)->setPosition(posPortal);
      
    }
    
    
    mPortal = NULL;
    mSegment = NULL;
  };
  
  void Tunnel::show() {
  
    mSceneMgr->getRootSceneNode()->addChild(mNode);
    //mNode->setVisible(true);
    
    bindToName("SettingsChanged", this, &Tunnel::evtSettingsChanged);
    bindToName("PortalReached", this, &Tunnel::evtPortalReached);
    bindToName("PortalEntered", this, &Tunnel::evtPortalEntered);
    
    fPassedEntrance = false;
    fPortalReached = false;
    fPortalSighted = false;
    
    fHasSfx = GameManager::getSingleton().currentState()->areSfxEnabled();
    
    // attach portal particle system only if the player is actually playing
    // NOT in zone screen (hence we're not checking for GameState.running() )
    if (GameManager::getSingleton().currentState()->getId() == STATE_GAME) {
    //if (Level::getSingleton().running()) {
      //if (fHasFx) {
        /*if (mPortalEffect->isAttached())
          mPortalEffect->getParentSceneNode()->detachObject(mPortalEffect);
        mEntrance->attachObject(mPortalEffect);
        mPortalEffect->startAndStopFade(0.5f);*/
        //mPortalEffect->start();
      //}
      
      // and the teleporting sound effect
      if (fHasSfx) {
        if (!mSfxPortal->isAttached()) {
          //mSfxPortal->getParentSceneNode()->detachObject(mSfxPortal);
          
          mEntrance->attachObject(mSfxPortal);
        }
        //mSfxPortal->play(true);
      }

      Event* evt = mEvtMgr->createEvt("PortalEntered");
      mEvtMgr->hook(evt);
    }

    //mLog->infoStream() << "Tunnel" << idObject << " is rendered";
    
    /*mLog->debugStream() << "My length: " << mLength << ", sphere is at " 
      << mSphereNode->getPosition().x << ", "
      << mSphereNode->getPosition().y << ", "
      << mSphereNode->getPosition().z << 
      ". Portal reached? " << (fPortalReached ? "yes" : "no");*/
    

  };
  void Tunnel::hide() {
  
    //mNode->setVisible(false);
    //mSceneMgr->detachSceneNode(mNode);
    mSceneMgr->getRootSceneNode()->removeChild(mNode);
    
    unbind("PortalEntered", this);
    unbind("PortalReached", this);
    unbind("SettingsChanged", this);
    
    fPassedEntrance = false;
    fPortalReached = false;
    fPortalSighted = false;
    
    //mSphereNode = 0;
    //mSphere = 0;
    
    if (fHasFx) {
      if (mPortalEffect->isAttached())
        mPortalEffect->getParentSceneNode()->detachObject(mPortalEffect);
      mPortalEffect->stop();
    }
        
    mLog->infoStream() << "Tunnel" << idObject << " is hidden";
  };
  
  void Tunnel::update(unsigned long lTimeElapsed) {
    processEvents();
    
    //if (fHasSfx)
    //  mSfxPortal->update(lTimeElapsed);
    
    // if the player is past our entrance portal, we can stop showing it 
    /*if (!fPassedEntrance && mSphere->getMasterNode()->getPosition().z > mEntrance->getPosition().z + 1000) {
      mLog->debugStream() << "stopped portal effect";
      if (fHasFx) {
        mPortalEffect->stop();
        
        //mPortal = mExit;
        mEntrance->detachObject(mPortalEffect);
        mExit->attachObject(mPortalEffect);
        mPortalEffect->start();
      }
	    //if (mSfxPortal->isAttached())
      //mSfxPortal->getParentSceneNode()->detachObject(mSfxPortal);   
	    //mExit->attachObject(mSfxPortal);
      
      fPassedEntrance = true;
    };*/
    


  };
    
	bool Tunnel::evtPortalSighted(Event* inEvt) {
	  //mPortal = mExit;
	  
	  mLog->debugStream() << "spawned exit portal";
	  
	  return true;
	};    

	bool Tunnel::evtPortalReached(Event* inEvt) {
	  //hide();
	  
    if (fHasFx) {
      //mPortalEffect->setFastForward(175, 2);
      mPortalEffect->stopFade();
    }

	  return true;
	};
	bool Tunnel::evtPortalEntered(Event* inEvt) {
	  //
	  mLog->debugStream() << "a new portal is entered";
	  
	  if (mPortalEffect->isAttached())
	    mPortalEffect->getParentSceneNode()->detachObject(mPortalEffect);
    mExit->attachObject(mPortalEffect);
    mPortalEffect->start();
    
    if (fHasSfx)
      mSfxPortal->play(true);
    
    return true;
	};
	
	SceneNode* Tunnel::getExitPortal() {
	  return mExit;
	};    
  SceneNode* Tunnel::getEntrancePortal() {
	  return mEntrance;
	};
	
	SceneNode* Tunnel::getNode() { return mNode; };
	
	Real& Tunnel::_getLength() { return mLength; };
	Real& Tunnel::_getSegmentLength() { return mSegmentLength; };
	
	bool Tunnel::evtSettingsChanged(Event* inEvt) {
	  mLog->debugStream() << "parsing new settings";
    fHasSfx = GameManager::getSingleton().currentState()->areSfxEnabled();
    //fHasFx = GameManager::getSingleton().currentState()->areFxEnabled();

    return true;	
	}; 
};
