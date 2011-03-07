/*
 *  UIEngine.cpp
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/11/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */

#include "UIEngine.h"

namespace Pixy {
	UIEngine* UIEngine::_myUIEngine = NULL;
	
	UIEngine* UIEngine::getSingletonPtr() {
		if( !_myUIEngine ) {
		    _myUIEngine = new UIEngine();
		}
		
		return _myUIEngine;
	}
	
	UIEngine::UIEngine()  {
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "UIEngine");
		mLog->infoStream() << "firing up";
		fSetup = false;
	}
	
	UIEngine::~UIEngine() {
		mLog->infoStream() << "shutting down";

		if (fSetup) {
			
			mUISystem = NULL;
			delete mLog; mLog = 0;
			fSetup = false;
		}
	}
	
	bool UIEngine::cleanup() {
		return true;
	}
	
	CEGUI::MouseButton UIEngine::convertButton(OIS::MouseButtonID buttonID)
	{
		switch (buttonID)
		{
			case OIS::MB_Left:
				return CEGUI::LeftButton;
				
			case OIS::MB_Right:
				return CEGUI::RightButton;
				
			case OIS::MB_Middle:
				return CEGUI::MiddleButton;
				
			default:
				return CEGUI::LeftButton;
		}
	}
	
	const char* UIEngine::getDataPathPrefix() const
	{
		static char dataPathPrefix[PATH_MAX];
		
#ifdef __APPLE__
		CFURLRef datafilesURL = CFBundleCopyResourceURL(CFBundleGetMainBundle(),
														CFSTR("UI"),
														0, 0);
		CFURLGetFileSystemRepresentation(datafilesURL, true,
										 reinterpret_cast<UInt8*>(dataPathPrefix),
										 PATH_MAX);
		CFRelease(datafilesURL);
#else
		char* envDataPath = 0;
		
		// get data path from environment var
		envDataPath = getenv(DATAPATH_VAR_NAME);
		
		// set data path prefix / base directory.  This will
		// be either from an environment variable, or from
		// a compiled in default based on original configure
		// options
		if (envDataPath != 0)
			strcpy(dataPathPrefix, envDataPath);
		else
			strcpy(dataPathPrefix, CEGUI_SAMPLE_DATAPATH);
#endif
		
		return dataPathPrefix;
	}
	
	bool UIEngine::setup() {
		if (fSetup)
			return true;
				
		fSetup = true;
		return true;
	}
	
	
	bool UIEngine::loadResources() { 
		
		mLog->infoStream() << "Loading resources...";
		
		return true;
	}

	
	void UIEngine::update( unsigned long lTimeElapsed ) {
		mUISystem->injectTimePulse(lTimeElapsed);
	}
	
	void UIEngine::mouseMoved( const OIS::MouseEvent &e )
	{
		mUISystem->injectMouseMove(e.state.X.rel, e.state.Y.rel);
	}
	
	void UIEngine::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		mUISystem->injectMouseButtonDown(convertButton(id));
	}
	
	void UIEngine::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		mUISystem->injectMouseButtonUp(convertButton(id));
	}
	
}