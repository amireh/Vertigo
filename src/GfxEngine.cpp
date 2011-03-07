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
#include "Combat.h"

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
	}
	
	bool GfxEngine::setup() {
		if (fSetup)
			return true;
		
		fSetup = true;
		return fSetup;
	}
	
	bool GfxEngine::setupCombat(std::string inPlayer1, std::string inPlayer2) {
		
		return true;
	}


	void GfxEngine::update(unsigned long lTimeElapsed) {
		 
	}
	
	bool GfxEngine::cleanup() {		
		return true;
	}
	
	
    void GfxEngine::setupSceneManager()
    {
		mLog->debugStream() << "setting up SceneManager";

    };
	
    void GfxEngine::setupViewports()
    {
		mLog->debugStream() << "setting up viewports";		
    };
	
    void GfxEngine::setupCamera()
    {
		mLog->debugStream() << "setting up cameras";	
	};
	
    void GfxEngine::setupTerrain()
    {
    };
	
    void GfxEngine::setupLights()
    {
		mLog->debugStream() << "setting up lights";
    };
	
    void GfxEngine::setupNodes()
    {
		mLog->debugStream() << "setting up nodes";

    };

	
    bool GfxEngine::attachToScene(Pixy::Entity* inEntity)
    {
		return true;
    };
	
	
	
    void GfxEngine::detachFromScene(Pixy::Entity* inEntity)
    {
    }
	
	void GfxEngine::mouseMoved( const OIS::MouseEvent &e )
	{
	}
	
	void GfxEngine::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
	{
	}
	
	void GfxEngine::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
	{
	}
	

}