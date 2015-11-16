/*
 * Copyright (c) 2015 HARBRICK TECHNOLOGIES, INC
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
 * \example data_generator.c
 *
 * Data Generator Example.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_node.h"
#include "polysync_message.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief Default update interval for all generators. [microseconds]
 *
 */
#define GENERATOR_UPDATE_INTERVAL (50000)


/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static sig_atomic_t global_exit_signal = 0;


/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "data-generator";


/**
 * @brief PolySync radar targets message name.
 *
 */
static const char PS_RADAR_TARGETS_MSG_NAME[] = "ps_radar_targets_msg";


/**
 * @brief PolySync LiDAR points message name.
 *
 */
static const char PS_LIDAR_POINTS_MSG_NAME[] = "ps_lidar_points_msg";


/**
 * @brief PolySync objects message name.
 *
 */
static const char PS_OBJECTS_MSG_NAME[] = "ps_objects_msg";




// *****************************************************
// public declarations
// *****************************************************

/**
 * @brief Initialize radar targets generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_radar_targets_msg which receives the initialization.
 *
 */
void radar_targets_init( const ps_timestamp time, ps_radar_targets_msg * const msg );


/**
 * @brief Update radar targets generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_radar_targets_msg which receives the update.
 *
 */
void radar_targets_update( const ps_timestamp time, ps_radar_targets_msg * const msg );


/**
 * @brief Initialize LiDAR points generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_lidar_points_msg which receives the initialization.
 *
 */
void lidar_points_init( const ps_timestamp time, ps_lidar_points_msg * const msg );


/**
 * @brief Update LiDAR points generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_lidar_points_msg which receives the update.
 *
 */
void lidar_points_update( const ps_timestamp time, ps_lidar_points_msg * const msg );


/**
 * @brief Initialize objects generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_objects_msg which receives the initialization.
 *
 */
void objects_init( const ps_timestamp time, ps_objects_msg * const msg );


/**
 * @brief Update objects generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_objects_msg which receives the update.
 *
 */
void objects_update( const ps_timestamp time, ps_objects_msg * const msg );




// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief Signal handler.
 *
 * @param [in] signal Signal value to handle.
 *
 */
static void sig_handler( int signal );




// *****************************************************
// static definitions
// *****************************************************


//
static void sig_handler( int sig )
{
	if( sig == SIGINT )
    {
        global_exit_signal = 1;
    }
}




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // node reference
    ps_node_ref node_ref = PSYNC_NODE_REF_INVALID;

    // 'ps_radar_targets_msg' message type
    ps_msg_type radar_targets_msg_type = PSYNC_MSG_TYPE_INVALID;

    // 'ps_radar_targets_msg' message
    ps_msg_ref radar_targets_msg = PSYNC_MSG_REF_INVALID;

    // 'ps_lidar_points_msg' message type
    ps_msg_type lidar_points_msg_type = PSYNC_MSG_TYPE_INVALID;

    // 'ps_lidar_points_msg' message
    ps_msg_ref lidar_points_msg = PSYNC_MSG_REF_INVALID;

    // 'ps_objects_msg' message type
    ps_msg_type objects_msg_type = PSYNC_MSG_TYPE_INVALID;

    // 'ps_objects_msg' message
    ps_msg_ref objects_msg = PSYNC_MSG_REF_INVALID;

    // current time
    ps_timestamp timestamp = 0;


	// init core API
    if( (ret = psync_init(
            NODE_NAME,
            PSYNC_NODE_TYPE_API_USER,
            PSYNC_DEFAULT_DOMAIN,
            PSYNC_SDF_KEY_INVALID,
            PSYNC_INIT_FLAG_STDOUT_LOGGING,
            &node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // nodes typically should shutdown after handling SIGINT
    // hook up the control-c signal handler, sets exit signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    // get message type
    if( (ret = psync_message_get_type_by_name( node_ref, PS_RADAR_TARGETS_MSG_NAME, &radar_targets_msg_type )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_get_type_by_name - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get message type
    if( (ret = psync_message_get_type_by_name( node_ref, PS_LIDAR_POINTS_MSG_NAME, &lidar_points_msg_type )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_get_type_by_name - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get message type
    if( (ret = psync_message_get_type_by_name( node_ref, PS_OBJECTS_MSG_NAME, &objects_msg_type )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_get_type_by_name - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get message
    if( (ret = psync_message_alloc( node_ref, radar_targets_msg_type, &radar_targets_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_alloc - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get message
    if( (ret = psync_message_alloc( node_ref, lidar_points_msg_type, &lidar_points_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_alloc - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get message
    if( (ret = psync_message_alloc( node_ref, objects_msg_type, &objects_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_alloc - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get timestamp
    if( (ret = psync_get_timestamp( &timestamp )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // init radar targets generator and resources
    radar_targets_init( timestamp, (ps_radar_targets_msg*) radar_targets_msg );

    // init lidar points generator and resources
    lidar_points_init( timestamp, (ps_lidar_points_msg*) lidar_points_msg );

    // init objects generator and resources
    objects_init( timestamp, (ps_objects_msg*) objects_msg );


    // main event loop
    // loop until signaled (control-c)
    while( global_exit_signal == 0 )
    {
        // get timestamp
        if( (ret = psync_get_timestamp( &timestamp )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // update radar targets
        radar_targets_update( timestamp, (ps_radar_targets_msg*) radar_targets_msg );

        // publish radar targets message
        if( (ret = psync_message_publish( node_ref, radar_targets_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish - ret: %d", ret );
        }

        // update lidar points
        lidar_points_update( timestamp, (ps_lidar_points_msg*) lidar_points_msg );

        // publish lidar points message
        if( (ret = psync_message_publish( node_ref, lidar_points_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish - ret: %d", ret );
        }

        // update objects
        objects_update( timestamp, (ps_objects_msg*) objects_msg );

        // publish objects message
        if( (ret = psync_message_publish( node_ref, objects_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish - ret: %d", ret );
        }

        // wait for next update
        psync_sleep_micro( GENERATOR_UPDATE_INTERVAL );
    }


    // using 'goto' to allow for an easy example exit
    GRACEFUL_EXIT_STMNT:
    global_exit_signal = 1;

    // release message
    if( (ret = psync_message_free( node_ref, &radar_targets_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_free - ret: %d", ret );
    }

    // release message
    if( (ret = psync_message_free( node_ref, &lidar_points_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_free - ret: %d", ret );
    }

    // release message
    if( (ret = psync_message_free( node_ref, &objects_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_free - ret: %d", ret );
    }

	// release core API
    if( (ret = psync_release( &node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }


	return EXIT_SUCCESS;
}
