/**
 * @file ground_plane.c
 * @brief Ground Plane Rendering Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gl_headers.h"
#include "common.h"
#include "drawable_type.h"
#include "ground_plane.h"




// *****************************************************
// static global structures
// *****************************************************




// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************




// *****************************************************
// static definitions
// *****************************************************




// *****************************************************
// public definitions
// *****************************************************

//
void ground_plane_draw( const gui_context_s * const gui, const double r, const double g, const double b )
{
    // set color
    glClearColor( r, g, b, 1.0 );
}
