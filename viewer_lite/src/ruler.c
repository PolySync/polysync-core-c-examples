/**
 * @file ruler.h
 * @brief Ruler Rendering Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gl_headers.h"
#include "common.h"
#include "drawable_type.h"
#include "render.h"
#include "ruler.h"




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
void ruler_draw( const gui_context_s * const gui, const ruler_data_s * const ruler )
{
    // set vector line color
    glColor4dv( ruler->color_rgba );

    // line width
    glLineWidth( (GLfloat) GUI_DEFAULT_LINE_WIDTH );


    // check if p1 set
    if( ruler->p1_set != 0 )
    {
        // save state
        glPushMatrix();

        // translate to center, since it scales
        glTranslated( ruler->x1, ruler->y1, 0.0 );

        // draw x
        render_cross_2d( 0.0, 0.0, 2.0, 2.0 );

        // restore state
        glPopMatrix();
    }

    // check if p2 set
    if( ruler->p2_set != 0 )
    {
        // save state
        glPushMatrix();

        // translate to center, since it scales
        glTranslated( ruler->x2, ruler->y2, 0.0 );

        // draw x
        render_cross_2d( 0.0, 0.0, 2.0, 2.0 );

        // restore state
        glPopMatrix();
    }

    // check if both points are set
    if( (ruler->p1_set != 0) && (ruler->p2_set != 0) )
    {
        // start lines
        glBegin( GL_LINES );

        // line between points
        glVertex2d( ruler->x1, ruler->y1 );
        glVertex2d( ruler->x2, ruler->y2 );

        // end lines
        glEnd();
    }
}
