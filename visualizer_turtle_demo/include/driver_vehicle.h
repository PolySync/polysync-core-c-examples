/* 
 * File:   driver_vehicle.h
 * Author: lbuckland
 *
 * Created on September 7, 2016, 10:44 AM
 */

#ifndef DRIVER_VEHICLE_H
#define DRIVER_VEHICLE_H


#include "sliding_filter.h"


#define HEADING_SLIDING_AVG_SIZE 1000

#define RAD2DEG (180.0/M_PI)

#define DEG2RAD (M_PI/180.0)

#define STEERING_CONVERSION_FACTOR (40.0/360.0)

#define STEERING_GAIN 2000

#define THROTTLE_GAIN 0.001


typedef struct
{
    //
    //
    double x;
    //
    //
    double y;    
    //
    //
    double dx;
    //
    //
    double dy;
    //
    //
    double heading;
    //
    //
    double lastHeading;
    //
    //
    sliding_filter_params_s lowPassHeadingFilter;
} vehicle_position_s;


typedef struct
{
    //
    //
    double currentSteeringCommand;
    //
    //
    double currentThrottleCommand;
} vehicle_commands_s;


//void publish_current_vehicle_position( node_data_s * user_data, vehicle_commands_s commands, vehicle_position_s * current_position );

void init_vehicle_position( vehicle_position_s * vehicle_position );

void draw_vehicle_position( vehicle_position_s * vehicle_position, GLuint textureToRender );


#endif /* DRIVER_VEHICLE_H */

