/**
 * @file render.h
 * @brief Primitive Rendering Interface.
 *
 */




#ifndef RENDER_H
#define	RENDER_H




#include "gl_headers.h"
#include "drawable_type.h"







void render_circle_2d( const GLdouble cx, const GLdouble cy, const GLdouble radius );


void render_cross_2d( const GLdouble cx, const GLdouble cy, const GLdouble length, const GLdouble width );


void render_rectangle_2d( const GLdouble cx, const GLdouble cy, const GLdouble length, const GLdouble width );


void render_triangle_2d( const GLdouble cx, const GLdouble cy, const GLdouble length, const GLdouble width );


void render_ellipse_2d( const GLdouble cx, const GLdouble cy, const GLdouble major, const GLdouble minor );


void render_text_2d( const GLdouble cx, const GLdouble cy, const char * const text, const void * const font );




#endif	/* RENDER_H */
