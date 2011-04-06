// TinyXMLResourceManager.h class for Ogre
// Author: xadhoom
// License: Do whatever you want with it.

#ifndef __TinyXMLResourceManager_h__
#define __TinyXMLResourceManager_h__

#include "Ogre.h"
#include "OgreSingleton.h"
#include "OgreResourceManager.h"
#include "TinyXMLResource.h"


class TinyXMLResourceManager : public Ogre::ResourceManager, public Ogre::Singleton<TinyXMLResourceManager>
{
protected:

   // Overridden from ResourceManager
   Ogre::Resource* createImpl(const Ogre::String& name, Ogre::ResourceHandle handle, 
      const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader,
      const Ogre::NameValuePairList* params);

public:
   /** Default constructor.
   */
   TinyXMLResourceManager();

   /** Default destructor.
   */
   virtual ~TinyXMLResourceManager();

   /** Override standard Singleton retrieval.
   @par
   This method just delegates to the template version anyway,
   but the implementation stays in this single compilation unit,
   preventing link errors.
   */
   static TinyXMLResourceManager& getSingleton();

   /** Override standard Singleton retrieval.
   @par
   This method just delegates to the template version anyway,
   but the implementation stays in this single compilation unit,
   preventing link errors.
   */
   static TinyXMLResourceManager* getSingletonPtr();

   /** Creates a new TinyXMLResource specifically for manual definition rather
   than loading from an object file. 
   */
   TinyXMLPtr createManual( const Ogre::String& name, const Ogre::String& groupName, Ogre::ManualResourceLoader* loader = 0);
};

#endif  // __TinyXMLResourceManager_h__
