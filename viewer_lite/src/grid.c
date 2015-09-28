/**
 * @file grid.c
 * @brief Grid Rendering Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gl_headers.h"
#include "common.h"
#include "drawable_type.h"
#include "render.h"
#include "grid.h"




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
void grid_draw_cartesian( const gui_context_s * const gui, const double range, const double step )
{
    // local vars
    double val = 0.0;


    // set color
    glColor4d( 0.4, 0.4, 0.4, 0.5 );

    // line width
    glLineWidth( 1.1f );

    // start lines
    glBegin( GL_LINES );

    // for each vertical line
    for( val = -range; val <= range; val += step )
    {
        // draw line at y
        glVertex2d( range, val );
        glVertex2d( -range, val );
    }

    // for each horizontal line
    for( val = -range; val <= range; val += step )
    {
        // draw line at x
        glVertex2d( val, range );
        glVertex2d( val, -range );
    }

    // end lines
    glEnd();
}


//
void grid_draw_radial( const gui_context_s * const gui, const double range, const double step )
{
    // local vars
    double val = 0.0;


    // set color
    glColor4d( 0.4, 0.4, 0.4, 0.5 );

    // line width
    glLineWidth( 1.1f );

    // for each radial
    for( val = step; val <= range; val += step )
    {
        // render circle
        render_circle_2d( 0.0, 0.0, val );
    }
}
