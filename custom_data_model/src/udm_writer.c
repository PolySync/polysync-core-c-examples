/******************************************************
** FILE: udm_writer.c
**
** ABSTRACT:
**
** DOCUMENTS:
**
** NOTES:
**
*******************************************************/



/**
 * @example polysync_writer.c
 *
 * Example data writer.
 */


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// API headers
#include "polysync_core.h"

// User Data Model
#include "my_dm.h"
#include "my_dmDcps.h"






// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static sig_atomic_t                 exit_signaled			= 0;


/**
 * @brief PolySync message pointer.
 *
 */
static ps_platform_motion_msg      *pmotion_msg            = NULL;


/**
 * @brief UDM message pointer.
 *
 */
static site_info_msg                   *udm_msg             = NULL;





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
static void sig_handler( int signal )
{
	//if( signal == SIGINT )
	exit_signaled = 1;
}






// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // polysync node name
    const char *node_name = "udm-writer";

    // flag to enable stdout logs in addition to the normal syslog output
    unsigned int stdout_logging_enabled = 1;


    //
	// init core API
    if( (ret = psync_init( PSYNC_NID_API, node_name, stdout_logging_enabled )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


    //
    // nodes typically should shutdown after handling SIGINT

    // hook up the control-c signal handler, sets exit_signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );


    // load UDM
    if( (ret = psync_load_user_data_model( "udm/libudm.so" )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_load_user_data_model - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get PolySync message
    if( (ret = psync_message_request_instance( MSG_TYPE_PLATFORM_MOTION, (void**) &pmotion_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get UDM message
    if( (ret = psync_message_request_instance( MSG_TYPE_SITE_INFO_MSG, (void**) &udm_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


    // some dummy data
    udm_msg->site_id = 1;
    udm_msg->toolboxes._buffer = DDS_sequence_toolbox_s_allocbuf( 2 );
    udm_msg->toolboxes._length = 2;
    udm_msg->toolboxes._maximum = 2;

    udm_msg->toolboxes._buffer[ 0 ].cable.pin_numbers[ 0 ] = 1;
    udm_msg->toolboxes._buffer[ 0 ].cable.pin_numbers[ 1 ] = 2;
    udm_msg->toolboxes._buffer[ 0 ].owner._buffer = DDS_sequence_char_allocbuf( 3 );
    udm_msg->toolboxes._buffer[ 0 ].owner._length = 3;
    udm_msg->toolboxes._buffer[ 0 ].owner._maximum = 3;


    udm_msg->toolboxes._buffer[ 1 ].cable.pin_numbers[ 0 ] = 3;
    udm_msg->toolboxes._buffer[ 1 ].cable.pin_numbers[ 1 ] = 4;
    udm_msg->toolboxes._buffer[ 1 ].owner._buffer = DDS_sequence_char_allocbuf( 4 );
    udm_msg->toolboxes._buffer[ 1 ].owner._length = 4;
    udm_msg->toolboxes._buffer[ 1 ].owner._maximum = 4;


    site_info_msg *copied_msg = NULL;

    // get UDM message
    if( (ret = psync_message_request_instance( MSG_TYPE_SITE_INFO_MSG, (void**) &copied_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // copy UDM
    if( (ret = psync_message_copy( udm_msg, copied_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_copy - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // publish UDM message
    if( (ret = psync_message_publish_instance( (void*) copied_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


    // main processing loop
    while( 1 )
    {
        //
        // check for an exit signal
        if( exit_signaled )
        {
            goto GRACEFUL_EXIT_STMNT;
        }


        // get timestamp
        if( (ret = psync_get_timestamp_micro( &pmotion_msg->header.timestamp )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp_micro - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // get timestamp
        if( (ret = psync_get_timestamp_micro( &udm_msg->header.timestamp )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp_micro - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }




        // publish PolySync message
        if( (ret = psync_message_publish_instance( (void*) pmotion_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish_instance - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // publish UDM message
        if( (ret = psync_message_publish_instance( (void*) udm_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish_instance - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }



        // test some values
        udm_msg->site_id++;
        pmotion_msg->velocity[0] += 0.1;

        //
        // wait a little
        psync_sleep_micro( 20000 );
    }



    // using 'goto' to allow for an easy example exit
GRACEFUL_EXIT_STMNT:
    exit_signaled = 1;



    // release PolySync message
    if( (ret = psync_message_release_instance( (void**) &pmotion_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance - ret: %d", ret );
    }

    // release UDM message
    if( (ret = psync_message_release_instance( (void**) &udm_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance - ret: %d", ret );
    }

    // release UDM message
    if( (ret = psync_message_release_instance( (void**) &copied_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance - ret: %d", ret );
    }




	// release core API
    if( (ret = psync_release( 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }



	return 0;
}

