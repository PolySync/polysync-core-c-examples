/******************************************************
** FILE: polysync_reader.c
**
** ABSTRACT:
**
** DOCUMENTS:
**
** NOTES:
**
*******************************************************/



/**
 * @example polysync_reader.c
 *
 * Example data reader.
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
 * @brief API on_data handler for object stream messages.
 *
 * @warning Do not modify topic_data.
 */
static void on_psync_data_ps_object_stream_msg( void *usr_data, ps_msg_type msg_type, void *topic_data );


/**
 * @brief API on_data handler for lane model messages.
 *
 * @warning Do not modify topic_data.
 */
static void on_psync_data_ps_lane_model_msg( void *usr_data, ps_msg_type msg_type, void *topic_data );


/**
 * @brief API on_data handler for platform motion messages.
 *
 * @warning Do not modify topic_data.
 */
static void on_psync_data_ps_platform_motion_msg( void *usr_data, ps_msg_type msg_type, void *topic_data );


/**
 * @brief API on_data handler for event messages.
 *
 * @warning Do not modify topic_data.
 */
static void on_psync_data_ps_event_msg( void *usr_data, ps_msg_type msg_type, void *topic_data );


/**
 * @brief API on_data handler for parameter stream messages.
 *
 * @warning Do not modify topic_data.
 */
static void on_psync_data_ps_parameter_stream_msg( void *usr_data, ps_msg_type msg_type, void *topic_data );


/**
 * @brief Print utility.
 *
 */
static void print_confidence_type( ps_confidence_kind type );


/**
 * @brief Print utility.
 *
 */
static void print_boundary_type( ps_lane_boundary_kind type );




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
static void on_psync_data_ps_object_stream_msg( void *usr_data, ps_msg_type msg_type, void *topic_data )
{
	// cast reference
	ps_object_stream_msg *message = (ps_object_stream_msg*) topic_data;

    psync_log_message( LOG_LEVEL_INFO, "ps_object_stream_msg -- timestamp: %llu", message->header.timestamp );

    // TODO
    // copy message
    // enqueue new message
}


//
static void on_psync_data_ps_lane_model_msg( void *usr_data, ps_msg_type msg_type, void *topic_data )
{
	// cast reference
	ps_lane_model_msg *message = (ps_lane_model_msg*) topic_data;

    psync_log_message( LOG_LEVEL_INFO, "ps_lane_model_msg -- timestamp: %llu", message->header.timestamp );

    // print left lane info
    if( message->left_lane.is_valid )
    {
        printf( "left_lane.is_valid\n" );
        print_confidence_type( message->left_lane.detection_confidence );
        print_boundary_type( message->left_lane.boundary_type );
        printf( "offset: %.2f\n", message->left_lane.lane_offset );
        printf( "curvature: %.2f\n", message->left_lane.curvature );
        printf( "\n" );
    }

    // print right lane info
    if( message->right_lane.is_valid )
    {
        printf( "right_lane.is_valid\n" );
        print_confidence_type( message->right_lane.detection_confidence );
        print_boundary_type( message->right_lane.boundary_type );
        printf( "offset: %.2f\n", message->right_lane.lane_offset );
        printf( "curvature: %.2f\n", message->right_lane.curvature );
        printf( "\n" );
    }
}


//
static void on_psync_data_ps_platform_motion_msg( void *usr_data, ps_msg_type msg_type, void *topic_data )
{
    // rpy orientation
    double rpy[3] = { 0 };

    // cast reference
	ps_platform_motion_msg *message = (ps_platform_motion_msg*) topic_data;

    psync_log_message( LOG_LEVEL_INFO, "ps_platform_motion_msg -- timestamp: %llu", message->header.timestamp );

    double v_bar = sqrt( (message->velocity[ 0 ] * message->velocity[ 0 ]) + (message->velocity[ 1 ] * message->velocity[ 1 ]) + (message->velocity[ 2 ] * message->velocity[ 2 ]) );

    // use stdout to avoid bloating the log file
    printf( "v_bar: %.2f\nyaw_rate: %.2f\nlat: %f\nlon: %f\nalt: %f\n\n",
            v_bar, message->rotation_rate[ 2 ], message->latitude, message->longitude, message->altitude );

    // quaternion to rpy
    (void) psync_transform_quaternion_to_rpy( message->orientation, rpy );

    // print
    printf( "q1: %.2f q2: %.2f q3: %.2f q4: %.2f - r: %.2f p: %.2f y: %.2f\n",
            message->orientation[0], message->orientation[1], message->orientation[2], message->orientation[3],
            rpy[0], rpy[1], rpy[2] );
}


//
static void on_psync_data_ps_event_msg( void *usr_data, ps_msg_type msg_type, void *topic_data )
{
    // cast reference
	ps_event_msg *message = (ps_event_msg*) topic_data;

    // log
    psync_log_message( LOG_LEVEL_INFO, "ps_event_msg -- timestamp: %llu", message->header.timestamp );

    // print
    printf( "event type: " );
    if( message->id > PSYNC_EVENT_ID_MAX )
    {
        printf( "USER\n" );
    }
    else
    {
        printf( "PolySync\n" );
    }
    printf( "value(double): %.4f\n", message->value._u.d_value );
    printf( "dest_guid: 0x%016llX\n\n", message->dest_guid );
}


//
static void on_psync_data_ps_parameter_stream_msg( void *usr_data, ps_msg_type msg_type, void *topic_data )
{
    // index
    unsigned int idx = 0;

    // cast reference
	ps_parameter_stream_msg *message = (ps_parameter_stream_msg*) topic_data;

    // only handle responses
    if( message->command != PARAMETER_COMMAND_RESPONSE )
    {
        return;
    }

    // could log, instead just using printf for readability
    //psync_log_message( LOG_LEVEL_INFO, "ps_parameter_stream_msg -- timestamp: %llu", message->header.timestamp );

    // print message details
    printf( "\nParameter Responses - number of parameters in response: %u\n", message->parameters._length );
    printf( "Source GUID: 0x%llX - timestamp: %llu\n", message->header.src_guid, message->header.timestamp );

    // print responses
    for( idx = 0; idx < message->parameters._length; idx++ )
    {
        // parameter index, ID, timestamp
        printf( "\nParameter[ %u ]\n", idx );
        printf( "ID: %lu\n", (unsigned long) message->parameters._buffer[ idx ].id );
        printf( "Timestamp [micro]: %llu\n", message->parameters._buffer[ idx ].timestamp );

        // flags
        printf( "Flags: 0x%02X", (unsigned int) message->parameters._buffer[ idx ].flags );

        // check flag bit
        if( message->parameters._buffer[ idx ].flags & PSYNC_PARAMETER_FLAG_READONLY )
        {
            printf( " * READ-ONLY" );
        }

        // check flag bit
        if( message->parameters._buffer[ idx ].flags & PSYNC_PARAMETER_FLAG_USERDEFINED )
        {
            printf( " * USER-DEFINED" );
        }
        printf( "\n" );

        printf( "Description: %s\n", message->parameters._buffer[ idx ].description._buffer );

        if( message->parameters._buffer[ idx ].value._d == PARAMETER_ULONGLONG )
        {
            printf( "Value: %llu - Type: unsigned long long\n", message->parameters._buffer[ idx ].value._u.ull_value );
        }
        else if( message->parameters._buffer[ idx ].value._d == PARAMETER_LONGLONG )
        {
            printf( "Value: %lld - Type: long long\n", message->parameters._buffer[ idx ].value._u.ll_value );
        }
        else if( message->parameters._buffer[ idx ].value._d == PARAMETER_DOUBLE )
        {
            printf( "Value: %f - Type: double\n", message->parameters._buffer[ idx ].value._u.d_value );
        }
    }
}


//
static void print_confidence_type( ps_confidence_kind type )
{
    printf( "det_confidence: " );
    if( type == CONFIDENCE_NONE )
        printf( "NONE\n" );
    else if( type == CONFIDENCE_LOW )
        printf( "LOW\n" );
    else if( type == CONFIDENCE_MED )
        printf( "MED\n" );
    else if( type == CONFIDENCE_HIGH )
        printf( "HIGH\n" );
    else
        printf( "NA\n" );
}


//
static void print_boundary_type( ps_lane_boundary_kind type )
{
    printf( "boundary_type: " );
    if( type ==  LANE_BOUNDARY_NONE )
        printf( "NONE\n" );
    else if( type == LANE_BOUNDARY_INVALID )
        printf( "INVALID\n" );
    else if( type == LANE_BOUNDARY_SOLID )
        printf( "SOLID\n" );
    else if( type == LANE_BOUNDARY_DASHED )
        printf( "DASHED\n" );
    else if( type == LANE_BOUNDARY_VIRTUAL )
        printf( "VIRTUAL\n" );
    else if( type == LANE_BOUNDARY_DOTS )
        printf( "DOTS\n" );
    else if( type == LANE_BOUNDARY_ROAD_EDGE )
        printf( "ROAD_EDGE\n" );
    else if( type == LANE_BOUNDARY_UNDECIDED )
        printf( "UNDECIDED\n" );
    else if( type == LANE_BOUNDARY_DOUBLE_MARKER )
        printf( "DOUBLE_MARKER\n" );
    else
        printf( "NA\n" );
}




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // polysync node name
    const char *node_name = "ex-reader";

    // flag to enable stdout logs in addition to the normal syslog output
    unsigned int stdout_logging_enabled = 1;



	// init core API
    if( (ret = psync_init( PSYNC_NID_API, node_name, stdout_logging_enabled )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    //
    // set event message subscribers to have RELIABLE QoS
    if( (ret = psync_set_subscriber_reliability_qos( MSG_TYPE_EVENT, RELIABILITY_QOS_RELIABLE )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_set_subscriber_reliability_qos - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


    // hook up the control-c signal handler, sets exit_signaled flag
	signal( SIGINT, sig_handler );

	// allow signals to interrupt
	siginterrupt( SIGINT, 1 );


    // register a listener for object stream messages
    if( (ret = psync_message_register_listener( MSG_TYPE_OBJECT_STREAM, on_psync_data_ps_object_stream_msg, NULL )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_register_listener - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // register a listener for lane model messages
    if( (ret = psync_message_register_listener( MSG_TYPE_LANE_MODEL, on_psync_data_ps_lane_model_msg, NULL )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_register_listener - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // register a listener for platform motion messages
    if( (ret = psync_message_register_listener( MSG_TYPE_PLATFORM_MOTION, on_psync_data_ps_platform_motion_msg, NULL )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_register_listener - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // register a listener for event messages
    if( (ret = psync_message_register_listener( MSG_TYPE_EVENT, on_psync_data_ps_event_msg, NULL )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_register_listener - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // register a listener for parameter stream messages
    if( (ret = psync_message_register_listener( MSG_TYPE_PARAMETER_STREAM, on_psync_data_ps_parameter_stream_msg, NULL )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_register_listener - ret: %d", ret );
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


		// sleep a little
		psync_sleep_micro( 100000 );
	}



	// using 'goto' to allow for an easy example exit
GRACEFUL_EXIT_STMNT:



	// release
	if( (ret = psync_release( 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }


	return 0;
}

