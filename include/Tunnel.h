/*
 *  Tunnel.h
 *  Vertigo
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
    
    /*
     * Remove the tunnel from the scene and stop its effects.
     */
    void hide();
    
	protected:
	  bool evtPortalSighted(Event* inEvt);
	  bool evtPortalReached(Event* inEvt);
	  
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
    
    typedef Ogre::SceneNode Portal;
    Portal *mEntrance, *mExit;
    
    Sphere* mSphere;
    SceneNode* mSphereNode;
    
    void generateSegments();
    void generatePortals();
    
    static int nrTunnels;
    static bool fMeshGenerated;
    static bool fEffectPrepared;
    static ParticleSystem* mPortalEffect;
    
	private:
		Tunnel(const Tunnel& src);
		Tunnel& operator=(const Tunnel& rhs);
	};
}
#endif
