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
#include "ps_interface.h"
#include "grid.h"
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
 * @brief Convert 2D screen coordinates to world coordinates.
 *
 * Transforms the x/y values into the coordinate system.
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the configuration(s).
 * @param [in] sx Screen X coordinate.
 * @param [in] sy Screen X coordinate.
 * @param [out] wx A pointer to GLdouble which receives the world X coordinate.
 * @param [out] wy A pointer to GLdouble which receives the world Y coordinate.
 *
 */
static void screen_to_world_2d( const gui_context_s * const gui, const GLdouble sx, const GLdouble sy, GLdouble * const wx, GLdouble * const wy );

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
static void screen_to_world_2d( const gui_context_s * const gui, const GLdouble sx, const GLdouble sy, GLdouble * const wx, GLdouble * const wy )
{
    // local vars
    const double    width = ((GLdouble) gui->win_width);
    const double    height = ((GLdouble) gui->win_height);


    // screen to world coordinates, x/y swap, apply zoom scale
    (*wy) = -(sx - width/2.0) * 1.0/gui->config.zoom_scale;
    (*wx) = -(sy - height/2.0) * 1.0/gui->config.zoom_scale;

    // add camera position offsets
    (*wx) += gui->config.camera_pos[ 0 ];
    (*wy) += gui->config.camera_pos[ 1 ];
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
    else if( key == GUI_KEY_CLEAR_WAYPOINTS )
    {
        zero_waypoints( global_gui_context->waypoints );

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


    // check key
    if( key == GLUT_KEY_LEFT )
    {
        // Do nothing
    }
    else if( key == GLUT_KEY_RIGHT )
    {
        // Do nothing
    }
    else if( key == GLUT_KEY_UP )
    {
        // Do nothing
    }
    else if( key == GLUT_KEY_DOWN )
    {
        // Do nothing
    }
}


//
static void on_mouse( int button, int state, int x, int y )
{
    // ignore if no reference
    if( global_gui_context == NULL )
    {
        return;
    }

    // local vars
    GLdouble        wx = 0.0;
    GLdouble        wy = 0.0;
    GLdouble        sx = 0.0;
    GLdouble        sy = 0.0;


    // check state, 1 is released, 0 is pressed
    if( state == 0 )
    {
        // get x/y
        sx = (GLdouble) x;
        sy = (GLdouble) y;

        // check button
        if( button == GLUT_LEFT_BUTTON )
        {
            // store button
            global_gui_context->mouse_button = button;

            // store state
            global_gui_context->mouse_state = 1;

            // store x/y
            global_gui_context->mouse_x = sx;
            global_gui_context->mouse_y = sy;
            
            screen_to_world_2d( global_gui_context, sx, sy, &wx, &wy );
            
            add_new_waypoint( wx, wy, global_gui_context->waypoints );
        }
        else if( button == GLUT_RIGHT_BUTTON )
        {
            // Do nothing
        }
        else if( button == 3 )
        {
            // Do nothing
        }
        else if( button == 4 )
        {
            // Do nothing
        }
    }
    else
    {
        // clear state
        global_gui_context->mouse_state = 0;
        global_gui_context->mouse_button = 0;
    }
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


    // default 2D ortho
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
    glLineWidth( (GLfloat) GUI_DEFAULT_LINE_WIDTH );

    // set default point size
    glPointSize( 1.0f );

    // enable blending
    glEnable( GL_BLEND );

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    //
    // setup coordinate system
    //

    // get into view mode's coordinate system
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    gluOrtho2D( -width/2.0, width/2.0, -height/2.0, height/2.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // save state
    glPushMatrix();

    // rotate 90 degrees counter clockwise to get into PolySync coordinates
    glRotated( 90.0, 0.0, 0.0, 1.0 );

    // apply zoom scale and translation
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

    // draw cartesian grid
    grid_draw_cartesian( global_gui_context, global_gui_context->grid_scale/2.0, 5.0 );

    // draw entities
    draw_waypoints( global_gui_context->waypoints, global_gui_context );
    
    // restore state from zoom/translation
    glPopMatrix();

    // restore state from rotation
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

    text_y = height - text_delta;

    // key maps
    render_text_2d( 5.0, text_y, "key map", NULL );
    text_y -= text_delta;
    snprintf( string, sizeof(string), "'%c' - %s", GUI_KEY_CLEAR_WAYPOINTS, "clear waypoints" );
    render_text_2d( 5.0, text_y, string, NULL );

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

    // default configurations
    gui->config.wireframe_width = GUI_DEFAULT_WIRE_LINE_WIDTH;
    gui->config.zoom_scale = 1.9;

    // set window data
    strncpy( gui->win_title, win_title, sizeof(gui->win_title) );
    gui->win_width = win_width;
    gui->win_height = win_height;
    gui->grid_scale = grid_scale;

    // FPS max
    gui->max_fps = GUI_DEFAULT_MAX_FPS;
    
    // Init waypoints
    zero_waypoints( gui->waypoints );

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
