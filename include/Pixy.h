/* -----------------------------------------------
 *  Filename: Pixy.h
 *  Date Created: 7/3/2011
 *
 *  Original Author:
 *      Ahmad Amireh ( ahmad.amireh@gmail.com )
 *
 *  Last Update:
 *      Date:   7/3/2011
 *      By:     Ahmad Amireh
 * ----------------------------------------------- */

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
#define PROJECT_LOG_DIR "../Resources/log"
#define PROJECT_RESOURCES "../Resources"
#define PROJECT_SCRIPTS "../Resources/scripts"
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
