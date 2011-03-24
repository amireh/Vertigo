/*
 *  Geometry.h
 *  Vertigo
 *
 */

#ifndef H_Geometry_H
#define H_Geometry_H

#include <Ogre.h>

using Ogre::Vector3;
using Ogre::Real;
using Ogre::SceneNode;
using Ogre::String;
using std::string;

namespace Pixy {
		
	/*! \class Geometry
	 *	\brief
	 */
	class Geometry {
		
	public:
    static void createSphere(
      const string& meshName, 
      const float r, 
      const int nRings = 16, 
      const int nSegments = 16);
  
    static void createTube(
      const string& meshName,
      const float radius,
      const int nrSegments = 24);
      
	protected:
		Ogre::Root           *mRoot;
		Ogre::SceneManager   *mSceneMgr;
		Ogre::Viewport       *mViewport;
		Ogre::RenderWindow	 *mRenderWindow;

	private:
		Geometry();
		Geometry(const Geometry& src);
		Geometry& operator=(const Geometry& rhs);
	};
}
#endif
