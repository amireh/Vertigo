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
#include "Intro.h"

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
		
		
		
		fSetup = true;
		return fSetup;
	}
	
	bool GfxEngine::deferredSetup() {
		mSphere = Intro::getSingleton().getSphere();
		mCamera->setAutoTracking (true, mSphere->getSceneNode());
		
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
		lCamPos.x = 0;
		lCamPos.y = 50;
		lCamPos.z = 100;
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
		Ogre::MeshManager* meshManager = &Ogre::MeshManager::getSingleton();
        // create floor plane
        mPlane.normal = Vector3::UNIT_Y;
		mPlane.d = 0;
		meshManager->createPlane("meshFloor",
								 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
								 mPlane,
								 1000,
								 10000,
								 10,
								 10,
								 true,
								 1,
								 50,
								 50,
								 Vector3::UNIT_Z);
		pPlaneEnt = mSceneMgr->createEntity("Plane_Floor", "meshFloor");
		pPlaneEnt->setMaterialName("Examples/GrassFloor");
		pPlaneEnt->setCastShadows(false);
		mNode = mSceneMgr->createSceneNode("Node_Floor");
		mSceneMgr->getRootSceneNode()->addChild(mNode);
		mNode->setPosition(Vector3(0,-16,0));
		mNode->attachObject(pPlaneEnt);
		mNode->showBoundingBox(true);
		 
		 
		
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
		mLog->debugStream() << "rendering entity " << inEntity->getName();
        Ogre::Entity* mEntity;
		
        String entityName = "";
		entityName += inEntity->getName();
		entityName += stringify(inEntity->getObjectId());
		
		mEntity = mSceneMgr->createEntity(entityName, inEntity->getMesh());
		mEntity->setUserAny(Ogre::Any(inEntity));
		inNode->attachObject(mEntity);
		
		inEntity->attachSceneNode(inNode);
		inEntity->attachSceneObject(mEntity);
		
		return inNode;

    };
	
	
    bool GfxEngine::attachToScene(Pixy::Entity* inEntity)
    {
		
		return renderEntity(inEntity, createNode(inEntity->getName(), 
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
		
		
		mLog->debugStream() << "I'm detaching Entity '" << inEntity->getName() << "' from SceneNode : " + mTmpNode->getName();
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
		//if (mCameraMan)
		//	mCameraMan->injectMouseDown(e, id);
	}
	
	void GfxEngine::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
	{
		//if (mCameraMan)
		//	mCameraMan->injectMouseUp(e, id);
	}
	
	
	
	void GfxEngine::createSphere(const std::string& strName, const float r, const int nRings, const int nSegments)
	{
		using namespace Ogre;
		
		MeshPtr pSphere = MeshManager::getSingleton().createManual(strName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		SubMesh *pSphereVertex = pSphere->createSubMesh();
		
		pSphere->sharedVertexData = new VertexData();
		VertexData* vertexData = pSphere->sharedVertexData;
		
		// define the vertex format
		VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
		size_t currOffset = 0;
		// positions
		vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_POSITION);
		currOffset += VertexElement::getTypeSize(VET_FLOAT3);
		// normals
		vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_NORMAL);
		currOffset += VertexElement::getTypeSize(VET_FLOAT3);
		// two dimensional texture coordinates
		vertexDecl->addElement(0, currOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
		currOffset += VertexElement::getTypeSize(VET_FLOAT2);
		
		// allocate the vertex buffer
		vertexData->vertexCount = (nRings + 1) * (nSegments+1);
		HardwareVertexBufferSharedPtr vBuf = HardwareBufferManager::getSingleton().createVertexBuffer(vertexDecl->getVertexSize(0), vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
		VertexBufferBinding* binding = vertexData->vertexBufferBinding;
		binding->setBinding(0, vBuf);
		float* pVertex = static_cast<float*>(vBuf->lock(HardwareBuffer::HBL_DISCARD));
		
		// allocate index buffer
		pSphereVertex->indexData->indexCount = 6 * nRings * (nSegments + 1);
		pSphereVertex->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(HardwareIndexBuffer::IT_16BIT, pSphereVertex->indexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
		HardwareIndexBufferSharedPtr iBuf = pSphereVertex->indexData->indexBuffer;
		unsigned short* pIndices = static_cast<unsigned short*>(iBuf->lock(HardwareBuffer::HBL_DISCARD));
		
		float fDeltaRingAngle = (Math::PI / nRings);
		float fDeltaSegAngle = (2 * Math::PI / nSegments);
		unsigned short wVerticeIndex = 0 ;
		
		// Generate the group of rings for the sphere
		for( int ring = 0; ring <= nRings; ring++ ) {
			float r0 = r * sinf (ring * fDeltaRingAngle);
			float y0 = r * cosf (ring * fDeltaRingAngle);
			
			// Generate the group of segments for the current ring
			for(int seg = 0; seg <= nSegments; seg++) {
				float x0 = r0 * sinf(seg * fDeltaSegAngle);
				float z0 = r0 * cosf(seg * fDeltaSegAngle);
				
				// Add one vertex to the strip which makes up the sphere
				*pVertex++ = x0;
				*pVertex++ = y0;
				*pVertex++ = z0;
				
				Vector3 vNormal = Vector3(x0, y0, z0).normalisedCopy();
				*pVertex++ = vNormal.x;
				*pVertex++ = vNormal.y;
				*pVertex++ = vNormal.z;
				
				*pVertex++ = (float) seg / (float) nSegments;
				*pVertex++ = (float) ring / (float) nRings;
				
				if (ring != nRings) {
					// each vertex (except the last) has six indices pointing to it
					*pIndices++ = wVerticeIndex + nSegments + 1;
					*pIndices++ = wVerticeIndex;               
					*pIndices++ = wVerticeIndex + nSegments;
					*pIndices++ = wVerticeIndex + nSegments + 1;
					*pIndices++ = wVerticeIndex + 1;
					*pIndices++ = wVerticeIndex;
					wVerticeIndex ++;
				}
			}; // end for seg
		} // end for ring
		
		// Unlock
		vBuf->unlock();
		iBuf->unlock();
		// Generate face list
		pSphereVertex->useSharedVertices = true;
		
		// the original code was missing this line:
		pSphere->_setBounds( AxisAlignedBox( Vector3(-r, -r, -r), Vector3(r, r, r) ), false );
		pSphere->_setBoundingSphereRadius(r);
		// this line makes clear the mesh is loaded (avoids memory leaks)
		pSphere->load();
	}

	void GfxEngine::update(unsigned long lTimeElapsed) {
		processEvents();
		
		//
		mCamera->setPosition(mSphere->getSceneNode()->getPosition().x,
							 mSphere->getSceneNode()->getPosition().y+100,
							 mSphere->getSceneNode()->getPosition().z-150);
		//mCameraMan->update(lTimeElapsed);
		
		using namespace Ogre;
		
	}
}