/******************************************************
** FILE: load_logfile.c
**
** ABSTRACT:
**
** DOCUMENTS:
**
** NOTES:
**
*******************************************************/



/**
 * @example load_logfile.c
 *
 * Reads a logfile.
 */


#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <glib-2.0/glib.h>

// API headers
#include "polysync_core.h"
#include "psyncDcps.h"

/**
 * @brief Whether or not we are using user data model resources
 *
 */
#ifdef DO_MY_DM_TEST
// User Data Model
#include "udm/my_dm.h"
#include "udm/my_dmDcps.h"
#endif




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
static void sig_handler( int signal )
{
	//if( signal == SIGINT )
	exit_signaled = 1;
}










// *****************************************************
// public definitions
// *****************************************************

// main
int main( int argc, char **argv )
{
    // polysync return status
    int             ret = DTC_NONE;

    // polysync node name
    const char      *node_name = "load-logfile";

    // flag to enable stdout logs in addition to the normal syslog output
    unsigned int    stdout_logging_enabled = 1;

    // manual logfile path
    char            logfile_path[ 256 ] = { 0 };

    // node GUID
    ps_guid         node_guid = 0;

    // logfile
    ps_logfile      logfile;


    // zero
    memset( &logfile, 0, sizeof(logfile) );


    // check for argument (filename)
    if( (argc == 2) && (strlen(argv[1]) > 0) )
    {
        strncpy( logfile_path, argv[1], sizeof(logfile_path) );
    }
    else
    {
        printf( "missing input file argument\n" );
        return EXIT_FAILURE;
    }


    // nodes typically should shutdown after handling SIGINT

    // hook up the control-c signal handler, sets exit_signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );


    // init core API
    if( (ret = psync_init( PSYNC_NID_API, node_name, stdout_logging_enabled )) != DTC_NONE )
    {
        printf( "main -- psync_init - returned: %d\n", ret );
        return EXIT_FAILURE;
    }

#ifdef DO_MY_DM_TEST
    // load UDM
    if( (ret = psync_load_user_data_model( "udm/libudm.so" )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_load_user_data_model - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }
#endif

    // get this node's GUID
    if( (ret = psync_get_GUID( &node_guid )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_GUID returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // init logger
    if( (ret = psync_logfile_init( &logfile, node_name, node_guid, 0, 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_logfile_init returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // enable the replay output queue so we can parse outgoing messages without listening to the bus
    if( (ret = psync_logfile_enable_replay_queue( &logfile, 1 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_logfile_enable_replay_queue returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set manual file name
    if( (ret = psync_logfile_set_logfile_name( &logfile, logfile_path )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_logfile_set_logfile_name returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // enable mode - starts loading logfile data - returns when entire log is loaded in memory (could take a while)
    if( (ret = psync_logfile_update_mode( &logfile, LOGGER_MODE_REPLAY )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_logfile_update_mode returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set desired state, starting replay
    if( (ret = psync_logfile_update_state( &logfile, LOGGER_STATE_REPLAY )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_logfile_update_state returned: %d", ret );
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

        // replay thread will put a copy of the data into a queue (when enabled)
        void *msg = g_async_queue_try_pop( logfile.replay_output_queue );

        // if we got a message
        if( msg )
        {
            // message type
            ps_msg_type msg_type = 0;

            // message header.timestamp
            ps_timestamp msg_timestamp = 0;

            // get message type
            if( (ret = psync_message_get_type( msg, &msg_type )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_get_type returned: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }

            // get message timestamp
            if( (ret = psync_message_get_timestamp( msg, &msg_timestamp )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_get_timestamp returned: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }

            // release the message
            if( (ret = psync_message_release_instance( (void**) &msg )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance returned: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }

            // print instead of log so we don't bloat the syslog
            printf( "replay message - type: %u - timestamp: %llu\n", (unsigned int) msg_type, msg_timestamp );
        }


        // wait a little
        psync_sleep_micro( 20000 );
    }



    // using 'goto' to allow for an easy example exit
GRACEFUL_EXIT_STMNT:
    exit_signaled = 1;




    // release logger
    if( (ret = psync_logfile_release( &logfile )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_logfile_release returned: %d", ret );
    }

	// release core API
    if( (ret = psync_release( 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release returned: %d", ret );
    }



	return 0;
}

