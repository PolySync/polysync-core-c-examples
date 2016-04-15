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
#include "polysync_node.h"
#include "polysync_sdf.h"
#include "polysync_rnr.h"




// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Standard options string.
 *
 */
static const char GETOPT_STRING[] = "hqwt:s:S:eE";


/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "polysync-rnr-control-c";




// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief Message "ps_rnr_sessions_msg" handler.
 *
 * Prints out the RnR sessions.
 *
 * @param [in] msg_type Message type identifier for the message, as seen by the data model.
 * @param [in] message Message reference to be handled by the function.
 * @param [in] user_data A pointer to void which specifies a user data pointer.
 *
 */
static void ps_rnr_sessions_msg__handler( const ps_msg_type msg_type, const ps_msg_ref const message, void * const user_data );


/**
 * @brief Command line argument handler.
 *
 */
static void handle_arguments( ps_node_ref const node_ref, int argc, char **argv );




// *****************************************************
// static definitions
// *****************************************************

//
static void print_rnr_sessions(
        const ps_rnr_session * const sessions,
        const unsigned long num_sessions )
{
    // local vars
    unsigned long idx = 0;

    printf( "number of sessions: %lu\n", num_sessions );
    for( idx = 0; idx < num_sessions; idx += 1 )
    {
        const unsigned long num_files = sessions[idx].logfile_attributes._length;

        printf( " [%lu] session ID: %llu - files: %lu\n",
                idx,
                sessions[idx].id,
                num_files );

        unsigned long jdx = 0;
        for( jdx = 0 ; jdx < num_files; jdx += 1 )
        {
            const ps_rnr_logfile_attributes * const file_attribs = &sessions[idx].logfile_attributes._buffer[jdx];

            printf( "    name '%s'\n", file_attribs->filename._buffer );
            printf( "    - file_size %lu bytes\n", (unsigned long) file_attribs->file_size );
            printf( "    - duration %.4f seconds\n", (double) file_attribs->duration / 1000000.0 );
        }
    }
}


//
static void ps_rnr_sessions_msg__handler( const ps_msg_type msg_type, const ps_msg_ref const message, void * const user_data )
{
    char string[256];


    // cast
    const ps_rnr_sessions_msg * const msg = (ps_rnr_sessions_msg*) message;

    // convert IP address to string
    (void) psync_interface_address_value_to_string(
            msg->host_address,
            string,
            sizeof(string) );

    psync_log_message( LOG_LEVEL_INFO, "received RnR sessions message from node 0x%016X (IP '%s')",
            msg->header.src_guid,
            string );

    print_rnr_sessions( msg->sessions._buffer, msg->sessions._length );
}


//
static void handle_arguments( ps_node_ref const node_ref, int argc, char **argv )
{
    // local vars
    int ret = DTC_NONE;
    int optret = 0;
    ps_msg_type rnr_msg_type = PSYNC_MSG_TYPE_INVALID;
    ps_msg_type rnr_sessions_msg_type = PSYNC_MSG_TYPE_INVALID;
    ps_msg_ref msg = PSYNC_MSG_REF_INVALID;
    ps_rnr_msg *rnr_msg = NULL;

    // flags from command line arguments
    unsigned int record_enabled = 0;
    ps_rnr_session_id session_id = PSYNC_RNR_SESSION_ID_INVALID;
    ps_timestamp start_time = 0;
    unsigned int quit_session = 0;
    unsigned int enumerate_all_sessions = 0;
    unsigned int enumerate_local_sessions = 0;


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
        else if( optret == 'e' )
        {
            enumerate_all_sessions = 1;
        }
        else if( optret == 'E' )
        {
            enumerate_local_sessions = 1;
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

    // get 'ps_rnr_sessions_msg' type
    ret = psync_message_get_type_by_name(
            node_ref,
            "ps_rnr_sessions_msg",
            &rnr_sessions_msg_type );

    // error check
    if( ret != DTC_NONE )
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "%s : (%u) -- psync_message_get_type_by_name returned DTC %d",
            __FILE__,
            __LINE__,
            ret );

        return;
    }

    // get 'ps_rnr_msg' type
    ret = psync_message_get_type_by_name(
            node_ref,
            "ps_rnr_msg",
            &rnr_msg_type );

    // error check
    if( ret != DTC_NONE )
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "%s : (%u) -- psync_message_get_type_by_name returned DTC %d",
            __FILE__,
            __LINE__,
            ret );

        return;
    }

    // get instance of RnR message
    ret = psync_message_alloc( node_ref, rnr_msg_type, &msg );

    // error check
    if( ret != DTC_NONE )
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "%s : (%u) -- psync_message_alloc returned DTC %d",
            __FILE__,
            __LINE__,
            ret );

        return;
    }

    // cast
    rnr_msg = (ps_rnr_msg*) msg;

    // check for quit
    if( quit_session != 0 )
    {
        psync_log_message( LOG_LEVEL_INFO, "sending quit logfile session command" );

        // fill command
        ret = psync_rnr_fill_logfile_set_mode(
                node_ref,
                LOGFILE_MODE_OFF,
                PSYNC_RNR_SESSION_ID_INVALID,
                rnr_msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_rnr_fill_logfile_set_mode returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // publish command
        ret = psync_message_publish( node_ref, msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }
    }
    else if( enumerate_all_sessions != 0 )
    {
        psync_log_message( LOG_LEVEL_INFO, "sending enumerate RnR sessions command" );

        // set QoS to reliable for our response message subscriber
        ret = psync_node_set_subscriber_reliability_qos(
                node_ref,
                rnr_sessions_msg_type,
                RELIABILITY_QOS_RELIABLE );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_node_set_subscriber_reliability_qos returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // register subscriber for the response message type
        ret = psync_message_register_listener(
                node_ref,
                rnr_sessions_msg_type,
                ps_rnr_sessions_msg__handler,
                NULL );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_register_listener returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // set command type
        rnr_msg->type = RNR_MSG_GET_RNR_SESSIONS;

        // publish command
        ret = psync_message_publish( node_ref, msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // wait a little for responses
        (void) psync_sleep_micro( 3000000 );
    }
    else if( enumerate_local_sessions != 0 )
    {
        psync_log_message( LOG_LEVEL_INFO, "using RnR API to enumerate local RnR sessions" );

        DDS_sequence_ps_rnr_session sessions;
        memset( &sessions, 0, sizeof(sessions) );

        // session enumeration can optionally provide more detailed information
        // such as start and end times, duration, record count, average size, etc.
        const unsigned int calculate_opt = 1;

        // enumerate RnR sessions on this host
        ret = psync_rnr_enumerate_sessions( node_ref, calculate_opt, &sessions );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_rnr_enumerate_sessions returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        print_rnr_sessions( sessions._buffer, sessions._length );
    }
    else if( record_enabled != 0 )
    {
        // check session ID
        if( session_id == PSYNC_RNR_SESSION_ID_INVALID )
        {
            psync_log_message(
                    LOG_LEVEL_ERROR,
                    "%s : (%u) -- invalid session identifier, expects a positive integer",
                    __FILE__,
                    __LINE__ );

            return;
        }

        // send command to quit first
        ret = psync_rnr_fill_logfile_set_mode(
                node_ref,
                LOGFILE_MODE_OFF,
                PSYNC_RNR_SESSION_ID_INVALID,
                rnr_msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_rnr_fill_logfile_set_mode returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // publish command
        ret = psync_message_publish( node_ref, msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // wait a little
        (void) psync_sleep_micro( 500000 );

        // log
        psync_log_message(
                LOG_LEVEL_INFO,
                "sending enable record mode command - session ID: %llu - start time: %llu",
                session_id,
                start_time );

        // send command to enter record mode
        ret = psync_rnr_fill_logfile_set_mode(
                node_ref,
                LOGFILE_MODE_WRITE,
                session_id,
                rnr_msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_rnr_fill_logfile_set_state returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // publish command
        ret = psync_message_publish( node_ref, msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // wait a little
        (void) psync_sleep_micro( 500000 );

        // send command to enable state
        ret = psync_rnr_fill_logfile_set_state(
                node_ref,
                LOGFILE_STATE_ENABLED,
                start_time,
                rnr_msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_rnr_fill_logfile_set_state returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // publish command
        ret = psync_message_publish( node_ref, msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }
    }
    else
    {
        // default to send replay command

        // check session ID
        if( session_id == PSYNC_RNR_SESSION_ID_INVALID )
        {
            psync_log_message(
                    LOG_LEVEL_ERROR,
                    "%s : (%u) -- invalid session identifier, expects a positive integer",
                    __FILE__,
                    __LINE__ );

            return;
        }

        // send command to quit first
        ret = psync_rnr_fill_logfile_set_mode(
                node_ref,
                LOGFILE_MODE_OFF,
                PSYNC_RNR_SESSION_ID_INVALID,
                rnr_msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_rnr_fill_logfile_set_mode returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // publish command
        ret = psync_message_publish( node_ref, msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // wait a little
        (void) psync_sleep_micro( 500000 );

        // log
        psync_log_message(
                LOG_LEVEL_INFO,
                "sending enable replay mode command - session ID: %llu - start time: %llu",
                session_id,
                start_time );

        // send command
        ret = psync_rnr_fill_logfile_set_mode(
                node_ref,
                LOGFILE_MODE_READ,
                session_id,
                rnr_msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_rnr_fill_logfile_set_mode returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // publish command
        ret = psync_message_publish( node_ref, msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // wait a little
        (void) psync_sleep_micro( 500000 );

        // send command to enable state
        ret = psync_rnr_fill_logfile_set_state(
                node_ref,
                LOGFILE_STATE_ENABLED,
                start_time,
                rnr_msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_rnr_fill_logfile_set_state returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

            return;
        }

        // publish command
        ret = psync_message_publish( node_ref, msg );

        // error check
        if( ret != DTC_NONE )
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

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

    // no-arguments
    const int GETOP_NOARG_FLAG = (int) '?';


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
            printf( " -e\t\tsend command to enumerate all available RnR sessions\n" );
            printf( " -E\t\tdisplay the local RnR sessions using the RnR API\n" );
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
    ret = psync_init(
            NODE_NAME,
            PSYNC_NODE_TYPE_API_USER,
            PSYNC_DEFAULT_DOMAIN,
            PSYNC_SDF_ID_INVALID,
            PSYNC_INIT_FLAG_STDOUT_LOGGING,
            &node_ref );

    // error check
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_init returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        return EXIT_FAILURE;
    }

    // process arguments
    handle_arguments( node_ref, argc, argv );

	// release core API
    ret = psync_release( &node_ref );

    // error check
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_release returned DTC %d",
                __FILE__,
                __LINE__,
                ret );
    }


	return EXIT_SUCCESS;
}
