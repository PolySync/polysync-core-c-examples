/**
 * @file radar_target_gen.c
 * @brief TODO.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// API headers
#include "polysync_core.h"
#include "polysync_message.h"
#include "polysync_sdf.h"




// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static sig_atomic_t                 exit_signaled			= 0;




// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief Signal handler.
 *
 */
static void sig_handler( int signal );





// *****************************************************
// static definitions
// *****************************************************

//
static void sig_handler( int sig )
{
	//if( sig == SIGINT )
    {
        exit_signaled = 1;
    }
}




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // polysync node name
    const char *node_name = "radar-target-gen";

    ps_node_ref node_ref = PSYNC_NODE_REF_INVALID;



	// init core API
    ret = psync_init( node_name, PSYNC_NODE_TYPE_API_USER, PSYNC_DEFAULT_DOMAIN, PSYNC_SDF_KEY_INVALID, 0, &node_ref );

    // error check
    if( ret != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


    // nodes typically should shutdown after handling SIGINT

    // hook up the control-c signal handler, sets exit_signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );



    ps_msg_type radar_targets_msg_type = PSYNC_MSG_TYPE_INVALID;

    ps_msg_ref msg = PSYNC_MSG_REF_INVALID;

    if( (ret = psync_message_get_type_by_name( node_ref, "ps_radar_targets_msg", &radar_targets_msg_type )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_get_type_by_name - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


    if( (ret = psync_message_request_instance( node_ref, radar_targets_msg_type, &msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


    ps_radar_targets_msg *radar_msg = (ps_radar_targets_msg*) msg;


    radar_msg->targets._buffer = DDS_sequence_ps_radar_target_allocbuf( 1 );
    radar_msg->targets._length = 1;
    radar_msg->targets._maximum = 1;
    radar_msg->targets._release = 1;

    memset( radar_msg->targets._buffer, 0, radar_msg->targets._maximum * sizeof(*radar_msg->targets._buffer) );




    // test loop
    while( exit_signaled == 0 )
    {
        ps_radar_target *target = &radar_msg->targets._buffer[ 0 ];


        if( (ret = psync_get_timestamp( &radar_msg->header.timestamp )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }


        target->id = 1;
        target->timestamp = radar_msg->header.timestamp;

        target->position[ 0 ] = 10.0;
        target->position[ 1 ] = 0.0;
        target->position[ 2 ] = 0.0;

        target->size[ 0 ] = 10.0;
        target->size[ 1 ] = 0.0;
        target->size[ 2 ] = 0.0;

        target->velocity[ 0 ] = 5.0;
        target->velocity[ 1 ] = 0.0;
        target->velocity[ 2 ] = 0.0;

        target->track_status = TRACK_STATUS_RAW_TARGET;


        if( (ret = psync_get_timestamp( &radar_msg->header.timestamp )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        if( (ret = psync_message_publish_instance( node_ref, msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish_instance - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // wait a little
        psync_sleep_micro( 50000 );
    }



    // using 'goto' to allow for an easy example exit
GRACEFUL_EXIT_STMNT:
    exit_signaled = 1;


    (void) psync_message_release_instance( node_ref, &msg );

	// release core API
    if( (ret = psync_release( &node_ref, 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }



	return 0;
}
