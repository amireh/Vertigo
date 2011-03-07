/*
 *  AudioEngine.h
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/15/10.
 *  Copyright 2010 JU. All rights reserved.
 *
 */


#ifndef H_AudioEngine_H
#define H_AudioEngine_H

#include "Pixy.h"
#include "Engine.h"
#include "EventListener.h"

namespace Pixy
{
	
	/*! \class AudioEngine
	 *	\brief
	 *	Provides audio support, loads sound resources, and handles all sound
	 *	related events.
	 */
    class AudioEngine : public Engine, public EventListener
	{
	};
};



#endif