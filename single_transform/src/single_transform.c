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
 * \example single_transform.c
 *
 * Transform Example to demonstrate a single level transform.
 *
 * Shows how to use the transform API to perform a single operation on the
 * transform stack.
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

#define ROT 0
#define PITCH 1
#define YAW 2


// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;
    const char* failure = NULL;

    // Create a Transform object, an origin, and an orientation
    ps_transform transform;
    double origin[3] = { 0.0, 0.0, 0.0 };
    double rpy[3] = { 1.0, 2.0, 3.0 };

    // Initialize the transform
    ret = psync_transform_init(&transform);

    if (DTC_NONE == ret)
    {
        // Use the PSYNC_COORDINATE_FRAME_PLATFORM as the base of the transform
        ps_identifier id = PSYNC_COORDINATE_FRAME_PLATFORM;

        // Set the origin for the transform
        ret = psync_transform_set_origin( &transform, origin[IX], origin[IY], origin[IZ] );

        if ( DTC_NONE == ret )
        {
            // Set the orientation for the transform
            ret = psync_transform_set_orientation_rpy( &transform, rpy[ROT], rpy[PITCH], rpy[YAW] );

            if ( DTC_NONE == ret )
            {
                // Push this transform on to the transform stack
                ret = psync_transform_push( &transform, id );

                if ( DTC_NONE == ret )
                {
                    // Create a start and end point
                    // Initialize the end point to 0,0,0
                    double start[3] = { 3.3, 2.2, 1.1 };
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
                    failure = "main -- psync_transform_push - ret: %d";
                }
            }
            else
            {
                failure = "main -- psync_transform_set_orientation_rpy - ret: %d";
            }
        }
        else
        {
            failure = "main -- psync_transform_set_origin - ret: %d";
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
