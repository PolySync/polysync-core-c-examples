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
 * \example transform_stack.c
 *
 * Transform Example to demonstrate a transform stack.
 *
 * Shows how to use the transform API to perform multiple transforms in the
 * same stack.  The transforms can incorporate changes in orientation and
 * spatial location e.g. the object is spinning while moving forward.
 *
 * Transform Class definition
 *
 */


#include <stdio.h>
#include <stdlib.h>

// API headers
#include "polysync_core.h"
#include "polysync_transform.h"


// I[XYZ] = Index [XYZ]

#define IX 0
#define IY 1
#define IZ 2

#define ROLL 0
#define PITCH 1
#define YAW 2


/**
 * @brief do_push_transform
 *
 * @param t - pointer to the transform stack to use
 * @param id - pointer to the transform id to use
 * @param o - pointer to an array of doubles that contain the origin
 * @param r - pointer to an array of doubles that contain the orientation
 *
 * @return - DTC code
 */
int do_push_transform( ps_transform* t, ps_identifier id, double* o, double* r)
{
    // Set the origin for the transform
    int ret = psync_transform_set_origin( t, o[IX], o[IY], o[IZ] );

    if ( DTC_NONE == ret )
    {
        // Set the orientation for the transform
        ret = psync_transform_set_orientation_rpy( t, r[ROLL], r[PITCH], r[YAW] );

        if ( DTC_NONE == ret )
        {
            // Push this transform on to the transform stack
            ret = psync_transform_push( t, id );
        }
    }
    return ret;
}

// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;
    const char* failure = NULL;

    // Create a Transform object, an origin, and an orientation
    // The first transform is only a location translation with no
    // orientation change.
    ps_transform transform;
    double origin[3] = { 10.0, 0.0, 0.0 };
    double orient[3] = { 0.0, 0.0, 0.0 };

    // Initialize the transform
    ret = psync_transform_init(&transform);

    if (DTC_NONE == ret)
    {
        // Use the PSYNC_COORDINATE_FRAME_PLATFORM as the base of the transform
        ps_identifier id = PSYNC_COORDINATE_FRAME_PLATFORM;

        // Push this first transform on to the stack
        ret = do_push_transform( &transform, id, origin, orient );

        if ( DTC_NONE == ret )
        {
            // Increment variable to create a new ID in the stack
            id++;

            // The second stage of the transform involves both a change in the
            // origin and a change in orientation.
            // Transform moves to the right and rotates around the z-axis
            origin[IX] = 0.0;
            origin[IY] = -10.0;

            orient[YAW] = 2.0;

            // Push this second transform on to the stack
            ret = do_push_transform( &transform, id, origin, orient );

            if ( DTC_NONE == ret )
            {
                // Create a start and end point
                // Initialize the end point to 0,0,0
                double start[3] = { 1.0, 1.0, 1.0 };
                double end[3] = { 0.0, 0.0, 0.0 };

                //  Apply the transform to the start point
                ret = psync_transform_apply( &transform, start[IX], start[IY], start[IZ],
                        &end[IX], &end[IY], &end[IZ] );

                if ( DTC_NONE == ret )
                {
                    ret = EXIT_SUCCESS;
                    printf( "Starting Position: %f, %f, %f\r\n", start[IX], start[IY], start[IZ]);
                    printf( "  Ending Position: %f, %f, %f\r\n", end[IX], end[IY], end[IZ]);
                }
                else
                {
                    failure = "main -- psync_transform_apply - ret: %d";
                }
            }
            else
            {
                failure = "main -- failed to push second transform - ret: %d";
            }
        }
        else
        {
            failure = "main -- failed to push first transform - ret: %d";
        }
    }
    else
    {
        failure = "main -- psync_transform_init - ret: %d";
    }


    if (DTC_NONE != ret)
    {
        psync_log_message( LOG_LEVEL_ERROR, failure, ret );
    }

    return ret;
}
