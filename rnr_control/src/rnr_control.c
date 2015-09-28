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



    // process arguments




    // disable stdout errors in getopt
    opterr = 0;

    // reset scanner
    optind = 0;



    // for each option in optstring
    while( (optret = getopt( argc, argv, GETOPT_STRING )) != -1 )
    {
        // check for option character
        if( optret == 'h' )
        {

        }




        // check for optchar
//        if( optret == (int) optchar )
//        {
//            // found option
//
//            // check for error
//            if( optret == optret_no_arg )
//            {
//                // option expects an argument but didn't find one
//                return DTC_NOINPUT;
//            }
//
//            // if expected an optarg
//            if( (arg_buffer != NULL) && (arg_buffer_len != 0) )
//            {
//                // check for optarg
//                if( (optarg != NULL) && (strlen(optarg) > 0) )
//                {
//                    // copy
//                    strncpy( arg_buffer, optarg, arg_buffer_len );
//                }
//            }
//
//            // done
//            return DTC_NONE;
//        }
    }




    // get 'ps_rnr_msg' type
    if( (ret = psync_message_get_type_by_name( node_ref, "ps_rnr_msg", &rnr_msg_type )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_get_type_by_name returned: %d\n", __LINE__, ret );
        return;
    }

    // get instance of RnR message
    if( (ret = psync_message_request_instance( node_ref, rnr_msg_type, &msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "(%u) : psync_message_request_instance returned: %d\n", __LINE__, ret );
        return;
    }

    // release message instance
    (void) psync_message_release_instance( node_ref, &msg );
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

            // show options help "hqwt:s:S:";
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
    }

	// init core API
    if( (ret = psync_init( NODE_NAME, PSYNC_NODE_TYPE_API_USER, PSYNC_DEFAULT_DOMAIN, PSYNC_SDF_KEY_INVALID, 0, &node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        return 1;
    }

    // nodes typically should shutdown after handling SIGINT
    // hook up the control-c signal handler, sets exit_signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    // process arguments
    handle_arguments( node_ref, argc, argv );

	// release core API
    if( (ret = psync_release( &node_ref, 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }


	return 0;
}
