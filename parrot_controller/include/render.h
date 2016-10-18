/*
 * Copyright (c) 2016 PolySync
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file render.h
 * @brief Primitive Rendering Interface.
 *
 */




#ifndef RENDER_H
#define RENDER_H




#include "gl_headers.h"




void render_circle_2d( const GLdouble cx, const GLdouble cy, const GLdouble radius );


void render_line_between_points( const GLdouble cx, const GLdouble cy, const GLdouble bx, const GLdouble by );


void render_cross_2d( const GLdouble cx, const GLdouble cy, const GLdouble length, const GLdouble width );


void render_rectangle_2d( const GLdouble cx, const GLdouble cy, const GLdouble length, const GLdouble width );


void render_triangle_2d( const GLdouble cx, const GLdouble cy, const GLdouble length, const GLdouble width );


void render_ellipse_2d( const GLdouble cx, const GLdouble cy, const GLdouble major, const GLdouble minor );


void render_text_2d( const GLdouble cx, const GLdouble cy, const char * const text, const void * const font );




#endif	/* RENDER_H */
