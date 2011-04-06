/*
 *  SfxEngine.h
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/15/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */


#ifndef H_SfxEngine_H
#define H_SfxEngine_H

#include "Pixy.h"
#include "Engine.h"
#include "Sphere.h"
#include "OgreOggSound.h"
#include <map>

using std::map;
using Ogre::SceneNode;
using namespace OgreOggSound;
namespace Pixy
{
	

	/*! \class SfxEngine
	 *	\brief
	 */
  class SfxEngine : public Engine, public EventListener
	{
	public:
		
		virtual ~SfxEngine();
		static SfxEngine* getSingletonPtr();
		
		virtual bool setup();
		virtual bool deferredSetup();
		virtual void update(unsigned long lTimeElapsed);
		virtual bool cleanup();
		
    void startMusic();
    void pauseMusic();
    
    void toggleAudioState();
    
    void attachListener(Entity* inEntity);
				
		OgreOggSound::OgreOggSoundManager *getSoundMgr();
	protected:
    void (SfxEngine::*mUpdater)(unsigned long);
    bool (SfxEngine::*mSetup)();
    
    bool setupIntro();
    bool setupGame();
    
    void updateIntro(unsigned long lTimeElapsed);
    void updateGame(unsigned long lTimeElapsed);
    
    bool evtObstacleCollided(Event* inEvt);
    bool evtPortalEntered(Event* inEvt);
  
    
		OgreOggISound *mMusicTrack;
		unsigned int idSound;
		
		Sphere* mSphere;
		Entity* mListener;
		OgreOggSound::OgreOggSoundManager *mSoundMgr;
    
    bool fAudioStopped;		
	private:
		static SfxEngine* _mySfxEngine;
		SfxEngine();
		SfxEngine(const SfxEngine& src);
		SfxEngine& operator=(const SfxEngine& rhs);		
    };
};



#endif
