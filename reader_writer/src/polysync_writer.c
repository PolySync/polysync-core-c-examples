/******************************************************
** FILE: polysync_writer.c
**
** ABSTRACT:
**
** DOCUMENTS:
**
** NOTES:
**
*******************************************************/



/**
 * @example polysync_writer.c
 *
 * Example data writer.
 */


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// API headers
#include "polysync_core.h"



// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static sig_atomic_t                 exit_signaled			= 0;


/**
 * @brief Object stream message pointer.
 *
 */
static ps_object_stream_msg         *object_stream_msg      = NULL;


/**
 * @brief Lane model message pointer.
 *
 */
static ps_lane_model_msg            *lane_model_msg         = NULL;


/**
 * @brief Event model message pointer.
 *
 */
static ps_event_msg                 *event_msg              = NULL;


/**
 * @brief Parameter stream message pointer.
 *
 */
static ps_parameter_stream_msg      *parameter_msg          = NULL;




// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief Signal handler.
 *
 */
static void sig_handler( int signal );


/**
 * @brief Publish a message to get all supported parameters from all nodes.
 *
 * See example polysync_reader.c for subscriber/parser for parameter responses.
 *
 */
static int get_all_supported_parameters( ps_parameter_stream_msg *msg );



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
static int get_all_supported_parameters( ps_parameter_stream_msg *msg )
{
    // local vars
    int ret = 0;

    // set buffer length, 1 for "get all" command
    if( (ret = psync_message_ensure_stream_length( (void*) msg, 1 )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "get_all_supported_parameters -- psync_message_ensure_stream_length - ret: %d", ret, ret );
        return ret;
    }

    // update timestamp, not really needed by good practice
    if( (ret = psync_get_timestamp_micro( &msg->header.timestamp )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "get_all_supported_parameters -- psync_get_timestamp_micro - ret: %d", ret, ret );
        return ret;
    }

    // any of the get* will work when using parameter ID == 0
    msg->command = PARAMETER_COMMAND_GET_ALL;

    // set the first parameter in the buffer to the "get all" command, requesting all supported parameters
    msg->parameters._buffer[ 0 ].id = 0;

    // publish request
    if( (ret = psync_message_publish_instance( (void*) msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "get_all_supported_parameters -- psync_message_publish_instance - ret: %d", ret, ret );
        return ret;
    }

    return DTC_RET( DTC_NONE );
}




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_RET( DTC_NONE );

    // polysync node name
    const char *node_name = "ex-writer";

    // flag to enable stdout logs in addition to the normal syslog output
    unsigned int stdout_logging_enabled = 1;


	// init core API
    if( (ret = psync_init( PSYNC_NID_API, node_name, stdout_logging_enabled )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret, ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set event message publishers to have RELIABLE QoS
    if( (ret = psync_set_publisher_reliability_qos( MSG_TYPE_EVENT, RELIABILITY_QOS_RELIABLE )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_set_publisher_reliability_qos - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


    // nodes typically should shutdown after handling SIGINT

    // hook up the control-c signal handler, sets exit_signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );


    // get an object message
    if( (ret = psync_message_request_instance( MSG_TYPE_OBJECT_STREAM, (void**) &object_stream_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get a lane model message
    if( (ret = psync_message_request_instance( MSG_TYPE_LANE_MODEL, (void**) &lane_model_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get an event message
    if( (ret = psync_message_request_instance( MSG_TYPE_EVENT, (void**) &event_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // setup space for 2 objects
    if( (ret = psync_message_ensure_stream_length( (void*) object_stream_msg, 2 )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_ensure_stream_length - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // get an parameter stream message
    if( (ret = psync_message_request_instance( MSG_TYPE_PARAMETER_STREAM, (void**) &parameter_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set object data
    object_stream_msg->sensor_descriptor.serial_number = 123;
    object_stream_msg->objects._buffer[ 0 ].classification_type = CLASSIFICATION_PEDESTRIAN;
    object_stream_msg->objects._buffer[ 0 ].pos[ 0 ] = 10.0;
    object_stream_msg->objects._buffer[ 0 ].size[ 0 ] = 1.0;
    object_stream_msg->objects._buffer[ 0 ].size[ 1 ] = 1.0;
    object_stream_msg->objects._buffer[ 0 ].size[ 2 ] = 1.0;
    object_stream_msg->objects._buffer[ 0 ].id = 1;

    object_stream_msg->objects._buffer[ 1 ].classification_type = CLASSIFICATION_CAR;
    object_stream_msg->objects._buffer[ 1 ].pos[ 0 ] = 25.0;
    object_stream_msg->objects._buffer[ 1 ].size[ 0 ] = 1.0;
    object_stream_msg->objects._buffer[ 1 ].size[ 1 ] = 1.0;
    object_stream_msg->objects._buffer[ 1 ].size[ 2 ] = 1.0;
    object_stream_msg->objects._buffer[ 1 ].id = 2;



    // set example lane model data

    // ref point
    lane_model_msg->sensor_descriptor.type = PSYNC_SENSOR_TYPE_MOBILEYE_EPM2;
    lane_model_msg->reference_point[ 0 ] = 3.0;
    lane_model_msg->reference_point[ 1 ] = 0.0;
    lane_model_msg->reference_point[ 2 ] = 0.0;

    // left lane
    lane_model_msg->left_lane.is_valid = 1;
    lane_model_msg->left_lane.boundary_type = LANE_BOUNDARY_SOLID;
    lane_model_msg->left_lane.lane_offset = 1.0;
    lane_model_msg->left_lane.heading_angle = 0.0;
    lane_model_msg->left_lane.curvature = 0.001;
    lane_model_msg->left_lane.curvature_derivative = -0.001;

    // right lane
    lane_model_msg->right_lane.is_valid = 1;
    lane_model_msg->right_lane.boundary_type = LANE_BOUNDARY_SOLID;
    lane_model_msg->right_lane.lane_offset = -1.0;
    lane_model_msg->right_lane.heading_angle = 0.0;
    lane_model_msg->right_lane.curvature = 0.001;
    lane_model_msg->right_lane.curvature_derivative = -0.001;

    // no remaining lanes
    lane_model_msg->additional_lanes._length = 0;


    // set example event
    // set ID to first available user ID
    event_msg->id = (PSYNC_EVENT_ID_MAX + 1);
    event_msg->value._d = PARAMETER_DOUBLE;
    event_msg->value._u.d_value = 0.1;


    // start off by publishing a command to get all parameters from all nodes
    // set the destination GUID in the parameter command to all node types
    if( (ret = psync_set_node_type( PSYNC_NID_ALL, &parameter_msg->dest_guid )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_set_node_type - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // package up and publish the parameter command
    if( (ret = get_all_supported_parameters( parameter_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- get_all_supported_parameters - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


    // main processing loop
    while( 1 )
    {
        // check for an exit signal
        if( exit_signaled )
        {
            goto GRACEFUL_EXIT_STMNT;
        }


        // get timestamp
        if( (ret = psync_get_timestamp_micro( &object_stream_msg->header.timestamp )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp_micro - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // get timestamp
        if( (ret = psync_get_timestamp_micro( &lane_model_msg->header.timestamp )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp_micro - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // get timestamp
        if( (ret = psync_get_timestamp_micro( &event_msg->header.timestamp )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp_micro - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }


        // publish object message
        if( (ret = psync_message_publish_instance( (void*) object_stream_msg )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish_instance - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // publish lane model message
        if( (ret = psync_message_publish_instance( (void*) lane_model_msg )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish_instance - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // publish lane model message
        if( (ret = psync_message_publish_instance( (void*) event_msg )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish_instance - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }




        // simulate lane movement
        if( lane_model_msg->left_lane.curvature < 0.046 )
        {
            lane_model_msg->left_lane.curvature += 0.0005;
        }
        else
        {
            lane_model_msg->left_lane.curvature = -0.01;
        }

        // right equal to left
        lane_model_msg->right_lane.curvature = lane_model_msg->left_lane.curvature;



        // wait a little
        usleep( 20000 );
    }



    // using 'goto' to allow for an easy example exit
GRACEFUL_EXIT_STMNT:
    exit_signaled = 1;



    // release object message
    if( (ret = psync_message_release_instance( (void**) &object_stream_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance - ret: %d", ret );
    }

    // release lane model message
    if( (ret = psync_message_release_instance( (void**) &lane_model_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance - ret: %d", ret );
    }

    // release event message
    if( (ret = psync_message_release_instance( (void**) &event_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance - ret: %d", ret );
    }

    // release event message
    if( (ret = psync_message_release_instance( (void**) &parameter_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance - ret: %d", ret );
    }



	// release core API
    if( (ret = psync_release( 0 )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }



	return 0;
}

