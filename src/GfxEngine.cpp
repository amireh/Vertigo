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
#include "OGRE/OgreTerrain.h"
#include "OGRE/OgreTerrainGroup.h"

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
		mPlayers.clear();
		//mCameraMan = 0;
		mFallVelocity = 0;
		//mSceneLoader = 0;
		
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
		
		mRenderWindow->reposition(1420 - mRenderWindow->getWidth(),
								  50);
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
		
		fSetup = true;
		return fSetup;
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
	
	void GfxEngine::update(unsigned long lTimeElapsed) {
		processEvents();
		
		//mCameraMan->update(lTimeElapsed);
		
		using namespace Ogre;
		
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
        // Alter the camera aspect ratio to match the viewport	
        mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
		
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
		lCamPos.x = 0;
		lCamPos.y = 20;
		lCamPos.z = -250;
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
		mSceneMgr->setSkyDome(true, "Examples/CloudySky", 65, 8);
		//mSceneMgr->setSkyBox(true, "Sky/EarlyMorning", 2000, true);				
		 
		
		
		Ogre::Plane mPlane;
        Ogre::SceneNode* mNode;
        Ogre::Entity* pPlaneEnt;
		
        // create floor plane
        mPlane.normal = Vector3::UNIT_Y;
		 mPlane.d = 0;
		 Ogre::MeshManager::getSingleton().createPlane("meshFloor",
		 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, mPlane,
		 1000,1000,10,10,true,1,50,50,Vector3::UNIT_Z);
		 pPlaneEnt = mSceneMgr->createEntity("Plane_Floor", "meshFloor");
		 pPlaneEnt->setMaterialName("Examples/GrassFloor");
		 pPlaneEnt->setCastShadows(false);
		 mNode = mSceneMgr->createSceneNode("Node_Floor");
		 mSceneMgr->getRootSceneNode()->addChild(mNode);
		 mNode->setPosition(Vector3(0,0,0));
		 mNode->attachObject(pPlaneEnt);
		
		 
		 
		
    };
	
    void GfxEngine::setupLights()
    {
		mLog->debugStream() << "setting up lights";
        Ogre::Light *light;
        /* now let's setup our light so we can see the shizzle */
        light = mSceneMgr->createLight("Light1");
        light->setType(Ogre::Light::LT_POINT);
        light->setPosition(Vector3(0, 150, 250));
        light->setDirection(Vector3(0,0,0));
        light->setDiffuseColour(1.0, 1.0, 1.0);
        light->setSpecularColour(1.0, 1.0, 1.0);
		
        /*light = mSceneMgr->createLight("Light2");
		 light->setType(Ogre::Light::LT_POINT);
		 light->setPosition(Vector3(500, 500, 1000));
		 light->setDirection(Vector3(0,0,1200));
		 light->setDiffuseColour(1.0, 1.0, 1.0);
		 light->setSpecularColour(1.0, 1.0, 1.0);*/
    };
	
    void GfxEngine::setupNodes()
    {
		mLog->debugStream() << "setting up nodes";
		
    };

	
    Ogre::SceneNode* 
	GfxEngine::createNode(String& inName, 
						  Vector3& inPosition, 
						  Vector3& inScale, 
						  Vector3& inDirection, 
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
		mLog->debugStream() << "rendering entity " << inEntity->getName();
        Ogre::Entity* mEntity;
		
        String entityName = "";
		entityName += inEntity->getName();
		entityName += stringify(inEntity->getObjectId());
		
		std::string _meshPath = "";
		mEntity = mSceneMgr->createEntity(entityName, inEntity->getMesh());
		mEntity->setUserAny(Ogre::Any(inEntity));
		inNode->attachObject(mEntity);
		
		inEntity->attachSceneNode(inNode);
		inEntity->attachSceneObject(mEntity);
		
		return inNode;

    };
	
	
    bool GfxEngine::attachToScene(Pixy::Entity* inEntity)
    {
		renderEntity(inEntity, NULL);
		return true;
    };
	
	
	
    void GfxEngine::detachFromScene(Pixy::Entity* inEntity)
    {
        //Ogre::String ownerName = inEntity->getOwner();// == ID_HOST) ? "host" : "client";
        Ogre::SceneNode* mTmpNode = NULL;
		
		mTmpNode = inEntity->getSceneNode();
		
		
		mLog->debugStream() << "I'm detaching Entity '" << inEntity->getName() << "' from SceneNode : " + mTmpNode->getName();
		mTmpNode->showBoundingBox(false);
		mTmpNode->detachObject(inEntity->getSceneObject());
		
		mSceneMgr->destroyEntity((Ogre::Entity*)inEntity->getSceneObject());
		
    }
	
    void GfxEngine::setupWaypoints()
    {
    };
	
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
		//if (mCameraMan)
		//	mCameraMan->injectMouseUp(e, id);
	}
	

}