/**
 * @file render.c
 * @brief Primitive Rendering Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "gl_headers.h"
#include "common.h"
#include "drawable_type.h"
#include "render.h"




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

//
GLuint render_get_circle_segments( const GLdouble r )
{
    return (GLuint) (40.0 * sqrt( r ));
}




// *****************************************************
// public definitions
// *****************************************************

//
void render_circle_2d( const GLdouble cx, const GLdouble cy, const GLdouble radius )
{
    // local vars
    GLuint idx = 0;
    double t = 0.0;

    // start at angle = 0
	double x = radius;
	double y = 0.0;


    // get segments
    const GLuint segments = render_get_circle_segments( radius );

    // get theta
    const double theta = (2.0 * M_PI) / (double) segments;

    // precalculate sine and cosine
	const double c = cos( theta );
    const double s = sin( theta );

    // repeat rotation with line loop
	glBegin(GL_LINE_LOOP);
	for( idx = 0; idx < segments; idx += 1 )
	{
        // output vertex
		glVertex2d( x + cx, y + cy );

		// apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
}


//
void render_cross_2d( const GLdouble cx, const GLdouble cy, const GLdouble length, const GLdouble width )
{
    // save state
    glPushMatrix();

    // scale x/y circle
    glScaled( length, width, 1.0 );

    // begin lines
    glBegin( GL_LINES );

    // vertical
    glVertex2d( cx - 0.5, cy );
    glVertex2d( cx + 0.5, cy );

    // horizontal
    glVertex2d( cx, cy - 0.5 );
    glVertex2d( cx, cy + 0.5 );

    // end lines
    glEnd();

    // restore state
    glPopMatrix();
}


//
void render_rectangle_2d( const GLdouble cx, const GLdouble cy, const GLdouble length, const GLdouble width )
{
    // begin quad
    glBegin( GL_QUADS );

    // v0, v1, v2, v3
    glVertex2d( cx - length/2.0, cy + width/2.0 );
    glVertex2d( cx - length/2.0, cy - width/2.0 );
    glVertex2d( cx + length/2.0, cy - width/2.0 );
    glVertex2d( cx + length/2.0, cy + width/2.0 );

    // end quad
    glEnd();
}


//
void render_triangle_2d( const GLdouble cx, const GLdouble cy, const GLdouble length, const GLdouble width )
{
    // begin triangle
    glBegin( GL_TRIANGLES );

    // v0, v1, v2,
    glVertex2d( cx - length/2.0, cy + width/2.0 );
    glVertex2d( cx - length/2.0, cy - width/2.0 );
    glVertex2d( cx + length/2.0, cy );

    // end triangle
    glEnd();
}


//
void render_ellipse_2d( const GLdouble cx, const GLdouble cy, const GLdouble major, const GLdouble minor )
{
    // save state
    glPushMatrix();

    // scale x/y circle
    glScaled( major, minor, 1.0 );

    // render circle
    render_circle_2d( cx, cy, 0.5 );

    // restore state
    glPopMatrix();
}


//
void render_text_2d( const GLdouble cx, const GLdouble cy, const char * const text, const void * const font )
{
    // local vars
    const char  *ptr    = text;
    void        *m_font = GLUT_BITMAP_HELVETICA_12;

    // set font
    if( font != NULL )
    {
        m_font = (void*) font;
    }

    // save state
    glPushMatrix();

    // start at
    glRasterPos2d( cx, cy );

    // render each char
    while( *ptr != '\0' )
    {
        glutBitmapCharacter( m_font, *ptr );
        ptr += 1;
    }

    // restore state
    glPopMatrix();
}
