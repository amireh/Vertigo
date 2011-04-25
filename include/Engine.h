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
    
#ifndef H_Engine_H
#define H_Engine_H

#include "Pixy.h"

namespace Pixy
{

	/*! \brief
	 *	Skeleton Engine that defines the base behaviour of all game Engines.
	 */
  class Engine
  {
    public:
      inline virtual ~Engine() { mLog = NULL; fSetup = false; };

      /*! \brief
       *  General, shared initialisation of the engine and its resources
       *  is done here.
       */
      virtual bool setup()=0;
      /*! \brief
       *  GameState-specific initialisation is carried out here.
       */
      virtual bool deferredSetup()=0;
      
      virtual void update(unsigned long lTimeElapsed)=0;
      
      /*! \brief
       *  Cleans up any resources allocated in deferredSetup();
       */
      virtual bool cleanup()=0;
		
    protected:
		  inline Engine() { mLog = NULL; fSetup = false; };
      Engine(const Engine& src);
      Engine& operator=(const Engine& rhs);
		
		  log4cpp::Category* mLog;
		  bool fSetup;
	};
};

#endif
