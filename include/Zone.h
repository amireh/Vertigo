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

#ifndef H_Zone_H
#define H_Zone_H

#include "Tunnel.h"
#include "Obstacle.h"
#include "Sphere.h"
#include <list>
#include "TinyXMLResource.h"
#include "TinyXMLResourceManager.h"
#include "tinyxml.h"
#include "tinystr.h"

using std::list;
namespace Pixy {

  typedef enum {
    ARCADE,
    SURVIVAL,
    DODGY,
    NERVEBREAK
  } GAMEMODE;
  
  struct ZoneSettings {
    GAMEMODE mMode;
    std::string mSkyboxMaterial;
    float mMoveSpeed;
    float mMaxSpeedFactor;
    float mSpeedStep;
    float mMaxSpeedStep;
    bool fResetVelocity;
    
    // obstacle settings
    int mSpawnRate;
    bool fFixedSpawnRate;
    int mObstacleCap;
    float mOMoveSpeed;
    float mOMaxSpeedFactor;
    float mOMaxSpeedStep;
    bool fFixedSpawnPosition;
    
    std::vector<OBSTACLE_CLASS> mRegisteredObstacleClasses;
    OBSTACLE_CLASS mDominantObstacleClass;
  };

	/*! \class Zone
	 *	\brief
	 *  A zone is a collection of Tunnels joined by Portals in which the player
	 *  plays. AKA: a level.
	 */
  class Level;
	class Zone : public EventListener {
		
	public:
	  /* \brief
	   * Optionally loads and prepares the zone defined in the file called inFilename.
	   * See load();
	   */
    Zone(const std::string &inFilename, bool fAutoload = true);
    ~Zone();
    
    // loads a zone from a file, this function may be called explicitly in case
    // you want to defer the parsing to a later stage than when you initialize
    // the Zone
    bool load();
    
    // updates the current Tunnel in the queue, this must be called every frame step
    void update(unsigned long lTimeElapsed);
    
    // starts processing the Tunnels in queue, effectively starting the game
    void engage(bool fForceReload = false);
    
    // stops updating the Tunnels
    void disengage();
    
    Tunnel *currentTunnel() const;
    int currentTunnelNr() const;
    std::string &name();
    std::string &filePath();
    // DEBUG
    void _dump();
    
    Tunnel *firstTunnel() const;
    
    ZoneSettings& getSettings();
    
    // gets the time this zone has been running for in seconds
    double getTimeElapsed();
    
	protected:
	  void parseFile();
	  void _parseSetting(const char* inName, const char* inValue);
	  void _parseObstacleSetting(const char* inName, const char* inValue);
	  void _parseTunnel(const char* inName, const char* inValue);
	  void _registerObstacleClass(const char* inName, bool fDominant);
	  
	  bool fLoaded;
	  bool fParsed;
	  int mCurrentTunnelNr;
	  
	  //bool evtPortalSighted(Event* inEvt);
	  bool evtPortalReached(Event* inEvt);
	  
	  EventManager *mEvtMgr;
	  Level* mLevel;
		log4cpp::Category* mLog;
		Sphere* mSphere;
		
		Tunnel *mTunnel;
    list<Tunnel*> mTunnels;
    std::string mFilename;
    std::string mName;
    ZoneSettings mSettings;
    
    unsigned long mTimeElapsed;
    
    static ::TinyXMLResourceManager* _tinyXMLResourceManager;
	private:
	  Zone();
		Zone(const Zone& src);
		Zone& operator=(const Zone& rhs);
	};
}
#endif
