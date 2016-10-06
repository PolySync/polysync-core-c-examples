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
#define	RENDER_H




#include "gl_headers.h"


/**
 * @brief Half size of cube to render texture on. [int]
 *
 */
#define HALF_CUBE_WIDTH (35)


/**
 * @brief Setup the infrastructure to render texture using SDL and openGL.
 *
 * @param [in] file_name File name of texture to be rendered.
 * @param [in] width Width of image.
 * @param [in] height Height of image.
 * @param [out] image Reference number of texture to be rendered.
 *
 */
void init_render_texture_SDL( const char * file_name, const int * const width, const int * const height, GLuint * const image );

/**
 * @brief Load texture from file.
 *
 * @param [in] textbind Reference number of texture to be rendered.
 * @param [in] filename Name of file which contains image to render to texture.
 * 
 * @return Error on failure, success on not failure.
 *
 */
int load_texture( const GLuint textbind, const char *filename);

/**
 * @brief Render a cube with the texture on top.
 *
 * @param [in] texture Texture to be rendered on cube.
 *
 */
void render_cube_with_texture( const GLuint texture );



#endif	/* RENDER_H */
