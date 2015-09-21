/**
 * @file origin_model.c
 * @brief Original Model Rendering Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gl_headers.h"
#include "common.h"
#include "drawable_type.h"
#include "render.h"
#include "origin_model.h"




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
void origin_model_draw( const gui_context_s * const gui, const platform_data_s * const platform )
{
    // save state
    glPushMatrix();

    // go to center
    glTranslated( platform->x, platform->y, 0.0 );

    // start lines
    glBegin( GL_LINES );

    // if vectors enabled
    if( gui->config.platform_vectors_visible != 0 )
    {
        // set vector line color
        glColor4dv( platform->color_rgba );

        // line width
        glLineWidth( (GLfloat) GUI_DEFAULT_LINE_WIDTH );

        // velocity vector
        glVertex2d( 0.0, 0.0 );
        glVertex2d( platform->adjusted_vx, platform->adjusted_vy );
    }

    // set origin line color
    glColor4d( 0.5, 0.5, 0.5, 0.8 );

    // line width
    glLineWidth( 0.5f );

    // draw vertical origin line
    glVertex2d( -platform->length/2.0, 0.0 );
    glVertex2d( platform->length/2.0, 0.0 );

    // draw horizontal origin line
    glVertex2d( 0.0, -platform->width/2.0 );
    glVertex2d( 0.0, platform->width/2.0 );

    // end lines
    glEnd();

    // restore state
    glPopMatrix();
}
