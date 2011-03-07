/*
 *  Engine.h
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/11/10.
 *  Copyright 2010 JU. All rights reserved.
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
			
            virtual bool setup()=0;
            virtual void update(unsigned long lTimeElapsed)=0;
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