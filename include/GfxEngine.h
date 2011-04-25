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

#include <Ogre.h>

#include "Engine.h"
#include "EventListener.h"
#include "Entity.h"
#include "InputManager.h"
#include "Probe.h"
#include "ParticleUniverseSystemManager.h"
#include <map>
#include <vector>

using Ogre::Vector3;
using Ogre::Real;
using Ogre::SceneNode;
using Ogre::String;
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
		
		/*! \brief
		 *  Creates a SceneNode and an Ogre::Entity using the mesh specified by
		 *  Pixy::Entity's mMesh. If successful, the handles will be assigned to
		 *  the Entity.
		 *
		 *  \note
		 *  Called internally upon Entity::live();
		 */
		bool attachToScene(Entity* inEntity);
		
		/*! \brief
		 *  Detaches a Pixy::Entity from a SceneNode and removes it from the scene.
		 *
		 *  \note
		 *  This is called internally by all Entities upon destruction.
		 */
		void detachFromScene(Entity* inEntity);
	
	  inline Ogre::Camera* getCamera() {
		  return mCamera;
	  };
	
	  inline Ogre::Root* getRoot() {
		  return mRoot;
	  };
	
	  inline Ogre::SceneManager* getSM() {
		  return mSceneMgr;
	  };
	
	  inline Ogre::Viewport* getViewport() {
		  return mViewport;
	  };
		
		/*! \brief
		 *  Bindings for switching camera mode and taking screenies.
		 */
		void keyPressed( const OIS::KeyEvent &e );
		
		/*! \brief
		 *  Activates the RadialBlur compositor on the current viewport for duration
		 *  seconds.
		 *
		 *  \note
		 *  Called by Probe on a miss collision.
		 */
		void applyMotionBlur(float duration);
    
	protected:
	  void (GfxEngine::*mUpdate)(unsigned long);
	  void (GfxEngine::*mUpdateCamera)();
	  
	  /*! \brief
	   *  Updates the camera according to its mode, and activates compositor
	   *  effects if requested.
	   */
	  void updateGame(unsigned long lTimeElapsed);
	  /*! \brief
	   *  Switched to when in Intro state or the menu is shown.
	   */
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
	  bool evtProbeDied(Event* inEvt);
	  bool evtDroneCollided(Event* inEvt);
	  bool evtPortalEntered(Event* inEvt);
	  bool evtTakingScreenshot(Event* inEvt);
    
		//! Sets up OGRE SceneManager
		void setupSceneManager();
		
		//! Sets up OGRE Viewport to which our Camera will be attached
		void setupViewport();
		
		//! Sets up OGRE Camera, attaches it to Viewport, and fixes it position
		void setupCamera();
		
		//! Sets up OGRE Terrain, using terrain.cfg file for config
		void setupSky();
		
		//! Sets up OGRE Lights, attaches 2 spotlights, each for a Puppet respectively
		void setupLights();
		
		void setupParticles();

    void playEffect(std::string inEffect, Entity* inEntity);
    void playEffect(std::string inEffect, const Vector3& pos);
    	  
	  bool fPortalReached;
	  bool fPortalSighted;
	  
		Ogre::Root           *mRoot;
		Ogre::Camera         *mCamera, *mCamera2, *mCamera3, *mCamera4;
		Ogre::SceneManager   *mSceneMgr;
		Ogre::Viewport       *mViewport;
		Ogre::RenderWindow	 *mRenderWindow;
		Ogre::OverlayManager *mOverlayMgr;
		EventManager		 *mEvtMgr;
		
		Ogre::Entity* sphereEntity;
		Ogre::SceneNode* sphereNode;
		Ogre::Light *mSpotLight;
		
		//! used for setting Objects' direction in Scene
		Vector3 mDirection, mProbePos;

		//! regulates the movement speed by (mWalkSpeed * mTimeElapsed) keeping it consistent
		Real mMoveSpeed;
		
		Pixy::Probe* mProbe;
		
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
