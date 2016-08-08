/*
 * Copyright (c) 2016 PolySync
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
 * \example logfile_reader.c
 *
 * Shows how to use the Logfile API routines to replay a PolySync logfile.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_node.h"
#include "polysync_message.h"
#include "polysync_logfile.h"




// *****************************************************
// static global types/macros
// *****************************************************


/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static sig_atomic_t global_exit_signal = 0;


/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "polysync-logfile-reader-c";


/**
 * @brief Logfile path we'll use instead of the automatic API-generated name.
 *
 */
static const char LOGFILE_PATH[] = "/tmp/polysync_logfile.plog";




// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief Signal handler.
 *
 */
static void sig_handler( int signal );


/**
 * @brief Message "ps_byte_array_msg" handler.
 *
 * @param [in] msg_type Message type identifier for the message, as seen by the data model.
 * @param [in] message Message reference to be handled by the function.
 * @param [in] user_data A pointer to void which specifies the internal data.
 *
 */
static void ps_byte_array_msg__handler(
        const ps_msg_type msg_type,
        const ps_msg_ref const message,
        void * const user_data );




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
static void ps_byte_array_msg__handler(
        const ps_msg_type msg_type,
        const ps_msg_ref const message,
        void * const user_data )
{
    // cast
    const ps_byte_array_msg * const byte_array_msg = (ps_byte_array_msg*) message;


    printf( "received 'ps_byte_array_msg' - %lu - num_bytes: %lu\n",
            (unsigned long) byte_array_msg->data_type,
            (unsigned long) byte_array_msg->bytes._length );
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

    // 'ps_byte_array_msg' type identifier
    ps_msg_type byte_array_msg_type = PSYNC_MSG_TYPE_INVALID;


    // init core API
    if( (ret = psync_init(
            NODE_NAME,
            PSYNC_NODE_TYPE_API_USER,
            PSYNC_DEFAULT_DOMAIN,
            PSYNC_SDF_ID_INVALID,
            PSYNC_INIT_FLAG_STDOUT_LOGGING,
            &node_ref )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_init - ret: %d",
                ret );
        return EXIT_FAILURE;
    }

    // nodes typically should shutdown after handling SIGINT
    // hook up the control-c signal handler, sets exit signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    // get byte array message type
    if( (ret = psync_message_get_type_by_name(
            node_ref,
            "ps_byte_array_msg",
            &byte_array_msg_type )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_message_get_type_by_name - ret: %d",
                ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // register subscriber for byte array message type
    // it will get called if the logfile being replayed contains that data type
    if( (ret = psync_message_register_listener(
            node_ref,
            byte_array_msg_type,
            ps_byte_array_msg__handler,
            NULL )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_message_register_listener - ret: %d",
                ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // initialize logfile API resources
    if( (ret = psync_logfile_init( node_ref )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_init - ret: %d",
                ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set the logfile path, over rides the default file name logic
    if( (ret = psync_logfile_set_file_path(
            node_ref,
            LOGFILE_PATH )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_set_file_path - ret: %d",
                ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // enable replay/read mode, the example session ID value 1 is not used when a manual file path is set
    if( (ret = psync_logfile_set_mode(
            node_ref,
            LOGFILE_MODE_READ,
            1 )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_set_mode - ret: %d",
                ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // enable the current logfile state - starts replay
    if( (ret = psync_logfile_set_state(
            node_ref,
            LOGFILE_STATE_ENABLED,
            0 )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_set_state - ret: %d",
                ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // main event loop
    // loop until signaled (control-c)
    while( global_exit_signal == 0 )
    {
        // do nothing while the subscriber callback function handles replay data
        psync_sleep_micro( 10000 );
    }

    // using 'goto' to allow for an easy example exit
    GRACEFUL_EXIT_STMNT:
    global_exit_signal = 1;

    // disable current mode
    if( (ret = psync_logfile_set_mode(
            node_ref,
            LOGFILE_MODE_OFF,
            PSYNC_RNR_SESSION_ID_INVALID )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_set_mode - ret: %d",
                ret );
    }

    // release logfile API resources
    if( (ret = psync_logfile_release( node_ref )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_release - ret: %d",
                ret );
    }

	// release core API
    if( (ret = psync_release( &node_ref )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_release - ret: %d",
                ret );
    }


	return EXIT_SUCCESS;
}
