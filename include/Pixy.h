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

#ifndef H_Pixy_H
#define H_Pixy_H

#ifdef OGRE_USE_BOOST
#undef OGRE_USE_BOOST
#endif

// misc
#include <exception>
#include <stdint.h>
#include <OgrePlatform.h>

// VERTIGO
#include "PixyLog.h"

#define VERTIGO_VERSION "0.1"

/* -------------
 * LOG CONTEXTS
 * ------------- */
#define CLIENT_LOG_CATEGORY "Vertigo"

/* -------------
 * PATHS
 * ------------- */
#define PROJECT_ROOT ".."

#if OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS || defined(_WIN32)
#define PROJECT_LOG_DIR "..\\log"
#define PROJECT_RESOURCES "\\resources"
#define PROJECT_SCRIPTS "\\resources\\scripts"
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#define PROJECT_RESOURCES "/Resources"
#define PROJECT_LOG_DIR "/Resources/log"
#define PROJECT_SCRIPTS "/Resources/scripts"
#else
#define PROJECT_LOG_DIR "../log"
#define PROJECT_RESOURCES "/resources"
#define PROJECT_SCRIPTS "/resources/scripts"
#endif

#ifndef PATH_MAX
#define PATH_MAX 200
#endif

namespace Pixy
{
	typedef unsigned short ushort;

	enum GAME_STATE {
		STATE_INTRO,
		STATE_GAME,
		STATE_PAUSE,
		STATE_SCORE,
	};
	

} // end of namespace Pixy
#endif
