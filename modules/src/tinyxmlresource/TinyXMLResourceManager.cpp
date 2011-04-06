// TinyXMLResourceManager.cpp class for Ogre
// Author: xadhoom
// License: Do whatever you want with it.

#include "TinyXMLResourceManager.h"

using namespace Ogre;


template<> TinyXMLResourceManager* Singleton<TinyXMLResourceManager>::ms_Singleton = 0;
TinyXMLResourceManager* TinyXMLResourceManager::getSingletonPtr()
{
   return ms_Singleton;
}


TinyXMLResourceManager& TinyXMLResourceManager::getSingleton()
{
   assert( ms_Singleton );  return ( *ms_Singleton );
}


TinyXMLResourceManager::TinyXMLResourceManager()
{
   // Resource type
   mResourceType = "TinyXML";

   // Register with resource group manager
   ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}


TinyXMLResourceManager::~TinyXMLResourceManager()
{
   // Resources cleared by superclass
   // Unregister with resource group manager
   ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}


Resource* TinyXMLResourceManager::createImpl(const String& name, ResourceHandle handle,
                                             const String& group, bool isManual, ManualResourceLoader* loader,
                                             const NameValuePairList* params)
{
   return OGRE_NEW TinyXMLResource(this, name, handle, group, isManual, loader);
}


TinyXMLPtr TinyXMLResourceManager::createManual( const String& name, const String& groupName, ManualResourceLoader* loader)
{
   // Don't try to get existing, create should fail if already exists
   return create(name, groupName, true, loader);
}
