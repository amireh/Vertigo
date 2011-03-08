/*
 *  Handler.h
 *  Elementum
 *
 *  Created by Ahmad Amireh on 3/3/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */

#ifndef H_Handler_H
#define H_Handler_H

namespace Pixy
{
    class Event;
    class Handler
		{
        public:
            inline virtual ~Handler(){ };
            virtual bool call(Event*)=0;
		};
}
#endif // H_Handler_H
