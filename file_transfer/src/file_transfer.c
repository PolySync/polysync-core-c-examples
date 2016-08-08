/*
 * Copyright (c) 2016 HARBRICK TECHNOLOGIES, INC
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
 * \example file_transfer.c
 *
 * File Transfer Example.
 *
 * Demonstrates how to use the PolySync file transfer API to initiate and
 * support file transfer operations between nodes.
 *
 */



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>

// PolySync API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_message.h"
#include "polysync_node.h"

// Header for PolySync File Transfer API,
// which is the topic of this example.
#include "polysync_file_transfer.h"


// All polysync-manager nodes support file transfer operations.
// This example will attempt a file transfer with the first polysync-manager
// it can find. This semaphore is used to signal when the first polysync-manager
// has responded, and its GUID has been captured.
static sem_t manager_response;


// After a polysync-manager node has been requested to provide its status,
// it will respond. This is the name of the type of that message.
static const char manager_status_msg_name[] = "ps_response_msg";


// When polysync-manager responds with its status,
// this function will capture its GUID and signal 
// that the program may proceeed via the semaphore.
static void ps_manager_status_msg_handler(
        const ps_msg_type msg_type,
        const ps_msg_ref const message,
        void * const user_data )
{
    // The pointer to the GUID of the first responding polysync-manager node
    // was passed as the user_data field.
    ps_guid * const manager_guid = (ps_guid * const)user_data;

    // This function is only subscribed to 'ps_manager_status_msg' and
    // so it is safe to cast the message reference to that type.
    const ps_response_msg * const manager_status =
            (ps_response_msg*)message;

    // Set the manager GUID based on the header of the response.
    *manager_guid = manager_status->header.src_guid;

    // Signal to the main thread to resume because
    // the manager GUID has been retrieved.
    sem_post( &manager_response );
}


// This function allocates and publishes a command
// for all polysync-manager nodes to provide their statuses.
static int request_manager_response(
        const ps_node_ref node_ref )
{
    const char command_msg_name[] = "ps_command_msg";
    ps_msg_type command_msg_type = PSYNC_MSG_TYPE_INVALID;

    int ret;

    ps_command_msg * const command_msg;

    if( (ret = psync_message_get_type_by_name(
            node_ref,
            command_msg_name,
            &command_msg_type )) != DTC_NONE )
    {
        printf( "failed to get message type of %s\n", command_msg_name );
        return ret;
    }

    if( (ret = psync_message_alloc(
            node_ref,
            command_msg_type,
            (ps_msg_ref*)&command_msg )) != DTC_NONE )
    {
        printf( "failed to allocate %s\n", command_msg_name );
        return ret;
    }

    // Send command message to any listening node.
    command_msg->dest_guid = PSYNC_GUID_INVALID;

    // Set the command type to get polysync-manager status.
    command_msg->id = PSYNC_COMMAND_GET_MANAGER_STATUS;

    printf( "requesting polysync-manager nodes to respond with status\n" );

    if( (ret = psync_message_publish(
            node_ref,
            (ps_msg_ref)command_msg )) != DTC_NONE )
    {
        printf( "failed to publish %s\n", command_msg_name );
    }

    (void)psync_message_free( node_ref, (ps_msg_ref*)&command_msg );

    return ret;
}


// This function is passed to the file transfer API
// and will be invoked when the file transfer makes progress.
static void print_transfer_progress(
        const ps_file_transfer_state * const transfer_state,
        void * user_data )
{
    printf( "%s:%s\n"
            "%u / %u\n\n",
            transfer_state->transfer_options->source_path,
            transfer_state->transfer_options->destination_path,
            transfer_state->current_chunk_id,
            transfer_state->total_chunks );
}


int main( int argc, char ** argv )
{
    // Used to track the return code of PolySync API calls.
    int ret;

    // Node reference is invalid until later initialized.
    ps_node_ref node_ref = PSYNC_NODE_REF_INVALID;

    if( (ret = psync_init(
            "polysync-file-transfer-c",
            PSYNC_NODE_TYPE_API_USER, // Node is a basic API user.
            PSYNC_DEFAULT_DOMAIN, // Node is in the default domain.
            PSYNC_SDF_ID_INVALID, // Node does not exist in the System Design File.
            PSYNC_INIT_FLAG_STDOUT_LOGGING, // Node uses stdout for logging.
            &node_ref )) != DTC_NONE )
    {
        printf( "file transfer example node failed to initialize\n" );
        return EXIT_FAILURE;
    }

    ps_msg_type manager_status_msg_type = PSYNC_MSG_TYPE_INVALID;

    if( (ret = psync_message_get_type_by_name(
            node_ref,
            manager_status_msg_name,
            &manager_status_msg_type )) != DTC_NONE )
    {
        printf( "failed to get message type of %s\n", manager_status_msg_name );
        return EXIT_FAILURE;
    }

    // This will be set to the GUID of the responding polysync-manager.
    ps_guid manager_guid = PSYNC_GUID_INVALID;
    ps_guid local_guid = PSYNC_GUID_INVALID;
    psync_node_get_guid( node_ref, &local_guid );

    // Initialize the semaphore to wait for
    // the signal that polysync-manager has replied.
    sem_init( &manager_response, 0, 0 );

    // Subscribe to the response from polysync-manager and retrieve its GUID.
    if( (ret = psync_node_set_subscriber_reliability_qos(
            node_ref,
            manager_status_msg_type,
            RELIABILITY_QOS_RELIABLE )) != DTC_NONE )
    {
        printf( "failed to set %s subscriber reliability\n", manager_status_msg_name );
        return EXIT_FAILURE;
    }

    // Register a function to handle polysync-manager's response.
    // A pointer to the pending manager's GUID is passed as data so that
    // the handler function may assign the correct GUID upon receiving a response.
    if( (ret = psync_message_register_listener(
            node_ref,
            manager_status_msg_type,
            &ps_manager_status_msg_handler,
            &manager_guid )) != DTC_NONE )
    {
        printf( "failed to subscribe to %s\n", manager_status_msg_name );
        return EXIT_FAILURE;
    }

    // Publish a request for polysync-manager to provide status.
    if( (ret = request_manager_response(
            node_ref )) != DTC_NONE )
    {
        return EXIT_FAILURE;
    }

    // Wait for polysync-manager to have responded and its GUID captured.
    printf( "Waiting for polysync-manager to respond...\n"
            "If this is taking a while, polysync-manager may not be running.\n"
            "Press Ctrl+c to interrupt and start polysync-manager\n");

    sem_wait( &manager_response );
    sem_destroy( &manager_response );

    // This example may now disregard polysync-manager status messages.
    if( (ret = psync_message_unregister_listener(
            node_ref,
            manager_status_msg_type )) != DTC_NONE )
    {
        printf( "failed to unsubscribe from %s\n", manager_status_msg_name );
        return EXIT_FAILURE;
    }

    // Now the GUID of a node which supports file transfer is known.
    printf( "polysync-manager GUID: 0x%016llX (%llu)\n", manager_guid, manager_guid );

    // A file transfer state contains data needed by file transfer operations
    // and may perform multiple operations simultaneously. It must be initialized
    // before being used.
    ps_file_transfer_handle transfer_handle;

    if( (ret = psync_file_transfer_init(
            &transfer_handle, node_ref )) != DTC_NONE )
    {
        printf( "file transfer state failed to initialize\n" );
        return EXIT_FAILURE;
    }

    if( (ret = psync_file_transfer_register_default_handling(
            transfer_handle )) != DTC_NONE )
    {
        printf( "failed to register default file transfer handling" );
        return EXIT_FAILURE;
    }

    // Transfer options are used to configure file transfer options.
    // These options include required fields such as source and destination
    // file paths, as well as other optional fields.
    ps_file_transfer_options transfer_options;
    memset( &transfer_options, 0, sizeof(transfer_options) );

    // polysync-manager will be the other participant in this file transfer.
    transfer_options.source_guid = manager_guid;
    transfer_options.destination_guid = local_guid;

    // This example will transfer a copy of the PolySync EULA
    // because it is a small file which should be present
    // in the PolySync install.
    char source_file_path[ PSYNC_DEFAULT_STRING_LEN ];
    
    strcpy( source_file_path, getenv( "PSYNC_HOME" ) );
    strcat( source_file_path, "/doc/EULA.txt" );

    strcpy( transfer_options.source_path, source_file_path );
    strcpy( transfer_options.destination_path, "copy_of_license.txt" );

    transfer_options.on_progress = print_transfer_progress;

    printf( "transferring %s from node 0x%016llX (%llu) to %s\n",
            source_file_path,
            manager_guid,
            manager_guid,
            transfer_options.destination_path );

    psync_file_transfer_start(
            transfer_handle,
            &transfer_options );

    printf("Waiting...\n");
    ret = psync_file_transfer_wait_for_all( transfer_handle );
    printf("Unregistering...\n");
    ret = psync_file_transfer_unregister_default_handling( transfer_handle );
    printf("Releasing...\n");
    ret = psync_file_transfer_release( &transfer_handle );
    printf("Node releasing...\n");
    ret = psync_release( &node_ref );

    if( ret != DTC_NONE )
    {
        printf( "file transfer cleanup failed\n" );
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
