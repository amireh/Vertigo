/*
 *  EventHandler.h
 *  Elementum
 *
 *  Created by Ahmad Amireh on 3/3/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */

#ifndef H_EventHandler_H
#define H_EventHandler_H

#include <iostream>
#include <utility>

#include "Handler.h"

namespace Pixy
{
    class Event;
	
    /*! \class EventHandler
     *
     *  \brief
     *  Functionoid encapsulating event handling methods
     *
     *  \note
     *  You should never need to create, call, or register an EventHandler
     *  directly. Use EventListener for binding instead.
     */
    template < class T, class EventType >
    class EventHandler : public Handler	{
        public:
            typedef bool (T::*methodPtr)(EventType*);
			
            //! ctor
            inline EventHandler(T* inInstance, methodPtr inMethod) :
            mClassInstance(inInstance),
            mMethod(inMethod)
            {
            };
			
            //! dtor
            inline virtual ~EventHandler()
            {
                mClassInstance = NULL;
                mMethod = NULL;
            };
			
            /*!
             *  \brief
             *  Responsible of calling the appropriate handler method of the subscriber.
             *
             *  \remarks
             *  Depending on the EventType, call() will be able to determine on runtime
             *  the proper registered method to call.
             *
             *  \note
             *  This method is called internally by Pixy::EventHandler
             *  and should not be called directly.
             */
            inline virtual bool call(Event* inEvt)
            {
                return ((*mClassInstance).*mMethod)(static_cast<EventType*>(inEvt));
            };
			
        protected:
            T* mClassInstance;
            methodPtr mMethod;
		};
}
#endif // H_EventHandler_H
