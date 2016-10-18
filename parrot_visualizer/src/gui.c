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

#include "polysync_core.h"
#include "gl_headers.h"
#include "common.h"
#include "render.h"
#include "gui.h"




// *****************************************************
// static global structures
// *****************************************************




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
    
    // Do nothing
}


//
static void on_special_key( int key, int x, int y )
{
    // ignore if no reference
    if( global_gui_context == NULL )
    {
        return;
    }

    // Do nothing
}


//
static void on_mouse( int button, int state, int x, int y )
{
    // ignore if no reference
    if( global_gui_context == NULL )
    {
        return;
    }
    
    // Do nothing
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

    // clear buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // reset background    
    glClearColor( 0.79, 0.88, 0.93, 0.0);

    // reset color
    glColor4d( 1.0, 1.0, 1.0, 1.0 );

    // set default line width
    glLineWidth( (GLfloat) GUI_DEFAULT_LINE_WIDTH );

    // set default point size
    glPointSize( 1.0f );

    // enable blending
    glEnable( GL_BLEND );

    // configure 2D
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    //
    // setup coordinate system
    //

    // get into view mode's coordinate system
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // 2D ortho
    gluOrtho2D( -width/2.0, width/2.0, -height/2.0, height/2.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // save state
    glPushMatrix();

    // rotate 90 degrees counter clockwise to get into PolySync coordinates
    glRotated( 90.0, 0.0, 0.0, 1.0 );

    // scale
    glScaled( global_gui_context->config.zoom_scale, global_gui_context->config.zoom_scale, 1.0 );

    // translate
    glTranslated( -global_gui_context->config.camera_pos[ 0 ], -global_gui_context->config.camera_pos[ 1 ], 0.0 );

    // save state
    glPushMatrix();

    //
    // rendering, order defines the depth test
    //

    // disable blending
    glDisable( GL_BLEND );
    
    // draw entities
    draw_vehicle_position( &global_gui_context->vehicle_position, global_gui_context->renderImage );
    
    // restore state from zoom/translation
    glPopMatrix();

    // restore state from rotation
    glPopMatrix();

    // disable blending
    glDisable( GL_BLEND );

    // set screen coordinate system
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0, width, 0.0, height );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // swap buffers
    glutSwapBuffers();
}




// *****************************************************
// public definitions
// *****************************************************

//
gui_context_s *gui_init( const char *win_title, const unsigned int win_width, const unsigned int win_height, const double grid_scale )
{
    if( (win_title == NULL) || (win_width < 1) || (win_height < 1) || (grid_scale < 1.0) )
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
    
    gui->config.zoom_scale = 1.9;    

    // set window data
    strncpy( gui->win_title, win_title, sizeof(gui->win_title) );
    gui->win_width = win_width;
    gui->win_height = win_height;
    gui->grid_scale = grid_scale;

    // FPS max
    gui->max_fps = GUI_DEFAULT_MAX_FPS;

    // init GL
    glutInit( &gui->gl_argc, gui->gl_argv );
    
    // init vehicle
    init_vehicle_position( &gui->vehicle_position );

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
    
    init_render_texture_SDL( "res/parrot.png", NULL, NULL, &gui->renderImage );
   
    // set callbacks
    glutCloseFunc( on_close );
    glutKeyboardFunc( on_key );
    glutSpecialFunc( on_special_key );
    glutMouseFunc( on_mouse );
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

    // main loop returns on window exit
    glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );

    // clear the color buffer, background, to black, RGBA
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

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

    // signal GL exit
    glutExit();

    // de-ref
    global_gui_context = NULL;
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
