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

// API headers
#include "polysync_core.h"
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




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // polysync node name
    const char *node_name = "rnr-control";

    // node reference
    ps_node_ref node_ref = PSYNC_NODE_REF_INVALID;


	// init core API
    if( (ret = psync_init( node_name, PSYNC_NODE_TYPE_API_USER, PSYNC_DEFAULT_DOMAIN, PSYNC_SDF_KEY_INVALID, 0, &node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        goto EXIT_JUMP;
    }

    // nodes typically should shutdown after handling SIGINT

    // hook up the control-c signal handler, sets exit_signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );





    // using 'goto' to allow for an easy example exit
EXIT_JUMP:
    global_exit_signal = 1;

	// release core API
    if( (ret = psync_release( &node_ref, 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }


	return 0;
}
