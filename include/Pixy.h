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

// misc
#include <exception>

// VERTIGO
#include "PixyLog.h"

#define VERTIGO_VERSION "0.1"

/* -------------
 * LOG CONTEXTS
 * ------------- */
#define CLIENT_LOG_CATEGORY "Elementum"

/* -------------
 * PATHS
 * ------------- */
#define PROJECT_ROOT "/Users/kandie/Workspace/Projects/Vertigo"
#define PROJECT_LOG_DIR "/Users/kandie/Workspace/Projects/Vertigo/log"
#define PROJECT_RESOURCES "/resources"
#define PROJECT_SCRIPTS "/resources/scripts"

#ifndef PATH_MAX
#define PATH_MAX 200
#endif

namespace Pixy
{
	typedef unsigned short ushort;

	enum GAME_STATE {
		STATE_INTRO,
		STATE_LOBBY,
		STATE_COMBAT
	};
} // end of namespace Pixy
#endif
