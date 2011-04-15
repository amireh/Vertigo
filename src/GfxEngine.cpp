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
#include "Level.h"
#include "Pause.h"
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
		//mCameraMan = 0;
		//mFallVelocity = 0;
		//mSceneLoader = 0;
		//mMoveSpeed = 0.1;
		//mDirection = Ogre::Vector3::ZERO;
		//fPortalReached = false;
		
	}
	
	GfxEngine::~GfxEngine() {
		mLog->infoStream() << "shutting down";
		if (fSetup) {


			delete mLog;
			mLog = 0;

			//if (mSceneLoader)
			//	delete mSceneLoader;

			fSetup = false;
		}
		
		_myGfxEngine = NULL;
	}
	
	void GfxEngine::loadResources() {

	  
	};
	bool GfxEngine::setup() {
		if (fSetup)
			return true;
		
		mRoot         = Ogre::Root::getSingletonPtr();
		mOverlayMgr   = Ogre::OverlayManager::getSingletonPtr();
		if (mRoot->hasSceneManager("GameScene"))
			mSceneMgr     = mRoot->getSceneManager( "GameScene" );
		else
		mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "GameScene");
		
		if (mSceneMgr->hasCamera("Sphere_Camera"))
		  mCamera = mSceneMgr->getCamera("Sphere_Camera");
		else
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
    // set up sky only in game state
    if (GameManager::getSingleton().currentState()->getId() == STATE_GAME) {
      
    } else if (GameManager::getSingleton().currentState()->getId() == STATE_INTRO) {
      // set up a nice intro scene
      
    };
    setupLights();
    setupNodes();
    setupParticles();
		
		mDirection = Vector3(0,0,1);
		mMoveSpeed = 0;
    //mCameraMan = new OgreBites::SdkCameraMan(mCamera);
    //mRenderWindow->setActive(true);

    
    bindToName("GameShown", this, &GfxEngine::evtGameShown);
    bindToName("MenuShown", this, &GfxEngine::evtMenuShown);
    
		fSetup = true;
		return fSetup;
	}
	
	bool GfxEngine::deferredSetup() {
		
		//mCamera->setAutoTracking (true, mSphere->getSceneNode());
		//mCamera->setPosition(Vector3(200, 200, 200));
		//mCamera->lookAt(mSphere->getSceneNode()->getPosition());
		//mCamera->lookAt(mSphere->getSceneNode()->getPosition());
		
		GameState *currentState = (GameManager::getSingleton().currentState());
		if (currentState->getId() == STATE_INTRO) {
		  mUpdate = &GfxEngine::updateIntro;
		  return true;
		}
		
		mSphere = Level::getSingleton().getSphere();
		mUpdate = &GfxEngine::updateGame;
				
		mCamera->setPosition(Vector3(0,75, -200));
		mCamera->lookAt(Vector3(0,75, 100));

    bindToName("ZoneEntered", this, &GfxEngine::evtZoneEntered);
    bindToName("GameStarted", this, &GfxEngine::evtGameStarted);
    bindToName("PlayerWon", this, &GfxEngine::evtPlayerWon);
    bindToName("SphereDied", this, &GfxEngine::evtSphereDied);    
    //bindToName("ObstacleAlive", this, &GfxEngine::evtObstacleAlive);
    bindToName("ObstacleCollided", this, &GfxEngine::evtObstacleCollided);
    bindToName("PortalEntered", this, &GfxEngine::evtPortalEntered);
    //bindToName("PortalReached", this, &GfxEngine::evtPortalReached);
    //bindToName("PortalSighted", this, &GfxEngine::evtPortalSighted);
    
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
    if (!fSetup)
      return true;
    
	  ParticleUniverse::ParticleSystemManager::getSingletonPtr()->destroyAllParticleSystems(mSceneMgr);
	  Ogre::CompositorManager::getSingleton().removeCompositor(mViewport, "Radial Blur");
	  mSceneMgr->destroyAllLights();
	  mRenderWindow->removeViewport(0);
	  
		mRoot = 0;
		mSceneMgr = 0;
		mCamera  = 0;
		mViewport = 0;
		mRenderWindow = 0;	  
  
    fSetup = false;
    
		return true;
	}
	
	
    void GfxEngine::setupSceneManager()
    {
		  mLog->debugStream() << "setting up SceneManager";
          //mSceneMgr->setAmbientLight(Ogre::ColourValue(1,1,1));

		  //mCamera = mSceneMgr->getCamera("Sphere_Camera");
		
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
      mViewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
      
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
	
      mCamera->setNearClipDistance( 10 );
      mCamera->setFarClipDistance( 10000 );
      mCamera->setPosition(Vector3(0,70,-200));
	    mCamera->lookAt(Ogre::Vector3(0, 70, 0));
	    
	    setCameraMode(CAMERA_CHASE);
	  };
	
    void GfxEngine::setupTerrain()
    {
		
		/*
        mLog->noticeStream() << "Setting world geometry";
        mSceneMgr->setWorldGeometry("terrain.cfg");
		 */
        // skyz0rs
        mLog->noticeStream() << "Setting up sky";
		  mSceneMgr->setSkyDome(true, "Vertigo/Sky", 2, 1);
		  //mSceneMgr->setSkyBox(true, "Vertigo/Sky/Vortex", 5000, true);				
		 

		
    };
	
    void GfxEngine::setupLights()
    {
		  mLog->debugStream() << "setting up lights";
      mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
      Ogre::Light *light;

     light = mSceneMgr->createLight("Light2");
		 light->setType(Ogre::Light::LT_DIRECTIONAL);
		 //light->setPosition(Vector3(0, 0, 1000));
		 light->setDirection(Vector3(0,0,-1));
		 light->setDiffuseColour(0.9f, 0.9f, 0.9f);
		 light->setSpecularColour(0.9f, 0.9f, 0.9f);

     light = mSceneMgr->createLight("Light3");
		 light->setType(Ogre::Light::LT_DIRECTIONAL);
		 //light->setPositiontion(Vector3(0, 0, 1000));
		 light->setDirection(Vector3(0,-0.5f,1));
		 light->setDiffuseColour(0.9f, 0.9f, 0.9f);
		 light->setSpecularColour(0.6f, 0.6f, 0.6f);
		 

      /* now let's setup our light so we can see the shizzle */

      mSpotLight = mSceneMgr->createLight("TunnelLight0");
      mSpotLight->setType(Ogre::Light::LT_POINT);
      mSpotLight->setPosition(Vector3(0, 60, 30));
      mSpotLight->setDiffuseColour(0.3f, 0.3f, 0.3f);
      mSpotLight->setSpecularColour(0.3f, 0.3f, 0.3f);
      
      mSpotLight = mSceneMgr->createLight("TunnelLight1");
      mSpotLight->setType(Ogre::Light::LT_POINT);
      mSpotLight->setPosition(Vector3(0, 20, 500));
      mSpotLight->setDiffuseColour(0.5f, 0.5f, 0.5f);
      mSpotLight->setSpecularColour(0.5f, 0.5f, 0.5f);
		  
		  mSpotLight = mSceneMgr->createLight("TunnelLight2");
      mSpotLight->setType(Ogre::Light::LT_POINT);
      mSpotLight->setPosition(Vector3(30, 0, 2500));
      mSpotLight->setDiffuseColour(0.9f, 0.9f, 0.9f);
      mSpotLight->setSpecularColour(0.3f, 0.3f, 0.3f);
      
      mSpotLight = mSceneMgr->createLight("TunnelLight3");
      mSpotLight->setType(Ogre::Light::LT_POINT);
      mSpotLight->setPosition(Vector3(0, 30, 4500));
      mSpotLight->setDiffuseColour(0.1f, 0.1f, 0.1f);
      mSpotLight->setSpecularColour(0.9f, 0.9f, 0.9f);
      
      /* now let's setup our light so we can see the shizzle */
      /*mSpotLight = mSceneMgr->createLight("PlayerLight2");
      mSpotLight->setType(Ogre::Light::LT_DIRECTIONAL);
      mSpotLight->setDirection(Vector3(1,1,-1));
      mSpotLight->setDiffuseColour(0.6f, 0.6f, 0.6f);
      mSpotLight->setSpecularColour(0.8f, 0.8f, 0.8f);*/		  
		 
		 Ogre::ColourValue fadeColour(0.0f, 0.0f, 0.0f);
     //mViewport->setBackgroundColour(fadeColour);
		 //mSceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0.0, 300, 800);
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
        if (!inParent) {
          mNode = mSceneMgr->createSceneNode(inName);
          mNode->setPosition(inPosition);
        } else
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
		
		  //mLog->debugStream() << "Creating an Entity with name " << entityName;
		  mEntity = mSceneMgr->createEntity(entityName, inEntity->getMesh());
		  //inNode->setUserAny(Ogre::Any(inEntity));
		  inNode->attachObject(mEntity);
		
		
		  inEntity->attachSceneNode(inNode);
		  inEntity->attachSceneObject(mEntity);
		
		  return inNode;// create aureola around ogre head perpendicular to the ground
    };
	
	
    bool GfxEngine::attachToScene(Pixy::Entity* inEntity)
    {
      /*if (inEntity->getSceneNode() && inEntity->getSceneObject()) {
        inEntity->getSceneNode()->setVisible(true);
        return true;
      }*/
      
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
        Ogre::SceneNode* mTmpNode = NULL;
		    //inEntity->getSceneNode()->setVisible(false);
		    mTmpNode = inEntity->getSceneNode();

		    //mLog->debugStream() << "I'm detaching Entity '" << inEntity->getName() << "' from SceneNode : " + mTmpNode->getName();
		    //mTmpNode->showBoundingBox(false);
		    mTmpNode->detachObject(inEntity->getSceneObject());
		
		    mSceneMgr->destroyEntity((Ogre::Entity*)inEntity->getSceneObject());
		    mSceneMgr->destroySceneNode(inEntity->getSceneNode());
	      
    }

	
	void GfxEngine::mouseMoved( const OIS::MouseEvent &e )
	{
		//if (mCameraMan)
		//	mCameraMan->injectMouseMove(e);
	}
	
	void GfxEngine::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
	{
		//if (mCameraMan)
		//	mCameraMan->injectMouseDown(e, id);
	}
	
	void GfxEngine::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
  {
  
	}
	
	
	void GfxEngine::keyPressed( const OIS::KeyEvent &e )
	{
	  std::string fileName;
	  time_t seconds;
		switch (e.key) {   
		  case OIS::KC_K:
		    // pause the game
		    GameManager::getSingleton().pushState(Pause::getSingletonPtr());
	      
	      // take a screenshot
        seconds = time (NULL);
        fileName = "Screenshot_";
        fileName += stringify(seconds);
        fileName += ".png";
		    mRenderWindow->writeContentsToFile(fileName);
		    
		    // resume the game
		    GameManager::getSingleton().popState();
		    
		    break;
		  case OIS::KC_1:
		    setCameraMode(CAMERA_CHASE);
		    break;
		  case OIS::KC_2:
		    setCameraMode(CAMERA_FIXED);
		    break;
		  case OIS::KC_3:
		    setCameraMode(CAMERA_STICKY);
		    break;
		  case OIS::KC_4:
		    setCameraMode(CAMERA_LOLLY);
		    break;
		}
	}
	
	void GfxEngine::keyReleased( const OIS::KeyEvent &e ) {
			//mCameraMan->injectKeyUp(e);
	}
	
	

	void GfxEngine::update(unsigned long lTimeElapsed) {
	  processEvents();
	  
	  (this->*mUpdate)(lTimeElapsed);
	};
	
	void GfxEngine::updateIntro(unsigned long lTimeElapsed) {
	};
	
	void GfxEngine::updateNothing(unsigned long lTimeElapsed) {
	
	};
	
	void GfxEngine::updateGame(unsigned long lTimeElapsed) {
		
		if (mEffectEnabled && mEffectTimer.getMilliseconds() > mEffectDuration * 1000) {
		  mEffectTimer.reset();
		  mEffectEnabled = false;
		  Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, mEffect, mEffectEnabled);
		}
		
		mSpherePos = mSphere->getPosition();
		
		(this->*mUpdateCamera)();
		
	}
	
	void GfxEngine::setCameraMode(CAMERA_MODE inMode) {
	
	  switch (inMode) {
	    case CAMERA_CHASE:
	      mUpdateCamera = &GfxEngine::updateCameraChase;
	      break;
	    case CAMERA_FIXED:
	      mUpdateCamera = &GfxEngine::updateCameraFixed;
	      break;
	    case CAMERA_STICKY:
	      mUpdateCamera = &GfxEngine::updateCameraSticky;
	      break;
	    case CAMERA_LOLLY:
	      mUpdateCamera = &GfxEngine::updateCameraLolly;
	      break;
	    default:
	      mUpdateCamera = &GfxEngine::updateCameraChase;
	  };
	  
	}
	
	void GfxEngine::updateCameraChase() {
		mSpherePos = mSphere->getPosition();
		
		mCamera->setPosition(
		  mSpherePos.x,
		  mSpherePos.y + 35,
      mSpherePos.z-120
    );
    
	  mCamera->lookAt(
	    mSpherePos.x,
	    mSpherePos.y + 20,
	    mSpherePos.z
	  );	
	}
	
	void GfxEngine::updateCameraFixed() {
		mSpherePos = mSphere->getPosition();
		
		mCamera->setPosition(
		  0,
		  35,
      mSpherePos.z-120
    );
    
	  mCamera->lookAt(
	    0,
	    35,
	    mSpherePos.z
	  );	
	}
	void GfxEngine::updateCameraSticky() {
		mSpherePos = mSphere->getPosition();
		
		mCamera->setPosition(
		  mSpherePos.x,
		  mSpherePos.y + 35,
      mSpherePos.z-120
    );
    
	  mCamera->lookAt(
	    mSpherePos.x,
	    35,
	    mSpherePos.z
	  );	
	}
	
	void GfxEngine::updateCameraLolly() {
		mSpherePos = mSphere->getPosition();
		
		mCamera->setPosition(
		  mSpherePos.x,
		  35,
      mSpherePos.z-120
    );
    
	  mCamera->lookAt(
	    0,
	    mSpherePos.y+35,
	    mSpherePos.z
	  );	
	}
	
  
	void GfxEngine::setupParticles() {

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
      "FxMortar",
      "Vertigo/FX/Mortar", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("Mortar", effect));    

    /*effect = fxMgr->createParticleSystem(
      "FxSpawnPoint",
      "Vertigo/FX/SpawnPoint", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("SpawnPoint", effect));  */

    effect = fxMgr->createParticleSystem(
      "FxDespawn",
      "Vertigo/FX/Despawn", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("Despawn", effect));  

    effect = fxMgr->createParticleSystem(
      "FxSphereExplosion",
      "Vertigo/FX/SphereExplosion", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("SphereExplosion", effect));
          
    effectMap::iterator itr;
    for (itr = effects.begin(); itr != effects.end(); ++itr) {
      effect = itr->second;
      effect->setScale(Ogre::Vector3(0.2f, 0.2f, 0.2f));
      effect->setScaleVelocity(0.2f);
    }
    
    effects["SphereExplosion"]->setScale(Ogre::Vector3(1.0f, 1.0f, 1.0f));
    effects["SphereExplosion"]->setScaleVelocity(1.0f);
    effect = NULL;
    //mSphere->getMasterNode()->attachObject(effectExplosion);
    //effectExplosion->start();

    
    mPortableEffect = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    //mSpawnPoint = mSceneMgr->getRootSceneNode()->createChildSceneNode("NodeSpawnPoint");
    //mSpawnPoint->attachObject(effects["Despawn"]);
    
	}
	
	void GfxEngine::playEffect(std::string inEffect, Entity* inEntity) {
	  effectMap::iterator cursor = effects.find(inEffect);
	  if (cursor != effects.end()) {
	    ParticleUniverse::ParticleSystem* effect = cursor->second;
	    if (effect->isAttached())
	      effect->getParentSceneNode()->detachObject(effect);
	    inEntity->getMasterNode()->attachObject(effect);
	    
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
	    if (inEffect == "BlackHole")
	      effect->startAndStopFade(5.0f);
	    else
        effect->start();
	  }	
	};
	
	
	bool GfxEngine::evtObstacleAlive(Event* inEvt) {

	  return true;
	};
	
	bool GfxEngine::evtObstacleCollided(Event* inEvt) {
	  Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  playEffect( (mObs->shield() == FIRE) ? "Explosion" : "Shatter", mSphere); 

    //if (mSphere->shield() != mObs->shield())
	      
	  
	  return true;
	};
	

	
	bool GfxEngine::evtPortalEntered(Event* inEvt) {
	  //mSphere->getMasterNode()->setVisible(true);
	  //fPortalReached = false;
	  
	  this->playEffect("Despawn", Level::getSingleton().getSphere());
	  
	  return true;
	};
	
	bool GfxEngine::evtPortalReached(Event* inEvt) {
		//mSphere->getMasterNode()->setVisible(false);
		//fPortalReached = true;
		
	
		return true;	  
	};
	
	bool GfxEngine::evtPortalSighted(Event* inEvt) {
		return true;	  
	};
	
	bool GfxEngine::evtSphereDied(Event* inEvt) {
	  this->playEffect("SphereExplosion", Level::getSingleton().getSphere()->getMasterNode()->getPosition());
	 
	  return true;
	};
	
	
	bool GfxEngine::evtPlayerWon(Event* inEvt) {
	  mUpdate = &GfxEngine::updateNothing;
	  
	  Level::getSingleton().getSphere()->getMasterNode()->setVisible(false);
	  
	  Vector3 camPos = Level::getSingleton().getSphere()->getMasterNode()->getPosition();
	  camPos.z += 30;
	  Vector3 effectPos = Vector3(camPos);
	  effectPos.z += 50;
	  
	  //mCamera->move(camPos);
	  mCamera->lookAt(effectPos);
	  this->playEffect("BlackHole", effectPos);
	  return true;
	};
	
	bool GfxEngine::evtGameStarted(Event* inEvt) {
	  mUpdate = &GfxEngine::updateGame;
	  
	  Level::getSingleton().getSphere()->getMasterNode()->setVisible(true);
	  
	  return true;
	};
	
	bool GfxEngine::evtZoneEntered(Event* inEvt) {
		mCamera->setPosition(Vector3(0,75, -300));
		mCamera->lookAt(Vector3(0,75, 100));
		
		mUpdate = &GfxEngine::updateNothing;
		
		
		return true;	
	};
	
	bool GfxEngine::evtGameShown(Event* inEvt) {
	  // don't start updating the game if it hasn't started yet
	  // this event is triggered whenever the menu is hidden, even if it's at
	  // entering a zone
	  if (mUpdate == &GfxEngine::updateIntro)
	    mUpdate = &GfxEngine::updateGame;
	  
	  return true;
	};	

	bool GfxEngine::evtMenuShown(Event* inEvt) {
	  mUpdate = &GfxEngine::updateIntro;
	  
	  return true;
	};	

}

