/*
 * Copyright (c) 2015 HARBRICK TECHNOLOGIES, INC
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
 * @file radar_targets.c
 * @brief Radar Targets Generator Source.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#define RADAR_SENSOR_ID (11)


/**
 * @brief Number of radar targets generated.
 *
 */
#define RADAR_TARGETS_COUNT (2)


/**
 * @brief Default radar target amplitude. [decibels]
 *
 */
#define RADAR_TARGET_AMPLITUDE (2.0)


/**
 * @brief Radar target X velocity. [meters/second]
 *
 */
#define RADAR_TARGET_VEL_X (3.5)


/**
 * @brief Radar target X velocity. [meters/second]
 *
 */
#define RADAR_TARGET_VEL_Y (3.5)



// *****************************************************
// static declarations
// *****************************************************




// *****************************************************
// static definitions
// *****************************************************

//
static void reset_targets( const ps_timestamp time, ps_radar_targets_msg * const msg )
{
    // local vars
    unsigned long idx = 0;


    // for each target
    for( idx = 0; idx < RADAR_TARGETS_COUNT; idx += 1 )
    {
        // cast
        ps_radar_target * const target = &msg->targets._buffer[ idx ];

        // invalidate unused
        target->size[0] = PSYNC_SIZE_NOT_AVAILABLE;
        target->size[1] = PSYNC_SIZE_NOT_AVAILABLE;
        target->size[2] = PSYNC_SIZE_NOT_AVAILABLE;
        target->range_type = RANGE_NOT_AVAILABLE;
        target->zone_type = ZONE_NOT_AVAILABLE;
        target->quality = QUALITY_NOT_AVAILABLE;
        target->magnitude = PSYNC_MAGNITUDE_NOT_AVAILABLE;
        target->alias = PSYNC_VELOCITY_ALIAS_NOT_AVAILABLE;
        target->cross_section = PSYNC_RADAR_CROSS_SECTION_NOT_AVAILABLE;
        target->scan_index = 0;

        // set ID
        target->id = (ps_identifier) (idx + 1);

        // valid status
        target->track_status = TRACK_STATUS_ACTIVE;

        // set timestamp
        target->timestamp = time;

        // set amplitude
        target->amplitude = RADAR_TARGET_AMPLITUDE;

        // set position
        target->position[ 0 ] = (DDS_double) (idx + 2) * 2.0;
        target->position[ 1 ] = (DDS_double) (idx + 2) * 4.0;
        target->position[ 2 ] = 0.5;

        // set velocity
        target->velocity[ 0 ] = RADAR_TARGET_VEL_X;
        target->velocity[ 1 ] = RADAR_TARGET_VEL_Y;
        target->velocity[ 2 ] = 0.0;
    }
}




// *****************************************************
// public definitions
// *****************************************************

//
void radar_targets_init( const ps_timestamp time, ps_radar_targets_msg * const msg )
{
    if( (time == 0) || (msg == NULL) )
    {
        return;
    }


    // set default sensor descriptor
    msg->sensor_descriptor.id = RADAR_SENSOR_ID;
    msg->sensor_descriptor.transform.parent_id = PSYNC_COORDINATE_FRAME_LOCAL;
    msg->sensor_descriptor.type = PSYNC_SENSOR_KIND_NOT_AVAILABLE;

    // allocate targets buffer
    msg->targets._buffer = DDS_sequence_ps_radar_target_allocbuf( RADAR_TARGETS_COUNT );

    // set sequence
    msg->targets._maximum = RADAR_TARGETS_COUNT;
    msg->targets._length = RADAR_TARGETS_COUNT;
    msg->targets._release = 1;

    // zero
    memset( msg->targets._buffer, 0, sizeof(msg->targets._buffer[0]) * RADAR_TARGETS_COUNT );

    // set header timestamp
    msg->header.timestamp = time;

    // reset targets
    reset_targets( time, msg );
}


//
void radar_targets_update( const ps_timestamp time, ps_radar_targets_msg * const msg )
{
    if( (time == 0) || (msg == NULL) )
    {
        return;
    }

    // local vars
    unsigned long idx = 0;
    unsigned int do_reset = 0;


    // for each target
    for( idx = 0; idx < RADAR_TARGETS_COUNT; idx += 1 )
    {
        // cast
        ps_radar_target * const target = &msg->targets._buffer[ idx ];

        // get time difference from last update
        const ps_timestamp time_diff = (time - target->timestamp);

        // get dt in seconds
        const double dt = ((double) time_diff) / 1000000.0;

        // update position using p = p_0 + v * dt
        target->position[ 0 ] += (target->velocity[ 0 ] * dt);
        target->position[ 1 ] += (target->velocity[ 1 ] * dt);

        // set timestamp
        target->timestamp = time;

        // check for reset condition
        if( target->position[ 0 ] > 30.0 )
        {
            // set flag
            do_reset = 1;
        }
    }

    // if reset flag set
    if( do_reset != 0 )
    {
        reset_targets( time, msg );
    }

    // set header timestamp
    msg->header.timestamp = time;
}
