/******************************************************
** FILE: vsv_common.h
**
** ABSTRACT:
**
** DOCUMENTS:
**
** AUTHOR:
** JL
**
** NOTES:
** 
*******************************************************/



/**
 * @example vsv_common.h
 * 
 * Example Project Resources.
 * 
 */




#ifndef VSV_COMMON_H
#define	VSV_COMMON_H


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include <stdarg.h>
#include <getopt.h>
#include <string.h>
#include <glib-2.0/glib.h>

// Image data resources
#include <SDL/SDL.h>
#include <SDL/SDL_video.h>
#include <SDL/SDL_rotozoom.h>

// API headers
#include "polysync_core.h"

// data structures
#include "vsv_structures.h"


/**
 * @brief Default window title.
 * 
 */
#define		DEF_WIN_TITLE           "Video Stream Viewer"






#endif	/* VSV_COMMON_H */

