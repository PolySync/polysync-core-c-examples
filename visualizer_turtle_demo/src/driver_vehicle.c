/**
 * @file driver_vehicle.c
 * @brief Driver vehicle source.
 *
 */




#include "math.h"

#include "polysync_message.h"

#include "gl_headers.h"
#include "render.h"
#include "ps_interface.h"
#include "sliding_filter.h"
#include "driver_vehicle.h"


void rotateZ( double* x, double* y, double ang )
{
    double xval = *x;
    double yval = *y;
    
    double rotzMatrix[ 2 ][ 2 ] = 
            { { cos( ang ), -sin( ang ) },
              { sin( ang ), cos( ang ) }  };

    *x = ( xval*rotzMatrix[ 0 ][ 0 ] + yval*rotzMatrix[ 0 ][ 1 ] );
    *y = ( xval*rotzMatrix[ 1 ][ 0 ] + yval*rotzMatrix[ 1 ][ 1 ] );
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


void init_vehicle_position( vehicle_position_s * vehicle_position )
{
    vehicle_position->x = 0;
    
    vehicle_position->y = 0;
    
    vehicle_position->dx = 1;
    
    vehicle_position->dy = 0;
    
    vehicle_position->heading = 0;
    
    vehicle_position->lastHeading = 0;
    
    init_sliding_filter( 
            &vehicle_position->lowPassHeadingFilter, 
            HEADING_SLIDING_AVG_SIZE );
}


void calculate_vehicle_position( 
        vehicle_commands_s commands, 
        vehicle_position_s * current_vehicle_position )
{
    double steeringAngle = STEERING_CONVERSION_FACTOR*commands.currentSteeringCommand;
    
    if( steeringAngle*RAD2DEG > 40 )
    {
        steeringAngle = 40*DEG2RAD;
    }
    
    if( steeringAngle*RAD2DEG < -40 )
    {
        steeringAngle = -40*DEG2RAD;
    }
    
    double throttle = THROTTLE_GAIN*commands.currentThrottleCommand;
    
    rotateZ( &current_vehicle_position->dx, 
            &current_vehicle_position->dy,
            steeringAngle/STEERING_GAIN );
    
    double headingInput = 
            atan2( current_vehicle_position->dx, current_vehicle_position->dy );
    
    double heading = input_to_sliding_filter( 
            &current_vehicle_position->lowPassHeadingFilter,
            headingInput );
    
    if( current_vehicle_position->dy > 0.999 )
    {
        heading = 0;
    }
    else if( current_vehicle_position->dy < -0.99 )
    {
        heading = M_PI;
    }
    
    // convert from (-180, 180) to (0, 360)
    current_vehicle_position->heading = heading + M_PI;
    
    current_vehicle_position->x += current_vehicle_position->dx*throttle;
    
    current_vehicle_position->y += current_vehicle_position->dy*throttle;
}


int publish_platform_motion_msg( 
        ps_node_ref const node_ref, 
        ps_msg_type msg_type, 
        double x, 
        double y, 
        double heading )
{
    int ret = DTC_NONE;
    
    ps_msg_ref msg = PSYNC_MSG_REF_INVALID;
    
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
    
    // Set fields in message
    ps_platform_motion_msg * platformMotionMsg = (ps_platform_motion_msg *) msg;
    
    platformMotionMsg->heading = heading;
    
    platformMotionMsg->position[ 0 ] = x;
    
    platformMotionMsg->position[ 1 ] = y;
    
    // set publish timestamp in message header
    ret = psync_get_timestamp( &platformMotionMsg->header.timestamp );

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


publish_current_vehicle_position( 
        node_data_s * user_data, vehicle_commands_s commands, 
        vehicle_position_s * current_position )
{
    calculate_vehicle_position( commands, current_position );
    
    publish_platform_motion_msg(
            user_data->node, 
            user_data->msg_type_platform_motion, 
            current_position->x, 
            current_position->y, 
            current_position->heading );
}


void draw_vehicle_position( vehicle_position_s * vehicle_position, GLuint textureToRender )
{
    glPushMatrix();
    
    glTranslated( vehicle_position->x, vehicle_position->y, 0.0 );
    
    // convert to different heading frame
    double heading = -vehicle_position->heading*RAD2DEG + 270.0;
    
    if( heading < 0 )
    {
        heading += 360.0;
    }
    
    double lastHeading = vehicle_position->lastHeading;
    
    // filter for jerky movement of poly
    double angle = calculate_smallest_interior_angle( heading, lastHeading );
    
    if( fabs( angle ) > 50 )
    {
        heading = lastHeading;
    }
    else
    {
        vehicle_position->lastHeading = heading;
    }
    
    glRotatef( heading, 0, 0, 1 );

    render_cube_with_texture( textureToRender );

    glPopMatrix();
}