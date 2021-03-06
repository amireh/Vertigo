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
 
#include "Zone.h"
#include "Level.h"

namespace Pixy {
  
  ::TinyXMLResourceManager* Zone::_tinyXMLResourceManager = NULL;
   
  Zone::Zone(const std::string &inFilename, bool fAutoload) {
    mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Zone");
    
    mFilename = inFilename;

    // set some defaults
    mSettings.mMode = ARCADE;
    mSettings.mMoveSpeed = 6;
    mSettings.mMaxSpeedFactor = 3;
    mSettings.mSpeedStep = 0.01f;
    mSettings.mMaxSpeedStep = 0.25f;
    mSettings.fResetVelocity = false;
    mSettings.mSpawnRate = 900;
    mSettings.fFixedSpawnRate = false;
    mSettings.mOMoveSpeed = 2;
    mSettings.mOMaxSpeedFactor = 3;
    mSettings.mOMaxSpeedStep = 0.25f;
    mSettings.fFixedSpawnPosition = false;
    
    mCurrentTunnelNr = 0;
    mTimeElapsed = 0;
    
    mEvtMgr = EventManager::getSingletonPtr();
    mLevel = Level::getSingletonPtr();
    
    // if the sphere isn't created yet, we'll check again in engage()
    mProbe = mLevel->getProbe();
    
		bindToName("PortalReached", this, &Zone::evtPortalReached);
		
    mTunnels.clear();
    mTunnel = 0;
    
    fLoaded = false;
    
    if (fAutoload)
      load();
      
    mLog->debugStream() << "created";
  };
  
  Zone::~Zone() {
    mTunnel = 0;
    
    unbind("PortalReached", this);
    
		for (std::list<Tunnel*>::iterator _itr = mTunnels.begin(); 
		     _itr != mTunnels.end();
		     ++_itr) {
		    //mLog->debugStream() << "updating objects";
		  delete *_itr;
		}
		mTunnels.clear(); 
		
		mLog->debugStream() << "destroyed";
		if (mLog)
		  delete mLog;
		
		fLoaded = false;
		
		mLog = 0;   
  };
  
  void Zone::parseFile() {

    // create our ResourceManager
    if (!_tinyXMLResourceManager)
      _tinyXMLResourceManager = OGRE_NEW ::TinyXMLResourceManager();

    // create a new resource for our xml file 'MyFile.xml'
    ::TinyXMLPtr mXMLFile = ::TinyXMLResourceManager::getSingleton().create(mFilename, "General");

    // actually load our xml file
    mXMLFile->load();

    // if there was no loading error we can now retrieve the TiniYML node below
    if(mXMLFile->getError())
    {
       mLog->errorStream() << "An error occured in file " 
        << mXMLFile->getName() 
        << " : " 
        << Ogre::String(mXMLFile->getErrorDesc());
        
       return;
    }
    
    TiXmlDocument* mRoot = static_cast<TiXmlDocument*>(mXMLFile->getXMLData());
    TiXmlHandle mHandler(mRoot->RootElement());

    // zone name
    mName = mRoot->RootElement()->Attribute("Name");

    mLog->debugStream() << "name: " << mName;

    // parse general settings
    TiXmlElement* xSetting = mHandler.FirstChild("Settings").FirstChild().ToElement();
    while (xSetting) {
      _parseSetting(xSetting->Attribute("Name"), xSetting->Attribute("Value"));
      xSetting = xSetting->NextSiblingElement();
    }
    
    // parse obstacle settings
    TiXmlElement* xDroneSetting = mHandler.FirstChild("Drones").FirstChild().ToElement();
    while (xDroneSetting) {
      if (std::string(xDroneSetting->Value()) == "Class")
        _registerDroneClass(xDroneSetting->Attribute("Name"), xDroneSetting->Attribute("Dominant") != NULL);
      else if (std::string(xDroneSetting->Value()) == "Property")
        _parseDroneSetting(xDroneSetting->Attribute("Name"), xDroneSetting->Attribute("Value"));
      else
        mLog->errorStream() << "invalid obstacle setting!! " << 
          "<" << xDroneSetting->Value() <<
          " " << xDroneSetting->FirstAttribute()->Name() <<
          "=\"" << xDroneSetting->FirstAttribute()->Value() << "\">";
      
      xDroneSetting = xDroneSetting->NextSiblingElement();
    }
    
    // if there were no obstacle classes registered, register a default dumb one
    if (mSettings.mRegisteredDroneClasses.empty()) {
      mSettings.mRegisteredDroneClasses.push_back(DUMB);
      mSettings.mDominantDroneClass = DUMB;
    }
     
    // loop through the tunnel definitions
    TiXmlNode* xTunnel = mHandler.FirstChild("Geometry").FirstChild().ToNode();
    while (xTunnel != NULL)
    {

      //mLog->debugStream() << "parsing a tunnel";

      TiXmlElement* mTunnelProperty = xTunnel->FirstChild()->ToElement();
      
      // parse the material
      std::string xMaterial = mTunnelProperty->LastAttribute()->Value();
      
      //mLog->debugStream() << "Tunnel material: " << xMaterial;
      
      // number of segments
      mTunnelProperty = mTunnelProperty->NextSiblingElement();
      int xNrSegments = convertTo<int>(mTunnelProperty->LastAttribute()->Value());
      
      //mLog->debugStream() << "Tunnel # segments: " << xNrSegments;
      
      // and finally segment length
      mTunnelProperty = mTunnelProperty->NextSiblingElement();
      int xSegmentLength = convertTo<int>(mTunnelProperty->LastAttribute()->Value());
      
      //mLog->debugStream() << "Tunnel segment length: " << xSegmentLength;
      
      // now create our tunnel and store it
      mTunnels.push_back(new Tunnel(xMaterial, xNrSegments, xSegmentLength));

      xTunnel = xTunnel->NextSibling();
    }
    
    //_dump();
    //mLog->debugStream() << "I have found " << mTunnels.size() << " tunnel definitions";
  };
  
  void Zone::_dump() {
    std::ostringstream msg;
    msg << "-- Settings --\n";
    msg << "Game Mode: " << 
      ((mSettings.mMode == ARCADE) ? "Arcade"
      : (mSettings.mMode == SURVIVAL) ? "Survival"
      : (mSettings.mMode == DODGY) ? "Dodgy" : "Nervebreak")
       << "\n";
    msg << "Skybox: " << mSettings.mSkyboxMaterial << "\n";
    msg << "MoveSpeed: " << mSettings.mMoveSpeed << "\n";
    msg << "MaxSpeedFactor: " << mSettings.mMaxSpeedFactor << "\n";
    msg << "SpeedStep: " << mSettings.mSpeedStep << "\n";
    msg << "MaxSpeedStep: " << mSettings.mMaxSpeedStep << "\n";
    msg << "Reset velocity? " << (mSettings.fResetVelocity ? "Yes" : "No") << "\n";
    
    msg << "-- Drone Settings --\n";
    msg << "Spawn Rate: " << mSettings.mSpawnRate << "\n";
    msg << "Fixed spawn rate?: " << (mSettings.fFixedSpawnRate ? "Yes" : "No") << "\n";
    msg << "Registered obstacle classes:\n";
    for (int i =0; i <mSettings.mRegisteredDroneClasses.size(); ++i) {
      std::string _oc;
      switch (mSettings.mRegisteredDroneClasses[i]) {
        case CHASE:
          _oc = "Chase";
          break;
        case DUMB:
          _oc = "Dumb";
          break;
        case SPINNER:
          _oc = "Spinner";
          break;
        case DUETTE:
          _oc = "Duette";
          break;
      }
      msg << "\t[" << i+1 << "] Drone class: " << _oc;
      if (mSettings.mDominantDroneClass == mSettings.mRegisteredDroneClasses[i])
        msg << " and is dominant";
      
      msg << "\n";
    }
    
    msg << "-- Tunnels --\n";
    msg << "There are " << mTunnels.size() << " tunnel definitions parsed.";
    
    mLog->debugStream() << msg.str();
  };
  
  void Zone::_parseSetting(const char* inCName, const char* inCValue) {
    
    std::string inName(inCName);
    std::string inValue(inCValue);
    
    // parse game mode
    if (inName == "Mode") {
      GAMEMODE _mode;
      if (inValue == "Arcade")
        _mode = ARCADE;
      else if (inValue == "Survival")
        _mode = SURVIVAL;
      else if (inValue == "Dodgy")
        _mode = DODGY;
      else if (inValue == "Nervebreak")
        _mode = NERVEBREAK;
      else {
        mLog->errorStream() << "Invalid game mode " << inValue << ", resetting to Arcade";
        _mode = ARCADE;
      }
      mSettings.mMode = _mode;
      
    } else if (inName == "Skybox") {
      mSettings.mSkyboxMaterial = inValue;
      // TODO: validate whether material exists and respond accordingly
    } else if (inName == "MoveSpeed") {
      mSettings.mMoveSpeed = convertTo<float>(inValue);
    } else if (inName == "MaxSpeedFactor") {
      mSettings.mMaxSpeedFactor = convertTo<float>(inValue);
    } else if (inName == "SpeedStep") {
      mSettings.mSpeedStep = convertTo<float>(inValue);
    } else if (inName == "MaxSpeedStep") {
      mSettings.mMaxSpeedStep = convertTo<float>(inValue);
    } else if (inName == "ResetVelocity") {
      mSettings.fResetVelocity = (inValue == "Yes") ? true : false;
    }
  };
  
  void Zone::_parseDroneSetting(const char* inCName, const char* inCValue) {
    std::string inName(inCName);
    std::string inValue(inCValue);
    
    if (inName == "SpawnRate")
      mSettings.mSpawnRate = convertTo<int>(inValue);
    else if (inName == "FixedSpawnRate")
      mSettings.fFixedSpawnRate = (inValue == "Yes") ? true : false;
    else if (inName == "MoveSpeed")
      mSettings.mOMoveSpeed = convertTo<float>(inValue);
    else if (inName == "MaxSpeedFactor")
      mSettings.mOMaxSpeedFactor = convertTo<float>(inValue);
    else if (inName == "MaxSpeedStep")
      mSettings.mOMaxSpeedStep = convertTo<float>(inValue);
    else if (inName == "FixedSpawnPosition")
      mSettings.fFixedSpawnPosition = (inValue == "Yes") ? true : false;
  };
  
  void Zone::_registerDroneClass(const char* inCName, bool fDominant) {
    std::string inName(inCName);
    
    if (inName == "Dumb") {
      mSettings.mRegisteredDroneClasses.push_back(DUMB);
      if (fDominant)
        mSettings.mDominantDroneClass = DUMB;
    } else if (inName == "Chase") {
      mSettings.mRegisteredDroneClasses.push_back(CHASE);
      if (fDominant)
        mSettings.mDominantDroneClass = CHASE;    
    } else if (inName == "Duette") {
      mSettings.mRegisteredDroneClasses.push_back(DUETTE);
      if (fDominant)
        mSettings.mDominantDroneClass = DUETTE;
    } else if (inName == "Spinner") {
      mSettings.mRegisteredDroneClasses.push_back(SPINNER);
      if (fDominant)
        mSettings.mDominantDroneClass = SPINNER;
    }
    
    // if there was a dominant class, add more instances of it to our queue thus
    // increasing the chance of it spawning
    if (fDominant)
      for (int i=0; i<2;++i)
        mSettings.mRegisteredDroneClasses.push_back(mSettings.mDominantDroneClass);
  }
  
  bool Zone::load() {
    if (fLoaded)
      return true;
    
    try {
      parseFile();
    } catch (std::exception& e) {
      mLog->errorStream() << "could not parse zone from " << mFilename << "!! " << e.what();
      throw e;
      return false;
    }
    
    mLog->debugStream() << "loaded";
    
    fLoaded = true;
    return fLoaded;
  };
  
  void Zone::engage(bool fForceReload) {
    if (!fLoaded) {
      mLog->errorStream() << "attempting to engage an unloaded zone!! aborting";
      return; 
    }
    
    // if the zone is already engaged, do nothing
    //if (mTunnel)
    //  return;
      
    //if (!mProbe)
    mProbe = mLevel->getProbe();
    
    mLog->debugStream() << " I have " << mTunnels.size() << " tunnels";
    // show our first tunnel
    if (!mTunnel)
      mTunnel = mTunnels.front();
    
    //if (fForceReload)
    //  mTunnel->hide();
    
    mTunnel->show();
    
    mCurrentTunnelNr = 0;
    
    // change sky
    
    mLog->debugStream() << "engaged";
  };
  
  void Zone::disengage() {
    if (!fLoaded || !mTunnel) {
      mLog->errorStream() << "attempting to disengage an unloaded zone!! aborting";
      return; 
    }
    
    mTunnel->hide();
    
    mTunnel = 0;
    
    mLog->debugStream() << "disengaged";
  };
  
  void Zone::update(unsigned long lTimeElapsed) {
    processEvents();
    
    mTimeElapsed += lTimeElapsed;
    mTunnel->update(lTimeElapsed);
  };
  
  
  bool Zone::evtPortalReached(Event *inEvt) {
    // was this the last tunnel? if so, the player won
	  if (mTunnel == mTunnels.back()) {
	    // if it's survival mode, there's no winning, just go back to tunnel #1
	    if (mSettings.mMode == SURVIVAL) {
	      mTunnel->hide();
	      mTunnel = mTunnels.front();
	      ++mCurrentTunnelNr;
	    } else {
	      // the player has won
	      mEvtMgr->hook(mEvtMgr->createEvt("PlayerWon"));
	      return true;
	    }
	  }
	  else {
	    mTunnel->hide();
	    for (std::list<Tunnel*>::iterator _itr = mTunnels.begin();
	         _itr != mTunnels.end();
	         ++_itr) {
	      if ((*_itr) == mTunnel)
	        mTunnel = *(++_itr);   // this is our next tunnel  
	    }
	    
	    ++mCurrentTunnelNr;
	  }
	  
	  // relocate the player
	  Vector3 pos = mTunnel->getNode()->getPosition();
	  btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(pos.x,pos.y+35,pos.z));
	  mProbe->getRigidBody()->proceedToTransform(trans);
	  mProbe->getMasterNode()->setPosition(pos);
	  mLog->debugStream() << "relocating sphere to " << pos.x << ", " << pos.y << ", " << pos.z;
	  
	  // and show the shizzle
	  mTunnel->show();
	  
	  // increase obstacle spawn rate
	  if (!mSettings.fFixedSpawnRate)
	    mLevel->increaseSpawnRate(mTunnels.size());
	  
	  return true;
  };
  
  Tunnel* Zone::currentTunnel() const { return mTunnel; };
  std::string& Zone::name() { return mName; };
  
  ZoneSettings& Zone::getSettings() { return mSettings; };
  int Zone::currentTunnelNr() const { return mCurrentTunnelNr; };
  
  std::string& Zone::filePath() { return mFilename; };
  
  Tunnel* Zone::firstTunnel() const { 
    if (!mTunnels.empty()) 
      return mTunnels.front();
    
    return NULL;
  };
  
  double Zone::getTimeElapsed() { return mTimeElapsed / 1000.0f; };
};
