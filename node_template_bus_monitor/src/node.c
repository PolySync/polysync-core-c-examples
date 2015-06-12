/******************************************************
** FILE: node.c
**
** ABSTRACT:
**
** DOCUMENTS:
**
** AUTHOR:
** JL
** Harbrick LLC
**
** NOTES:
**
*******************************************************/



/**
 * @example node.c
 *
 * Node Template Source.
 *
 */

#include "node.h"





// *****************************************************
// public global data
// *****************************************************


// PolySync Message string table.
const char               *MESSAGE_TYPE_STRING_TABLE[] =
{
    [MSG_TYPE_BYTE_STREAM]            = PSYNC_BYTE_STREAM_MSG_NAME,
    [MSG_TYPE_DIAGNOSTIC_STATE]       = PSYNC_DIAGNOSTIC_STATE_MSG_NAME,
    [MSG_TYPE_LOGGER_PACKET]          = PSYNC_LOGGER_PACKET_MSG_NAME,
    [MSG_TYPE_CAN_DATA]               = PSYNC_CAN_DATA_MSG_NAME,
    [MSG_TYPE_RADAR_TRACK_STREAM]     = PSYNC_RADAR_TRACK_STREAM_MSG_NAME,
    [MSG_TYPE_OBJECT_STREAM]          = PSYNC_OBJECT_STREAM_MSG_NAME,
    [MSG_TYPE_LIDAR_POINT_STREAM]     = PSYNC_LIDAR_POINT_STREAM_MSG_NAME,
    [MSG_TYPE_IO_SAMPLE_STREAM]       = PSYNC_IO_SAMPLE_STREAM_MSG_NAME,
    [MSG_TYPE_VIDEO_STREAM]           = PSYNC_TOPIC_VIDEO_STREAM_MSG_NAME,
    [MSG_TYPE_PLATFORM_MOTION]        = PSYNC_PLATFORM_MOTION_MSG_NAME,
    [MSG_TYPE_VEHICLE_CONTROL]        = PSYNC_VEHICLE_CONTROL_MSG_NAME,
    [MSG_TYPE_VEHICLE_OBD]            = PSYNC_VEHICLE_OBD_MSG_NAME,
    [MSG_TYPE_LANE_MODEL]             = PSYNC_LANE_MODEL_MSG_NAME,
    [MSG_TYPE_EVENT]                  = PSYNC_EVENT_MSG_NAME,
    [MSG_TYPE_PARAMETER_STREAM]       = PSYNC_PARAMETER_STREAM_MSG_NAME,
    [MSG_TYPE_TERRAIN_DATA]           = PSYNC_TERRAIN_DATA_MSG_NAME,
    [MSG_TYPE_OBSTACLE_STREAM]        = PSYNC_OBSTACLE_STREAM_MSG_NAME,
    [MSG_TYPE_POINT_STREAM]           = PSYNC_POINT_STREAM_MSG_NAME,
    [MSG_TYPE_TRAFFIC_SIGN_STREAM]    = PSYNC_TRAFFIC_SIGN_STREAM_MSG_NAME,
    NULL
};




// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief PolySync message on-data callback.
 *
 * @param usr_data User data pointer.
 * @param msg_type Message type.
 * @param topic_data Message data.
 *
 */
static void psync_message_handler( void *usr_data, ps_msg_type msg_type, void *topic_data );










// *****************************************************
// static definitions
// *****************************************************

//
static void psync_message_handler( void *usr_data, ps_msg_type msg_type, void *topic_data )
{
    // local vars
    int                     ret         = DTC_NONE;

    // storage for new message for the queue
    void                    *message    = NULL;

    // node context reference
    ps_node_context         *node_context = NULL;

    // node data reference
    node_data               *data       = NULL;


    // get context
    node_context = (ps_node_context*) usr_data;

    // sanity check
    if( !node_context )
    {
        psync_log_message( LOG_LEVEL_ERROR, "psync_message_handler -- bad node context reference" );
        return;
    }

    // get node data
    data = (node_data*) node_context->configuration_data.user_data;

    // enabled check, queue valid check
    if( !g_atomic_int_get( &data->received_message_queue_enabled ) || !data->received_message_queue )
    {
        return;
    }

    // get an instance
    if( (ret = psync_message_request_instance( msg_type, &message ) ) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "psync_message_handler -- psync_message_request_instance failed - error_code: %d", ret );
        return;
    }

    // copy message into new message
    if( (ret = psync_message_copy( topic_data, message )) != DTC_NONE)
    {
        psync_log_message( LOG_LEVEL_ERROR, "psync_message_handler -- psync_message_copy failed - error_code: %d", ret );
        return;
    }

    // enqueue
    g_async_queue_push( data->received_message_queue, (gpointer) message );

    // de-ref
    message = NULL;
    node_context = NULL;
}














// *****************************************************
// public definitions
// *****************************************************


// MAIN
int main( int argc, char **argv )
{
    // use PolySync main entry
    return( psync_node_main_entry( argc, argv ) );
}


//
int node_register_message_listeners( ps_node_context *context, unsigned long enabled_message_types )
{
    if( !context )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node_register_message_listeners -- bad arg" );
        return DTC_USAGE;
    }


    // local vars
    int             ret         = DTC_NONE;
    ps_msg_type     msg_type    = 0;
    node_data       *data       = NULL;


    // get node data
    data = (node_data*) context->configuration_data.user_data;

    if( !data )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node_register_message_listeners -- bad node_data" );
        return DTC_USAGE;
    }

    // create queue
    if( !data->received_message_queue )
    {
        if( (data->received_message_queue = g_async_queue_new()) == NULL )
        {
            psync_log_message( LOG_LEVEL_ERROR, "node_register_message_listeners -- g_async_queue_new failed" );
            return ret;
        }
    }

    // register listeners
    for( msg_type = 0; msg_type < PSYNC_MESSAGE_COUNT; msg_type++ )
    {
        // if bit is set, register for that message type
        if( enabled_message_types & (1 << msg_type) )
        {
            psync_log_message( LOG_LEVEL_DEBUG, "node_register_message_listeners -- registering a listener for message: %s", MESSAGE_TYPE_STRING_TABLE[ msg_type ] );

            if( (ret = psync_message_register_listener( msg_type, psync_message_handler, (void*) context )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "node_register_message_listeners -- psync_message_register_listener failed" );
                return ret;
            }
        }
    }

    // enable queues
    g_atomic_int_or( &data->received_message_queue_enabled, 1 );
    if( (ret = psync_set_node_flags_bit( NODE_FLAG_HANDLERS_ACTIVE, 1 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node_register_message_listeners -- psync_set_node_flags_bit failed" );
		return ret;
    }


    return DTC_NONE;
}


//
int node_poll_for_message( ps_node_context *context, void **message_ptr )
{
    if( !context || !message_ptr )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node_poll_for_message -- bad arg" );
        return DTC_USAGE;
    }


    // local vars
    node_data       *data           = NULL;


    // get node data
    data = (node_data*) context->configuration_data.user_data;

    if( !data || !data->received_message_queue )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node_poll_for_message -- bad node_data" );
        return DTC_USAGE;
    }


    // poll for message
    (*message_ptr) = g_async_queue_try_pop( data->received_message_queue );


    return DTC_NONE;
}


//
int node_wait_for_message( ps_node_context *context, void **message_ptr, ps_timestamp timeout )
{
    if( !context || !message_ptr )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node_wait_for_message -- bad arg" );
        return DTC_USAGE;
    }


    // local vars
    node_data       *data           = NULL;


    // get node data
    data = (node_data*) context->configuration_data.user_data;

    if( !data || !data->received_message_queue )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node_wait_for_message -- bad node_data" );
        return DTC_USAGE;
    }

    // poll for message
    (*message_ptr) = g_async_queue_timeout_pop( data->received_message_queue, timeout );


    return DTC_NONE;
}


//
int node_release_user_data( ps_node_configuration_data *configuration )
{
    if( !configuration )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node_release_user_data -- bad arg" );
        return DTC_USAGE;
    }


    // local vars
    int             ret             = 0;
    void            *q_element      = NULL;
    unsigned int    stale_messages  = 0;
    node_data       *data           = NULL;


    // get node data
    data = (node_data*) configuration->user_data;

    // clear flag
    g_atomic_int_and( &data->received_message_queue_enabled, 0 );

    // flush queue
    stale_messages = 0;
    if( data && data->received_message_queue )
    {
        // get messages in queue
        while( (q_element = g_async_queue_try_pop( data->received_message_queue )) )
        {
            stale_messages++;

            // release instance
            if( (ret = psync_message_release_instance( &q_element ) ) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "node_release_user_data -- psync_message_release_instance failed - error_code: %d", ret );
                // try and continue
            }
        }

        // unref
        g_async_queue_unref( data->received_message_queue );
        data->received_message_queue = NULL;

        // inform
        if( stale_messages )
        {
            psync_log_message( LOG_LEVEL_INFO, "node_release_user_data -- flushed %u stale messages in received_message_queue", stale_messages );
        }
    }


    return DTC_NONE;
}


//
int node_create_user_data( ps_node_configuration_data *configuration )
{
    if( !configuration )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node_create_user_data -- bad arg" );
        return DTC_USAGE;
    }


    // local vars
    node_data       *data           = NULL;


    // create node data if needed
    if( !configuration->user_data )
    {
        // create data
        if( !(configuration->user_data = malloc( sizeof(*data) )) )
        {
            return DTC_SOFTWARE ;
        }

        // zero
        memset( configuration->user_data, 0, sizeof(*data) );
    }


    return DTC_NONE;
}
