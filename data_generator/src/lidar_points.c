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
 * @file lidar_points.c
 * @brief LiDAR Points Generator Source.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_node.h"
#include "polysync_message.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief Example valid sensor identifier.
 *
 */
#define LIDAR_POINTS_SENSOR_ID (11)


/**
 * @brief Grid side length. [points]
 *
 */
#define GRID_SIDE_LENGTH (100)


/**
 * @brief Grid scale.
 *
 * If side length is 100 and scale is 10.0,
 * 10 points across half the length, 50 points every 10 meters
 *
 */
#define GRID_SCALE (10.0)


/**
 * @brief Number of points used in the sine wave.
 *
 */
#define LIDAR_POINTS_POINT_COUNT (GRID_SIDE_LENGTH * GRID_SIDE_LENGTH)


/**
 * @brief Global relative update timestamp used to update the sine wave generator. [seconds]
 *
 */
static double global_relative_time = 0.0;




// *****************************************************
// static declarations
// *****************************************************




// *****************************************************
// static definitions
// *****************************************************

//
static void update_points( const ps_timestamp time, ps_lidar_points_msg * const msg )
{
    // local vars
    unsigned long idx = 0;
    unsigned long x = 0;
    unsigned long y = 0;
    double u = 0.0;
    double v = 0.0;
    double w = 0.0;

    const double scale = GRID_SCALE;
    const double sine_freq = 4.0;


    // scan start time
    msg->start_timestamp = time;

    // scan end time
    msg->end_timestamp = time;

    // for each point
    for( idx = 0; idx < (unsigned long) msg->points._length; idx += 1 )
    {
        // cast
        ps_lidar_point * const point = &msg->points._buffer[ idx ];

        // max intensity
        point->intensity = 255;

        // get x/y
        x = (idx % GRID_SIDE_LENGTH);
        y = (idx / GRID_SIDE_LENGTH);

        // get u/v
        u = ((double) x / (double) GRID_SIDE_LENGTH);
        v = ((double) y / (double) GRID_SIDE_LENGTH);

        // center u/v at origin
        u = (u * 2.0) - 1.0;
        v = (v * 2.0) - 1.0;

        // compute sine
        w = sin((u * sine_freq) + global_relative_time) * cos((v * sine_freq) + global_relative_time) * 0.5;

        // convert to cartesian
        point->position[ 0 ] = u * scale;
        point->position[ 1 ] = v * scale;
        point->position[ 2 ] = w * scale;
    }
}




// *****************************************************
// public definitions
// *****************************************************

//
void lidar_points_init( const ps_timestamp time, ps_lidar_points_msg * const msg )
{
    if( (time == 0) || (msg == NULL) )
    {
        return;
    }


    // set default sensor descriptor
    msg->sensor_descriptor.id = LIDAR_POINTS_SENSOR_ID;
    msg->sensor_descriptor.transform.parent_id = PSYNC_COORDINATE_FRAME_LOCAL;
    msg->sensor_descriptor.type = PSYNC_SENSOR_KIND_NOT_AVAILABLE;

    // allocate points buffer
    msg->points._buffer = DDS_sequence_ps_lidar_point_allocbuf( LIDAR_POINTS_POINT_COUNT );

    // set sequence
    msg->points._maximum = LIDAR_POINTS_POINT_COUNT;
    msg->points._length = LIDAR_POINTS_POINT_COUNT;
    msg->points._release = 1;

    // zero
    memset( msg->points._buffer, 0, sizeof(msg->points._buffer[0]) * LIDAR_POINTS_POINT_COUNT );

    // zero global timestamp
    global_relative_time = 0.0;

    // set header timestamp
    msg->header.timestamp = time;

    // reset points
    update_points( time, msg );
}


//
void lidar_points_update( const ps_timestamp time, ps_lidar_points_msg * const msg )
{
    if( (time == 0) || (msg == NULL) )
    {
        return;
    }


    // get time difference from last update
    const ps_timestamp time_diff = (time - msg->start_timestamp);

    // get dt in seconds
    const double dt = ((double) time_diff) / 1000000.0;

    // add to global time
    global_relative_time += dt;

    // update points using global time
    update_points( time, msg );

    // set header timestamp
    msg->header.timestamp = time;
}
