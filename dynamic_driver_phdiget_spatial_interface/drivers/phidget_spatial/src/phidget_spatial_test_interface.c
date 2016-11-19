/*
 * @file: phidget_spatial_test_interface.c
 * @brief The Phidget Spatial test interface source code.
 */




#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <signal.h>
#include <phidget21.h>

#include "psyncDcps.h"
#include "polysync_core.h"
#include "phidget_spatial_driver.h"




// *****************************************************
// static global types/macros
// *****************************************************


enum
{
    //
    //
    SUCCESS = 0,
    //
    //
    FAILURE = 1
};


/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static volatile sig_atomic_t global_exit_signal = 0;




// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief Signal handler.
 *
 * @param [in] sig Signal being handled.
 *
 */
static void sig_handler( int sig );




// *****************************************************
// static definitions
// *****************************************************


//
static void sig_handler( int sig )
{
    // graceful exit on SIGINT
    if( sig == SIGINT )
    {
        global_exit_signal = 1;
    }
}


//
static int coordinate_max_bound_check( double * array, unsigned int length, double max_bound )
{
    if( array == NULL )
    {
        return FAILURE;
    }

    for( unsigned long idx = 0; idx < length; ++idx )
    {
        if( array[ idx ] > max_bound )
        {
            return FAILURE;
        }
    }


    return SUCCESS;
}


//
static int coordinate_min_bound_check( double * array, unsigned int length, double min_bound )
{
    if( array == NULL )
    {
        return FAILURE;
    }

    for( unsigned long idx = 0; idx < length; ++idx )
    {
        if( array[ idx ] < min_bound )
        {
            return FAILURE;
        }
    }


    return SUCCESS;
}




// *****************************************************
// public definitions
// *****************************************************


//
int main(int argc, char** argv)
{
    int ret = DTC_NONE;
    phidget_spatial_handle handle;
    double acceleration[ INDEX_COUNT ]; // x,y,z
    double angular_rate[ INDEX_COUNT ]; // x,y,z
    double magnetic_field[ INDEX_COUNT ]; // x,y,z

    // sample rate in microseconds
    const ps_timestamp sample_interval = (32 * 1000ULL);

    memset( &handle, 0, sizeof(handle) );

    // hook up the control-c signal handler, sets exit_signaled flag
    (void) signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    printf( "\nopening device\n" );

    (void) phidget_spatial_open_device( &handle );

    printf( "configuring\n" );

    printf( "setting data rate - %llu ms\n", (unsigned long long) (sample_interval / 1000ULL) );

    // set the data rate to our desired value, set in the local vars
    (void) phidget_spatial_set_data_rate( &handle, sample_interval );

    printf( "zeroing gyroscope\n" );

    // zero the gyroscope
    (void) phidget_spatial_zero_gyroscope( &handle );

    (void) psync_sleep_micro( sample_interval );

    printf( "\n\n" );

    // until CTRL+C signaled
    while( global_exit_signal == 0 )
    {
        ps_timestamp time_begin = 0;
        ps_timestamp time_end = 0;
        (void) psync_get_timestamp( &time_begin );

        //
        // acceleration
        //

        printf( "timestamp: %llu\n", time_begin );

        ret = phidget_spatial_get_acceleration(
            &handle,
            &acceleration[ X_INDEX ],
            &acceleration[ Y_INDEX ],
            &acceleration[ Z_INDEX ] );

        if( ret == DTC_NONE )
        {
            printf( "acceleration: (%f, %f, %f)\n",
                acceleration[ X_INDEX ],
                acceleration[ Y_INDEX ],
                acceleration[ Z_INDEX ] );

            if( coordinate_min_bound_check(
                acceleration,
                INDEX_COUNT,
                PHIDGET_SPATIAL_ACCELERATION_MIN ) == FAILURE )
            {
                printf( "acceleration reported below min\n" );
            }

            if( coordinate_max_bound_check(
                acceleration,
                INDEX_COUNT,
                PHIDGET_SPATIAL_ACCELERATION_MAX ) == FAILURE )
            {
                printf( "acceleration reported above max\n" );
            }
        }
        else
        {
            printf( "error reading acceleration - %d\n", ret );
        }

        //
        // angular rate
        //

        ret = phidget_spatial_get_angular_rate(
            &handle,
            &angular_rate[ X_INDEX ],
            &angular_rate[ Y_INDEX ],
            &angular_rate[ Z_INDEX ] );

        if( ret == DTC_NONE )
        {
            printf( "angular rate: (%f, %f, %f)\n",
                angular_rate[ X_INDEX ],
                angular_rate[ Y_INDEX ],
                angular_rate[ Z_INDEX ] );

            if( coordinate_min_bound_check(
                angular_rate,
                INDEX_COUNT,
                PHIDGET_SPATIAL_ANGULAR_RATE_MIN ) == FAILURE )
            {
                printf( "angular rate reported below min\n" );
            }

            if( coordinate_max_bound_check(
                angular_rate,
                INDEX_COUNT,
                PHIDGET_SPATIAL_ACCELERATION_MAX ) == FAILURE )
            {
                printf( "angular rate reported above max\n" );
            }
        }
        else
        {
            printf( "error reading angular rate - %d\n", ret);
        }

        //
        // magnetic field strength
        //

        ret = phidget_spatial_get_magnetic_field(
            &handle,
            &magnetic_field[ X_INDEX ],
            &magnetic_field[ Y_INDEX ],
            &magnetic_field[ Z_INDEX ] );

        if( ret == DTC_NONE )
        {
            printf( "magnetic field strength: (%f, %f, %f)\n",
                magnetic_field[ X_INDEX ],
                magnetic_field[ Y_INDEX ],
                magnetic_field[ Z_INDEX ] );

            if( coordinate_min_bound_check(
                magnetic_field,
                INDEX_COUNT,
                PHIDGET_SPATIAL_MAGNETIC_FIELD_MIN ) == FAILURE )
            {
                printf( "magnetic field strength reported below min\n" );
            }

            if( coordinate_max_bound_check(
                magnetic_field,
                INDEX_COUNT,
                PHIDGET_SPATIAL_MAGNETIC_FIELD_MAX ) == FAILURE )
            {
                printf( "magnetic field strength reported above max\n" );
            }
        }
        else
        {
            printf( "error reading the magnetic field strength - %d\n", ret );
        }

        (void) psync_get_timestamp( &time_end );

        // sleep for the remainder of the sample interval
        if( time_end >= time_begin )
        {
            const ps_timestamp sample_duration = (time_end - time_begin);

            if( sample_duration <= sample_interval )
            {
                const ps_timestamp remainder = (sample_interval - sample_duration);

                if( remainder != 0 )
                {
                    (void) psync_sleep_micro( remainder );
                }
            }
        }

        printf( "\n\n" );
    }

    (void) phidget_spatial_close_device( &handle );


    return EXIT_SUCCESS;
}

