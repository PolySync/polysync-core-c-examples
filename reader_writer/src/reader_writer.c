/**
 * \example reader_writer.c
 *
 * Reader/Writer Example.
 *
 * Shows how to use the message API to do publish/subscribe logic.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>

// API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_node.h"
#include "polysync_message.h"




// *****************************************************
// static global data
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
static const char NODE_NAME[] = "reader-writer";




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
 * @brief Publish a get-all parameters command message.
 *
 * @param [in] node_ref Node reference used by the example.
 * @param [in] msg Parameters message to populate and publish.
 *
 */
static void send_get_all_parameters_command( ps_node_ref node_ref, ps_parameters_msg * const msg );


/**
 * @brief Message "ps_parameters_msg" handler for parameter get/set command responses.
 *
 * Prints the response information.
 *
 * @param [in] msg_type Message type identifier for the message, as seen by the data model.
 * @param [in] message Message reference to be handled by the function.
 * @param [in] user_data A pointer to void which specifies the internal data.
 *
 */
static void ps_parameters_msg__handler( const ps_msg_type msg_type, const ps_msg_ref const message, void * const user_data );




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
static void send_get_all_parameters_command( ps_node_ref node_ref, ps_parameters_msg * const msg )
{
    if( (node_ref == PSYNC_NODE_REF_INVALID) || (msg == NULL) )
    {
        return;
    }

    // local vars
    ps_parameter parameter;


    // zero
    memset( &parameter, 0, sizeof(parameter) );

    // invalid destination GUID will be handled by all nodes
    msg->dest_guid = PSYNC_GUID_INVALID;

    // set ID to special get-all ID
    parameter.id = PSYNC_PARAM_ID_ALL;

    // point message sequence buffer to the local variable since we just doing a get all
    msg->parameters._buffer = &parameter;
    msg->parameters._length = 1;
    msg->parameters._maximum = 1;

    // set type to get-all
    msg->type = PARAMETER_MESSAGE_GET_ALL;

    // update timestamp
    (void) psync_get_timestamp( &msg->header.timestamp );

    // publish
    (void) psync_message_publish( node_ref, msg );

    // de-ref our pointer to local buffer element
    memset( &msg->parameters, 0, sizeof(msg->parameters) );
}


//
static void ps_parameters_msg__handler( const ps_msg_type msg_type, const ps_msg_ref const message, void * const user_data )
{
    // index
    unsigned long idx = 0;

    // cast reference
    const ps_parameters_msg *msg = (ps_parameters_msg*) message;


    // only handle responses from other nodes
    if( msg->type != PARAMETER_MESSAGE_RESPONSE )
    {
        return;
    }

    printf( "parameter response - source node GUID: 0x%llX - timestamp: %llu\n", msg->header.src_guid, msg->header.timestamp );

    // node does not support any parameters
    if( msg->parameters._length == 0 )
    {
        printf( "    no parameters supported\n" );
    }

    // for each parameter in the response
    for( idx = 0; idx < msg->parameters._length; idx++ )
    {
        // get convenience ref
        const ps_parameter * const param = &msg->parameters._buffer[ idx ];

        printf( "- [%lu]\n", idx );
        printf( "    ID: 0x%llX (%llu)\n", (unsigned long long) param->id, (unsigned long long) param->id );
        printf( "    description: '%s'\n", param->description._buffer );
        printf( "    value type: %u\n", (unsigned int) param->value._d );
        printf( "    flags: 0x%X", (unsigned int) param->flags );

        // format flag bit strings if set
        if( param->flags != 0 )
        {
            // check bits
            if( param->flags & PSYNC_PARAMETER_FLAG_DISABLED )
            {
                printf( ", DISABLED" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_USERDEFINED )
            {
                printf( ", USER-DEFINED" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_ANALYTIC )
            {
                printf( ", ANALYTIC" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_READONLY_VALUE )
            {
                printf( ", READ-ONLY VALUE" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_READONLY_MIN )
            {
                printf( ", READ-ONLY MIN" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_READONLY_MAX )
            {
                printf( ", READ-ONLY MAX" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_READONLY_STEP )
            {
                printf( ", READ-ONLY STEP" );
            }

            printf( "\n" );
        }
        else
        {
            printf( "\n" );
        }
    }

    printf( "\n" );
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

    // 'ps_parameters_msg' message type
    ps_msg_type parameters_msg_type = PSYNC_MSG_TYPE_INVALID;

    // 'ps_parameters_msg' message
    ps_msg_ref parameters_msg = PSYNC_MSG_REF_INVALID;


	// init core API
    if( (ret = psync_init( NODE_NAME, PSYNC_NODE_TYPE_API_USER, PSYNC_DEFAULT_DOMAIN, PSYNC_SDF_KEY_INVALID, PSYNC_INIT_FLAG_STDOUT_LOGGING, &node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // nodes typically should shutdown after handling SIGINT
    // hook up the control-c signal handler, sets exit signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    // get parameters message type
    if( (ret = psync_message_get_type_by_name( node_ref, "ps_parameters_msg", &parameters_msg_type )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_get_type_by_name - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get parameters message
    if( (ret = psync_message_alloc( node_ref, parameters_msg_type, &parameters_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_alloc - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set default subscriber/listener QoS for parameter message types to reliable
    if( (ret = psync_set_subscriber_reliability_qos( node_ref, parameters_msg_type, RELIABILITY_QOS_RELIABLE )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_set_subscriber_reliability_qos - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // register listener for 'ps_parameters_msg' type
    if( (ret = psync_message_register_listener( node_ref, parameters_msg_type, ps_parameters_msg__handler, NULL )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_register_listener - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // send a get-all parameters command
    send_get_all_parameters_command( node_ref, parameters_msg );

    // main event loop
    // loop until signaled (control-c)
    while( global_exit_signal == 0 )
    {
        // wait a little
        psync_sleep_micro( 1000 );
    }


    // using 'goto' to allow for an easy example exit
    GRACEFUL_EXIT_STMNT:
    global_exit_signal = 1;

    // release parameters message
    if( (ret = psync_message_free( node_ref, &parameters_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_free - ret: %d", ret );
    }

	// release core API
    if( (ret = psync_release( &node_ref, 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }


	return EXIT_SUCCESS;
}
