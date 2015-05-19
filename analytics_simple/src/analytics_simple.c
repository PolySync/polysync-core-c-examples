/******************************************************
** FILE: analytics_simple.c
**
** ABSTRACT:
**
** DOCUMENTS:
**
** NOTES:
**
*******************************************************/



/**
 * @example analytics_simple.c
 *
 * Example analytics listener.
 * Listens to the first PolySync manager node to respond, prints formatted
 * output to stdout intended for feedgnuplot.
 *
 * feedgnuplot can be obtained from: https://github.com/dkogan/feedgnuplot
 *
 * This node disables stdout logging since it is configurated to output to feedgnuplot.
 * See the machine's syslog (/var/log/syslog) for logging messages.
 */



#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <glib-2.0/glib/gthread.h>
#include <glib-2.0/glib/deprecated/gthread.h>

// API headers
#include "polysync_core.h"
#include "psyncDcps.h"





/**
 * @brief Node data helper structure.
 *
 */
typedef struct
{
    //
    //
    ps_guid         guid; /*!< Node GUID. */
    //
    //
    gint            is_set; /*!< Flag indicating the node has been seen, and guid is set. */
    //
    //
    GMutex          guid_lock; /*!< Node GUID lock. */
} node_data_s;







// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static sig_atomic_t                 exit_signaled                       = 0;






// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief Signal handler.
 *
 */
static void sig_handler( int signal );


/**
 * @brief API on_data handler for \ref ps_parameter_stream_msg messages.
 *
 * @warning Do not modify topic_data.
 */
static void on_psync_data( void *usr_data, ps_msg_type msg_type, void *topic_data );


/**
 * @brief Request analytics parameters.
 *
 *
 *  @return DTC code, DTC_NONE if success.
 */
static int get_analytics_parameters( ps_parameter_stream_msg *msg, ps_guid dest_guid );






// *****************************************************
// static definitions
// *****************************************************


//
static void sig_handler( int signal )
{
	//if( signal == SIGINT )
	exit_signaled = 1;
}


//
static void on_psync_data( void *usr_data, ps_msg_type msg_type, void *topic_data )
{
    unsigned int    idx = 0;

    // get node data reference
    node_data_s *node_data = (node_data_s*) usr_data;

	// cast reference
	ps_parameter_stream_msg *message = (ps_parameter_stream_msg*) topic_data;

    // ignore everything but responses
    if( message->command != PARAMETER_COMMAND_RESPONSE )
    {
        return;
    }

    // check if we are search for a node
    if( g_atomic_int_get( &node_data->is_set ) == 0 )
    {
        // use this node from now on

        psync_log_message( LOG_LEVEL_INFO, "selecting manager node GUID: 0x%llX", message->header.src_guid );

        // lock
        g_mutex_lock( &node_data->guid_lock );

        // update
        node_data->guid = message->header.src_guid;

        // unlock
        g_mutex_unlock( &node_data->guid_lock );

        // set flag
        g_atomic_int_set( &node_data->is_set, 1 );

        // done
        return;
    }

    // first token is timestamp
    // micro to sec
    printf( "%llu ", message->header.timestamp / 1000000ULL );

    // for each parameter in stream
    for( idx = 0; idx < message->parameters._length; idx++ )
    {
        // order is important, should be the same as requested

        // if host CPU load
        if( message->parameters._buffer[ idx ].id == 304 )
            printf( "%f ", message->parameters._buffer[ idx ].value._u.d_value );
        // rx utilization
        else if( message->parameters._buffer[ idx ].id == 310 )
            printf( "%f ", message->parameters._buffer[ idx ].value._u.d_value );
        // tx utilization
        else if( message->parameters._buffer[ idx ].id == 311 )
            printf( "%f ", message->parameters._buffer[ idx ].value._u.d_value );
    }

    printf( "\n" );
    fflush( stdout );
}


//
static int get_analytics_parameters( ps_parameter_stream_msg *msg, ps_guid dest_guid )
{
    // local vars
    int                 ret         = 0;
    const unsigned int  NUM_PARAMS  = 5;


    // set buffer length, number of parameters we want to get
    if( (ret = psync_message_ensure_stream_length( (void*) msg, NUM_PARAMS )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "get_analytics_parameters -- psync_message_ensure_stream_length returned: %d", ret );
        return ret;
    }

    // for each parameter request

    // only want the parameter value
    msg->command = PARAMETER_COMMAND_GET_VALUE;

    // set parameter ID's
    msg->parameters._buffer[ 0 ].id = 304;
    msg->parameters._buffer[ 1 ].id = 308;
    msg->parameters._buffer[ 2 ].id = 309;
    msg->parameters._buffer[ 3 ].id = 310;
    msg->parameters._buffer[ 4 ].id = 311;

    // set destination GUID
    msg->dest_guid = dest_guid;

    // publish request
    if( (ret = psync_message_publish_instance( (void*) msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "get_analytics_parameters -- psync_message_publish_instance returned: %d", ret );
        return ret;
    }


    return DTC_NONE;
}






// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // polysync node name
    const char *node_name = "analytics-simple";

    // flag to enable stdout logs in addition to the normal syslog output
    unsigned int stdout_logging_enabled = 0;

    // command message
    ps_parameter_stream_msg *parameter_cmd_msg = NULL;

    // destination GUID
    ps_guid                 dest_guid = 0;

    // node data
    node_data_s             node_data;


    // init
    node_data.guid = 0;
    g_atomic_int_set( &node_data.is_set, 0 );
    g_mutex_init( &node_data.guid_lock );

	// init core API
    if( (ret = psync_init( PSYNC_NID_API, node_name, stdout_logging_enabled )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // hook up the control-c signal handler, sets exit_signaled flag
	signal( SIGINT, sig_handler );

	// allow signals to interrupt
	siginterrupt( SIGINT, 1 );

    // get an instance
    if( (ret = psync_message_request_instance( MSG_TYPE_PARAMETER_STREAM, (void**) &parameter_cmd_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // register a listener for \ref ps_parameter_stream_msg
    if( (ret = psync_message_register_listener( MSG_TYPE_PARAMETER_STREAM, on_psync_data, (void*) &node_data )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_register_listener returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set destination GUID to all manager nodes
    if( (ret = psync_set_node_type( PSYNC_NID_PM, &dest_guid )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_set_node_type returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get all manager nodes to respond, select the first one to respond
    if( (ret = get_analytics_parameters( parameter_cmd_msg, dest_guid )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- get_analytics_parameters returned: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // reset destination GUID
    dest_guid = 0;

    // main loop
    while( 1 )
    {
        // check for exit
        if( exit_signaled )
        {
            psync_log_message( LOG_LEVEL_INFO, "main -- exiting" );

            // tell the plotter we are exiting
            printf( "exit" );
            fflush( stdout );

            goto GRACEFUL_EXIT_STMNT;
        }

        // if we haven't found a node yet
        if( dest_guid == 0 )
        {
            // look for a manager node, set by callback
            if( g_atomic_int_get( &node_data.is_set ) != 0 )
            {
                // lock
                g_mutex_lock( &node_data.guid_lock );

                // get node GUID
                dest_guid = node_data.guid;

                // unlock
                g_mutex_unlock( &node_data.guid_lock );
            }
        }
        else
        {
            // request analytics, let callback handle them
            if( (ret = get_analytics_parameters( parameter_cmd_msg, dest_guid )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- get_analytics_parameters returned: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }
        }

        // wait 1000 ms
        if( (ret = psync_sleep_micro( 1000000 )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_sleep_micro returned: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }
    }




	// using 'goto' to allow for an easy example exit
GRACEFUL_EXIT_STMNT:


    // release
    g_mutex_free( &node_data.guid_lock );

    // release message
    if( (ret = psync_message_release_instance( (void**) &parameter_cmd_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance returned: %d", ret );
    }

	// release
	if( (ret = psync_release( 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release returned: %d", ret );
    }


	return 0;
}

