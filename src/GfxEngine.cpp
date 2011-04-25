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
	}
	
	GfxEngine::~GfxEngine() {
		mLog->infoStream() << "shutting down";
		if (fSetup) {

			delete mLog;
			mLog = 0;

			fSetup = false;
		}
		
		_myGfxEngine = NULL;
	}
	
	bool GfxEngine::setup() {
		if (fSetup)
			return true;
		
		mRoot = Ogre::Root::getSingletonPtr();
		mOverlayMgr = Ogre::OverlayManager::getSingletonPtr();
		if (mRoot->hasSceneManager("GameScene"))
			mSceneMgr = mRoot->getSceneManager( "GameScene" );
		else
		mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "GameScene");
		
		if (mSceneMgr->hasCamera("Probe_Camera"))
		  mCamera = mSceneMgr->getCamera("Probe_Camera");
		else
		  mCamera       = mSceneMgr->createCamera("Probe_Camera");

		mRenderWindow = mRoot->getAutoCreatedWindow();
				
		mEvtMgr = EventManager::getSingletonPtr();

		setupSceneManager();
    setupViewport();
    setupCamera();
    setupSky();
    setupLights();
    setupParticles();
		
		mDirection = Vector3(0,0,1);
		mMoveSpeed = 0;
    //mRenderWindow->setActive(true);

    
    bindToName("GameShown", this, &GfxEngine::evtGameShown);
    bindToName("MenuShown", this, &GfxEngine::evtMenuShown);
    
		fSetup = true;
		return fSetup;
	}
	
	bool GfxEngine::deferredSetup() {
		
		//mCamera->setAutoTracking (true, mProbe->getSceneNode());
		//mCamera->setPosition(Vector3(200, 200, 200));
		//mCamera->lookAt(mProbe->getSceneNode()->getPosition());
		//mCamera->lookAt(mProbe->getSceneNode()->getPosition());
		
		GameState *currentState = (GameManager::getSingleton().currentState());
		if (currentState->getId() == STATE_INTRO) {
		  mUpdate = &GfxEngine::updateNothing;
		  return true;
		}
		
		mProbe = Level::getSingleton().getProbe();
		mUpdate = &GfxEngine::updateGame;
				
		mCamera->setPosition(Vector3(0,75, -200));
		mCamera->lookAt(Vector3(0,75, 100));

    bindToName("ZoneEntered", this, &GfxEngine::evtZoneEntered);
    bindToName("GameStarted", this, &GfxEngine::evtGameStarted);
    bindToName("PlayerWon", this, &GfxEngine::evtPlayerWon);
    bindToName("ProbeDied", this, &GfxEngine::evtProbeDied);    
    bindToName("DroneCollided", this, &GfxEngine::evtDroneCollided);
    bindToName("PortalEntered", this, &GfxEngine::evtPortalEntered);
    bindToName("TakingScreenshot", this, &GfxEngine::evtTakingScreenshot);
        
		return true;
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

	  mViewport = mRenderWindow->addViewport(mCamera);
	  if (!mViewport) {
		  mLog->errorStream() << "Viewport doesn't exist!!!";
	  }
	
	  //mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
  };

  void GfxEngine::setupViewport()
  {
	  mLog->debugStream() << "setting up the viewport";
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
    mLog->debugStream() << "setting up the camera";

    mCamera->setNearClipDistance( 10 );
    mCamera->setFarClipDistance( 10000 );
    mCamera->setPosition(Vector3(0,70,-200));
    mCamera->lookAt(Ogre::Vector3(0, 70, 0));
    
    setCameraMode(CAMERA_CHASE);
  };

  void GfxEngine::setupSky()
  {
    mLog->noticeStream() << "setting up sky";
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
    light->setDirection(Vector3(0,0,-1));
    light->setDiffuseColour(0.9f, 0.9f, 0.9f);
    light->setSpecularColour(0.9f, 0.9f, 0.9f);

    light = mSceneMgr->createLight("Light3");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
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
	 
    //Ogre::ColourValue fadeColour(0.0f, 0.0f, 0.0f);
    //mViewport->setBackgroundColour(fadeColour);
    //mSceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0.0, 300, 800);
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

	  inEntity->_attachSceneNode(inNode);
	  inEntity->_attachSceneObject(mEntity);
	
	  return inNode;
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
	
	void GfxEngine::keyPressed( const OIS::KeyEvent &e )
	{
		switch (e.key) {   
		  case OIS::KC_K:
	      mEvtMgr->hook(mEvtMgr->createEvt("TakingScreenshot"));
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

	void GfxEngine::update(unsigned long lTimeElapsed) {
	  processEvents();
	  
	  (this->*mUpdate)(lTimeElapsed);
	};
	
	void GfxEngine::updateNothing(unsigned long lTimeElapsed) {
	
	};
	
	void GfxEngine::updateGame(unsigned long lTimeElapsed) {
		
		if (mEffectEnabled && mEffectTimer.getMilliseconds() > mEffectDuration * 1000) {
		  mEffectTimer.reset();
		  mEffectEnabled = false;
		  Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, mEffect, mEffectEnabled);
		}
		
		mProbePos = mProbe->getPosition();
		
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
		mProbePos = mProbe->getPosition();
		
		mCamera->setPosition(
		  mProbePos.x,
		  mProbePos.y + 35,
      mProbePos.z-120
    );
    
	  mCamera->lookAt(
	    mProbePos.x,
	    mProbePos.y + 20,
	    mProbePos.z
	  );	
	}
	
	void GfxEngine::updateCameraFixed() {
		mProbePos = mProbe->getPosition();
		
		mCamera->setPosition(
		  0,
		  35,
      mProbePos.z-120
    );
    
	  mCamera->lookAt(
	    0,
	    35,
	    mProbePos.z
	  );	
	}
	void GfxEngine::updateCameraSticky() {
		mProbePos = mProbe->getPosition();
		
		mCamera->setPosition(
		  mProbePos.x,
		  mProbePos.y + 35,
      mProbePos.z-120
    );
    
	  mCamera->lookAt(
	    mProbePos.x,
	    35,
	    mProbePos.z
	  );	
	}
	
	void GfxEngine::updateCameraLolly() {
		mProbePos = mProbe->getPosition();
		
		mCamera->setPosition(
		  mProbePos.x,
		  35,
      mProbePos.z-120
    );
    
	  mCamera->lookAt(
	    0,
	    mProbePos.y+35,
	    mProbePos.z
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

    effect = fxMgr->createParticleSystem(
      "FxDespawn",
      "Vertigo/FX/Despawn", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("Despawn", effect));  

    effect = fxMgr->createParticleSystem(
      "FxProbeExplosion",
      "Vertigo/FX/ProbeExplosion", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("ProbeExplosion", effect));
          
    effectMap::iterator itr;
    for (itr = effects.begin(); itr != effects.end(); ++itr) {
      effect = itr->second;
      effect->setScale(Ogre::Vector3(0.2f, 0.2f, 0.2f));
      effect->setScaleVelocity(0.2f);
    }
    
    effects["ProbeExplosion"]->setScale(Ogre::Vector3(1.0f, 1.0f, 1.0f));
    effects["ProbeExplosion"]->setScaleVelocity(1.0f);
    effect = NULL;
        
    mPortableEffect = mSceneMgr->getRootSceneNode()->createChildSceneNode();    
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
		
	bool GfxEngine::evtDroneCollided(Event* inEvt) {
	  Drone* mObs = static_cast<Drone*>(inEvt->getAny());
	  this->playEffect( (mObs->getShield() == FIRE) ? "Explosion" : "Shatter", mProbe); 

	  return true;
	};
	

	
	bool GfxEngine::evtPortalEntered(Event* inEvt) {
	  this->playEffect("Despawn", Level::getSingleton().getProbe());
	  
	  return true;
	};
	
	bool GfxEngine::evtProbeDied(Event* inEvt) {
	  this->playEffect("ProbeExplosion", Level::getSingleton().getProbe()->getMasterNode()->getPosition());
	 
	  return true;
	};
	
	
	bool GfxEngine::evtPlayerWon(Event* inEvt) {
	  mUpdate = &GfxEngine::updateNothing;
	  
	  Level::getSingleton().getProbe()->getMasterNode()->setVisible(false);
	  
	  Vector3 camPos = Level::getSingleton().getProbe()->getMasterNode()->getPosition();
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
	  
	  Level::getSingleton().getProbe()->getMasterNode()->setVisible(true);
	  
	  switch (Level::getSingleton().currentZone()->getSettings().mMode) {
	    case DODGY:
	      setCameraMode(CAMERA_FIXED);
	      break;
	    default:
	      setCameraMode(CAMERA_CHASE);
	      break;
	  };
	  
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
	  if (mUpdate == &GfxEngine::updateNothing)
	    mUpdate = &GfxEngine::updateGame;
	  
	  return true;
	};	

	bool GfxEngine::evtMenuShown(Event* inEvt) {
	  mUpdate = &GfxEngine::updateNothing;
	  
	  return true;
	};	

  bool GfxEngine::evtTakingScreenshot(Event* inEvt) {
    // take a screenshot
    mLog->debugStream() << "taking screenshot now";
    
    time_t seconds = time(NULL);
    std::string fileName = "Screenshot_";
    fileName += stringify(seconds);
    fileName += ".png";
    mRenderWindow->writeContentsToFile(fileName);  
        
    mEvtMgr->hook(mEvtMgr->createEvt("ScreenshotTaken"));
    return true;
  }
}

