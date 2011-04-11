/*
 *  Zone.h
 *  Vertigo
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
    DODGY
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
    void engage();
    
    // stops updating the Tunnels
    void disengage();
    
    Tunnel *currentTunnel() const;
    std::string &name();
    
    // DEBUG
    void _dump();
    
	protected:
	  void parseFile();
	  void _parseSetting(const char* inName, const char* inValue);
	  void _parseObstacleSetting(const char* inName, const char* inValue);
	  void _parseTunnel(const char* inName, const char* inValue);
	  void _registerObstacleClass(const char* inName, bool fDominant);
	  
	  bool fLoaded;
	  bool fParsed;
	  
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
    
    static ::TinyXMLResourceManager* _tinyXMLResourceManager;
	private:
	  Zone();
		Zone(const Zone& src);
		Zone& operator=(const Zone& rhs);
	};
}
#endif
