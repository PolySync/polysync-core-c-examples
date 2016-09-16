/*
 * Copyright (c) 2016 PolySync
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
 * @file driver_vehicle.h
 * @brief Very simple vehicular motion simulation based on steering angle and
 * throttle.
 *
 */




#ifndef DRIVER_VEHICLE_H
#define DRIVER_VEHICLE_H




#include "gl_headers.h"
#include "sliding_filter.h"


#define HEADING_SLIDING_AVG_SIZE 1000

#define RAD2DEG (180.0/M_PI)

#define DEG2RAD (M_PI/180.0)

#define STEERING_CONVERSION_FACTOR (40.0/360.0)

#define STEERING_GAIN 0.0005

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


void init_vehicle_position( vehicle_position_s * vehicle_position );

void draw_vehicle_position( vehicle_position_s * vehicle_position, GLuint textureToRender );


#endif /* DRIVER_VEHICLE_H */

