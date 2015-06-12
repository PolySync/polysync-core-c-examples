/******************************************************
** FILE: write_logfile.c
**
** ABSTRACT:
**
** DOCUMENTS:
**
** NOTES:
**
*******************************************************/



/**
 * @example write_logfile.c
 *
 * Writes to a log file.
 *
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
    const char      *node_name = "write-logfile";

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
        printf( "missing output file argument\n" );
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

    // set manual file name
    if( (ret = psync_logfile_set_logfile_name( &logfile, logfile_path )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_logfile_set_logfile_name returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // enable mode
    if( (ret = psync_logfile_update_mode( &logfile, LOGGER_MODE_WRITE )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_logfile_update_mode returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set desired state
    if( (ret = psync_logfile_update_state( &logfile, LOGGER_STATE_RECORD )) != DTC_NONE )
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

        // record thread will make a copy of this message and write it to disk
        void *msg = NULL;



#ifdef DO_MY_DM_TEST
        // create our desired message
        if( (ret = psync_message_request_instance( MSG_TYPE_SITE_INFO_MSG, &msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance returned: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // put some dummy data in
        {
            // cast
            site_info_msg *udm_msg = (site_info_msg*) msg;

            // set timestamp
            if( (ret = psync_get_timestamp_micro( &udm_msg->header.timestamp )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp_micro returned: %d", ret );
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
        }
#else
        // create our desired message
        if( (ret = psync_message_request_instance( MSG_TYPE_EVENT, &msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance returned: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // put some dummy data in
        {
            // cast
            ps_event_msg *event_msg = (ps_event_msg*) msg;

            // set timestamp
            if( (ret = psync_get_timestamp_micro( &event_msg->header.timestamp )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp_micro returned: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }

            // set id to timestamp
            event_msg->id = event_msg->header.timestamp;

            // set value
            event_msg->value._d = PARAMETER_DOUBLE;
            event_msg->value._u.d_value = 2.0;
        }
#endif

        // write to disk
        if( (ret = psync_logfile_write_instance( &logfile, msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_logfile_write_instance returned: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // release
        if( (ret = psync_message_release_instance( &msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance returned: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // wait a little
        psync_sleep_micro( 50000 );
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

