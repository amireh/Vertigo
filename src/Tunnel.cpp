#include "Tunnel.h"
#include "GfxEngine.h"
#include "StateGame.h"

namespace Pixy {

  int Tunnel::idTunnel = 0;
  bool Tunnel::fMeshGenerated = 0;
  bool Tunnel::fEffectPrepared = 0;
  ParticleSystem* Tunnel::mPortalEffect = NULL;
  
  Tunnel::Tunnel(const int inNrSegments, 
         const Real inSegmentLength, 
         const Real inRadius, 
         const String& inMaterial, 
         const Vector3& inPosition,
         bool inAutoShow) {
    
    ++idTunnel;
    mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Tunnel" + stringify(idTunnel));
    
    mNrSegments = inNrSegments;
    mSegmentLength = inSegmentLength;
    mRadius = inRadius;
    mMaterial = inMaterial;
    
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
      createChildSceneNode(String("Terrain/Tunnel/" + stringify(idTunnel)));
    mNode->setPosition(inPosition);

    mEntrance = mExit = NULL;
    fPassedEntrance = false;
    fPortalSighted = false;
    
    generateSegments();
    generatePortals();
    
    bindToName("PortalSighted", this, &Tunnel::evtPortalSighted);
    bindToName("PortalReached", this, &Tunnel::evtPortalReached);
    
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

    if (!fMeshGenerated) {
      Procedural::Root::getInstance()->sceneManager = mSceneMgr;
            
	    Procedural::TubeGenerator()
	    .setOuterRadius(mRadius)
	    .setInnerRadius(mRadius - 2.0f)
	    .setHeight(mSegmentLength)
	    .setNumSegBase(32)
	    .setNumSegHeight(1)
	    .realizeMesh("TubeMesh");
			
			fMeshGenerated = true;
	  }

	  Vector3 offset = mNode->getPosition();
	  Ogre::Entity* tmpEntity;
	  Ogre::SceneNode* tmpNode;
	  Ogre::String tmpName = "";
	  for (int i =0; i < mNrSegments; ++i) {
		  tmpName = String("Tunnel" + stringify(idTunnel) + "/Segment" + stringify(i));

  		tmpEntity = mSceneMgr->createEntity(tmpName, "TunnelMesh");
		  tmpNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
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
	  
	  tmpNode = NULL;
	  tmpEntity = NULL;
  };
  
  void Tunnel::generatePortals() {
  
    if (!fEffectPrepared) {
      mPortalEffect = mFxMgr->createParticleSystem(
        "FxPortal",
        "Vertigo/FX/Portal", 
        mSceneMgr);
      mPortalEffect->prepare();
      fEffectPrepared = true;
    }

    Portal* mPortal;
    SceneNode* mSegment;
    Ogre::String tmpName;
    Vector3 offset;
    int i;
    for (i = 0; i < 2; ++i) {
      tmpName = String("Tunnel" + stringify(idTunnel) + "/Portal/" + stringify(i));
      
      if (i % 2 == 0) {
        mPortal = mEntrance;
        mSegment = mSegments.front();
        offset.z = 0;
      } else {
        mPortal = mExit;
        mSegment = mSegments.back();
        offset.z = mSegmentLength;
      }
      
      // create our node
      mPortal = mSceneMgr->getRootSceneNode()->createChildSceneNode(tmpName);
      
      // place it at the end of the tunnel or at the beginning
      Vector3 posPortal = mSegment->getPosition();
      posPortal.y = 70;
      posPortal.z += offset.z; 
      mPortal->setPosition(posPortal);
      
    }
    mEntrance->attachObject(mPortalEffect);
    
    mPortal = mEntrance;
    mSegment = NULL;
  };
  
  void Tunnel::show() {
    mNode->setVisible(true);
    mPortalEffect->start();
  };
  void Tunnel::hide() {
    mNode->setVisible(false);
    mPortalEffect->stop();    
  };
  
  void Tunnel::update(unsigned long lTimeElapsed) {

	  if (mSphereNode->getPosition().z >= mPortal->getPosition().z) {
	    Event* evt = mEvtMgr->createEvt("PortalReached");
	    mEvtMgr->hook(evt);
      mSphereNode->setPosition(mPortal->getPosition());
      
      return;		
	  };

    // if the player is past our entrance portal, we can stop showing it 
    if (!fPassedEntrance && mSphere->getMasterNode()->getPosition().z > mEntrance->getPosition().z + 1000) {
      mPortalEffect->stop();
      mPortal = mExit;
      mEntrance->detachObject(mPortalEffect);
      
      fPassedEntrance = true;
    };
    		
	  if (!fPortalSighted && mSphereNode->getPosition().z >= mSegments.back()->getPosition().z) {
	    Event* evt = mEvtMgr->createEvt("PortalSighted");
	    mEvtMgr->hook(evt);
	    
	    fPortalSighted = true;
	  };
  };
    
	bool Tunnel::evtPortalSighted(Event* inEvt) {
	  mPortal = mExit;
	  mPortal->attachObject(mPortalEffect);
	  mPortalEffect->start();
	  fPortalSighted = true;
	  
	  return true;
	};    

	bool Tunnel::evtPortalReached(Event* inEvt) {
	  hide();
	  return true;
	};    

};
