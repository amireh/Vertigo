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
#include "Probe.h"
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
	 *  A tunnel is made of a number of segments, and is capped at one side with
	 *  a portal.
	 */
	class GfxEngine;
	class Tunnel : public EventListener {
		
	public:
	  /*! \brief
	   *  Constructor for creating the tunnels, allows for defining all related
	   *  attributes.
	   *
	   *  @param inNrSegments: the number of tubes to make up this tunnel
	   *  @param inSegmentLength: the length of each of these segments
	   *  @param inRadius: the radius of each of these segments
	   *  @param inAutoShow: will render the moment it's created and start its portal effect
	   *  @param inCapped: if true, there will be portals placed at both ends of the tunnel
	   */
    Tunnel(const String inMaterial,
           const int inNrSegments = 10, 
           const Real inSegmentLength = 500, 
           const Real inRadius = 80, 
           const Vector3& inPosition = Vector3(0,0,0),
           bool inAutoShow = false);
    virtual ~Tunnel();
    
    void update(unsigned long lTimeElapsed);
    
    /*! \brief
     *  Emits the "PortalEntered" event, renders the tunnel and binds its
     *  event handlers.
     */
    void show();
    
    /*! \brief
     *  Remove the tunnel from the scene and stops its particle effects and unbinds
     *  its event handlers.
     */
    void hide();
        
    /*! \brief
     *  Returns the Ogre::SceneNode that is the parent of all the segment
     *  SceneNodes.
     *
     *  \note
     *  Useful for finding out the length of the whole tunnel. 
     *  Called by the Probe to find out whether it has reached the portal or not.
     */
    inline SceneNode* getNode() {
      return mNode;
    };
    
    /*! \brief
     *  Returns the SceneNode that contains the Portal of the tunnel.
     *
     *  \note
     *  Called by the Probe on evtPortalSighted to calculate a direction vector
     *  so the Probe moves to the center of the portal.
     */
    inline SceneNode* getExitPortal() {
      return mExit;
    };
    
    inline Real& _getLength() {
      return mLength;
    };
    inline Real& _getSegmentLength() {
      return mSegmentLength;
    };
    
    
    
	protected:
    /*! \brief
     *  Stops the PortalEffect.
     */
	  bool evtPortalReached(Event* inEvt);
	  
	  /*! \brief
	   *  Relocates the PortalEffect particle system to the Exit node of the tunnel
	   *  and plays the effect.
	   */
	  bool evtPortalEntered(Event* inEvt);
	  /*! \brief
	   *  Toggles sound effects playability according to settings.
	   */
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
