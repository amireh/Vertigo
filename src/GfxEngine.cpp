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
		
    Geometry::createSphere("ObstacleMesh", 12, 16, 16);

		
		mTrayMgr = new OgreBites::SdkTrayManager("AOFTrayMgr", mRenderWindow, InputManager::getSingletonPtr()->getMouse(), 0);
		mTrayMgr->hideCursor();
    mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);

    mCameraMan = new OgreBites::SdkCameraMan(mCamera);
    //mRenderWindow->setActive(true);
                
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
		
		//mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
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
		 /*
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
		  int nr_tubes = 45;
		  for (int i =0; i < nr_tubes; ++i) {
			  mEntityName = "myTube_";
			  mEntityName += i;

			
			
    		mEntity = mSceneMgr->createEntity(mEntityName, "TubeMesh");
			  mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			  mEntity->setMaterialName("BumpMapping/Terrain");
			  mNode->attachObject(mEntity);
			  mNode->setPosition(Vector3(0, 70, i * tube_length));
			  //mNode->roll(Ogre::Degree(90));
			  mNode->pitch(Ogre::Degree(90));
			  //mNode->yaw(Ogre::Degree(30));
			  //mNode->showBoundingBox(true);			
			
		  }*/
		
    };
	
    void GfxEngine::setupLights()
    {
		  mLog->debugStream() << "setting up lights";
      mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));
      Ogre::Light *light;

     light = mSceneMgr->createLight("Light2");
		 light->setType(Ogre::Light::LT_DIRECTIONAL);
		 //light->setPosition(Vector3(0, 0, 1000));
		 light->setDirection(Vector3(0,0,-1));
		 light->setDiffuseColour(1.0f, 1.0f, 1.0f);
		 light->setSpecularColour(1.0f, 1.0f, 1.0f);
		 
      /* now let's setup our light so we can see the shizzle */
      mSpotLight = mSceneMgr->createLight("PlayerLight");
      mSpotLight->setType(Ogre::Light::LT_POINT);
      mSpotLight->setPosition(Vector3(0, 0, -100));
      mSpotLight->setDirection(Vector3(0,0.5f,1));
      mSpotLight->setDiffuseColour(0.6f, 0.6f, 0.6f);
      mSpotLight->setSpecularColour(0.8f, 0.8f, 0.8f);
		  
		 
		 Ogre::ColourValue fadeColour(0.0f, 0.0f, 0.0f);
     //mViewport->setBackgroundColour(fadeColour);
		 //mSceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0.0, 800, 1500);
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
    	mNode->showBoundingBox(true);
		
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
		mLog->debugStream() << "Creating an Entity with name " << entityName;
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
		mLog->debugStream() << "Creating a SceneNode with name " << nodeName;
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
	  mCameraMan->injectKeyDown(e);
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
				break;
			case OIS::KC_G:
				mCamera->move(Vector3(0, -1, 0));
				break;	*/
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
			mCameraMan->injectKeyUp(e);
	}
	
	


	void GfxEngine::update(unsigned long lTimeElapsed) {
		//processEvents();

	  if (shakingScreen)
	    applyScreenShake(lTimeElapsed);
		
		if (mEffectEnabled && mEffectTimer.getMilliseconds() > mEffectDuration * 1000) {
		  mEffectTimer.reset();
		  mEffectEnabled = false;
		  Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, mEffect, mEffectEnabled);
		}
		
		mCamera->setPosition(
		  mSphere->getPosition().x,
      mSphere->getPosition().y + 35,
      mSphere->getPosition().z-160
    );
	  
	  mCamera->setOrientation(mSphere->getMasterNode()->getOrientation());
	  
	  mCamera->lookAt(
	    mSphere->getPosition().x,
	    mSphere->getPosition().y + 20,
	    mSphere->getPosition().z
	  );
							 
		evt.timeSinceLastEvent = lTimeElapsed;
		evt.timeSinceLastFrame = lTimeElapsed;
		
		mTrayMgr->frameRenderingQueued(evt);
		mCameraMan->update(lTimeElapsed);
		using namespace Ogre;
		
	}
	
  void GfxEngine::applyScreenShake(unsigned long lTimeElapsed) {
    if (!mEffectStarted) {
      //mEffectEnabled = true;
      mEffectStarted = true;
      shakingScreen = true;
      mSpotLight->setDiffuseColour(0, 0, 0);
      mSpotLight->setSpecularColour(0, 0, 0);
      //mEffectDuration = 0.5f;
      mEffectTimer.reset();
      reachedThreshold = false;
    }

    float dimScale = 0.002f; 
    
    if (!reachedThreshold && mSpotLight->getDiffuseColour().r < 0.5f) {
    } else {
      reachedThreshold = true;
      dimScale *= -1;
    }
     
    float color = mSpotLight->getDiffuseColour().r+ dimScale * lTimeElapsed;
    mSpotLight->setDiffuseColour(color, 0, 0);
    mSpotLight->setSpecularColour(color, 0, 0);

    //if (shakingScreen && mEffectTimer.getMilliseconds() > mEffectDuration * 1000) {
    if (shakingScreen && reachedThreshold && mSpotLight->getSpecularColour().r <= 0) {
      mSpotLight->setDiffuseColour(0.8, 0.8, 0.8);
      mSpotLight->setSpecularColour(1.0, 1.0, 1.0);
      mEffectTimer.reset();
      //mEffectEnabled = false;
      shakingScreen = false;
      mEffectDuration = 0;
      mEffectStarted = false;
    }
  };
  
	void GfxEngine::setupParticles() {
	  using namespace Ogre;
		ParticleSystem::setDefaultNonVisibleUpdateTimeout(1);  // set nonvisible timeout

		/*ParticleSystem* ps;
    // create aureola around ogre head perpendicular to the ground
    ps = mSceneMgr->createParticleSystem("Nimbus", "Examples/GreenyNimbus");
		mSphere->getSceneNode()->attachObject(ps);*/

	}
}
