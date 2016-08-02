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
 * \example enumerate_dynamic_drivers.c
 *
 * Shows how to use a command message to enumerate all dynamic driver nodes.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// API headers
#include "polysync_core.h"
#include "polysync_node.h"
#include "polysync_sdf.h"
#include "polysync_message.h"
#include "polysync_node_template.h"
#include "polysync_dynamic_driver_commands.h"




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
static const char NODE_NAME[] = "polysync-enumerate-dynamic-drivers";


/**
 * @brief Command message type name.
 *
 */
static const char COMMAND_MSG_NAME[] = "ps_command_msg";


/**
 * @brief Response message type name.
 *
 */
static const char RESPONSE_MSG_NAME[] = "ps_response_msg";




// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief Signal handler.
 *
 * @param [in] signal Signal value to handle.
 *
 */
static void sig_handler( int signal );


/**
 * @brief Send a basic command.
 *
 * Basic commands don't use the data section of the message.
 *
 * @param [in] node_ref Node reference used to publish the message.
 * @param [in] cmd Command identifier to publish.
 * @param [in] dest_guid Destination node GUID to target. Value \ref PSYNC_GUID_INVALID is allowed.
 * @param [out] cmd_msg A pointer to \ref ps_command_msg which receives the data before being published.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int send_basic_command(
        ps_node_ref node_ref,
        const ps_command_id cmd,
        const ps_guid dest_guid,
        ps_command_msg * const cmd_msg );


/**
 * @brief Response message handler.
 *
 * Prints all received response messages to stdout.
 *
 * @param [in] msg_type Message type received.
 * @param [in] message Reference to the received message.
 * @param [in] user_data Our node reference.
 *
 */
static void ps_response_msg_handler(
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
static int send_basic_command(
        ps_node_ref node_ref,
        const ps_command_id cmd,
        const ps_guid dest_guid,
        ps_command_msg * const cmd_msg )
{
    int ret = DTC_NONE;
    unsigned long dest_sdf_id = 0;
    ps_node_type dest_node_type = 0;
    ps_identifier dest_node_id = 0;


    // break down the destination GUID
    (void) psync_guid_get_sdf_id( dest_guid, &dest_sdf_id );
    (void) psync_guid_get_node_type( dest_guid, &dest_node_type );
    (void) psync_guid_get_node_id( dest_guid, &dest_node_id );

    printf( "\n" );

    printf( "sending command\n" );

    printf( "  ID: %llu\n",
            (unsigned long long) cmd );

    printf( "  dest_guid: 0x%016llX (%llu)\n",
            (unsigned long long) dest_guid,
            (unsigned long long) dest_guid );

    printf( "    .node_type: %lu\n",
            (unsigned long) dest_node_type );

    printf( "    .sdf_id: %lu\n", dest_sdf_id );

    printf( "    .node_id: %lu\n",
            (unsigned long) dest_node_id );

    printf( "\n" );

    // set command ID and destination GUID
    cmd_msg->dest_guid = dest_guid;
    cmd_msg->id = cmd;

    // set Tx and command timestamp
    ret = psync_get_timestamp( &cmd_msg->header.timestamp );
    cmd_msg->timestamp = cmd_msg->header.timestamp;

    // publish
    if( ret == DTC_NONE )
    {
        ret = psync_message_publish(
                node_ref,
                (ps_msg_ref) cmd_msg );
    }


    return ret;
}


//
static void ps_response_msg_handler(
        const ps_msg_type msg_type,
        const ps_msg_ref const message,
        void * const user_data )
{
    if( user_data == NULL )
    {
        return;
    }

    ps_node_ref node_ref = (ps_node_ref) user_data;
    ps_guid my_guid = 0;
    unsigned long src_sdf_id = 0;
    ps_node_type src_node_type = 0;
    ps_identifier src_node_id = 0;

    const ps_response_msg * const rsp_msg = (ps_response_msg*) message;


    // get our node's GUID
    (void) psync_node_get_guid( node_ref, &my_guid );

    // ignore our own messages
    if( my_guid == rsp_msg->header.src_guid )
    {
        return;
    }

    // break down the source GUID
    (void) psync_guid_get_sdf_id( rsp_msg->header.src_guid, &src_sdf_id );
    (void) psync_guid_get_node_type( rsp_msg->header.src_guid, &src_node_type );
    (void) psync_guid_get_node_id( rsp_msg->header.src_guid, &src_node_id );

    printf( "received ps_response_msg - header.timestamp: %llu\n",
            (unsigned long long) rsp_msg->header.timestamp );

    printf( "  command ID: %llu\n",
            (unsigned long long) rsp_msg->id );

    printf( "  DTC: %llu\n",
            (unsigned long long) rsp_msg->dtc );

    printf( "  src_guid: 0x%016llX (%llu)\n",
            (unsigned long long) rsp_msg->header.src_guid,
            (unsigned long long) rsp_msg->header.src_guid );

    printf( "    .node_type: %lu\n",
            (unsigned long) src_node_type );

    printf( "    .sdf_id: %lu\n", src_sdf_id );

    printf( "    .node_id: %lu\n",
            (unsigned long) src_node_id );

    printf( "\n" );
}




// *****************************************************
// public definitions
// *****************************************************

//
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // node reference
    ps_node_ref node_ref = PSYNC_NODE_REF_INVALID;

    // message types for command/response messages
    ps_msg_type cmd_msg_type = PSYNC_MSG_TYPE_INVALID;
    ps_msg_type rsp_msg_type = PSYNC_MSG_TYPE_INVALID;

    // command message
    ps_msg_ref cmd_msg = PSYNC_MSG_REF_INVALID;


    // init core API
    ret = psync_init(
            NODE_NAME,
            PSYNC_NODE_TYPE_API_USER,
            PSYNC_DEFAULT_DOMAIN,
            PSYNC_SDF_ID_INVALID,
            PSYNC_INIT_FLAG_STDOUT_LOGGING,
            &node_ref );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_init - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // nodes typically should shutdown after handling SIGINT
    // hook up the control-c signal handler, sets exit signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    // get command message type
    ret = psync_message_get_type_by_name(
            node_ref,
            COMMAND_MSG_NAME,
            &cmd_msg_type );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_message_get_type_by_name - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // get response message type
    ret = psync_message_get_type_by_name(
            node_ref,
            RESPONSE_MSG_NAME,
            &rsp_msg_type );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_message_get_type_by_name - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // allocate command message
    ret = psync_message_alloc(
            node_ref,
            cmd_msg_type,
            &cmd_msg );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_message_alloc - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // set response message subscriber QoS to reliable
    ret = psync_node_set_subscriber_reliability_qos(
            node_ref,
            rsp_msg_type,
            RELIABILITY_QOS_RELIABLE );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_node_set_subscriber_reliability_qos - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // register a listener for response messages
    ret = psync_message_register_listener(
            node_ref,
            rsp_msg_type,
            &ps_response_msg_handler,
            node_ref );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_message_register_listener - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // construct command to enumerate all dynamic driver interfaces
    ps_guid dest_guid = PSYNC_GUID_INVALID;

    // all nodes will receive the command message
    (void) psync_guid_set_node_type( PSYNC_NODE_TYPE_ALL, &dest_guid );

    // send command to enumerate all dynamic driver nodes
    ret = send_basic_command(
            node_ref,
            COMMAND_ID_ENUMERATE_DYNAMIC_DRIVER_INTERFACE,
            dest_guid,
            (ps_command_msg*) cmd_msg );

    // wait for cntrl-c to stop listening for response messages
    while( global_exit_signal == 0 )
    {
        // wait a little
        psync_sleep_micro( 10000 );
    }

    // free the command message
    ret = psync_message_free(
            node_ref,
            &cmd_msg );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_message_free - ret: %d", ret );
    }

    // release core API
    ret = psync_release( &node_ref );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_release - ret: %d", ret );
    }


    return EXIT_SUCCESS;
}
