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

AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
{
    FILE *File=NULL;									// File Handle

    if (!Filename)										// Make Sure A Filename Was Given
    {
            return NULL;									// If Not Return NULL
    }

    File=fopen(Filename,"r");							// Check To See If The File Exists

    if (File)											// Does The File Exist?
    {
            fclose(File);									// Close The Handle
            return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
    }

    return NULL;										// If Load Failed Return NULL
}

int LoadGLTextures( char *Filename, GLuint * image )									// Load Bitmaps And Convert To Textures
{
    int Status=FALSE;									// Status Indicator

    AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

    memset( TextureImage,0,sizeof(void *)*1 );           	// Set The Pointer To NULL

    // Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
    if ( ( *image ) = LoadBMP( Filename ) )
    {
            Status=TRUE;									// Set The Status To TRUE

            glGenTextures( 1, image );					// Create The Texture

            // Typical Texture Generation Using Data From The Bitmap
            glBindTexture(GL_TEXTURE_2D, ( *image )); 
            glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    }

    if (TextureImage[0])									// If Texture Exists
    {
            if (TextureImage[0]->data)							// If Texture Image Exists
            {
                    free(TextureImage[0]->data);					// Free The Texture Image Memory
            }

            free(TextureImage[0]);								// Free The Image Structure
    }

    return Status;										// Return The Status
}


void init_render_texture( char * file_name, int * width, int * height, GLuint * image )
{
    int wid, hei;
    load_png_texture( file_name, &wid, &hei, image );
}


void load_png_texture( char * file_name, int * width, int * height, GLuint * image )
{
    // This function was originally written by David Grayson for
    // https://github.com/DavidEGrayson/ahrs-visualizer

    png_byte header[8];

    FILE *fp = fopen( file_name, "rb" );
    
    printf("%d\n", __LINE__ );
    
    if ( fp == 0 )
    {
        perror(file_name);
        return 0;
    }
    
    printf("%d\n", __LINE__ );

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8))
    {
        printf("error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }
    
    printf("%d\n", __LINE__ );

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        printf("error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }
    
    printf("%d\n", __LINE__ );

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        printf("error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }
    
    printf("%d\n", __LINE__ );

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        printf("error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }
    
    printf("%d\n", __LINE__ );

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }
    
    printf("%d\n", __LINE__ );

    // init png reading
    png_init_io(png_ptr, fp);
    
    printf("%d\n", __LINE__ );

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);
    
    printf("%d\n", __LINE__ );

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);
    
    printf("%d\n", __LINE__ );

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);
    
    printf("%d\n", __LINE__ );

    if (width){ *width = temp_width; }
    if (height){ *height = temp_height; }
    
    printf("%d\n", __LINE__ );

    //printf("%s: %lux%lu %d\n", file_name, temp_width, temp_height, color_type);

    if (bit_depth != 8)
    {
        printf("%s: Unsupported bit depth %d.  Must be 8.\n", file_name, bit_depth);
        return 0;
    }

    GLint format;
    switch(color_type)
    {
    case PNG_COLOR_TYPE_RGB:
        format = GL_RGB;
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        format = GL_RGBA;
        break;
    default:
        printf("%s: Unknown libpng color type %d.\n", file_name, color_type);
        return 0;
    }
    
    printf("%d\n", __LINE__ );

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);
    
    printf("%d\n", __LINE__ );

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    
    printf("%d\n", __LINE__ );

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte * image_data = (png_byte *)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL)
    {
        printf("error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }
    
    printf("%d\n", __LINE__ );

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_byte ** row_pointers = (png_byte **)malloc(temp_height * sizeof(png_byte *));
    if (row_pointers == NULL)
    {
        printf("error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }
    
    printf("%d\n", __LINE__ );

    // set the individual row_pointers to point at the correct offsets of image_data
    for (unsigned int i = 0; i < temp_height; i++)
    {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }
    
    printf("%d\n", __LINE__ );

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // Generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, temp_width, temp_height, 0, format, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    printf("%d\n", __LINE__ );

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    
    printf("%d\n", __LINE__ );
    
    (*image) = texture;
    
    printf("%d\n", __LINE__ );
}


//glTexImage2D

void render_cube_with_texture( GLuint texture )
{
    glPushMatrix();
    
    glBegin(GL_QUADS);

    glEnable( GL_TEXTURE_2D );
    
    if( glIsTexture( texture ) == GL_TRUE )
    {
        printf( "this is a texture\n" );
    }
    else
    {
        printf( "this is not a texture\n" );
    }

    glBindTexture( GL_TEXTURE_2D, texture );

    double cubeSize = 10.0f;

    glTexCoord2f(0.0f, 0.0f); glVertex2d(-cubeSize, -cubeSize);
    glTexCoord2f(cubeSize, 0.0f); glVertex2d( cubeSize, -cubeSize);
    glTexCoord2f(cubeSize, cubeSize); glVertex2d( cubeSize,  cubeSize);
    glTexCoord2f(0.0f, cubeSize); glVertex2d(-cubeSize,  cubeSize);
    // Front Face
//        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
//        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
//        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
//        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
//        // Back Face
//        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
//        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
//        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
//        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
//        // Top Face
//        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
//        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
//        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
//        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
//        // Bottom Face
//        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
//        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
//        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
//        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
//        // Right face
//        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
//        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
//        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
//        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
//        // Left Face
//        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
//        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
//        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
//        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);

    glEnd();

    glBindTexture( GL_TEXTURE_2D, 0 );
    
    glPopMatrix();
}

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
