/*
 *  Listener.h
 *  Elementum
 *
 *  Created by Ahmad Amireh on 3/2/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */

#ifndef H_Listener_H
#define H_Listener_H

namespace Pixy
{
	/*! \class Listener
	 *	Listeners receive events and process them using
	 *	their event handlers.
	 */
    class Event;
    class Listener
		{
        public:
            inline virtual ~Listener(){ };
            virtual bool process(Event*)=0;
			
		protected:
			virtual bool dispatch(Event*)=0;
			
		};
}
#endif // H_Listener_H
