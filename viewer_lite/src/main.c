/**
 * @file main.c
 * @brief Main entry for viewer-lite.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <glib-2.0/glib.h>

#include "polysync_core.h"
#include "common.h"
#include "drawable_type.h"
#include "ps_interface.h"
#include "gui.h"
#include "entity_manager.h"




// *****************************************************
// static global structures
// *****************************************************

/**
 * @brief Global flag indicating exit signal was caught.
 *
 * Value zero means signal not received.
 *
 */
static volatile sig_atomic_t    global_exit_signal      = 0;




// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief Signal handler.
 *
 * Sets global \ref global_exit_signal flag to a non-zero value on SIGINT.
 *
 * @param [in] sig Signal being handled.
 *
 */
static void sig_handler( int sig );


/**
 * @brief Graceful release routine.
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the context to free. NULL is acceptable.
 * @param [in] data_queue A pointer to GAsyncQueue which specifies the PolySync message queue to free. NULL is acceptable.
 *
 */
static void release( gui_context_s * const gui, node_data_s * const node_data );




// *****************************************************
// static definitions
// *****************************************************

//
static void sig_handler( int sig )
{
    // graceful exit on SIGINT
    if( sig == SIGINT )
    {
        global_exit_signal = 1;
    }
}


//
static void release( gui_context_s * const gui, node_data_s * const node_data )
{
    // check node data
    if( node_data != NULL )
    {
        // release
        release_polysync( node_data );

        free( node_data );
    }

    // check GUI
    if( gui != NULL )
    {
        // release entities
        entity_release_all( gui->entity_list );

        // release
        gui_release( gui );

        // free
        free( gui );
    }
}




// *****************************************************
// public definitions
// *****************************************************


//
ps_timestamp get_micro_tick( void )
{
    // local vars
    struct timespec timespec;
    ps_timestamp timestamp = 0;


    // get time
    clock_gettime( CLOCK_MONOTONIC, &timespec );

    // convert to micro seconds
    timestamp = (ps_timestamp) SEC_2_MICRO( (ps_timestamp) timespec.tv_sec );

    // convert nanosecond remainder to micro seconds
    timestamp += (ps_timestamp) NANO_2_MICRO( (ps_timestamp) timespec.tv_nsec );


    return timestamp;
}


/**
 * @brief Main entry function.
 *
 * Starts the application.
 *
 * @param [in] argc Number of arguments in the argv argument list.
 * @param [in] argv Argument list.
 *
 * @return Zero on success, one on failure.
 *
 */
int main( int argc, char *argv[] )
{
    // local vars
    gui_context_s   *gui            = NULL;
    node_data_s     *node_data      = NULL;
    ps_timestamp    timestamp       = 0;
    ps_timestamp    time_to_draw    = 0;
    size_t          sleep_tick      = 0;
    unsigned int    msg_read        = 0;


    // hook up the control-c signal handler, sets exit_signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    // init PolySync
    if( (node_data = init_polysync()) == NULL )
    {
        printf( "failed to initialize PolySync\n" );
        release( gui, node_data );
        return EXIT_FAILURE;
    }

    // init GUI
    if( (gui = gui_init( PS_NODE_NAME, GUI_DEFAULT_WIDTH, GUI_DEFAULT_HEIGHT, GUI_DEFAULT_GRID_SCALE )) == NULL )
    {
        printf( "failed to initialize GUI\n" );
        release( gui, node_data );
        return EXIT_FAILURE;
    }

    // main event loop
    while( 1 )
    {
        // zero
        time_to_draw = 0;
        msg_read = 0;

        // check for an exit signal
        if( global_exit_signal != 0 )
        {
            release( gui, node_data );
            return EXIT_SUCCESS;
        }

        // get timestamp
        timestamp = get_micro_tick();

        // check and process message queue
        gui->entity_list = ps_process_message( node_data, gui, gui->entity_list, timestamp, &msg_read );

        // if message processed
        if( msg_read != 0 )
        {
            // reset ticker
            sleep_tick = 0;
        }

        // check timeouts if not in freeze-frame
        if( gui->config.freeze_frame == 0 )
        {
            gui->entity_list = entity_update_timeouts( gui->entity_list, timestamp );
        }

        // update gui
        gui_update( gui, timestamp, &time_to_draw );

        // disable sleep ticker if redraw time is less than 5 ms
        if( time_to_draw < 5000 )
        {
            // reset ticker
            sleep_tick = 0;
        }

        // check sleep ticker
        if( sleep_tick >= 10 )
        {
            // sleep ~1 ms
            usleep( 1000 );

            // reset to enabled
            sleep_tick = 10;
        }

        // increment sleep ticker
        sleep_tick++;
    }

    // shouldn't get here

    // release
    release( gui, node_data );


    // exit
    return EXIT_SUCCESS;
}
