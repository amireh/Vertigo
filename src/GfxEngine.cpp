/*
 *  GfxEngine.cpp
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/11/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */

#include "GfxEngine.h"
#include "GameManager.h"
#include "OGRE/Terrain/OgreTerrain.h"
#include "OGRE/Terrain/OgreTerrainGroup.h"
#include "Intro.h"
#include "Procedural.h"
#include "Geometry.h"

namespace Pixy {
	
	GfxEngine* GfxEngine::_myGfxEngine = NULL;
	
	GfxEngine* GfxEngine::getSingletonPtr() {
		if( !_myGfxEngine ) {
		    _myGfxEngine = new GfxEngine();
		}
		
		return _myGfxEngine;
	}
	
	GfxEngine::GfxEngine() {
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "GfxEngine");
		mLog->infoStream() << "firing up";
		fSetup = false;
		//mPlayers.clear();
		mCameraMan = 0;
		//mFallVelocity = 0;
		//mSceneLoader = 0;
		//mMoveSpeed = 0.1;
		//mDirection = Ogre::Vector3::ZERO;
		fPortalReached = false;
		
	}
	
	GfxEngine::~GfxEngine() {
		mLog->infoStream() << "shutting down";
		if (fSetup) {
			mRoot = 0;
			mSceneMgr = 0;
			mCamera = mCamera2 = mCamera3 = mCamera4 = 0;
			mViewport = 0;
			mRenderWindow = 0;
			delete mLog;
			mLog = 0;

			//if (mSceneLoader)
			//	delete mSceneLoader;

			fSetup = false;
		}
	}
	
	bool GfxEngine::setup() {
		if (fSetup)
			return true;
		
		mRoot         = Ogre::Root::getSingletonPtr();
		//mOverlayMgr   = OverlayManager::getSingletonPtr();
		//if (mRoot->hasSceneManager(Ogre::ST_GENERIC))
		//	mSceneMgr     = mRoot->getSceneManager( ST_GENERIC, "CombatScene" );
		//else
		mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "CombatScene");
		
		
		mCamera       = mSceneMgr->createCamera("Sphere_Camera");

		mRenderWindow = mRoot->getAutoCreatedWindow();
		
		//mRenderWindow->reposition(1920 - mRenderWindow->getWidth(), 50);
		//mViewport     = mRenderWindow->addViewport( mCamera );

		//mRenderWindow->getViewport(<#unsigned short index#>)
		
		mEvtMgr = EventManager::getSingletonPtr();
		/*
		 if (GameManager::getSingleton().gameState() == STATE_COMBAT)
			setupCombat();
		*/
		
		setupSceneManager();
        setupViewports();
        
		 setupCamera();
        
		 setupTerrain();
		 setupLights();
		
        setupNodes();
		
    Geometry::createSphere("ObstacleMesh", 10, 16, 16);

		
		mTrayMgr = new OgreBites::SdkTrayManager("AOFTrayMgr", mRenderWindow, InputManager::getSingletonPtr()->getMouse(), 0);
		mTrayMgr->hideCursor();
    mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);

    mCameraMan = new OgreBites::SdkCameraMan(mCamera);
    //mRenderWindow->setActive(true);
    
    bindToName("ObstacleAlive", this, &GfxEngine::evtObstacleAlive);
    bindToName("ObstacleCollided", this, &GfxEngine::evtObstacleCollided);
    bindToName("PortalReached", this, &GfxEngine::evtPortalReached);
    bindToName("PortalSighted", this, &GfxEngine::evtPortalSighted);
    
		fSetup = true;
		return fSetup;
	}
	
	bool GfxEngine::deferredSetup() {
		mSphere = Intro::getSingleton().getSphere();
		//mCamera->setAutoTracking (true, mSphere->getSceneNode());
		//mCamera->setPosition(Vector3(200, 200, 200));
		//mCamera->lookAt(mSphere->getSceneNode()->getPosition());
		//mCamera->lookAt(mSphere->getSceneNode()->getPosition());
		
		setupParticles();
		//mSphere->getSceneNode()->attachObject(mSceneMgr->getLight("Light2"));
		return true;
	}
	

	void GfxEngine::setCamera(const Ogre::String& inCameraName) {
		mCamera = mSceneMgr->createCamera(inCameraName);
	}
	
	/*SdkCameraMan* GfxEngine::getCameraMan() {
		return mCameraMan;
	}*/
	Ogre::Camera* GfxEngine::getCamera() {
		return mCamera;
	}
	
	Ogre::Root* GfxEngine::getRoot() {
		return mRoot;
	}
	
	Ogre::SceneManager* GfxEngine::getSM() {
		return mSceneMgr;
	}
	
	Ogre::Viewport* GfxEngine::getViewport() {
		return mViewport;
	}
	
	bool GfxEngine::cleanup() {		
	  ParticleUniverse::ParticleSystemManager::getSingletonPtr()->destroyAllParticleSystems(mSceneMgr);
	  
		return true;
	}
	
	
    void GfxEngine::setupSceneManager()
    {
		  mLog->debugStream() << "setting up SceneManager";
          //mSceneMgr->setAmbientLight(Ogre::ColourValue(1,1,1));

		  mCamera = mSceneMgr->getCamera("Sphere_Camera");
		
		  //mViewport = mCamera->getViewport();
		  mViewport = mRenderWindow->addViewport(mCamera);
		  if (!mViewport) {
			  mLog->errorStream() << "Viewport doesn't exist!!!";
		  }
		
		  //mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
    };
	
    void GfxEngine::setupViewports()
    {
		  mLog->debugStream() << "setting up viewports";
      mViewport->setBackgroundColour(Ogre::ColourValue(255,255,255));
      
      Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "Radial Blur");
      
      // Alter the camera aspect ratio to match the viewport	
      mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
		
    };
	
	  void GfxEngine::applyMotionBlur(float duration) {
	    mEffect = "Radial Blur";
	    mEffectTimer.reset();
	    mEffectDuration = duration;
	    mEffectEnabled = true;
	    mLog->debugStream() << "applying motion blur"; 
	    Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, mEffect, mEffectEnabled);
	  };
	  

	  
	  
    void GfxEngine::setupCamera()
    {
		mLog->debugStream() << "setting up cameras";
        Vector3 lCamPos;
		
        mCamera->setNearClipDistance( 10 );
        mCamera->setFarClipDistance( 10000 );
        /*lCamPos = mPuppetPos[ME];
        lCamPos.y += 800;
        lCamPos.z += 1000;
		 */

        mCamera->setPosition(lCamPos);
		
        mCamera->lookAt(Ogre::Vector3(0, 0, 0));
		
	};
	
    void GfxEngine::setupTerrain()
    {
		
		/*
        mLog->noticeStream() << "Setting world geometry";
        mSceneMgr->setWorldGeometry("terrain.cfg");
		 */
        // skyz0rs
        mLog->noticeStream() << "Setting up sky";
		  //mSceneMgr->setSkyDome(true, "Examples/CloudySky", 2, 0.5);
		  mSceneMgr->setSkyBox(true, "Vertigo/Sky/Vortex", 5000, true);				
		 
		  Ogre::Entity* mEntity;
		  Ogre::SceneNode* mNode;
		  Procedural::Root::getInstance()->sceneManager = mSceneMgr;

		  int tube_length = 500;
		  float tube_radius = 80.0f;

		  Procedural::TubeGenerator()
		  .setOuterRadius(tube_radius)
		  .setInnerRadius(tube_radius - 2.0f)
		  .setHeight(tube_length)
		  .setNumSegBase(32)
		  .setNumSegHeight(1)
		  .realizeMesh("TubeMesh");
				
		  std::string mEntityName = "";
		  int nr_tubes = 30;
		  for (int i =0; i < nr_tubes; ++i) {
			  mEntityName = "myTube_";
			  mEntityName += i;

    		mEntity = mSceneMgr->createEntity(mEntityName, "TubeMesh");
			  mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			  mEntity->setMaterialName("Vertigo/Terrain");
			  mNode->attachObject(mEntity);
			  mNode->setPosition(Vector3(0, 70, i * tube_length));
			  //mNode->roll(Ogre::Degree(90));
			  mNode->pitch(Ogre::Degree(90));
			  //mNode->yaw(Ogre::Degree(30));
			  //mNode->showBoundingBox(true);			
			  mTubes.push_back(mNode);
		  }
		
    };
	
    void GfxEngine::setupLights()
    {
		  mLog->debugStream() << "setting up lights";
      mSceneMgr->setAmbientLight(Ogre::ColourValue(0.4f, 0.4f, 0.4f));
      Ogre::Light *light;

     light = mSceneMgr->createLight("Light2");
		 light->setType(Ogre::Light::LT_DIRECTIONAL);
		 //light->setPosition(Vector3(0, 0, 1000));
		 light->setDirection(Vector3(0,0,-1));
		 light->setDiffuseColour(0.9f, 0.9f, 0.9f);
		 light->setSpecularColour(0.9f, 0.9f, 0.9f);
		 

      /* now let's setup our light so we can see the shizzle */

      mSpotLight = mSceneMgr->createLight("TunnelLight0");
      mSpotLight->setType(Ogre::Light::LT_POINT);
      mSpotLight->setPosition(Vector3(0, 0, 0));
      mSpotLight->setDiffuseColour(0.8f, 0.8f, 0.8f);
      mSpotLight->setSpecularColour(0.8f, 0.8f, 0.8f);
      
      mSpotLight = mSceneMgr->createLight("TunnelLight1");
      mSpotLight->setType(Ogre::Light::LT_POINT);
      mSpotLight->setPosition(Vector3(0, 20, 500));
      mSpotLight->setDiffuseColour(0.2f, 0.2f, 0.2f);
      mSpotLight->setSpecularColour(0.2f, 0.2f, 0.2f);
		  
		  mSpotLight = mSceneMgr->createLight("TunnelLight2");
      mSpotLight->setType(Ogre::Light::LT_POINT);
      mSpotLight->setPosition(Vector3(30, 0, 2500));
      mSpotLight->setDiffuseColour(0.2f, 0.2f, 0.2f);
      mSpotLight->setSpecularColour(0.2f, 0.2f, 0.2f);
      
      mSpotLight = mSceneMgr->createLight("TunnelLight3");
      mSpotLight->setType(Ogre::Light::LT_POINT);
      mSpotLight->setPosition(Vector3(0, 30, 4500));
      mSpotLight->setDiffuseColour(0.8f, 0.2f, 0.2f);
      mSpotLight->setSpecularColour(0.8f, 0.2f, 0.2f);
      
      /* now let's setup our light so we can see the shizzle */
      /*mSpotLight = mSceneMgr->createLight("PlayerLight2");
      mSpotLight->setType(Ogre::Light::LT_DIRECTIONAL);
      mSpotLight->setDirection(Vector3(1,1,-1));
      mSpotLight->setDiffuseColour(0.6f, 0.6f, 0.6f);
      mSpotLight->setSpecularColour(0.8f, 0.8f, 0.8f);*/		  
		 
		 Ogre::ColourValue fadeColour(0.0f, 0.0f, 0.0f);
     //mViewport->setBackgroundColour(fadeColour);
		 //mSceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0.0, 2000, 3000);
    };
	
    void GfxEngine::setupNodes()
    {
		  //mLog->debugStream() << "setting up nodes";
		
    };

	
    Ogre::SceneNode* 
	GfxEngine::createNode(std::string& inName, 
						  const Vector3& inPosition, 
						  const Vector3& inScale, 
						  const Vector3& inDirection, 
						  Ogre::SceneNode* inParent)
    {
        Ogre::SceneNode* mNode;
        if (!inParent)
            inParent = mSceneMgr->getRootSceneNode();
		
        mNode = inParent->createChildSceneNode(inName, inPosition);
        mNode->setScale(inScale);
        mNode->lookAt(inDirection, Ogre::Node::TS_WORLD);
    	//mNode->showBoundingBox(true);
		
        //mNode = NULL;
        return mNode;
    };
	
    Ogre::SceneNode* 
	GfxEngine::renderEntity(Pixy::Entity* inEntity, Ogre::SceneNode* inNode)
    {
		//mLog->debugStream() << "rendering entity " << inEntity->getName();
    Ogre::Entity* mEntity;
		
    String entityName = "Entity";
		entityName += inEntity->getName();
		entityName += stringify(inEntity->getObjectId());
		
		/*
		if (inEntity->type() == OBSTACLE) {
		  String meshName = "MeshObstacle";
		  meshName += stringify(inEntity->getObjectId());
		
		  // create obstacles mesh
		  createSphere(meshName, 2, 16, 16);
		  inEntity->setMesh(meshName);
		}
		*/
		//mLog->debugStream() << "Creating an Entity with name " << entityName;
		mEntity = mSceneMgr->createEntity(entityName, inEntity->getMesh());
		inNode->setUserAny(Ogre::Any(inEntity));
		inNode->attachObject(mEntity);
		
		
		inEntity->attachSceneNode(inNode);
		inEntity->attachSceneObject(mEntity);
		
		return inNode;// create aureola around ogre head perpendicular to the ground
    };
	
	
    bool GfxEngine::attachToScene(Pixy::Entity* inEntity)
    {
		Ogre::String nodeName = "Node";
		nodeName += inEntity->getName();
		nodeName += stringify(inEntity->getObjectId());
		//mLog->debugStream() << "Creating a SceneNode with name " << nodeName;
		return renderEntity(inEntity, createNode(nodeName, 
												 Vector3::ZERO, 
												 Vector3(1.0,1.0,1.0), 
												 Vector3::ZERO,
												 NULL)) && true;
    };
	
	
	
    void GfxEngine::detachFromScene(Pixy::Entity* inEntity)
    {
        //Ogre::String ownerName = inEntity->getOwner();// == ID_HOST) ? "host" : "client";
        Ogre::SceneNode* mTmpNode = NULL;
		
		mTmpNode = inEntity->getSceneNode();
		
		
		//mLog->debugStream() << "I'm detaching Entity '" << inEntity->getName() << "' from SceneNode : " + mTmpNode->getName();
		mTmpNode->showBoundingBox(false);
		mTmpNode->detachObject(inEntity->getSceneObject());
		
		mSceneMgr->destroyEntity((Ogre::Entity*)inEntity->getSceneObject());
		
    }

	
	void GfxEngine::mouseMoved( const OIS::MouseEvent &e )
	{
		if (mCameraMan)
			mCameraMan->injectMouseMove(e);
	}
	
	void GfxEngine::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
	{
		if (mCameraMan)
			mCameraMan->injectMouseDown(e, id);
	}
	
	void GfxEngine::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
	{
		if (mCameraMan)
			mCameraMan->injectMouseUp(e, id);
	}
	
	
	void GfxEngine::keyPressed( const OIS::KeyEvent &e )
	{
	  //mCameraMan->injectKeyDown(e);
		switch (e.key) {
		/*
			case OIS::KC_UP:
				mCamera->move(Vector3(0, 0, 1));
				break;
			case OIS::KC_DOWN:
				mCamera->move(Vector3(0, 0, -1));
				break;
			case OIS::KC_LEFT:
				mCamera->move(Vector3(-1, 0, 0));
				break;
			case OIS::KC_RIGHT:
				mCamera->move(Vector3(1, 0, 0));
				break;
			case OIS::KC_T:
				mCamera->move(Vector3(0, 1, 0));
				break;*/
			case OIS::KC_G:
				applyMotionBlur(500);
				break;	
		  case OIS::KC_R:
		    playEffect("Atomicity", mSphere);
		    break;
		  case OIS::KC_T:
		    playEffect("BlackHole", mSphere);
		    break;
		  case OIS::KC_Y:
		    playEffect("Explosion", mSphere);
		    break;
		  case OIS::KC_H:
		    playEffect("Shatter", mSphere);
		    break;		    
		  case OIS::KC_F:
		    playEffect("Despawn", mSphere);
		    break;		    
			
		  case OIS::KC_P:
	      time_t seconds;
        seconds = time (NULL);
        std::string fileName = "Screenshot_";
        fileName += seconds;
        fileName += ".png";
		    mRenderWindow->writeContentsToFile(fileName);
		    break;
		}
	}
	
	void GfxEngine::keyReleased( const OIS::KeyEvent &e ) {
			//mCameraMan->injectKeyUp(e);
	}
	
	


	void GfxEngine::update(unsigned long lTimeElapsed) {
		processEvents();

	  //if (shakingScreen)
	    //applyScreenShake(lTimeElapsed);
		if (fPortalReached)
		  return;
		
		if (mSphere->getMasterNode()->getPosition().z >= mPortal->getPosition().z) {
		  Event* evt = mEvtMgr->createEvt("PortalReached");
		  mEvtMgr->hook(evt);
      mSphere->getMasterNode()->setPosition(mPortal->getPosition());
      
      return;		
		};
		
		
		if (!fPortalSighted && mSphere->getSceneNode()->_getWorldAABB().intersects(mPortal->_getWorldAABB())) {
		  Event* evt = mEvtMgr->createEvt("PortalSighted");
		  mEvtMgr->hook(evt);
		  return;
		};
		
		if (mEffectEnabled && mEffectTimer.getMilliseconds() > mEffectDuration * 1000) {
		  mEffectTimer.reset();
		  mEffectEnabled = false;
		  Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, mEffect, mEffectEnabled);
		}
		
		mSpherePos = mSphere->getPosition();
		
		mCamera->setPosition(
		  mSpherePos.x,
      mSpherePos.y + 35,
      mSpherePos.z-120
    );
	  
	  //mCamera->setOrientation(mSphere->getMasterNode()->getOrientation());
	  
	  mCamera->lookAt(
	    mSpherePos.x,
	    mSpherePos.y + 20,
	    mSpherePos.z
	  );
							 
		evt.timeSinceLastEvent = lTimeElapsed;
		evt.timeSinceLastFrame = lTimeElapsed;
		
		mTrayMgr->frameRenderingQueued(evt);
		//mCameraMan->update(lTimeElapsed);
		//using namespace Ogre;
		
	}
	
  
	void GfxEngine::setupParticles() {
	  //using namespace ParticleUniverse;
		//ParticleSystem::setDefaultNonVisibleUpdateTimeout(1);  // set nonvisible timeout

		/*ParticleSystem* ps;
    // create aureola around ogre head perpendicular to the ground
    ps = mSceneMgr->createParticleSystem("Nimbus", "Examples/GreenyNimbus");
		mSphere->getSceneNode()->attachObject(ps);*/

    ParticleUniverse::ParticleSystemManager* fxMgr = 
      ParticleUniverse::ParticleSystemManager::getSingletonPtr();
      
    ParticleUniverse::ParticleSystem* effect = NULL;
    
    effect = fxMgr->createParticleSystem(
      "FxExplosion",
      "Vertigo/FX/Explosion", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("Explosion", effect));

    effect = fxMgr->createParticleSystem(
      "FxShatter",
      "Vertigo/FX/Shatter", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("Shatter", effect));
        
    effect = fxMgr->createParticleSystem(
      "FxBlackHole",
      "Vertigo/FX/BlackHole", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("BlackHole", effect));

    effect = fxMgr->createParticleSystem(
      "FxAtomicity",
      "Vertigo/FX/SphereTrail", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("Atomicity", effect));    

    effect = fxMgr->createParticleSystem(
      "FxPortal",
      "Vertigo/FX/Portal", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("Portal", effect));    
    
    
    effect = fxMgr->createParticleSystem(
      "FxSpawnPoint",
      "Vertigo/FX/SpawnPoint", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("SpawnPoint", effect));  

    effect = fxMgr->createParticleSystem(
      "FxDespawn",
      "Vertigo/FX/Despawn", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("Despawn", effect));  
        
    effectMap::iterator itr;
    for (itr = effects.begin(); itr != effects.end(); ++itr) {
      effect = itr->second;
      effect->setScale(Ogre::Vector3(0.2f, 0.2f, 0.2f));
      effect->setScaleVelocity(0.2f);
    }
    
    effect = NULL;
    //mSphere->getMasterNode()->attachObject(effectExplosion);
    //effectExplosion->start();

	  effect = effects["Portal"];
    mPortal = mSceneMgr->getRootSceneNode()->createChildSceneNode("NodePortal");
    mPortal->attachObject(effect);

    Vector3 posPortal = mTubes.back()->getPosition();
    posPortal.y = 70;
    posPortal.z += 500; 
    mPortal->setPosition(posPortal);
    effect->start();
    mPortal->showBoundingBox(true);
    
    mPortableEffect = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mSpawnPoint = mSceneMgr->getRootSceneNode()->createChildSceneNode("NodeSpawnPoint");
    mSpawnPoint->attachObject(effects["Despawn"]);
    
	}
	
	void GfxEngine::playEffect(std::string inEffect, Entity* inEntity) {
	  effectMap::iterator cursor = effects.find(inEffect);
	  if (cursor != effects.end()) {
	    ParticleUniverse::ParticleSystem* effect = cursor->second;
	    if (effect->isAttached())
	      effect->getParentSceneNode()->detachObject(effect);
	    inEntity->getMasterNode()->attachObject(effect);
	    if (inEffect == "BlackHole")
	      effect->startAndStopFade(1);
	    else
	      effect->start();
	  }
	};
	
	void GfxEngine::playEffect(std::string inEffect, const Vector3& inPos) {
	  effectMap::iterator cursor = effects.find(inEffect);
	  if (cursor != effects.end()) {
	    ParticleUniverse::ParticleSystem* effect = cursor->second;
	    if (effect->isAttached())
	      effect->getParentSceneNode()->detachObject(effect);
	    
	    mPortableEffect->setPosition(inPos);
	    mPortableEffect->attachObject(effect);
      effect->start();
	  }	
	};
	
	
	bool GfxEngine::evtObstacleAlive(Event* inEvt) {
	  Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  
	  Vector3 pos = mObs->getMasterNode()->getPosition();
	  pos.y += 30;
	  mSpawnPoint->setPosition(pos);
	  effects["Despawn"]->start();
	  //playEffect("SpawnPoint", pos);
	  
	  return true;
	};
	
	bool GfxEngine::evtObstacleCollided(Event* inEvt) {
	  Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  if (mObs->shield() == FIRE) {
	    playEffect("Explosion", mSphere);
	    if (mSphere->shield() == FIRE)
	      applyMotionBlur(0.5f);
	  } else {
	    playEffect("Shatter", mSphere);
	    if (mSphere->shield() == ICE)
	      applyMotionBlur(0.5f);
	    
	  }
	  
	  return true;
	};
	
	bool GfxEngine::evtPortalSighted(Event* inEvt) {
	
	  fPortalSighted = true;
	  
	  return true;
	};
	
	bool GfxEngine::evtPortalReached(Event* inEvt) {
	  mSphere->die();
	  mSpawnPoint->setVisible(false);
		mSphere->getSceneNode()->setVisible(false);
		fPortalReached = true;
		playEffect("Despawn", mSphere);
		
		
		return true;	  
	};
	
	SceneNode* GfxEngine::getPortal() { return mPortal; };
}
