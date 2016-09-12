#include "math.h"

#include "polysync_message.h"

#include "gl_headers.h"
#include "render.h"
#include "ps_interface.h"
#include "driver_vehicle.h"


#define SLIDING_AVG_SIZE 1000

double sliding_avg_x[ SLIDING_AVG_SIZE ];

double sliding_avg_y[ SLIDING_AVG_SIZE ];

int sliding_avg_count = 0;


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


void init_vehicle_position( vehicle_position_s * vehicle_position )
{
    vehicle_position->x = 0;
    
    vehicle_position->y = 0;
    
    vehicle_position->dx = 1;
    
    vehicle_position->dy = 0;
    
    vehicle_position->heading = 0;
}


void calculate_vehicle_position( vehicle_commands_s commands, vehicle_position_s * current_vehicle_position )
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
    
    //( "steering %f throttle %f\n", steeringAngle, throttle );
    
    rotateZ( &current_vehicle_position->dx, 
            &current_vehicle_position->dy, 
            steeringAngle/2000 );
    
    sliding_avg_x[ sliding_avg_count%SLIDING_AVG_SIZE ] = atan2(
            current_vehicle_position->dx, 
            current_vehicle_position->dy );

    double heading = 0;

    for( int i = 0; i < SLIDING_AVG_SIZE; i++ )
    {
        heading += sliding_avg_x[ i ];
    }
   
    heading = heading/SLIDING_AVG_SIZE;

    sliding_avg_count ++;

//    printf("dx %f dy %f heading %f\n", 
//            current_vehicle_position->dx,
//            current_vehicle_position->dy,
//            (heading*RAD2DEG + 180) );
    
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


publish_current_vehicle_position( node_data_s * user_data, vehicle_commands_s commands, vehicle_position_s * current_position )
{
    calculate_vehicle_position( commands, current_position );
    
    publish_platform_motion_msg(
        user_data->node, 
        user_data->msg_type_platform_motion, 
        current_position->x, 
        current_position->y, 
        current_position->heading );
}


void draw_vehicle_position( vehicle_position_s vehicle_position, GLuint textureToRender )
{
    glPushMatrix();

    //glTranslated( waypoints[ i ].x, waypoints[ i ].y, 0.0 );

    // set entity line width
    glLineWidth( (GLfloat) 3.0 );

    // set color
    //glColor4dv( color );
    glColor3b( 0, 126, 0 );

    //render_circle_2d( vehicle_position.x, vehicle_position.y, 5 );
    render_cube_with_texture( textureToRender );

    glPopMatrix();
}