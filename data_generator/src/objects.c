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
 * @file objects.c
 * @brief Objects Generator Source.
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
#define OBJECTS_SENSOR_ID (12)


/**
 * @brief Number of objects generated.
 *
 */
#define OBJECTS_COUNT (2)


/**
 * @brief Object length. [meters]
 *
 */
#define OBJECTS_SIZE_X (2.0)


/**
 * @brief Object width. [meters]
 *
 */
#define OBJECTS_SIZE_Y (1.5)


/**
 * @brief Object height. [meters]
 *
 */
#define OBJECTS_SIZE_Z (1.0)


/**
 * @brief Object X velocity. [meters/second]
 *
 */
#define OBJECTS_TARGET_VEL_X (3.5)


/**
 * @brief Object Y velocity. [meters/second]
 *
 */
#define OBJECTS_TARGET_VEL_Y (-3.5)



// *****************************************************
// static declarations
// *****************************************************




// *****************************************************
// static definitions
// *****************************************************

//
static void reset_objects( const ps_timestamp time, ps_objects_msg * const msg )
{
    // local vars
    unsigned long idx = 0;


    // for each target
    for( idx = 0; idx < OBJECTS_COUNT; idx += 1 )
    {
        // cast
        ps_object * const object = &msg->objects._buffer[ idx ];

        // set ID
        object->id = (ps_identifier) (idx + 1);

        // set timestamp
        object->timestamp = time;

        // set position
        object->position[ 0 ] = (DDS_double) (idx + 2) * 2.0;
        object->position[ 1 ] = -((DDS_double) (idx + 2) * 4.0);
        object->position[ 2 ] = 0.5;

        // set size
        object->size[ 0 ] = OBJECTS_SIZE_X;
        object->size[ 1 ] = OBJECTS_SIZE_Y;
        object->size[ 2 ] = OBJECTS_SIZE_Z;

        // set velocity
        object->velocity[ 0 ] = OBJECTS_TARGET_VEL_X;
        object->velocity[ 1 ] = OBJECTS_TARGET_VEL_Y;
        object->velocity[ 2 ] = 0.0;

        // set orientation angle to the direction of travel
        object->course_angle = atan2( object->velocity[ 1 ], object->velocity[ 0 ] );

        // classify as car
        object->classification = OBJECT_CLASSIFICATION_CAR;

        // high quality
        object->classification_quality = QUALITY_9;
    }
}




// *****************************************************
// public definitions
// *****************************************************

//
void objects_init( const ps_timestamp time, ps_objects_msg * const msg )
{
    if( (time == 0) || (msg == NULL) )
    {
        return;
    }


    // set default sensor descriptor
    msg->sensor_descriptor.id = OBJECTS_SENSOR_ID;
    msg->sensor_descriptor.transform.parent_id = PSYNC_COORDINATE_FRAME_LOCAL;
    msg->sensor_descriptor.type = PSYNC_SENSOR_KIND_NOT_AVAILABLE;

    // allocate objects buffer
    msg->objects._buffer = DDS_sequence_ps_object_allocbuf( OBJECTS_COUNT );

    // set sequence
    msg->objects._maximum = OBJECTS_COUNT;
    msg->objects._length = OBJECTS_COUNT;
    msg->objects._release = 1;

    // zero
    memset( msg->objects._buffer, 0, sizeof(msg->objects._buffer[0]) * OBJECTS_COUNT );

    // set header timestamp
    msg->header.timestamp = time;

    // reset targets
    reset_objects( time, msg );
}


//
void objects_update( const ps_timestamp time, ps_objects_msg * const msg )
{
    if( (time == 0) || (msg == NULL) )
    {
        return;
    }

    // local vars
    unsigned long idx = 0;
    unsigned int do_reset = 0;


    // for each target
    for( idx = 0; idx < OBJECTS_COUNT; idx += 1 )
    {
        // cast
        ps_object * const object = &msg->objects._buffer[ idx ];

        // get time difference from last update
        const ps_timestamp time_diff = (time - object->timestamp);

        // get dt in seconds
        const double dt = ((double) time_diff) / 1000000.0;

        // update position using p = p_0 + v * dt
        object->position[ 0 ] += (object->velocity[ 0 ] * dt);
        object->position[ 1 ] += (object->velocity[ 1 ] * dt);

        // set timestamp
        object->timestamp = time;

        // check for reset condition
        if( object->position[ 0 ] > 30.0 )
        {
            // set flag
            do_reset = 1;
        }
    }

    // if reset flag set
    if( do_reset != 0 )
    {
        reset_objects( time, msg );
    }

    // set header timestamp
    msg->header.timestamp = time;
}
