/* mTrayMgr->hideTrays();
 *  GfxEngine.cpp
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/11/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */

#include "GfxEngine.h"
#include "GameManager.h"
//#include "OGRE/Terrain/OgreTerrain.h"
//#include "OGRE/Terrain/OgreTerrainGroup.h"
#include "Level.h"
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
		  ParticleUniverse::ParticleSystemManager::getSingletonPtr()->destroyAllParticleSystems(mSceneMgr);
		  
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
		
		_myGfxEngine = NULL;
	}
	
	bool GfxEngine::setup() {
		if (fSetup)
			return true;
		
		mRoot         = Ogre::Root::getSingletonPtr();
		mOverlayMgr   = Ogre::OverlayManager::getSingletonPtr();
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
		
		mOverlayMgr->getByName("Vertigo/UI/Loading")->show();
		
    //Geometry::createSphere("ObstacleMesh", 12, 16, 16);

    setupParticles();
		
		mTrayMgr = new OgreBites::SdkTrayManager("AOFTrayMgr", mRenderWindow, InputManager::getSingletonPtr()->getMouse(), 0);
		mTrayMgr->hideCursor();
    mTrayMgr->showFrameStats(OgreBites::TL_TOPLEFT);
    mTrayMgr->hideTrays();
    
    mCameraMan = new OgreBites::SdkCameraMan(mCamera);
    //mRenderWindow->setActive(true);


    
		fSetup = true;
		return fSetup;
	}
	
	bool GfxEngine::deferredSetup() {
		mSphere = Level::getSingleton().getSphere();
		//mCamera->setAutoTracking (true, mSphere->getSceneNode());
		//mCamera->setPosition(Vector3(200, 200, 200));
		//mCamera->lookAt(mSphere->getSceneNode()->getPosition());
		//mCamera->lookAt(mSphere->getSceneNode()->getPosition());
		
		GameState *currentState = (GameManager::getSingleton().currentState());
		if (currentState->getId() == STATE_INTRO) {
		  mUpdate = &GfxEngine::updateIntro;
		  return true;
		}
		
		mUpdate = &GfxEngine::updateGame;
		
		mUISheet = mOverlayMgr->getByName("Vertigo/UI");
		mUISheet->show();
		
		mUISheetLoss = mOverlayMgr->getByName("Vertigo/UI/Loss");
		mUISheetWin = mOverlayMgr->getByName("Vertigo/UI/Win");
		mUISheetPrepare = mOverlayMgr->getByName("Vertigo/UI/Prepare");
		
		mOverlayMgr->getByName("Vertigo/UI/Loading")->hide();
		mUISheetLoss->hide();
		mUISheetWin->hide();
		mUISheetPrepare->show();		
		
		//mUIBarWidth = mUISheet->getChild("UI/FireShieldContainer")->getChild("UI/FireShield")->getWidth();
		mUIBarWidth = mViewport->getActualWidth() / 3;
		mUISheet->getChild("UI/FireShieldContainer")->setWidth(mUIBarWidth);
		mUISheet->getChild("UI/FireShieldContainer")->getChild("UI/FireShield")->setWidth(mUIBarWidth);
		
		float aspect_ratio = mViewport->getActualWidth() / mViewport->getActualHeight();
		/*mUISheet->getChild("UI/FireShieldContainer")->setHeight(64.0f / aspect_ratio);
		mUISheet->getChild("UI/FireShieldContainer")->getChild("UI/FireShield")->setHeight(64.0f / aspect_ratio);*/
		
		mUISheet->getChild("UI/IceShieldContainer")->setWidth(mUIBarWidth);
		mUISheet->getChild("UI/IceShieldContainer")->setLeft(-1*mUIBarWidth);
		mUISheet->getChild("UI/IceShieldContainer")->getChild("UI/IceShield")->setWidth(mUIBarWidth);
		mUISheet->getChild("UI/IceShieldContainer")->getChild("UI/IceShield")->setLeft(-1*mUIBarWidth);
		//mSphere->getSceneNode()->attachObject(mSceneMgr->getLight("Light2"));
		
		mUITimer.reset();

    // scale overlays' font size
    using Ogre::TextAreaOverlayElement;
    TextAreaOverlayElement* mTOverlay;
    float font_size = 32 / aspect_ratio;
    static_cast<TextAreaOverlayElement*>(
      mOverlayMgr->getByName("Vertigo/UI/Prepare")->
                   getChild("UI/Containers/Prepare")->
                   getChild("UI/Text/Prepare"))->setCharHeight(font_size);
    static_cast<TextAreaOverlayElement*>(
      mOverlayMgr->getByName("Vertigo/UI/Loading")->
                   getChild("UI/Containers/Loading")->
                   getChild("UI/Text/Loading"))->setCharHeight(font_size);
    static_cast<TextAreaOverlayElement*>(
      mOverlayMgr->getByName("Vertigo/UI/Loss")->
                   getChild("UI/Containers/Loss")->
                   getChild("UI/Text/Loss"))->setCharHeight(font_size);
    static_cast<TextAreaOverlayElement*>(
      mOverlayMgr->getByName("Vertigo/UI/Win")->
                   getChild("UI/Containers/Win")->
                   getChild("UI/Text/Win"))->setCharHeight(font_size);
    
    
		mCamera->setPosition(Vector3(0,75, -200));
		mCamera->lookAt(Vector3(0,75, 100));
    
    bindToName("GameStarted", this, &GfxEngine::evtGameStarted);
    bindToName("PlayerWon", this, &GfxEngine::evtPlayerWon);
    bindToName("SphereDied", this, &GfxEngine::evtSphereDied);    
    bindToName("ObstacleAlive", this, &GfxEngine::evtObstacleAlive);
    bindToName("ObstacleCollided", this, &GfxEngine::evtObstacleCollided);
    bindToName("PortalEntered", this, &GfxEngine::evtPortalEntered);
    bindToName("PortalReached", this, &GfxEngine::evtPortalReached);
    bindToName("PortalSighted", this, &GfxEngine::evtPortalSighted);
        
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
		 //light->setPosition(Vector3(0, 0, 1000));
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
      mSpotLight->setDiffuseColour(0.3f, 0.3f, 0.3f);
      mSpotLight->setSpecularColour(0.3f, 0.3f, 0.3f);
      
      mSpotLight = mSceneMgr->createLight("TunnelLight3");
      mSpotLight->setType(Ogre::Light::LT_POINT);
      mSpotLight->setPosition(Vector3(0, 30, 4500));
      mSpotLight->setDiffuseColour(0.1f, 0.1f, 0.1f);
      mSpotLight->setSpecularColour(0.1f, 0.1f, 0.1f);
      
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
		//if (mCameraMan)
		//	mCameraMan->injectMouseMove(e);
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
		    playEffect("Blaze", mSphere);
		    break;
		  case OIS::KC_T:
		    playEffect("Despawn", mSphere);
		    break;
		  case OIS::KC_Y:
		    playEffect("Explosion", mSphere);
		    break;
		  case OIS::KC_H:
		    playEffect("Shatter", mSphere);
		    break;		    
		  case OIS::KC_F:
		    //playEffect("Despawn", mSphere);
		    if (mTrayMgr->areTraysVisible())
		      mTrayMgr->hideTrays();
		    else
		      mTrayMgr->showTrays();
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
	  
	  (this->*mUpdate)(lTimeElapsed);
	};
	
	void GfxEngine::updateIntro(unsigned long lTimeElapsed) {
	
	};
	
	void GfxEngine::updateGame(unsigned long lTimeElapsed) {

		if (Level::getSingletonPtr()->isGameOver())
		  return;
		
		mUISheet->getChild("UI/TimerContainer")->getChild("UI/Timer")->setCaption(String(stringify(mUITimer.getMilliseconds() / 1000.0f)));

		if (fPortalReached)
		  return;
		
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
        
    /*effect = fxMgr->createParticleSystem(
      "FxBlackHole",
      "Vertigo/FX/BlackHole", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("BlackHole", effect));*/

    effect = fxMgr->createParticleSystem(
      "FxBlaze",
      "Vertigo/FX/Blaze", 
      mSceneMgr);
    effect->prepare();
    effects.insert(std::make_pair<std::string, ParticleUniverse::ParticleSystem*>("Blaze", effect));    

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
    mSpawnPoint = mSceneMgr->getRootSceneNode()->createChildSceneNode("NodeSpawnPoint");
    //mSpawnPoint->attachObject(effects["Despawn"]);
    
	}
	
	void GfxEngine::playEffect(std::string inEffect, Entity* inEntity) {
	  effectMap::iterator cursor = effects.find(inEffect);
	  if (cursor != effects.end()) {
	    ParticleUniverse::ParticleSystem* effect = cursor->second;
	    if (effect->isAttached())
	      effect->getParentSceneNode()->detachObject(effect);
	    inEntity->getMasterNode()->attachObject(effect);
	    /*if (inEffect == "BlackHole")
	      effect->startAndStopFade(1);
	    else*/
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
	  //Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  
	  //Vector3 pos = mObs->getMasterNode()->getPosition();
	  //pos.y += 30;
	  //mSpawnPoint->setPosition(pos);
	  //effects["Despawn"]->start();
	  //playEffect("SpawnPoint", pos);
	  
	  return true;
	};
	
	bool GfxEngine::evtObstacleCollided(Event* inEvt) {
	  Obstacle* mObs = static_cast<Obstacle*>(inEvt->getAny());
	  if (mObs->shield() == FIRE) {
	    playEffect("Explosion", mSphere);
	  } else {
	    playEffect("Shatter", mSphere);    
	  }

    if (mSphere->shield() != mObs->shield()) {
	      applyMotionBlur(0.5f);
	      


	  }
	  
	  return true;
	};
	
	void GfxEngine::updateUIShields() {
    using namespace Ogre;
    OverlayElement* mElement = (mSphere->shield() == FIRE) 
      ? mUISheet->getChild("UI/FireShieldContainer")->getChild("UI/FireShield")
      : mUISheet->getChild("UI/IceShieldContainer")->getChild("UI/IceShield");
    
    mElement->setWidth(mUIBarWidth * (mSphere->getShieldState() / 1000.0f));
    
    if (mElement->getHorizontalAlignment() == Ogre::GHA_RIGHT)
      mElement->setLeft(-1 * mElement->getWidth());
    
    //mLog->debugStream() << "updating UI, element's new width: " << mElement->getWidth();  
    //mElement->setCaption(String(stringify(mSphere->getShieldState())));	
	};
	
	bool GfxEngine::evtPortalEntered(Event* inEvt) {
	  mSphere->getMasterNode()->setVisible(true);
	  
	  fPortalReached = false;
	  
	  return true;
	};
	
	bool GfxEngine::evtPortalReached(Event* inEvt) {
	  //mSphere->die();
	  //mSpawnPoint->setVisible(false);
		mSphere->getMasterNode()->setVisible(false);
		fPortalReached = true;
		//playEffect("Despawn", mSphere);
		
		return true;	  
	};
	
	bool GfxEngine::evtPortalSighted(Event* inEvt) {
		
		return true;	  
	};
	
	bool GfxEngine::evtSphereDied(Event* inEvt) {
	  
	  playEffect("SphereExplosion", mSphere);
	  mUISheetLoss->show();
	  
	  return true;
	};
	
	SceneNode* GfxEngine::getPortal() { return mPortal; };
	
	bool GfxEngine::evtPlayerWon(Event* inEvt) {
	  
	  mUISheetWin->show();
	  
	  return true;
	};
	
	bool GfxEngine::evtGameStarted(Event* inEvt) {
	  mUISheetPrepare->hide();
	  mUITimer.reset();
	  return true;
	};
}
