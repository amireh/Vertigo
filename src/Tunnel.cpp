#include "Tunnel.h"
#include "GfxEngine.h"
#include "StateGame.h"

namespace Pixy {

  int Tunnel::nrTunnels = 0;
  bool Tunnel::fMeshGenerated = false;
  bool Tunnel::fEffectPrepared = false;
  ParticleUniverse::ParticleSystem* Tunnel::mPortalEffect = NULL;
  
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
    mSphere = StateGame::getSingletonPtr()->getSphere();
    if (mSphere)
      mSphereNode = mSphere->getMasterNode();
    else
      mLog->warnStream() << "could not get a handle on Sphere";
    
    // create our master node and set its starting position
    mNode = mSceneMgr->getRootSceneNode()->
      createChildSceneNode(String("Terrain/Tunnel/" + stringify(idObject)));
    mNode->setPosition(inPosition);
    
    
    mEntrance = mExit = NULL;
    fPassedEntrance = false;
    fPortalSighted = false;
    fPortalReached = false;
    
    mLog->debugStream() << "created";
    
    generateSegments();
    generatePortals();
    
    mNode->setVisible(false);
    
    if (inAutoShow)
      show();
  };
  
  Tunnel::~Tunnel() {
    if (mPortalEffect) {
      //mFxMgr->destroyParticleSystem(mPortalEffect);
      mPortalEffect->stop();
      mPortalEffect = NULL;
    }
    
    // TODO remove scene nodes
    
    if (mLog)
      delete mLog;
    
    mSphere = NULL;
    mSphereNode = NULL;
    mLog = NULL;
    mGfxEngine = NULL;
    mFxMgr = NULL;
    mSceneMgr = NULL;
  };
  
  void Tunnel::generateSegments() {

    mLog->debugStream() << "generating segments";
    
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
  
    mLog->debugStream() << "generating portals";
    
    if (!fEffectPrepared) {
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
  
    mNode->setVisible(true);
    
    bindToName("PortalSighted", this, &Tunnel::evtPortalSighted);
    bindToName("PortalReached", this, &Tunnel::evtPortalReached);
    
    fPassedEntrance = false;
    fPortalReached = false;
    fPortalSighted = false;
    
    mSphereNode = mSphere->getMasterNode();
    
    
    
    if (mPortalEffect->isAttached())
      mPortalEffect->getParentSceneNode()->detachObject(mPortalEffect);
    mEntrance->attachObject(mPortalEffect);
    mPortalEffect->start();
    
    Event* evt = mEvtMgr->createEvt("PortalEntered");
    mEvtMgr->hook(evt);
	    
    mLog->infoStream() << "Tunnel" << idObject << " is rendered";
    
    mLog->debugStream() << "My length: " << mLength << ", sphere is at " 
      << mSphereNode->getPosition().x << ", "
      << mSphereNode->getPosition().y << ", "
      << mSphereNode->getPosition().z << 
      ". Portal reached? " << (fPortalReached ? "yes" : "no");
  };
  void Tunnel::hide() {
  
    mNode->setVisible(false);
    
    unbind("PortalSighted");
    unbind("PortalReached");
    
    fPassedEntrance = false;
    fPortalReached = false;
    fPortalSighted = false;
    
    mSphereNode = NULL;
    
    if (mPortalEffect->isAttached())
      mPortalEffect->getParentSceneNode()->detachObject(mPortalEffect);
    mPortalEffect->stop();
    
    mLog->infoStream() << "Tunnel" << idObject << " is hidden";
  };
  
  void Tunnel::update(unsigned long lTimeElapsed) {
    processEvents();
    
    // if the player is past our entrance portal, we can stop showing it 
    if (!fPassedEntrance && mSphere->getMasterNode()->getPosition().z > mEntrance->getPosition().z + 1000) {
      mLog->debugStream() << "stopped portal effect";
      mPortalEffect->stop();
      
      //mPortal = mExit;
      mEntrance->detachObject(mPortalEffect);
	    mExit->attachObject(mPortalEffect);
	    mPortalEffect->start();      
      
      fPassedEntrance = true;
    };
    
	  if (!fPortalSighted && mSphereNode->getPosition().z >= mSegments.back()->getPosition().z) {
	    mLog->debugStream() << "exit portal is sighted";
	    
	    Event* evt = mEvtMgr->createEvt("PortalSighted");
	    mEvtMgr->hook(evt);
	    
	    mLog->debugStream() << "Portal reached? " << (fPortalReached ? "yes" : "no")
	      << ", node position z : " << mSphereNode->getPosition().z;
	    
	    fPortalReached = false;
	    fPortalSighted = true;
	  };

	  if (!fPortalReached && mSphereNode->getPosition().z >= mLength) {
	    mLog->debugStream() << "exit portal is reached";
	    
	    Event* evt = mEvtMgr->createEvt("PortalReached");
	    mEvtMgr->hook(evt);
      //mSphereNode->setPosition(mExit->getPosition());
      
      return;		
	  };

  };
    
	bool Tunnel::evtPortalSighted(Event* inEvt) {
	  //mPortal = mExit;
	  
	  mLog->debugStream() << "spawned exit portal";
	  
	  return true;
	};    

	bool Tunnel::evtPortalReached(Event* inEvt) {
	  //hide();
	  
	  fPortalReached = true;
	  return true;
	};
	
	SceneNode* Tunnel::getExitPortal() {
	  return mExit;
	};    
  SceneNode* Tunnel::getEntrancePortal() {
	  return mEntrance;
	};
	
	SceneNode* Tunnel::getNode() { return mNode; }; 
};
