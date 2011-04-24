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
 
#ifndef H_Tunnel_H
#define H_Tunnel_H

#include "EventManager.h"
#include "EventListener.h"
#include "Sphere.h"
#include <Ogre.h>
#include "Procedural.h"
#include "ParticleUniverseSystemManager.h"
#include <vector>

using Ogre::Vector3;
using Ogre::Real;
using Ogre::SceneNode;
using Ogre::String;
using ParticleUniverse::ParticleSystem;

namespace OgreOggSound {
  class OgreOggISound;
};

namespace Pixy {
  
	/*! \class Tunnel
	 *	\brief
	 */
	class GfxEngine;
	class Tunnel : public EventListener {
		
	public:
	  /*
	   * A tunnel is made of a number of segments, and is capped at one side with
	   * a portal, or capped on both ends.
	   *
	   *  inNrSegments: the number of tubes to make up this tunnel
	   *  inSegmentLength: the length of each of these segments
	   *  inRadius: the radius of each of these segments
	   *  inAutoShow: will render the moment it's created and start its portal effect
	   *  inCapped: if true, there will be portals placed at both ends of the tunnel
	   */
    Tunnel(const String inMaterial,
           const int inNrSegments = 10, 
           const Real inSegmentLength = 500, 
           const Real inRadius = 80, 
           const Vector3& inPosition = Vector3(0,0,0),
           bool inAutoShow = false);
    virtual ~Tunnel();
    
    void update(unsigned long lTimeElapsed);
    
    void show();
    
    // changes the materials of all the segments in this tunnel
    //void applyMaterial(Ogre::String inMaterial);
    
    SceneNode* getNode();
    
    SceneNode* getEntrancePortal();
    SceneNode* getExitPortal();
    
    Real& _getLength();
    Real& _getSegmentLength();
    
    /*
     * Remove the tunnel from the scene and stop its effects.
     */
    void hide();
    
	protected:
	  bool evtPortalSighted(Event* inEvt);
	  bool evtPortalReached(Event* inEvt);
	  bool evtPortalEntered(Event* inEvt);
	  bool evtSettingsChanged(Event* inEvt);
	  
	  EventManager *mEvtMgr;
		GfxEngine	*mGfxEngine;
		Ogre::SceneManager* mSceneMgr;
		ParticleUniverse::ParticleSystemManager* mFxMgr;
		log4cpp::Category* mLog;
		
    SceneNode *mNode;
    
    std::vector<SceneNode*> mSegments;
    int mNrSegments;
    int idObject;
    
    Real mSegmentLength;
    Real mLength;
    Real mRadius;
    String mMaterial;
    
    bool fPassedEntrance; // tracker for when the player passes the entrance portal
    bool fPortalSighted; // triggered when the player almost reaches the exit
    bool fPortalReached;
    bool fHasSfx;
    bool fHasFx;
    
    typedef Ogre::SceneNode Portal;
    Portal *mEntrance, *mExit;
    
    //Sphere* mSphere;
    //SceneNode* mSphereNode;
    
    void generateSegments();
    void generatePortals();
    
    static int nrTunnels;
    static bool fMeshGenerated;
    static bool fEffectPrepared;
    static ParticleSystem* mPortalEffect;
    
    static OgreOggSound::OgreOggISound *mSfxPortal;
    
	private:
		Tunnel(const Tunnel& src);
		Tunnel& operator=(const Tunnel& rhs);
	};
}
#endif
