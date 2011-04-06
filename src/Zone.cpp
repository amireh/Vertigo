#include "Zone.h"
#include "Level.h"

namespace Pixy {
  
  ::TinyXMLResourceManager* Zone::_tinyXMLResourceManager = NULL;
  
  Zone::Zone(const std::string &inFilename, bool fAutoload) {
    mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Zone");
    
    mFilename = inFilename;

    mEvtMgr = EventManager::getSingletonPtr();
    mLevel = Level::getSingletonPtr();
    
    // if the sphere isn't created yet, we'll check again in engage()
    mSphere = mLevel->getSphere();
    
		bindToName("PortalReached", this, &Zone::evtPortalReached);
		//bindToName("PortalSighted", this, &Zone::evtPortalSighted);
		    
    mTunnels.clear();
    mTunnel = 0;
    
    fLoaded = false;
    
    if (fAutoload)
      load();
      
    mLog->debugStream() << "created";
  };
  
  Zone::~Zone() {
    mTunnel = 0;
    
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
    // loop through the tunnel definitions
    //mHandler = mHandler.FirstChild("Zone");
    TiXmlNode* xTunnel = mHandler.FirstChild().ToNode();
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
    
    mLog->debugStream() << "I have found " << mTunnels.size() << " tunnel definitions";
  };
  
  bool Zone::load() {
    if (fLoaded)
      return true;
    
    try {
      parseFile();
    } catch (std::exception& e) {
      mLog->errorStream() << "could not parse zone from " << mFilename << "!!";
      throw e;
      return false;
    }
    
    mLog->debugStream() << "loaded";
    
    fLoaded = true;
    return fLoaded;
  };
  
  void Zone::engage() {
    if (!fLoaded) {
      mLog->errorStream() << "attempting to engage an unloaded zone!! aborting";
      return; 
    }
    
    if (!mSphere)
      mSphere = mLevel->getSphere();
      
    // show our first tunnel
    mTunnel = mTunnels.front();
    mTunnel->show();
        
    mLog->debugStream() << "engaged";
  };
  
  void Zone::disengage() {
    if (!fLoaded) {
      mLog->errorStream() << "attempting to disengage an unloaded zone!! aborting";
      return; 
    }
    
    mTunnel->hide();
    
    mLog->debugStream() << "disengaged";
  };
  
  void Zone::update(unsigned long lTimeElapsed) {
    processEvents();
    
    mTunnel->update(lTimeElapsed);
  };
  
  
  bool Zone::evtPortalReached(Event *inEvt) {
    // was this the last tunnel? if so, the player won
	  if (mTunnel == mTunnels.back()) {
	    // the player has won
	    mEvtMgr->hook(mEvtMgr->createEvt("PlayerWon"));
	    return true;
	  }
	  else {
	    mTunnel->hide();
	    for (std::list<Tunnel*>::iterator _itr = mTunnels.begin();
	         _itr != mTunnels.end();
	         ++_itr) {
	      if ((*_itr) == mTunnel)
	        mTunnel = *(++_itr);   // this is our next tunnel  
	    }
	  }
	  
	  // relocate the player
	  Vector3 pos = mTunnel->getNode()->getPosition();
	  btTransform trans = btTransform(btQuaternion(0,0,0,1),btVector3(pos.x,pos.y+35,pos.z));
	  mSphere->getRigidBody()->proceedToTransform(trans);
	  mSphere->getMasterNode()->setPosition(pos);
	  mLog->debugStream() << "relocating sphere to " << pos.x << ", " << pos.y << ", " << pos.z;
	  
	  // and show the shizzle
	  mTunnel->show();
	  
	  // increase obstacle spawn rate
	  mLevel->increaseSpawnRate(mTunnels.size());
	  
	  return true;
  };
  
  Tunnel* Zone::currentTunnel() const { return mTunnel; };
};
