/******************************************************
** FILE: udm_reader.c
**
** ABSTRACT:
**
** DOCUMENTS:
**
** NOTES:
**
*******************************************************/



/**
 * @example polysync_reader.c
 *
 * Example data reader.
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
static sig_atomic_t                 exit_signaled                       = 0;






// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief Signal handler.
 *
 */
static void sig_handler( int signal );


/**
 * @brief API on_data handler for platform motion messages.
 *
 * @warning Do not modify topic_data.
 */
static void on_psync_data_platform_motion_msg( void *usr_data, ps_msg_type msg_type, void *topic_data );


/**
 * @brief API on_data handler for user's site_info_msg messages.
 *
 * @warning Do not modify topic_data.
 */
static void on_udm_data_site_info_msg( void *usr_data, ps_msg_type msg_type, void *topic_data );







// *****************************************************
// static definitions
// *****************************************************


//
static void sig_handler( int signal )
{
	//if( signal == SIGINT )
	exit_signaled = 1;
}


//
static void on_psync_data_platform_motion_msg( void *usr_data, ps_msg_type msg_type, void *topic_data )
{
    // cast reference
	ps_platform_motion_msg *message = (ps_platform_motion_msg*) topic_data;

    printf( "ps_platform_motion_msg -- timestamp: %llu\n", message->header.timestamp );

    // use stdout to avoid bloating the log file
    printf( "vel_x: %.2f\nyaw_rate: %.2f\nlat: %f\nlon: %f\nalt: %f\n\n",
            message->velocity[0], message->rotation_rate[2], message->latitude, message->longitude, message->altitude );
}


//
static void on_udm_data_site_info_msg( void *usr_data, ps_msg_type msg_type, void *topic_data )
{
    // cast reference
	site_info_msg *message = (site_info_msg*) topic_data;

    printf( "site_info_msg -- timestamp: %llu\n", message->header.timestamp );

    printf( "site_id: %lu\n", (unsigned long) message->site_id );

    for( unsigned int i = 0; i < message->toolboxes._length; i++ )
    {
        printf( "toolbox[ %u ].cable.pin_numbers: %u %u ... - owner._length: %u\n",
                i, message->toolboxes._buffer[ i ].cable.pin_numbers[ 0 ], message->toolboxes._buffer[ i ].cable.pin_numbers[ 1 ],  message->toolboxes._buffer[ i ].owner._length );
    }

    printf( "\n" );
}





// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // polysync node name
    const char *node_name = "udm-reader";

    // flag to enable stdout logs in addition to the normal syslog output
    unsigned int stdout_logging_enabled = 1;



	// init core API
    if( (ret = psync_init( PSYNC_NID_API, node_name, stdout_logging_enabled )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


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

    // register a listener for PolySync platform motion messages
    if( (ret = psync_message_register_listener( MSG_TYPE_PLATFORM_MOTION, on_psync_data_platform_motion_msg, NULL )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_register_listener - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // register a listener for UDM site_info_msg messages
    if( (ret = psync_message_register_listener( MSG_TYPE_SITE_INFO_MSG, on_udm_data_site_info_msg, NULL )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_register_listener - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


	// main processing loop
	while( 1 )
	{
		// check for an exit signal
		if( exit_signaled )
		{
			goto GRACEFUL_EXIT_STMNT;
		}


		// sleep a little
		psync_sleep_micro( 100000 );
	}



	// using 'goto' to allow for an easy example exit
GRACEFUL_EXIT_STMNT:



	// release
	if( (ret = psync_release( 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }


	return 0;
}

