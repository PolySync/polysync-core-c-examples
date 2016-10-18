/**
 * @file render.c
 * @brief Primitive Rendering Interface Source.
 *
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "gl_headers.h"
#include "common.h"
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




// *****************************************************
// public definitions
// *****************************************************


void init_render_texture_SDL( const char * file_name, const int * const width, const int * const height, GLuint * const image )
{
    glGenTextures( 1, image );
    
    load_texture( ( *image ),  file_name );
}


int load_texture( const GLuint textbind, const char *filename)
{
    SDL_Surface *s = IMG_Load( filename );

    if (!s) return 0;

    glBindTexture( GL_TEXTURE_2D, textbind );
    
    glTexImage2D( GL_TEXTURE_2D,
        0, 3,
        s->w, s->h,
        0, GL_BGRA,
        GL_UNSIGNED_BYTE, s->pixels );

    /* Auto-produce mip-mapping */
    gluBuild2DMipmaps(
            GL_TEXTURE_2D,
            GL_RGBA,
            s->w, s->h,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            s->pixels
    );

    glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR
    );

    glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR
    );

    return 1;
}


void render_cube_with_texture( const GLuint texture )
{    
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_BLEND);
    
    glEnable( GL_TEXTURE_2D );
    
    glBindTexture( GL_TEXTURE_2D, texture );
    
    glBegin(GL_QUADS);
    
    glTexCoord2f(0, 0); glVertex2f( -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH );
    glTexCoord2f(0, 1); glVertex2f( HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH );
    glTexCoord2f(1, 1); glVertex2f( HALF_CUBE_WIDTH, HALF_CUBE_WIDTH );
    glTexCoord2f(1, 0); glVertex2f( -HALF_CUBE_WIDTH, HALF_CUBE_WIDTH );

    glEnd();

    glDisable(GL_TEXTURE_2D);

    glBindTexture( GL_TEXTURE_2D, 0 );
}
