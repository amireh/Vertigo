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

#ifndef H_GfxEngine_H
#define H_GfxEngine_H

#include "Engine.h"
#include "EventListener.h"
#include "Entity.h"
#include <Ogre.h>
//#include <OgreSample.h>
//#include <OgreSdkTrays.h>
/*#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreParticle.h>*/
//#include "UIEngine.h"
#include "InputManager.h"
#include "SdkCameraMan.h"
#include "Sphere.h"
#include "ParticleUniverseSystemManager.h"
#include <map>
#include <vector>

using Ogre::Vector3;
using Ogre::Real;
using Ogre::SceneNode;
using Ogre::String;
using OgreBites::SdkCameraMan;
using std::map;
using std::pair;
using std::vector;

namespace Pixy {

  typedef enum {
    CAMERA_CHASE,
    CAMERA_FIXED,
    CAMERA_STICKY,
    CAMERA_LOLLY
  } CAMERA_MODE;
	/*! \class GfxEngine
	 *	\brief
	 *	Handles all graphics related features of the game, acts as the immediate
	 *	wrapper over OGRE3D. The game scene is entirely managed by and through
	 *	this Engine.
	 */
	class GfxEngine : public Engine, public EventListener {
		
	public:
		virtual ~GfxEngine();
		static GfxEngine* getSingletonPtr();

		
		virtual bool setup();
		virtual bool deferredSetup();
		virtual void update(unsigned long lTimeElapsed);
		virtual bool cleanup();
		
		void setCamera(const Ogre::String& inCameraName);
		void setCameraMode(CAMERA_MODE inMode);
		
		//! Attaches a Pixy::Entity to an SceneNode and renders it
		/*!
		 * Called upon by CombatManager::createUnit();
		 * Determines the appropriate empty SceneNode in which to render
		 * the given inEntity. Moreover, "attaches" Pixy::Entity
		 * to the given Ogre::Entity for later retrieval, thus,
		 * linking the GameObject with SceneObject.
		 */
		bool attachToScene(Entity* inEntity);
		
		//! Detaches a Pixy::Entity from an SceneNode and removes it from Scene
		/*!
		 * Called upon by CombatManager::destroyUnit();
		 * Detaches the Ogre::Entity from SceneNode;
		 * stops rendering it. Also detaches Pixy::Entity
		 * from the Ogre::Entity.
		 */
		void detachFromScene(Entity* inEntity);
		
		//! Moves a SceneNode to a destination using a Waypoint
		/*!
		 * Moves a matching SceneNode to inDestination.
		 * @return true the node is still moving
		 * @return false the node is done moving
		 */
		//bool moveUnit(Unit* inUnit, int inDestination);
		
		//SdkCameraMan* getCameraMan();
		Ogre::Camera* getCamera();
		Ogre::Root* getRoot();
		Ogre::SceneManager* getSM();
		Ogre::Viewport* getViewport();
		

		
		
		void mouseMoved( const OIS::MouseEvent &e );
		void mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		void mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		
		void keyPressed( const OIS::KeyEvent &e );
		void keyReleased( const OIS::KeyEvent &e );
		
		
		void applyMotionBlur(float duration);
    
    void playEffect(std::string inEffect, Entity* inEntity);
    void playEffect(std::string inEffect, const Vector3& pos);
        
    /*void updateUIShields();
    
    void hideUI();
    void showUI();*/
    
	protected:
	  void (GfxEngine::*mUpdate)(unsigned long);
	  void (GfxEngine::*mUpdateCamera)();
	  
	  void updateIntro(unsigned long lTimeElapsed);
	  void updateGame(unsigned long lTimeElapsed);
	  void updateNothing(unsigned long lTimeElapsed);
	  
	  
	  void updateCameraChase();
	  void updateCameraFixed();
	  void updateCameraSticky();
	  void updateCameraLolly();
	  
	  bool evtGameShown(Event* inEvt);
	  bool evtMenuShown(Event* inEvt);
	  bool evtZoneEntered(Event* inEvt);
	  bool evtGameStarted(Event* inEvt);
	  bool evtPlayerWon(Event* inEvt);
	  bool evtSphereDied(Event* inEvt);
	  bool evtObstacleAlive(Event* inEvt);
	  bool evtObstacleCollided(Event* inEvt);
	  bool evtPortalEntered(Event* inEvt);
	  bool evtPortalReached(Event* inEvt);
	  bool evtPortalSighted(Event* inEvt);
	  bool evtTakingScreenshot(Event* inEvt);
    
    void loadResources();
    	  
		//! Sets up OGRE SceneManager
		void setupSceneManager();
		
		//! Sets up OGRE Viewport to which our Camera will be attached
		void setupViewports();
		
		//! Sets up OGRE Camera, attaches it to Viewport, and fixes it position
		void setupCamera();
		
		//! Sets up OGRE Terrain, using terrain.cfg file for config
		void setupTerrain();
		
		//! Sets up OGRE Lights, attaches 2 spotlights, each for a Puppet respectively
		void setupLights();
		
		void setupNodes();
		void setupParticles();
	  
	  bool fPortalReached;
	  bool fPortalSighted;
	  
		Ogre::Root           *mRoot;
		Ogre::Camera         *mCamera, *mCamera2, *mCamera3, *mCamera4;
		Ogre::SceneManager   *mSceneMgr;
		Ogre::Viewport       *mViewport;
		Ogre::RenderWindow	 *mRenderWindow;
		Ogre::OverlayManager *mOverlayMgr;
		//Ogre::Overlay        *mUISheet, *mUISheetLoss, *mUISheetWin, *mUISheetPrepare;
		EventManager		 *mEvtMgr;
		SdkCameraMan		 *mCameraMan;
		//DotSceneLoader		 *mSceneLoader;
		
		Ogre::Entity* sphereEntity;
		Ogre::SceneNode* sphereNode;
		Ogre::Light *mSpotLight;
		//OgreBites::SdkTrayManager*	        mTrayMgr;
		
		//! used for setting Objects' direction in Scene
		Vector3 mDirection, mSpherePos;

		//! regulates the movement speed by (mWalkSpeed * mTimeElapsed) keeping it consistent
		Real mMoveSpeed;
		
		Pixy::Sphere* mSphere;
		
		/*!
		 * \brief Creates an Ogre::Entity and renders it in an
		 * appropriate SceneNode. Must not be called directly;
		 * only reachable via attachToScene()
		 */
		SceneNode* renderEntity(Pixy::Entity* inEntity, Ogre::SceneNode* inNode);
		
		/*!
		 * \brief Creates a SceneNode with the given attributes
		 *
		 * \note used by setupNodes()
		 * @return the created SceneNode
		 */
		SceneNode* createNode(std::string& inName, 
							  const Vector3& inPosition,
							  const Vector3& inScale,
							  const Vector3& inDirection, 
							  SceneNode* inParent=NULL);
	
	  //Ogre::FrameEvent evt;
		
		SceneNode* mPortableEffect;
		
		Ogre::String mEffect;
		Ogre::Timer mEffectTimer;//, mUITimer;
		float mEffectDuration;
		bool mEffectEnabled;
		bool mEffectStarted;
		//bool shakingScreen, reachedThreshold;
		Ogre::Quaternion mCameraOrientation;

    typedef std::map< std::string, ParticleUniverse::ParticleSystem* > effectMap;
    effectMap effects;
        
    //float mUIBarWidth;
      
    vector<SceneNode*> mTubes;
    SceneNode *mSpawnPoint;
    SceneNode *mPortal;
    //ParticleUniverse::ParticleSystem *effectShatter, *effectExplosion;
	private:
		static GfxEngine* _myGfxEngine;
		GfxEngine();
		GfxEngine(const GfxEngine& src);
		GfxEngine& operator=(const GfxEngine& rhs);
	};
}
#endif
