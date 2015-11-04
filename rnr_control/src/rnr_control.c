/**
 * \example rnr_control.c
 *
 * Record and Replay Control.
 *
 * Shows how to use the rnr convenience routines to control the record and replay
 * abilities of PolySync nodes.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>

// API headers
#include "polysync_core.h"
#include "polysync_message.h"
#include "polysync_sdf.h"
#include "polysync_rnr.h"




// *****************************************************
// static global data
// *****************************************************


/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static sig_atomic_t global_exit_signal = 0;


/**
 * @brief Standard options string.
 *
 */
static const char GETOPT_STRING[] = "hqwt:s:S:";


/**
 * @brief Getopt no-argument flag.
 *
 */
static const int GETOP_NOARG_FLAG = (int) '?';


/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "rnr-control";




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
	if( sig == SIGINT )
    {
        global_exit_signal = 1;
    }
}


//
static void handle_arguments( ps_node_ref const node_ref, int argc, char **argv )
{
    // local vars
    int ret = DTC_NONE;
    int optret = 0;
    ps_msg_type rnr_msg_type = PSYNC_MSG_TYPE_INVALID;
    ps_msg_ref msg = PSYNC_MSG_REF_INVALID;
    ps_rnr_msg *rnr_msg = NULL;

    // flags from command line arguments
    unsigned int record_enabled = 0;
    ps_rnr_session_id session_id = PSYNC_RNR_SESSION_ID_INVALID;
    ps_timestamp start_time = 0;
    unsigned int quit_session = 0;


    // reset scanner
    optind = 0;

    // for each option in optstring
    while( (optret = getopt( argc, argv, GETOPT_STRING )) != -1 )
    {
        // check for option character
        if( optret == 'q' )
        {
            quit_session = 1;
        }
        else if( optret == 'w' )
        {
            record_enabled = 1;
        }
        else if( optret == 't' )
        {
            // check for argument
            if( (optarg != NULL) && (strlen(optarg) > 0) )
            {
                // get session ID
                if( atoll(optarg) > 0 )
                {
                    session_id = (ps_rnr_session_id) atoll(optarg);
                }
                else
                {
                    printf( "'t' option expects a valid RnR session ID (must be positive integer)\n" );
                    return;
                }
            }
        }
        else if( optret == 's' )
        {
            // get relative start time
            if( atoll(optarg) > 0 )
            {
                // get now time
                (void) psync_get_timestamp( &start_time );

                // add relative
                start_time += (ps_timestamp) atoll(optarg);
            }
            else
            {
                printf( "'s' option expects a valid relative start time (must be positive integer)\n" );
                return;
            }
        }
        else if( optret == 'S' )
        {
            // get absolute start time
            if( atoll(optarg) > 0 )
            {
                start_time = (ps_timestamp) atoll(optarg);
            }
            else
            {
                printf( "'S' option expects a valid absolute start time (must be positive integer)\n" );
                return;
            }
        }
    }

    // get 'ps_rnr_msg' type
    if( (ret = psync_message_get_type_by_name( node_ref, "ps_rnr_msg", &rnr_msg_type )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_get_type_by_name returned: %d\n", __LINE__, ret );
        return;
    }

    // get instance of RnR message
    if( (ret = psync_message_alloc( node_ref, rnr_msg_type, &msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_alloc returned: %d\n", __LINE__, ret );
        return;
    }

    // cast
    rnr_msg = (ps_rnr_msg*) msg;

    // check for quit
    if( quit_session != 0 )
    {
        psync_log_message( LOG_LEVEL_INFO, "sending quit logfile session command" );

        // send command
        if( (ret = psync_rnr_fill_logfile_set_mode( node_ref, LOGFILE_MODE_OFF, PSYNC_RNR_SESSION_ID_INVALID, rnr_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_rnr_fill_logfile_set_mode returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // publish command
        if( (ret = psync_message_publish( node_ref, msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_publish returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }
    }
    else if( record_enabled != 0 )
    {
        // check session ID
        if( session_id == PSYNC_RNR_SESSION_ID_INVALID )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : invalid session identifier, expects a positive integer", __LINE__ );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // send command to quit first
        if( (ret = psync_rnr_fill_logfile_set_mode( node_ref, LOGFILE_MODE_OFF, PSYNC_RNR_SESSION_ID_INVALID, rnr_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_rnr_fill_logfile_set_mode returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // publish command
        if( (ret = psync_message_publish( node_ref, msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_publish returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // wait a little
        (void) psync_sleep_micro( 500000 );

        // log
        psync_log_message( LOG_LEVEL_INFO, "sending enable record mode command - session ID: %llu - start time: %llu", session_id, start_time );

        // send command
        if( (ret = psync_rnr_fill_logfile_set_mode( node_ref, LOGFILE_MODE_WRITE, session_id, rnr_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_rnr_fill_logfile_set_mode returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // publish command
        if( (ret = psync_message_publish( node_ref, msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_publish returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // wait a little
        (void) psync_sleep_micro( 500000 );

        // send command to enable state
        if( (ret = psync_rnr_fill_logfile_set_state( node_ref, LOGFILE_STATE_ENABLED, start_time, rnr_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_rnr_fill_logfile_set_state returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // publish command
        if( (ret = psync_message_publish( node_ref, msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_publish returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }
    }
    else
    {
        // default to send replay command

        // check session ID
        if( session_id == PSYNC_RNR_SESSION_ID_INVALID )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : invalid session identifier, expects a positive integer", __LINE__ );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // send command to quit first
        if( (ret = psync_rnr_fill_logfile_set_mode( node_ref, LOGFILE_MODE_OFF, PSYNC_RNR_SESSION_ID_INVALID, rnr_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_rnr_fill_logfile_set_mode returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // publish command
        if( (ret = psync_message_publish( node_ref, msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_publish returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // wait a little
        (void) psync_sleep_micro( 500000 );

        // log
        psync_log_message( LOG_LEVEL_INFO, "sending enable replay mode command - session ID: %llu - start time: %llu", session_id, start_time );

        // send command
        if( (ret = psync_rnr_fill_logfile_set_mode( node_ref, LOGFILE_MODE_READ, session_id, rnr_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_rnr_fill_logfile_set_mode returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // publish command
        if( (ret = psync_message_publish( node_ref, msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_publish returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // wait a little
        (void) psync_sleep_micro( 500000 );

        // send command to enable state
        if( (ret = psync_rnr_fill_logfile_set_state( node_ref, LOGFILE_STATE_ENABLED, start_time, rnr_msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_rnr_fill_logfile_set_state returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }

        // publish command
        if( (ret = psync_message_publish( node_ref, msg )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_publish returned: %d\n", __LINE__, ret );
            (void) psync_message_free( node_ref, &msg );
            return;
        }
    }


    // release message instance
    (void) psync_message_free( node_ref, &msg );
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

    // getopt return
    int optret = 0;


    // look for 'h' first, we don't need to initialize a node if we're just showing help message
    while( (optret = getopt( argc, argv, GETOPT_STRING )) != -1 )
    {
        // check for option character
        if( optret == 'h' )
        {
            // show notes
            printf( "\nIt is recommended to use a relative start time of at least 5,000,000 (5 seconds)\n"
                    "to allow all nodes to handle the commands without looking for their status messages.\n" );

            //  show usage
            printf( "\nusage\n $%s [options]\n", NODE_NAME );
            printf( " $%s -h (options/help)\n", NODE_NAME );
            printf( "\n" );

            // show options help
            printf( "options\n" );
            printf( "(default: send command to start replay logfile session)\n" );
            printf( " -q\t\tsend command to release (quit) logfile session\n" );
            printf( " -w\t\tsend command to start a record (instead of replay) logfile session\n" );
            printf( " -t <N>\t\tset the session identifier to use\n" );
            printf( " -s <N>\t\tset relative start time [microseconds]\n" );
            printf( " -S <N>\t\tset absolute UTC start time, takes precedence over relative [microseconds]\n" );

            // done
            return 0;
        }
        else if( optret == GETOP_NOARG_FLAG )
        {
            printf( "invalid usage, check help\n" );
            return EXIT_FAILURE;
        }
    }

	// init core API
    if( (ret = psync_init( NODE_NAME, PSYNC_NODE_TYPE_API_USER, PSYNC_DEFAULT_DOMAIN, PSYNC_SDF_KEY_INVALID, PSYNC_INIT_FLAG_STDOUT_LOGGING, &node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // nodes typically should shutdown after handling SIGINT
    // hook up the control-c signal handler, sets exit_signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    // process arguments
    handle_arguments( node_ref, argc, argv );

	// release core API
    if( (ret = psync_release( &node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }


	return EXIT_SUCCESS;
}
