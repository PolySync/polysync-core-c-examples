/**
 * @file vehicle_control.c
 * @brief Simple vehicle control.
 *
 */




#include <stdlib.h>

#include "polysync_message.h"
#include "polysync_node.h"

#include "ps_interface.h"
#include "vehicle_control.h"




double calculate_distance_between_coordinates( double x1, double y1, double x2, double y2 )
{
      	double dx = ( x1 - x2 );
        
   	double dy = ( y1 - y2 );
        
   	return sqrt( dx*dx + dy*dy );
}


double calculate_angle_between_coordinates( double x1, double y1, double x2, double y2 )
{
    double dx = x2 - x1;
    
    double dy = y2 - y1;

    return atan2( dx, dy ) + M_PI;
}


double calculate_smallest_interior_angle( double angle1, double angle2 )
{
    if ( angle1 < 90 && angle2 > 270 ) 
    {
        return angle2 - ( 360 + angle1 );
    }
    else if ( angle1 > 270 && angle2 < 90 ) 
    {
        return ( 360 + angle2 ) - angle1; 
    }
    else 
    {
        return angle2 - angle1;
    }
}


int calculate_steering_angle_based_on_goal_waypoint( 
        waypoint_s * goalWaypoint, 
        waypoint_s currentPosition,
        double * steeringAngle )
{
    if( ! goalWaypoint->valid )
    {
        return 1;
    }
    
    double vehicleToWaypointAngle = calculate_angle_between_coordinates( 
            currentPosition.x, 
            currentPosition.y,
            goalWaypoint->x,
            goalWaypoint->y );
    
    double errorInHeading = calculate_smallest_interior_angle( 
            currentPosition.heading*RAD2DEG, 
            vehicleToWaypointAngle*RAD2DEG );
    
    double lateralError = sin( errorInHeading*DEG2RAD );
    
    ( *steeringAngle ) = -STEERING_CONTROL_FACTOR*lateralError;
    
    return 0;
}


int findNearestWaypoint( waypoint_s * waypoints, waypoint_s current_position )
{
    int index = -1;
    
    double minDistance = 0;
    
    for( int i = 0; i < MAX_WAYPOINTS; i++ )
    {
        if( waypoints[ i ].valid )
        {
            double distance = calculate_distance_between_coordinates( 
                    waypoints[ i ].x, 
                    waypoints[ i ].y, 
                    current_position.x, 
                    current_position.y );
            
            if( distance <= REACHED_WAYPOINT_DISTANCE )
            {
                waypoints[ i ].valid = 0;
            }
            else if( distance > REACHED_WAYPOINT_DISTANCE && 
                    ( distance < minDistance || index == -1 ) )
            {
                minDistance = distance;
                
                index = i;
            }
        }
    }
    
    return index;
}


int findNextWaypoint( waypoint_s * waypoints, waypoint_s * nextWaypoint, waypoint_s current_position )
{
    if( waypoints == NULL || nextWaypoint == NULL )
    {
        printf( "ERROR: Waypoints NULL\n" );
        return 1;
    }
    
    if( ! waypoints[ 0 ].valid )
    {
        return 1;
    }
    
    nextWaypoint->x = waypoints[ 0 ].x;
    
    nextWaypoint->y = waypoints[ 0 ].y;
    
    nextWaypoint->heading = waypoints[ 0 ].heading;
    
    nextWaypoint->valid = waypoints[ 0 ].valid;
    
    double distance = calculate_distance_between_coordinates( 
                    waypoints[ 0 ].x, 
                    waypoints[ 0 ].y, 
                    current_position.x, 
                    current_position.y );
    
    if( distance <= REACHED_WAYPOINT_DISTANCE )
    {
        for( int i = 0; i < MAX_WAYPOINTS - 1; i++ )
        {
            waypoints[ i ].x = waypoints[ i + 1 ].x;

            waypoints[ i ].y = waypoints[ i + 1 ].y;

            waypoints[ i ].heading = waypoints[ i + 1 ].heading;

            waypoints[ i ].valid = waypoints[ i + 1 ].valid;
            
            waypoints[ i + 1 ].valid = 0;
        }
    }
    
    return 0;
}


int publish_steering_command( ps_node_ref const node_ref, double commandedSteeringAngle )
{
    int ret = DTC_NONE;
    
    ps_msg_type msg_type = PSYNC_MSG_TYPE_INVALID;
    
    ps_msg_ref msg = PSYNC_MSG_REF_INVALID;
    
    ret = psync_message_get_type_by_name(
            node_ref,
            STEERING_CMD_MSG_NAME,
            &msg_type );
    
    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_get_type_by_name returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return ret;
    }
    
    ret = psync_message_alloc(
            node_ref,
            msg_type,
            &msg );
    
    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_alloc returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return ret;
    }
    
    // Set steering wheel angle in message 
    ps_platform_steering_command_msg * commandMsg = (ps_platform_steering_command_msg *) msg;
    
    commandMsg->steering_wheel_angle = commandedSteeringAngle;
    
    // set publish timestamp in message header
    ret = psync_get_timestamp( &commandMsg->header.timestamp );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_get_timestamp returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return ret;
    }
    
    // publish message
    ret = psync_message_publish(
            node_ref,
            msg );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return ret;
    }

    // free event message
    ret = psync_message_free(
            node_ref,
            &msg );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_free returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return ret;
    }
    
    return ret;
}


int publish_throttle_command( ps_node_ref const node_ref, double commandedThrottle )
{
    int ret = DTC_NONE;
    
    ps_msg_type msg_type = PSYNC_MSG_TYPE_INVALID;
    
    ps_msg_ref msg = PSYNC_MSG_REF_INVALID;
    
    ret = psync_message_get_type_by_name(
            node_ref,
            THROTTLE_CMD_MSG_NAME,
            &msg_type );
    
    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_get_type_by_name returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return ret;
    }
    
    ret = psync_message_alloc(
            node_ref,
            msg_type,
            &msg );
    
    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_alloc returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return ret;
    }
    
    // Set steering wheel angle in message 
    ps_platform_throttle_command_msg * commandMsg = (ps_platform_throttle_command_msg *) msg;
    
    commandMsg->throttle_command = commandedThrottle;
    
    // set publish timestamp in message header
    ret = psync_get_timestamp( &commandMsg->header.timestamp );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_get_timestamp returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return ret;
    }
    
    // publish message
    ret = psync_message_publish(
            node_ref,
            msg );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return ret;
    }

    // free event message
    ret = psync_message_free(
            node_ref,
            &msg );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_free returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return ret;
    }
    
    return ret;
}


void send_psync_messages_for_vehicle_control( node_data_s * user_data, 
        waypoint_s * waypoints )
{
    // local vars
    node_data_s     *node_data  = NULL;

    // cast
    node_data = ( node_data_s* ) user_data;

    // ignore if not valid
    if( ( node_data == NULL ) || ( waypoints == NULL ) )
    {
        return;
    }
    
    waypoint_s nextWaypoint;
    
    if( findNextWaypoint( waypoints, &nextWaypoint, node_data->current_vehicle_position ) )
    {
        return;
    }
   
    double steeringAngle = 0;
    
    if( calculate_steering_angle_based_on_goal_waypoint(
            &nextWaypoint, 
            node_data->current_vehicle_position,
            &steeringAngle ) )
    {
        return;
    }
    
    publish_steering_command( node_data->node, steeringAngle );
    
    publish_throttle_command( node_data->node, DEFAULT_THROTTLE );
}