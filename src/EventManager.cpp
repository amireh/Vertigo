/*
 *  EventManager.cpp
 *  Elementum
 *
 *  Created by Ahmad Amireh on 3/2/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */

#include "EventManager.h"
#include "EventListener.h"
/*
#include "UIEvt_Register.h"
#include "UIEvt_Login.h"
#include "UIEvt_Profile.h"
#include "SpellEvent.h"
 */
using std::cout;
namespace Pixy {

	EventManager* EventManager::mInstance = NULL;
	
	EventManager::EventManager() {
		//mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "EvtMgr"); 
		//mIdCounter = 0;
		cout << "EventManager: up and running\n";
	}
	
	EventManager::~EventManager()
	{
		std::cout << "EventManager: shutting down\n";
		// clean up events
		while (!mEventPool.empty()) {
			//cout << "removing event from pool\n";
			mEventPool.pop_back();
			//releaseEvt(mEventPool.back());
		}		
		while (!mEvents.empty()) {
			//cout << "removing event from queue\n";
			mEvents.pop_back();
		}
		
		// clean up Listeners

		mInstance = 0;

		mFullSubscribers.clear();
		mNameSubscribers.clear();
		mCatSubscribers.clear();
		/*
		if (mLog)
			delete mLog;
		
		mLog = 0;*/
		//mIdCounter = 0;
	};
	
	//! Retrieve the singleton instance of this class
	EventManager* EventManager::getSingletonPtr() {
		return mInstance = (!mInstance) ? new EventManager() : mInstance;
	}
	
	EventManager& EventManager::getSingleton() {
		return *getSingletonPtr();
	}
	
	void EventManager::shutdown() {
		delete mInstance;
	}
	
	void EventManager::subscribeToName(std::string inName, EventListener* inListener)
	{
		//std::cout << "subscribing a listener to events named `" << inName << "'";
		
		// is the event registered yet?
		name_subscription_t::iterator _itr = mNameSubscribers.find(inName);
		
		if (_itr == mNameSubscribers.end()) {
			// event doesn't exist in our map yet, register it
			vector<EventListener*> _listeners;
			_itr = mNameSubscribers.insert(make_pair(inName, _listeners)).first;
		}
		
		// is the listener already registered?
		if (alreadySubscribed(inListener, &_itr->second))
			return;

		
		//std::cout << "registering listener to event named " << inName;
		// it's safe to subscribe this Listener
		_itr->second.push_back(inListener);
	};
	
	void EventManager::subscribeToCat(std::string inCategory, EventListener* inListener)
	{
		//std::cout << "subscribing a listener to category `" << inCategory << "'";
		
		// is the event registered yet?
		cat_subscription_t::iterator _itr = mCatSubscribers.find(inCategory);
		
		if (_itr == mCatSubscribers.end()) {
			// event doesn't exist in our map yet, register it
			vector<EventListener*> _listeners;
			_itr = mCatSubscribers.insert(make_pair(inCategory, _listeners)).first;
		}
		
		// is the listener already registered?
		if (alreadySubscribed(inListener, &_itr->second))
			return;
		
		// it's safe to subscribe this Listener
		_itr->second.push_back(inListener);
	};
	
	void EventManager::subscribeToAll(EventListener* inListener) {
		if (alreadySubscribed(inListener, &mFullSubscribers))
			return;
		
		mFullSubscribers.push_back(inListener);
	}
	
	void EventManager::hook(Event* inEvt)
	{
		/* \------------------------------------------ *\
		 *
		 *  @ mPos : a map iterator pointing to the position
		 *      at which the Event will be inserted
		 *
		 *  @ evtItr : iterator used to find and hold
		 *      the Event we will be processing
		 *
		 *  @ posItr : iterator used to determine mPos
		 *
		 *  @ fFound : flag indicates whether a position was
		 *      found for a Minor event
		 *
		 * \------------------------------------------- */
		
		deque<Event*>::iterator mPos, posItr;
		
		int evtRank = inEvt->getWeight();
		
		// if the queue is empty, hook to front
		if (mEvents.empty()) {
			mPos = mEvents.begin();
			
		} 
		else if (evtRank == MINOR_EVT)
		{
			// if the first event in the queue is a major one, then
			// there is no need to iterate; we've found our position
			if (mEvents.front()->getWeight() == MAJOR_EVT)
				mPos = mEvents.begin();
			
			else
			{
				bool fFound = false;
				posItr = mEvents.begin();
				for (posItr; posItr != mEvents.end(); ++posItr)
				{
					// keep traversing until we find a Major event to push before
					if ((*posItr)->getWeight() == MAJOR_EVT)
					{
						mPos = posItr;
						fFound = true;
						break;
					}
				}
				
				if (!fFound)
					mPos = mEvents.end();
			}
		}
		else // the event is a major one; add to the rear
		{
			mPos = mEvents.end();
		}
		
		mEvents.insert(mPos, inEvt);
		//std::cout << inEvt << " is hooked";
	}; // hook()
	
	
	void EventManager::update()
	{
		if (!mEvents.empty())
		{
			dispatch(mEvents.front());
			cout << "EventManager:" << mEvents.front() << " was dispatched\n";
			dequeue();
		}
	}; // update()
	
	
	void EventManager::dispatch(Event* inEvt)
	{
		/*
		 *	a) notify all listeners with full subscription
		 *	b) notify all listeners subscribed to the event's category
		 *	c) notify all listeners subscribed to the event's name
		 */
	
		/* \------------------------------------------------------------ /*
		 *
		 *
		 *  @ mSubscribers : ptr to the container of the Event's
		 *      subscribers; this list will be transferred to our
		 *      mSubMap map.
		 *
		 *  @ _vecItr : iterates through the original subscribers
		 *      list during the transfer
		 *
		 *  @ mSubMap : maps each subscribed registered to the Event along
		 *      with its status; the flag indicates whether this helper method
		 *      is done processing or not. The Event will be flagged as EVT_DONE
		 *      only when ALL subscribers in the map are flagged as true.
		 *
		 *  @ _mapItr : used for iterating through the map and calling
		 *      the subscribers; and finally for determining the count
		 *      of subscribers handled
		 *
		 *  @ fDone : tracks the status of the subscribers;
		 *      when fDone is true, the Event will be dispatched.
		 *
		 *
		 * \--------------------------------------------------------------- */
		
		vector<EventListener*>* evtListeners = 0;
		vector<EventListener*>::const_iterator _listener;
		cat_subscription_t::iterator _catFinder;
		name_subscription_t::iterator _nameFinder;
		
		// if event is a request (outgoing to server)
		// then notify only the network manager
		if (inEvt->getType() == EVT_REQ && mNetworkDispatcher) {
			mNetworkDispatcher->enqueue(inEvt);
			return;
		}
		
		//mLog->debugStream() << "dispatching event: " << inEvt->getId() << " from server";
		
		// call full subscribed listeners
		evtListeners = &mFullSubscribers;
		_listener = evtListeners->begin();
		for (_listener; _listener != evtListeners->end(); ++_listener)
			(*_listener)->enqueue(inEvt);
		
		
		// are there any listeners for this named event?
		_nameFinder = mNameSubscribers.find(inEvt->getName());
		if (_nameFinder != mNameSubscribers.end()) {
			// there are listeners interested in this named event
			// notify them
			evtListeners = &_nameFinder->second;
			_listener = evtListeners->begin();
			for (_listener; _listener != evtListeners->end(); ++_listener)
				(*_listener)->enqueue(inEvt);
			
		}
		
		// are there any listeners for this category?
		_catFinder = mCatSubscribers.find(inEvt->getCategory());
		if (_catFinder != mCatSubscribers.end()) {
			
			// there are, notify them
			evtListeners = &_catFinder->second;
			_listener = evtListeners->begin();
			for (_listener; _listener != evtListeners->end(); ++_listener)
				(*_listener)->enqueue(inEvt);
		}

	} // dispatch()

	void EventManager::dequeue() {
		// remove from queue
		//mLog->debugStream() << "dequeuing event";
		Event* evt = mEvents.front();
		if (evt->getNrHandlers() == 0) {
			cout << "WARN - " << evt << " has no handlers! Releasing...\n";
			releaseEvt(evt);
			return;
		}
		mEvents.pop_front();

	}
	
	void EventManager::subscribeNetworkDispatcher(EventListener* inInstance) {
		mNetworkDispatcher = inInstance;
	}

	Event* EventManager::createEvt(std::string inName) {
		Event* lEvt = new Event(inName);
		mEventPool.push_back(lEvt);
		return lEvt;
	}
	/*
	EntityEvent* EventManager::createEntityEvt(std::string inName) {
		EntityEvent* lEvt = new EntityEvent(inName);
		mEventPool.push_back(lEvt);
		return lEvt;
	}	
	*/
	void EventManager::releaseEvt(Event* inEvt) {
		
		 // remove from pool
		 list<Event*>::iterator _itr;
		 for (_itr=mEventPool.begin(); _itr != mEventPool.end(); ++_itr)
			 if ((*_itr) == inEvt) {
				 mEventPool.erase(_itr);
				 break;
			 }
		
		deque<Event*>::iterator _q_itr;
		for (_q_itr=mEvents.begin(); _q_itr != mEvents.end(); ++_q_itr)
			if ((*_q_itr) == inEvt) {
				mEvents.erase(_q_itr);
				break;
			}
		
		
		 // destroy!!
		//std::cout << "| EvtMgr: destroying\n";
		delete inEvt;
		 
	}
	
	bool EventManager::alreadySubscribed(EventListener* inListener, vector<EventListener*>* inList) {
		vector<EventListener*>::const_iterator _finder = inList->begin();
		for (_finder; _finder != inList->end(); ++_finder)
			if (*_finder == inListener)
				return true; // oh here it is
		
		// not found
		return false;
	}
	
	
	void EventManager::unsubscribeFromName(std::string inName, EventListener* inListener) {
		//mLog->debugStream() << "detaching listener from name subscriptions";
		detachListener(inListener, &(mNameSubscribers.find(inName)->second));

	}
	
	void EventManager::unsubscribeFromCat(std::string inCategory, EventListener* inListener) {
		//mLog->debugStream() << "detaching listener from category subscriptions";
		detachListener(inListener, &(mCatSubscribers.find(inCategory)->second));

	}

	void EventManager::unsubscribe(EventListener* inListener) {
		//mLog->debugStream() << "detaching listener from full subscriptions";
		detachListener(inListener, &mFullSubscribers);
			

	}
	
	void EventManager::detachListener(EventListener* inListener, subscription_t* inContainer) {
		subscription_t::iterator lSubscriber = inContainer->begin();
		for (lSubscriber;
			 lSubscriber != inContainer->end();
			 ++lSubscriber)
			if (*lSubscriber == inListener) {
				inContainer->erase(lSubscriber);
				break;
			}
	}
}