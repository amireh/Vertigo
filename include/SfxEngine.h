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

#ifndef H_SfxEngine_H
#define H_SfxEngine_H

#include "Pixy.h"
#include "Engine.h"
#include "Probe.h"
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
		
		void keyReleased( const OIS::KeyEvent &e );	
		
		void playMusic();
    void stopMusic();
    
    void raiseVolume();
    void lowerVolume();
    
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
    
    bool evtDroneCollided(Event* inEvt);
    bool evtPortalEntered(Event* inEvt);
    bool evtSettingsChanged(Event* inEvt);

    
		OgreOggISound *mGameTrack, *mIntroTrack;
		unsigned int idSound;
		
		float mVolume;
		Probe* mProbe;
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
