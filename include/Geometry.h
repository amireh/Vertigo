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
