/**
 * @file gui.c
 * @brief GUI Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <glib-2.0/glib.h>

// API headers
#include "polysync_core.h"

// GUI headers
#include "gl_headers.h"
#include "gui.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief Convert radians to degrees. [double]
 *
 * angle = 180 / pi
 *
 */
#define         RAD_2_DEG(angle)        ((angle)*(180.0/M_PI))


/**
 * @brief Convert degrees to radians. [double]
 *
 * angle = pi / 180
 *
 */
#define         DEG_2_RAD(angle)        ((angle)*(M_PI/180.0))


/**
 * @brief Convert seconds to microseconds. [unsigned long long]
 *
 */
#define         SEC_2_MICRO(time)       (time * 1000000ULL)


/**
 * @brief Convert microseconds to seconds. [double]
 *
 */
#define         MICRO_2_SEC(time)       (time / 1000.0 / 1000.0)





// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Global GUI context reference used by graphics library handlers.
 *
 */
static gui_context_s        *global_gui_context     = NULL;




// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief GUI window close handler.
 *
 * Raises SIGINT signal.
 *
 */
static void on_close( void );


/**
 * @brief GUI key handler.
 *
 * @param [in] key Key value to handle.
 * @param [in] x X coordinate of key.
 * @param [in] y Y coordinate of key.
 *
 */
static void on_key( unsigned char key, const int x, int y );


/**
 * @brief GUI special key handler.
 *
 * @param [in] key Key value to handle.
 * @param [in] x X coordinate of key.
 * @param [in] y Y coordinate of key.
 *
 */
static void on_special_key( int key, int x, int y );


/**
 * @brief GUI mouse button handler.
 *
 * @param [in] button Mouse button to handle.
 * @param [in] state Button state.
 * @param [in] x X coordinate of mouse. [pixels]
 * @param [in] y Y coordinate of mouse. [pixels]
 *
 */
static void on_mouse( int button, int state, int x, int y );


/**
 * @brief GUI mouse motion handler.
 *
 * @param [in] x X coordinate of mouse. [pixels]
 * @param [in] y Y coordinate of mouse. [pixels]
 *
 */
static void on_mouse_motion( int x, int y );


/**
 * @brief GUI window resize handler.
 *
 * @param [in] w Window width. [pixels]
 * @param [in] h Window height. [pixels]
 *
 */
static void on_resize( int w, int h );


/**
 * @brief GUI drawing handler.
 *
 */
static void on_draw( void );




// *****************************************************
// static definitions
// *****************************************************

//
static void render_text_2d( const GLdouble cx, const GLdouble cy, const char * const text, const void * const font )
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


//
static void on_close( void )
{
    // ignore if no reference
    if( global_gui_context == NULL )
    {
        return;
    }

    // signal exit
    raise( SIGINT );

    // wait a little to handle the signal
    usleep( 10000 );

    // de-ref
    global_gui_context = NULL;
}


//
static void on_key( unsigned char key, const int x, int y )
{
    // ignore if no reference
    if( global_gui_context == NULL )
    {
        return;
    }


    // check key
    if( (key == '\e') || (key == GUI_KEY_EXIT) )
    {
        // close
        on_close();
        return;
    }
    else if( key == GUI_KEY_HELP )
    {
        // toggle help
        global_gui_context->config.help_visible = !global_gui_context->config.help_visible;

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_FREE_FRAME )
    {
        // toggle freeze-frame
        global_gui_context->config.freeze_frame = !global_gui_context->config.freeze_frame;

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_RESET_VIEW )
    {
        global_gui_context->config.zoom_scale = 1.0;
        global_gui_context->config.rotation = 0.0;

        // camera position
        global_gui_context->config.view_pos[ 0 ] = 0.0;
        global_gui_context->config.view_pos[ 1 ] = 0.0;
        global_gui_context->config.view_pos[ 2 ] = 0.0;

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_ROTATE_VIEW )
    {
        // add 90 degrees
        global_gui_context->config.rotation += 90.0;

        // constrain
        if( global_gui_context->config.rotation >= 360.0 )
        {
            global_gui_context->config.rotation = 0.0;
        }

        // redraw
        glutPostRedisplay();
    }
}


//
static void on_special_key( int key, int x, int y )
{
    // ignore if no reference
    if( global_gui_context == NULL )
    {
        return;
    }


    // do nothing
}


//
static void on_mouse( int button, int state, int x, int y )
{
    // ignore if no reference
    if( global_gui_context == NULL )
    {
        return;
    }


    // if pressed
    if( state == 0 )
    {
        // scroll up = 3, scroll down = 4
        if( button == 3 )
        {
            // zoom in
            global_gui_context->config.zoom_scale += 0.3;

            // redraw
            glutPostRedisplay();
        }
        else if( button == 4 )
        {
            // zoom out
            global_gui_context->config.zoom_scale -= 0.3;

            // cap
            if( global_gui_context->config.zoom_scale < 1.0 )
            {
                global_gui_context->config.zoom_scale = 1.0;
            }

            // redraw
            glutPostRedisplay();
        }
    }
}


//
static void on_mouse_motion( int x, int y )
{
    // ignore if no reference
    if( global_gui_context == NULL )
    {
        return;
    }


    // do nothing
}


//
static void on_resize( int w, int h )
{
    // ignore if no reference
    if( global_gui_context == NULL )
    {
        return;
    }

    // local vars
    const double dw = (double) w;
    const double dh = (double) h;


    // set viewport
    glViewport( 0, 0, w, h );

    // set projection
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // 2D ortho
    gluOrtho2D( -dw/2.0, dw/2.0, -dh/2.0, dh/2.0 );

    // set modelview
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // update
    global_gui_context->win_width = (unsigned int) w;
    global_gui_context->win_height = (unsigned int) h;


    // signal redraw
    glutPostRedisplay();
}


//
static void on_draw( void )
{
    // ignore if no reference
    if( global_gui_context == NULL )
    {
        // nothing to do
        glutSwapBuffers();
        return;
    }

    // local vars
    const GLdouble width = (GLdouble) global_gui_context->win_width;
    const GLdouble height = (GLdouble) global_gui_context->win_height;
    const GLdouble text_delta = 20.0;
    GLdouble text_y = 0.0;
    char string[PSYNC_DEFAULT_STRING_LEN];


    // clear buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // reset background
    glClearColor( 0.0, 0.0, 0.0, 1.0 );

    // reset color
    glColor4d( 1.0, 1.0, 1.0, 1.0 );

    // set default line width
    glLineWidth( (GLfloat) 1.0f );

    // set default point size
    glPointSize( 1.0f );

    // enable blending
    glEnable( GL_BLEND );

    //
    // setup coordinate system
    //

    // get into coordinate system
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // 2D ortho
    gluOrtho2D( -width/2.0, width/2.0, -height/2.0, height/2.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // save state
    glPushMatrix();

    // apply zoom scale
    glScaled( global_gui_context->config.zoom_scale, global_gui_context->config.zoom_scale, 1.0 );

    // apply rotation
    glRotated( global_gui_context->config.rotation, 0.0, 0.0, 1.0 );

    // save state
    glPushMatrix();

    //
    // rendering, order defines the depth test
    //

    // disable blending
    glDisable( GL_BLEND );

    // wait for first frame before drawing anything
    if( global_gui_context->frame_cnt != 0 )
    {
        // enable 2D texturing
        glEnable( GL_TEXTURE_2D );

        // bind to image data texture
        glBindTexture( GL_TEXTURE_2D, global_gui_context->image_texture );

        // draw image data textured quad
        glBegin( GL_QUADS );
        glTexCoord2d( 0.0, 1.0 );
        glVertex2d( -width/2.0, height/2.0 );
        glTexCoord2d( 0.0, 0.0 );
        glVertex2d( -width/2.0, -height/2.0 );
        glTexCoord2d( 1.0, 0.0 );
        glVertex2d( width/2.0, -height/2.0 );
        glTexCoord2d( 1.0, 1.0 );
        glVertex2d( width/2.0, height/2.0 );
        glEnd();

        // unbind
        glBindTexture( GL_TEXTURE_2D, 0 );
    }

    // restore state
    glPopMatrix();

    // restore state
    glPopMatrix();

    //
    // draw screen text
    //

    // disable blending
    glDisable( GL_BLEND );

    // text color
    glColor4d( 1.0, 1.0, 1.0, 1.0 );

    // set screen coordinate system
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0, width, 0.0, height );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // draw help text
    if( global_gui_context->config.help_visible != 0 )
    {
        text_y = height - text_delta;

        // key maps
        render_text_2d( 5.0, text_y, "key map", NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s", GUI_KEY_EXIT, "exit application" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s", GUI_KEY_HELP, "show help" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s", GUI_KEY_RESET_VIEW, "reset view" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_FREE_FRAME, "freeze frame", global_gui_context->config.freeze_frame ? "ON" : "OFF" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s - %.0f", GUI_KEY_ROTATE_VIEW, "rotation (+90 deg)", global_gui_context->config.rotation );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        text_y -= text_delta;

        snprintf( string, sizeof(string), "publisher GUID: 0x%016llX", global_gui_context->image_publisher_guid );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "image width: %lu - height: %lu", global_gui_context->image_width, global_gui_context->image_height );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "frame count: %lu", global_gui_context->frame_cnt );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "received FPS: %.0f", global_gui_context->rx_fps );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "rendered FPS: %.0f", global_gui_context->rendered_fps );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
    }

    // swap buffers
    glutSwapBuffers();
}




// *****************************************************
// public definitions
// *****************************************************

//
gui_context_s *gui_init( const char *win_title, const unsigned int win_width, const unsigned int win_height )
{
    if( (win_title == NULL) || (win_width < 1) || (win_height < 1) )
    {
        return NULL;
    }

    // local vars
    gui_context_s *gui = NULL;


    // create
    if( (gui = malloc( sizeof(*gui) )) == NULL)
    {
        return NULL;
    }

    // zero
    memset( gui, 0, sizeof(*gui) );

    // default configurations
    gui->config.freeze_frame = 0;
    gui->config.help_visible = 1;
    gui->config.zoom_scale = 1.0;
    gui->config.rotation = 0.0;

    // camera position
    gui->config.view_pos[ 0 ] = 0.0;
    gui->config.view_pos[ 1 ] = 0.0;
    gui->config.view_pos[ 2 ] = 0.0;

    // set window data
    strncpy( gui->win_title, win_title, sizeof(gui->win_title) );
    gui->win_width = win_width;
    gui->win_height = win_height;

    // set image data w/h
    gui->image_width = (unsigned long) win_width;
    gui->image_height = (unsigned long) win_height;

    // FPS max
    gui->max_fps = GUI_DEFAULT_MAX_FPS;

    // init GL
    glutInit( &gui->gl_argc, gui->gl_argv );

    // display mode, double buffers, RGB
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );

    // win size
    glutInitWindowSize( gui->win_width, gui->win_height );

    // create display window
    if( (gui->win_id = glutCreateWindow( gui->win_title )) < 0 )
    {
        free( gui );
        return NULL;
    }

    // set callbacks
    glutCloseFunc( on_close );
    glutKeyboardFunc( on_key );
    glutSpecialFunc( on_special_key );
    glutMouseFunc( on_mouse );
    glutMotionFunc( on_mouse_motion );
    glutReshapeFunc( on_resize );
    glutDisplayFunc( on_draw );

    // set config flags
    glDisable( GL_DEPTH );
    glDisable( GL_LIGHTING );
    glShadeModel( GL_SMOOTH );
    glDisable( GL_DEPTH_TEST );

    // smoothness
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POLYGON_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    // alpha blending config
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // configure polygon mode
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // main loop returns on window exit
    glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );

    // clear the color buffer, background, to black, RGBA
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // enable texture 2D
    glEnable( GL_TEXTURE_2D );

    // create image data texture
    glGenTextures( 1, &gui->image_texture );

    // bind the image data texture
    glBindTexture( GL_TEXTURE_2D, gui->image_texture );

    // create texture
    glTexImage2D(
            GL_TEXTURE_2D,      // target
            0,                  // level of detail
            GL_RGB,             // internal image format
            win_width,          // width
            win_height,         // height
            0,                  // border
            GL_RGB,             // format
            GL_UNSIGNED_BYTE,   // type
            NULL   // data
    );

    // configure the texture
    glTexEnvf( GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL );
    glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

    // unbind
    glBindTexture( GL_TEXTURE_2D, 0 );

    // set global reference
    global_gui_context = gui;

    // signal redraw
    glutPostRedisplay();

    // process glut events
    glutMainLoopEvent();

    // return new memory
    return gui;
}


//
void gui_release( gui_context_s * const gui )
{
    if( gui == NULL )
    {
        return;
    }


    // update global reference
    global_gui_context = gui;

    // destroy texture
    glDeleteTextures( 1, &gui->image_texture );

    // signal GL exit
    glutExit();

    // de-ref
    global_gui_context = NULL;
}


//
void gui_update_image_data( gui_context_s * const gui, const unsigned char * buffer, const unsigned long buffer_len )
{
    if( (gui == NULL) || (buffer == NULL) || (buffer_len == 0) )
    {
        return;
    }


    // update global reference
    global_gui_context = gui;

    // bind the image data texture
    glBindTexture( GL_TEXTURE_2D, gui->image_texture );

    // update texture data
    glTexSubImage2D(
            GL_TEXTURE_2D,      // target
            0,                  // level of detail
            0,                  // x offset
            0,                  // y offset
            gui->image_width,   // width
            gui->image_height,  // height
            GL_RGB,             // format
            GL_UNSIGNED_BYTE,   // type
            buffer              // data
    );

    // unbind
    glBindTexture( GL_TEXTURE_2D, 0 );
}


//
void gui_update( gui_context_s * const gui, const ps_timestamp update_time, ps_timestamp * const time_to_redraw )
{
    if( (gui == NULL) || (time_to_redraw == NULL) )
    {
        return;
    }

    // local vars
    ps_timestamp render_wait = 0;


    // update global reference
    global_gui_context = gui;

    // get wait interval
    render_wait = (ps_timestamp) SEC_2_MICRO( (1.0 / (double) gui->max_fps) );

    // check if force-redraw set
    if( (*time_to_redraw) == GUI_FORCE_REDRAW )
    {
        // force redraw

        // set time to next render
        (*time_to_redraw) = render_wait;

        // update rendered fps
        gui->rendered_fps = 1.0 / MICRO_2_SEC( (double) (update_time - gui->last_render_time) );

        // update timestamp
        gui->last_render_time = update_time;

        // signal redraw
        glutPostRedisplay();

        // process glut events
        glutMainLoopEvent();
    }
    else
    {
        // check frame rate
        if( (update_time - gui->last_render_time) > render_wait )
        {
            // interval met, do redraw

            // set time to next render
            (*time_to_redraw) = render_wait;

            // update rendered fps
            gui->rendered_fps = 1.0 / MICRO_2_SEC( (double) (update_time - gui->last_render_time) );

            // update timestamp
            gui->last_render_time = update_time;

            // signal redraw
            glutPostRedisplay();

            // process glut events
            glutMainLoopEvent();
        }
        else
        {
            // interval not met

            // update time to render
            (*time_to_redraw) = render_wait - (update_time - gui->last_render_time);
        }
    }
}
