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
		mCameraMan = 0;
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
		
		
		mCamera       = mSceneMgr->createCamera("Combat_Camera");
		mCamera2      = mSceneMgr->createCamera("Combat_Camera_2");
		mCamera3      = mSceneMgr->createCamera("Combat_Camera_3");
		mCamera4      = mSceneMgr->createCamera("Combat_Camera_4");

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
		fSetup = true;
		return fSetup;
	}
	

	

	void GfxEngine::setCamera(const Ogre::String& inCameraName) {
		mCamera = mSceneMgr->createCamera(inCameraName);
	}
	
	SdkCameraMan* GfxEngine::getCameraMan() {
		return mCameraMan;
	}
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
		
		mCameraMan->update(lTimeElapsed);
		
		using namespace Ogre;
		/*
		bool mFly = false;
		if (!mFly)
		{
		 
			// clamp to terrain
			Vector3 camPos = mCamera->getPosition();
			Ray ray;
			ray.setOrigin(Vector3(camPos.x, 10000, camPos.z));
			ray.setDirection(Vector3::NEGATIVE_UNIT_Y);
			
			TerrainGroup::RayResult rayResult = mSceneLoader->getTerrainGroup()->rayIntersects(ray);
			Real distanceAboveTerrain = 50;
			Real fallSpeed = 300;
			Real newy = camPos.y;
			if (rayResult.hit)
			{
				if (camPos.y > rayResult.position.y + distanceAboveTerrain)
				{
					mFallVelocity += lTimeElapsed * 20;
					mFallVelocity = std::min(mFallVelocity, fallSpeed);
					newy = camPos.y - mFallVelocity * lTimeElapsed;
					
				}
				newy = std::max(rayResult.position.y + distanceAboveTerrain, newy);
				mCamera->setPosition(camPos.x, newy, camPos.z);
				
			}
		//}
		 */
	}
	
	bool GfxEngine::cleanup() {		
		return true;
	}
	
	
    void GfxEngine::setupSceneManager()
    {
		mLog->debugStream() << "setting up SceneManager";
		//mSceneMgr->setSkyBox(true, "Sky/EarlyMorning", 2000, true);		
        //mSceneMgr->setAmbientLight(Ogre::ColourValue(1,1,1));
		/*mSceneLoader = new DotSceneLoader();
		mSceneLoader->parseDotScene("Elementum.scene", 
									"General", 
									mSceneMgr);*/
		

		mCamera = mSceneMgr->getCamera("Camera#0");
		
		
		mViewport = mCamera->getViewport();
		mRenderWindow->addViewport(mCamera);
		//mCameraMan->setStyle(OgreBites::CS_FREELOOK);

		
		/*Ogre::Vector3 lPos = mPuppetPos[ME];
		mCamera->setPosition(lPos.x, lPos.y+2, lPos.z-20);
		mCamera->lookAt(mPuppetPos[ME]);*/
		
		mCameraMan = new OgreBites::SdkCameraMan(mCamera);
		//mCameraMan->setTopSpeed(50);
		mCameraMan->setStyle(OgreBites::CS_ORBIT);

		//mCamera->yaw(Ogre::Radian(-45));
		

		//parseDotScene("Elementum.xml","General",mSceneMgr);
		//mWindow->setDebugText(getProperty("Robot","Life"));

		//mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
    };
	
    void GfxEngine::setupViewports()
    {
		mLog->debugStream() << "setting up viewports";
        //mViewport->setBackgroundColour(Ogre::ColourValue(255,255,255));
        // Alter the camera aspect ratio to match the viewport
        //mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
		
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
		lCamPos.x = 99;
		lCamPos.y = 189;
		lCamPos.z = -150;
        mCamera->setPosition(lCamPos);
        //mCamera->lookAt(mPuppetPos[ME]);
		
	};
	
    void GfxEngine::setupTerrain()
    {

		
		/*
		 char* lTerrainCfgPath = (char*)malloc(sizeof(char) * (strlen(PROJECT_ROOT) + 18));
		sprintf(lTerrainCfgPath, "%s/Resources/Config", PROJECT_ROOT);
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(lTerrainCfgPath, "FileSystem");
		
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();	
		*/
		
		
        mLog->noticeStream() << "Setting world geometry";
        mSceneMgr->setWorldGeometry("terrain.cfg");

        // skyz0rs
        mLog->noticeStream() << "Setting up sky";
		mSceneMgr->setSkyDome(true, "Examples/CloudySky", 65, 8);
		
		 
		
		/*
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
		
        // create wall planes
        
		 mPlane.normal = Vector3::UNIT_Z;
		 mPlane.d = 0;
		 Ogre::MeshManager::getSingleton().createPlane("meshWall",
		 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, mPlane,
		 1000,1000,10,10,true,1,50,50,Vector3::UNIT_Y);
		 pPlaneEnt = mSceneMgr->createEntity("Plane_Wall", "meshWall");
		 pPlaneEnt->setMaterialName("Examples/DarkMaterial");
		 pPlaneEnt->setCastShadows(false);
		 mNode = mSceneMgr->createSceneNode("Node_Wall1");
		 mSceneMgr->getRootSceneNode()->addChild(mNode);
		 mNode->setPosition(Vector3(500,500,0));
		 mNode->attachObject(pPlaneEnt);
		 
		 Ogre::Plane mPlane2;
		 mPlane2.normal = Vector3::NEGATIVE_UNIT_Z;
		 mPlane2.d = 0;
		 Ogre::MeshManager::getSingleton().createPlane("meshWall2",
		 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, mPlane2,
		 1000,1000,10,10,true,1,50,50,Vector3::UNIT_Y);
		 pPlaneEnt = mSceneMgr->createEntity("Plane_Wall2", "meshWall2");
		 pPlaneEnt->setMaterialName("Examples/DarkMaterial");
		 pPlaneEnt->setCastShadows(false);
		 mNode = mSceneMgr->createSceneNode("Node_Wall2");
		 mSceneMgr->getRootSceneNode()->addChild(mNode);
		 mNode->setPosition(Vector3(500,500,1500));
		 mNode->setDirection(Vector3(0,0,0));
		 mNode->attachObject(pPlaneEnt);
		 
		 Ogre::Plane mPlane3;
		 mPlane3.normal = Vector3::NEGATIVE_UNIT_Y;
		 mPlane3.d = 0;
		 Ogre::MeshManager::getSingleton().createPlane("meshWall3",
		 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, mPlane3,
		 1000,2000,10,10,true,1,50,50,Vector3::UNIT_Z);
		 pPlaneEnt = mSceneMgr->createEntity("Plane_Wall3", "meshWall3");
		 pPlaneEnt->setMaterialName("Examples/DarkMaterial");
		 pPlaneEnt->setCastShadows(false);
		 mNode = mSceneMgr->createSceneNode("Node_Wall3");
		 mSceneMgr->getRootSceneNode()->addChild(mNode);
		 mNode->setPosition(Vector3(500,1000,1000));
		 mNode->setDirection(Vector3(0,0,0));
		 mNode->attachObject(pPlaneEnt);
		 
		 
		*/
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

	
    Ogre::SceneNode* GfxEngine::createNode(String& inName, Vector3& inPosition, Vector3& inScale, Vector3& inDirection, Ogre::SceneNode* inParent)
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
	
    Ogre::SceneNode* GfxEngine::renderEntity(Pixy::Entity* inEntity)
    {
		mLog->debugStream() << "rendering entity " << inEntity->getName();
                bool found = false;
        SceneNode* mNode;
        Ogre::Entity* mEntity;
		
        String entityName = "", nodeName = "", ownerName = "";
		
        		
        // handle units
        entityName = "entity_" + stringify<int>(inEntity->getObjectId());
        // now we need to locate the nearest empty SceneNode
        // to render our Entity in
        int idNode;
        for (int i=0; i<10; i++)
        {
            nodeName = "node_" + Ogre::StringConverter::toString(i);
            mNode = mSceneMgr->getSceneNode(nodeName);
            if (mNode->numAttachedObjects() == 0)
            {
                found = true;
                break;
            }
        };
		
        // now we've got our Node, create the Entity & attach
        if (found)
        {
			/* TO_DO */
			std::string _meshPath = "";
            mEntity = mSceneMgr->createEntity(entityName, _meshPath);
            mEntity->setUserAny(Ogre::Any(inEntity));
            mNode->attachObject(mEntity);
			
            inEntity->attachSceneNode(mNode);
            inEntity->attachSceneObject(mEntity);
			
            return mNode;
            //mSceneMgr->getSceneNode(nodeName)->setUserAny(
        } else {
            mLog->errorStream() << "Could not attach Entity! No empty SceneNodes available";
            return NULL;
        }
        //LOG_F(__FUNCTION__);
    };
	
	
    bool GfxEngine::attachToScene(Pixy::Entity* inEntity)
    {
        //bool isPuppet = (inEntity->getRank() == 0) ? true : false;
		
        // render the object
        renderEntity(inEntity);
		/*
        // create and attach interface stats overlay
        if (!isPuppet)
            inEntity->attachSceneOverlay( mInterface->createUnitOverlay(inEntity) );
        else
            inEntity->attachSceneOverlay( mInterface->createPuppetOverlay(inEntity) );
		*/
		return true;
    };
	
	
	
    void GfxEngine::detachFromScene(Pixy::Entity* inEntity)
    {
        //Ogre::String ownerName = inEntity->getOwner();// == ID_HOST) ? "host" : "client";
        Ogre::String nodeName = "node_";
        Ogre::String entityName = "entity_" + Ogre::StringConverter::toString(inEntity->getObjectId());
        Ogre::SceneNode* mTmpNode = NULL;
		
        //for (int i=0; i<10; i++)
        //{
		//LOG("I'm loooping now.");
		
		//nodeName = ownerName + "_node_" + Ogre::StringConverter::toString(i);
		// retrieve our node
		mTmpNode = inEntity->getSceneNode();
		
		// retrieve the entity so we can destroy it after we detach it from node
		//Ogre::Entity* mOgreEntity = (Ogre::Entity*)inEntity->getSceneObject();
		//Pixy::Entity* mUnit = Ogre::any_cast<Pixy::Entity*> (mOgreEntity->getUserAny());
		// first let's check if our entity belongs to this scenenode
		//if (mUnit->getIdEntity() == inEntity->getIdEntity())
		//if (mOgreEntity->getParentSceneNode() == mTmpNode)
		//{
		// this is our node
		
		//Ogre::MovableObject* mOgreEntity = mTmpNode->getAttachedObject(entityName);
		// detach from node
		//mInterface->destroyUnitOverlay(inEntity->getSceneObject());
		
		
		mLog->debugStream() << "I'm detaching Entity '" << inEntity->getName() << "' from SceneNode : " + mTmpNode->getName();
		mTmpNode->showBoundingBox(false);
		mTmpNode->detachObject(inEntity->getSceneObject());
		// destroy entity
		
		//                LOG("I'm destroying Entity : " + mOgreEntity->getName());
		mSceneMgr->destroyEntity((Ogre::Entity*)inEntity->getSceneObject());
		
		// translate the node back to its original position
		/*
		int idNode = parseIdNodeFromName(mTmpNode->getName());
		deque<Vector3>* mWalklist = (inEntity->getOwner() == ID_HOST) ? &hWalklist[idNode] : &cWalklist[idNode];
		mTmpNode->setPosition((*mWalklist).at(POS_PASSIVE));
		*/
		//break;
		//mTmpNode->setUserAny((Ogre::Any)NULL);
		//};
		// LOG_F(__FUNCTION__);
        //};
    }
	
    void GfxEngine::setupWaypoints()
    {
		/*
        for (int i=0; i<10; i++)
        {
            createWaypoint(ME, i);
            createWaypoint(ENEMY, i);
        };
		 */
    };
	
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
	

}