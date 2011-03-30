/*
 *  AudioEngine.h
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/15/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */


#ifndef H_AudioEngine_H
#define H_AudioEngine_H

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
	
	typedef enum {
	  SFX_EXPLOSION,
	  SFX_SHATTER,
	} SOUND_EFFECT;
	
	/*! \class AudioEngine
	 *	\brief
	 */
  class AudioEngine : public Engine, public EventListener
	{
	public:
		
		virtual ~AudioEngine();
		static AudioEngine* getSingletonPtr();
		
		virtual bool setup();
		virtual bool deferredSetup();
		virtual void update(unsigned long lTimeElapsed);
		virtual bool cleanup();
		
    void startMusic();
    void pauseMusic();
    
    void attachListener(Entity* inEntity);
		
		void playEffect(SOUND_EFFECT inEffect, SceneNode* inEmitter);
		
		OgreOggSound::OgreOggSoundManager *getSoundMgr();
	protected:

    bool evtObstacleCollided(Event* inEvt);
    bool evtPortalEntered(Event* inEvt);
    
    
		OgreOggISound *mMusicTrack;
		unsigned int idSound;
		
		Sphere* mSphere;
		Entity* mListener;
		OgreOggSound::OgreOggSoundManager *mSoundMgr;
		typedef std::map<SOUND_EFFECT, unsigned long> t_sounds;
		t_sounds mSounds;
		
	private:
		static AudioEngine* _myAudioEngine;
		AudioEngine();
		AudioEngine(const AudioEngine& src);
		AudioEngine& operator=(const AudioEngine& rhs);		
    };
};



#endif
