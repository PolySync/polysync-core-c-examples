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
#include "drawable_type.h"
#include "ps_interface.h"
#include "ground_plane.h"
#include "grid.h"
#include "origin_model.h"
#include "entity_manager.h"
#include "ruler.h"
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
    else if( key == GUI_KEY_VEL_VECTORS )
    {
        // toggle velocity vectors
        global_gui_context->config.velocity_vectors_visible = !global_gui_context->config.velocity_vectors_visible;

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_COLOR_MODE )
    {
        // change color mode
        global_gui_context->config.color_mode += 1;

        // roll over
        if( global_gui_context->config.color_mode == COLOR_MODE_KIND_COUNT )
        {
            global_gui_context->config.color_mode = 0;
        }

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_RADAR_AMP_MAP )
    {
        // toggle adjusted radius mapper
        global_gui_context->config.adjusted_circle_radius = !global_gui_context->config.adjusted_circle_radius;

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_RULER )
    {
        // toggle ruler
        global_gui_context->config.ruler = !global_gui_context->config.ruler;

        // reset if off
        if( global_gui_context->config.ruler == 0 )
        {
            // zero
            global_gui_context->ruler.p1_set = 0;
            global_gui_context->ruler.p2_set = 0;

            // set string
            snprintf( global_gui_context->ruler.p1_string,
                    sizeof(global_gui_context->ruler.p1_string),
                    "P1 (NA) m" );

            // set string
            snprintf( global_gui_context->ruler.p2_string,
                    sizeof(global_gui_context->ruler.p2_string),
                    "P2 (NA) m" );

            // set string
            snprintf( global_gui_context->ruler.distance_string,
                    sizeof(global_gui_context->ruler.distance_string),
                    "distance: NA m" );
        }

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_PLATFORM_VECTORS )
    {
        // toggle ruler
        global_gui_context->config.platform_vectors_visible = !global_gui_context->config.platform_vectors_visible;

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_VIEW_MODE )
    {
        // reset birdseye
        global_gui_context->config.view_mode = VIEW_MODE_BIRDSEYE;
        global_gui_context->config.zoom_scale = 5.0;
        memset( global_gui_context->config.camera_pos, 0, sizeof(global_gui_context->config.camera_pos) );

        // reset origin model position
        global_gui_context->platform.x = 0.0;
        global_gui_context->platform.y = 0.0;
        global_gui_context->platform.z = 0.0;

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_CIRCLE_VISIBLE )
    {
        // toggle visibility
        global_gui_context->config.circle_visible = !global_gui_context->config.circle_visible;

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_RECTANGLE_VISIBLE )
    {
        // toggle visibility
        global_gui_context->config.rectangle_visible = !global_gui_context->config.rectangle_visible;

        // redraw
        glutPostRedisplay();
    }
    else if( key == GUI_KEY_ELLIPSE_VISIBLE )
    {
        // toggle visibility
        global_gui_context->config.ellipse_visible = !global_gui_context->config.ellipse_visible;

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
    if( key == GLUT_KEY_PAGE_UP )
    {
        // zoom in
        global_gui_context->config.zoom_scale += 0.3;

        // redraw
        glutPostRedisplay();
    }
    else if( key == GLUT_KEY_PAGE_DOWN )
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
    else if( key == GLUT_KEY_LEFT )
    {
        // move y in positive direction
        global_gui_context->config.camera_pos[ 1 ] += 1.0;

        // cap
        if( global_gui_context->config.camera_pos[ 1 ] > global_gui_context->grid_scale/2.0 )
        {
            global_gui_context->config.camera_pos[ 1 ] = global_gui_context->grid_scale/2.0;
        }

        // redraw
        glutPostRedisplay();
    }
    else if( key == GLUT_KEY_RIGHT )
    {
        // move y in negative direction
        global_gui_context->config.camera_pos[ 1 ] -= 1.0;

        // cap
        if( global_gui_context->config.camera_pos[ 1 ] < -global_gui_context->grid_scale/2.0 )
        {
            global_gui_context->config.camera_pos[ 1 ] = -global_gui_context->grid_scale/2.0;
        }

        // redraw
        glutPostRedisplay();
    }
    else if( key == GLUT_KEY_UP )
    {
        // move x in positive direction
        global_gui_context->config.camera_pos[ 0 ] += 1.0;

        // cap
        if( global_gui_context->config.camera_pos[ 0 ] > global_gui_context->grid_scale/2.0 )
        {
            global_gui_context->config.camera_pos[ 0 ] = global_gui_context->grid_scale/2.0;
        }

        // redraw
        glutPostRedisplay();
    }
    else if( key == GLUT_KEY_DOWN )
    {
        // move x in negative direction
        global_gui_context->config.camera_pos[ 0 ] -= 1.0;

        // cap
        if( global_gui_context->config.camera_pos[ 0 ] < -global_gui_context->grid_scale/2.0 )
        {
            global_gui_context->config.camera_pos[ 0 ] = -global_gui_context->grid_scale/2.0;
        }

        // redraw
        glutPostRedisplay();
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
        }
        else if( button == GLUT_RIGHT_BUTTON )
        {
            // check if using ruler
            if( global_gui_context->config.ruler != 0 )
            {
                // set points
                if( global_gui_context->ruler.p1_set == 0 )
                {
                    // screen to world coordinates
                    screen_to_world_2d( global_gui_context, sx, sy, &global_gui_context->ruler.x1, &global_gui_context->ruler.y1 );

                    // set
                    global_gui_context->ruler.p1_set = 1;

                    // set string
                    snprintf( global_gui_context->ruler.p1_string,
                            sizeof(global_gui_context->ruler.p1_string),
                            "P1 (%.2f, %.2f) m",
                            global_gui_context->ruler.x1, global_gui_context->ruler.y1 );
                }
                else if( global_gui_context->ruler.p2_set == 0 )
                {
                    // screen to world coordinates
                    screen_to_world_2d( global_gui_context, sx, sy, &global_gui_context->ruler.x2, &global_gui_context->ruler.y2 );

                    // set
                    global_gui_context->ruler.p2_set = 1;

                    // set string
                    snprintf( global_gui_context->ruler.p2_string,
                            sizeof(global_gui_context->ruler.p2_string),
                            "P2 (%.2f, %.2f) m",
                            global_gui_context->ruler.x2, global_gui_context->ruler.y2 );
                }

                // calculate ruler data if both set
                if( (global_gui_context->ruler.p1_set != 0) && (global_gui_context->ruler.p2_set != 0) )
                {
                    // distance = |p2-p1|
                    sx = global_gui_context->ruler.x2 - global_gui_context->ruler.x1;
                    sy = global_gui_context->ruler.y2 - global_gui_context->ruler.y1;
                    global_gui_context->ruler.distance = sqrt( (sx * sx) + (sy * sy) );

                    // set string
                    snprintf( global_gui_context->ruler.distance_string,
                            sizeof(global_gui_context->ruler.distance_string),
                            "distance: %.2f m",
                            global_gui_context->ruler.distance );
                }
            }
            else
            {
                // set platform origin at right-click position

                // store button
                global_gui_context->mouse_button = button;

                // store state
                global_gui_context->mouse_state = 1;

                // store x/y
                global_gui_context->mouse_x = sx;
                global_gui_context->mouse_y = sy;

                // screen to world coordinates
                screen_to_world_2d( global_gui_context, sx, sy, &wx, &wy );

                // set platform origin
                global_gui_context->platform.x = (GLdouble) wx;
                global_gui_context->platform.y = (GLdouble) wy;

                // redraw
                glutPostRedisplay();
            }
        }
        else if( button == 3 )
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
    else
    {
        // clear state
        global_gui_context->mouse_state = 0;
        global_gui_context->mouse_button = 0;
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

    // local vars
    const double    dx = (double) x;
    const double    dy = (double) y;
    double          cx = 0.0;
    double          cy = 0.0;
    double          mx = 0.0;
    double          my = 0.0;


    // check state
    if( global_gui_context->mouse_state != 0 )
    {
        // check view mode
        if( global_gui_context->config.view_mode == VIEW_MODE_BIRDSEYE )
        {
            // get mouse delta
            cx = (dx - global_gui_context->mouse_x);
            cy = (dy - global_gui_context->mouse_y);

            // check button
            if( global_gui_context->mouse_button == GLUT_LEFT_BUTTON )
            {
                // grab camera position
                mx = global_gui_context->config.camera_pos[ 0 ];
                my = global_gui_context->config.camera_pos[ 1 ];

                // translate the difference, x/y swap, scale with zoom to keep a consistent translation/distance ratio
                my += cx * (0.03 / global_gui_context->config.zoom_scale);
                mx += cy * (0.03 / global_gui_context->config.zoom_scale);

                // cap x
                if( mx < -global_gui_context->grid_scale/2.0 )
                {
                    mx = -global_gui_context->grid_scale/2.0;
                }
                else if( mx > global_gui_context->grid_scale/2.0 )
                {
                    mx = global_gui_context->grid_scale/2.0;
                }

                // cap y
                if( my < -global_gui_context->grid_scale/2.0 )
                {
                    my = -global_gui_context->grid_scale/2.0;
                }
                else if( my > global_gui_context->grid_scale/2.0 )
                {
                    my = global_gui_context->grid_scale/2.0;
                }

                // set new camera position
                global_gui_context->config.camera_pos[ 0 ] = mx;
                global_gui_context->config.camera_pos[ 1 ] = my;

                // redraw
                glutPostRedisplay();
            }
            else if( global_gui_context->mouse_button == GLUT_RIGHT_BUTTON )
            {
                // set platform origin at right-click position

                // screen to world coordinates
                screen_to_world_2d( global_gui_context, dx, dy, &mx, &my );

                // set platform origin
                global_gui_context->platform.x = (GLdouble) mx;
                global_gui_context->platform.y = (GLdouble) my;

                // redraw
                glutPostRedisplay();
            }
        }
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

    // update projection based on view mode
    if( global_gui_context->config.view_mode == VIEW_MODE_BIRDSEYE )
    {
        // 2D ortho
        gluOrtho2D( -dw/2.0, dw/2.0, -dh/2.0, dh/2.0 );
    }
    else if( global_gui_context->config.view_mode == VIEW_MODE_PERSPECTIVE )
    {
        // 3D perspective
        gluPerspective( 60.0, dw / dh, 1.0, 228.0 );
    }
    else if( global_gui_context->config.view_mode == VIEW_MODE_SIDE )
    {
        // 2D ortho
        gluOrtho2D( -dw/2.0, dw/2.0, -dh/2.0, dh/2.0 );
    }
    else
    {
        // default 2D ortho
        gluOrtho2D( -dw/2.0, dw/2.0, -dh/2.0, dh/2.0 );
    }

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

    // check view mode
    if( global_gui_context->config.view_mode == VIEW_MODE_BIRDSEYE )
    {
        // configure 2D
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else if( global_gui_context->config.view_mode == VIEW_MODE_PERSPECTIVE )
    {
        // configure 3D
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
    else if( global_gui_context->config.view_mode == VIEW_MODE_SIDE )
    {
        // configure 2D
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        // nothing to do
        glutSwapBuffers();
        return;
    }

    //
    // draw screen text
    //

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
        snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_FREE_FRAME, "freeze frame", global_gui_context->config.freeze_frame ? "ON" : "OFF" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_VEL_VECTORS, "velocity vectors", global_gui_context->config.velocity_vectors_visible ? "ON" : "OFF" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        if( global_gui_context->config.color_mode == COLOR_MODE_OBJECT_ID )
        {
            snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_COLOR_MODE, "color mode", "OBJECT_ID" );
            render_text_2d( 5.0, text_y, string, NULL );
        }
        else if( global_gui_context->config.color_mode == COLOR_MODE_CONTAINER_ID )
        {
            snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_COLOR_MODE, "color mode", "SENSOR_ID" );
            render_text_2d( 5.0, text_y, string, NULL );
        }
        else if( global_gui_context->config.color_mode == COLOR_MODE_PARENT_ID )
        {
            snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_COLOR_MODE, "color mode", "NODE_GUID" );
            render_text_2d( 5.0, text_y, string, NULL );
        }
        else
        {
            snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_COLOR_MODE, "color mode", "UNKNOWN" );
            render_text_2d( 5.0, text_y, string, NULL );
        }
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_RADAR_AMP_MAP, "radar amplitude mapper", global_gui_context->config.adjusted_circle_radius ? "ON" : "OFF" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_RULER, "ruler", global_gui_context->config.ruler ? "ON" : "OFF" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_PLATFORM_VECTORS, "platform motion vectors", global_gui_context->config.platform_vectors_visible ? "ON" : "OFF" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_VIEW_MODE, "Toggle view mode", "BIRDSEYE" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_CIRCLE_VISIBLE, "radar visible",
                global_gui_context->config.circle_visible ? "ON" : "OFF" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_RECTANGLE_VISIBLE, "object visible",
                global_gui_context->config.rectangle_visible ? "ON" : "OFF" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        snprintf( string, sizeof(string), "'%c' - %s - %s", GUI_KEY_ELLIPSE_VISIBLE, "fusion object visible",
                global_gui_context->config.ellipse_visible ? "ON" : "OFF" );
        render_text_2d( 5.0, text_y, string, NULL );
        text_y -= text_delta;
        text_y -= text_delta;

        // mouse logic
        render_text_2d( 5.0, text_y, "2D mouse logic", NULL );
        text_y -= text_delta;
        render_text_2d( 5.0, text_y, "left click and drag (or arrow keys) - translate camera", NULL );
        text_y -= text_delta;
        render_text_2d( 5.0, text_y, "right click or drag - translate origin model", NULL );
        text_y -= text_delta;
        render_text_2d( 5.0, text_y, "scroll wheel (or page up/down) - zoom in/out", NULL );
        text_y -= text_delta;
        text_y -= text_delta;

        // ruler logic
        render_text_2d( 5.0, text_y, "2D ruler logic", NULL );
        text_y -= text_delta;
        render_text_2d( 5.0, text_y, "1st right click - set point 1", NULL );
        text_y -= text_delta;
        render_text_2d( 5.0, text_y, "2nd right click - set point 2", NULL );
        text_y -= text_delta;
    }

    // draw ruler text
    if( global_gui_context->config.ruler != 0 )
    {
        // set vector line color
        glColor4dv( global_gui_context->ruler.color_rgba );

        // render text
        render_text_2d( 5.0, 10.0, global_gui_context->ruler.p1_string, NULL );
        render_text_2d( 5.0, 30.0, global_gui_context->ruler.p2_string, NULL );
        render_text_2d( 5.0, 50.0, global_gui_context->ruler.distance_string, NULL );
    }

    //
    // setup coordinate system
    //

    // get into view mode's coordinate system
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    if( global_gui_context->config.view_mode == VIEW_MODE_BIRDSEYE )
    {
        // 2D ortho
        gluOrtho2D( -width/2.0, width/2.0, -height/2.0, height/2.0 );
    }
    else if( global_gui_context->config.view_mode == VIEW_MODE_PERSPECTIVE )
    {
        // 3D perspective
        gluPerspective( 60.0, width / height, 1.0, 228.0 );
    }
    else if( global_gui_context->config.view_mode == VIEW_MODE_SIDE )
    {
        // 2D ortho
        gluOrtho2D( -width/2.0, width/2.0, -height/2.0, height/2.0 );
    }
    else
    {
        // default 2D ortho
        gluOrtho2D( -width/2.0, width/2.0, -height/2.0, height/2.0 );
    }
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // save state
    glPushMatrix();

    // rotate 90 degrees counter clockwise to get into PolySync coordinates
    glRotated( 90.0, 0.0, 0.0, 1.0 );

    // apply zoom scale and translation
    if( global_gui_context->config.view_mode == VIEW_MODE_BIRDSEYE )
    {
        // scale
        glScaled( global_gui_context->config.zoom_scale, global_gui_context->config.zoom_scale, 1.0 );

        // translate
        glTranslated( -global_gui_context->config.camera_pos[ 0 ], -global_gui_context->config.camera_pos[ 1 ], 0.0 );
    }

    // save state
    glPushMatrix();

    //
    // rendering, order is reversed
    //

    // draw ruler
    if( global_gui_context->config.ruler != 0 )
    {
        ruler_draw( global_gui_context, &global_gui_context->ruler );
    }

    // draw entities
    entity_draw_all( global_gui_context, global_gui_context->entity_list );

    // draw origin model
    origin_model_draw( global_gui_context, &global_gui_context->platform );

    // draw radial grid
    grid_draw_radial( global_gui_context, global_gui_context->grid_scale/2.0, 5.0 );

    // draw cartesian grid
    grid_draw_cartesian( global_gui_context, global_gui_context->grid_scale/2.0, 5.0 );

    // draw ground plane
    ground_plane_draw( global_gui_context, 0.01, 0.01, 0.01 );

    // restore state from zoom/translation
    glPopMatrix();

    // restore state from rotation
    glPopMatrix();

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
    gui->config.zoom_scale = 5.0;
    gui->config.view_mode = VIEW_MODE_BIRDSEYE;
    gui->config.color_mode = COLOR_MODE_OBJECT_ID;
    gui->config.circle_visible = 1;
    gui->config.rectangle_visible = 1;
    gui->config.ellipse_visible = 1;
    gui->config.points_visible = 1;

    // platform color
    gui->platform.color_rgba[ 1 ] = 1.0;
    gui->platform.color_rgba[ 2 ] = 1.0;
    gui->platform.color_rgba[ 3 ] = 0.6;

    // doesn't timeout
    gui->platform.timeout_interval = ENTITY_NO_TIMEOUT;

    // platform origin
    gui->platform.length = 10.0;
    gui->platform.width = 10.0;

    // ruler color
    gui->ruler.color_rgba[ 0 ] = 1.0;
    gui->ruler.color_rgba[ 1 ] = 0.5;
    gui->ruler.color_rgba[ 2 ] = 0.0;
    gui->ruler.color_rgba[ 3 ] = 1.0;

    // set string
    snprintf( gui->ruler.p1_string,
            sizeof(gui->ruler.p1_string),
            "P1 (NA) m" );

    // set string
    snprintf( gui->ruler.p2_string,
            sizeof(gui->ruler.p2_string),
            "P2 (NA) m" );

    // set string
    snprintf( gui->ruler.distance_string,
            sizeof(gui->ruler.distance_string),
            "distance: NA m" );

    // set window data
    strncpy( gui->win_title, win_title, sizeof(gui->win_title) );
    gui->win_width = win_width;
    gui->win_height = win_height;
    gui->grid_scale = grid_scale;

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
    glEnable( GL_DEPTH );
    glDisable( GL_LIGHTING );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );

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
