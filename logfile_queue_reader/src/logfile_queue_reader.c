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
 * \example logfile_queue_reader.c
 *
 * Shows how to use the Logfile API routines to replay a PolySync logfile using the
 * message queue instead of a subscriber.
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
static const char NODE_NAME[] = "polysync-logfile-queue-reader-c";


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
 * Sets global \ref global_exit_signal flag to a non-zero value on SIGINT.
 *
 * @param [in] sig Signal being handled.
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

    // 'ps_byte_array_msg' type identifier
    ps_msg_type byte_array_msg_type = PSYNC_MSG_TYPE_INVALID;

    // pointer to the logfile reader message queue
    GAsyncQueue *replay_queue = NULL;


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

    // disable logfile replay on byte array message types since we want to only have it delivered to our queue
    ps_msg_type reader_filter_list[1] = { byte_array_msg_type };
    if( (ret = psync_logfile_set_message_type_filters( node_ref, NULL, 0, reader_filter_list, 1 )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_set_message_type_filters - ret: %d",
                ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // enable the replay message queue
    if( (ret = psync_logfile_enable_output_queue(
            node_ref,
            1 )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_enable_output_queue - ret: %d",
                ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get a reference to the reader queue
    if( (ret = psync_logfile_get_replay_msg_queue(
            node_ref,
            &replay_queue )) != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_get_replay_msg_queue - ret: %d",
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
        // if replay queue is valid
        if( replay_queue != NULL )
        {
            // check for a message
            ps_msg_ref msg = g_async_queue_try_pop( replay_queue );

            if( msg != PSYNC_MSG_REF_INVALID )
            {
                // get message type
                ps_msg_type msg_type = PSYNC_MSG_TYPE_INVALID;

                (void) psync_message_get_type( msg, &msg_type );

                // check if the type is a byte array message
                if( msg_type == byte_array_msg_type )
                {
                    // cast
                    const ps_byte_array_msg *byte_array_msg = (ps_byte_array_msg*) msg;

                    printf( "received 'ps_byte_array_msg' - %lu - num_bytes: %lu\n",
                            (unsigned long) byte_array_msg->data_type,
                            (unsigned long) byte_array_msg->bytes._length );
                }

                (void) psync_message_free( node_ref, &msg );
            }
            else
            {
                // wait a little and try again
                psync_sleep_micro( 1 );
            }
        }
        else
        {
            global_exit_signal = 1;
        }
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
