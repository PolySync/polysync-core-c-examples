/**
 * @file phidget_spatial_driver.c
 * @brief The Phidget Spatial Hardware (HAL) Driver source code.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "psyncDcps.h"
#include "polysync_core.h"

#include "phidget_spatial_driver.h"




// *****************************************************
// static global types/macros
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************




// *****************************************************
// static definitions
// *****************************************************




// *****************************************************
// public definitions
// *****************************************************


// See the note in the tutorial about the handler below!
int CCONV phidget_example_spatial_data_handler(
        CPhidgetSpatialHandle handle,
        void * user_data,
        CPhidgetSpatial_SpatialEventDataHandle *data,
        int count )
{
    int idx = 0;

    printf("data packets in this event - %d\n", count);

    for( idx = 0; idx < count; idx++ )
    {
        printf( "acceleration: (%6f, %6f, %6f)\n",
            data[idx]->acceleration[ X_INDEX ],
            data[idx]->acceleration[ Y_INDEX ],
            data[idx]->acceleration[ Z_INDEX ] );

        printf( "angular rate: (%6f, %6f, %6f)\n\n",
            data[idx]->angularRate[ X_INDEX ],
            data[idx]->angularRate[ Y_INDEX ],
            data[idx]->angularRate[ Z_INDEX ] );

        printf( "magnetic field strength: (%6f, %6f, %6f)\n\n",
            data[idx]->magneticField[ X_INDEX ],
            data[idx]->magneticField[ Y_INDEX ],
            data[idx]->magneticField[ Z_INDEX ] );

        printf( "timestamp: %d seconds -- %d microseconds\n",
            data[idx]->timestamp.seconds,
            data[idx]->timestamp.microseconds );
    }

    return 0;
}


//
int phidget_spatial_open_device(
        phidget_spatial_handle * const handle )
{
    if( handle == NULL )
    {
        psync_log_error( "invalid spatial handle" );
        return DTC_USAGE;
    }

    // local vars
    int ret = DTC_NONE;
    const char * error = NULL;


    ret = CPhidgetSpatial_create( handle );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to create spatial -- %s", error );

        return DTC_UNAVAILABLE;
    }

    ret = CPhidget_open( (CPhidgetHandle) (*handle), -1 );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failure on open -- %s", error );

        return DTC_UNAVAILABLE;
    }

    ret = CPhidget_waitForAttachment( (CPhidgetHandle)(*handle), PHIDGET_SPATIAL_ATTACHMENT_WAIT_TIMEOUT );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to wait for attachment -- %s", error );

        return DTC_UNAVAILABLE;
    }

    /*
     * Optional: set on-data callback:
     *
     * CPhidgetSpatial_set_OnSpatialData_Handler( handle, phidget_example_spatial_data_handler, NULL );
     *
     * Registers a callback that will run according to the set data rate that will return the spatial data changes.
     * Requires the handle for the Spatial, the callback handler function that will be called,
     * and an arbitrary pointer that will be supplied to the callback function (may be NULL).
     */


    return DTC_NONE;
}


//
int phidget_spatial_close_device(
        phidget_spatial_handle * const handle )
{
    if( handle == NULL )
    {
        psync_log_error( "invalid spatial handle" );
        return DTC_USAGE;
    }

    int ret = DTC_NONE;
    const char * error = NULL;


    if( (*handle) != NULL )
    {
        ret = CPhidget_close( (CPhidgetHandle) (*handle) );

        if( ret != EPHIDGET_OK )
        {
            CPhidget_getErrorDescription( ret, &error );

            psync_log_error( "failure on close -- %s", error );

            return DTC_UNAVAILABLE;
        }

        ret = CPhidget_delete( (CPhidgetHandle) (*handle) );

        if( ret != EPHIDGET_OK )
        {
            CPhidget_getErrorDescription( ret, &error );

            psync_log_error( "failure on delete -- %s", error );

            return DTC_UNAVAILABLE;
        }

        (*handle) = NULL;
    }


    return DTC_NONE;
}


//
int phidget_spatial_get_acceleration(
        phidget_spatial_handle * const handle,
        double * const x,
        double * const y,
        double * const z )
{
    if( (handle == NULL)
         || (x == NULL) || (y == NULL) || (z == NULL) )
    {
        psync_log_error( "invalid arguments" );
        return DTC_USAGE;
    }

    int ret = DTC_NONE;
    const char * error = NULL;


    ret = CPhidgetSpatial_getAcceleration( (*handle), X_INDEX, x );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to get acceleration -- %s", error );

        return DTC_UNAVAILABLE;
    }

    ret = CPhidgetSpatial_getAcceleration( (*handle), Y_INDEX, y );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to get acceleration -- %s", error );

        return DTC_UNAVAILABLE;
    }

    ret = CPhidgetSpatial_getAcceleration( (*handle), Z_INDEX, z );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to get acceleration -- %s", error );

        return DTC_UNAVAILABLE;
    }


    return DTC_NONE;
}


//
int phidget_spatial_get_angular_rate(
        phidget_spatial_handle * const handle,
        double * const x,
        double * const y,
        double * const z )
{
    if( (handle == NULL)
         || (x == NULL) || (y == NULL) || (z == NULL) )
    {
        psync_log_error( "invalid arguments" );
        return DTC_USAGE;
    }

    int ret = DTC_NONE;
    const char * error = NULL;


    ret = CPhidgetSpatial_getAngularRate( (*handle), X_INDEX, x );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to get angular rate -- %s", error );

        return DTC_UNAVAILABLE;
    }

    ret = CPhidgetSpatial_getAngularRate( (*handle), Y_INDEX, y );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to get angular rate -- %s", error );

        return DTC_UNAVAILABLE;
    }

    ret = CPhidgetSpatial_getAngularRate( (*handle), Z_INDEX, z );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to get angular rate -- %s", error );

        return DTC_UNAVAILABLE;
    }


    return DTC_NONE;
}


//
int phidget_spatial_get_magnetic_field(
        phidget_spatial_handle * const handle,
        double * const x,
        double * const y,
        double * const z )
{
    if( (handle == NULL)
         || (x == NULL) || (y == NULL) || (z == NULL) )
    {
        psync_log_error( "invalid arguments" );
        return DTC_USAGE;
    }

    int ret = DTC_NONE;
    const char * error = NULL;


    ret = CPhidgetSpatial_getMagneticField( (*handle), X_INDEX, x );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to get magnetic field -- %s", error );

        return DTC_UNAVAILABLE;
    }

    ret = CPhidgetSpatial_getMagneticField( (*handle), Y_INDEX, y );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to get magnetic field -- %s", error );

        return DTC_UNAVAILABLE;
    }

    ret = CPhidgetSpatial_getMagneticField( (*handle), Z_INDEX, z );

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to get magnetic field -- %s", error );

        return DTC_UNAVAILABLE;
    }


    return DTC_NONE;
}


//
int phidget_spatial_get_data_rate(
        phidget_spatial_handle * const handle,
        ps_timestamp * const data_rate )
{
    if( (handle == NULL) || (data_rate == NULL) )
    {
        psync_log_error( "invalid arguments" );
        return DTC_USAGE;
    }

    int ret = DTC_NONE;
    const char * error = NULL;
    int m_datarate = 0;


    ret = CPhidgetSpatial_getDataRate( (*handle), &m_datarate );

    (*data_rate) = ((ps_timestamp) m_datarate * 1000ULL);

    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to wait for attachment -- %s\n", error );

        return DTC_UNAVAILABLE;
    }


    return DTC_NONE;
}


//
int phidget_spatial_set_data_rate(
        phidget_spatial_handle * const handle,
        const ps_timestamp data_rate )
{
    if( handle == NULL )
    {
        psync_log_error( "invalid spatial handle" );
        return DTC_UNAVAILABLE;
    }

    int ret = DTC_NONE;
    const char * error = NULL;
    const int m_datarate = (int) (data_rate / 1000ULL);


    ret = CPhidgetSpatial_setDataRate( (*handle), m_datarate );


    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to set data rate -- %s\n", error );

        return DTC_UNAVAILABLE;
    }


    return DTC_NONE;
}


//
int phidget_spatial_zero_gyroscope(
        phidget_spatial_handle * handle )
{
    if( handle == NULL )
    {
        psync_log_error( "invalid spatial handle" );
        return DTC_UNAVAILABLE;
    }

    int ret = DTC_NONE;
    const char * error = NULL;


    ret = CPhidgetSpatial_zeroGyro( (*handle) );


    if( ret != EPHIDGET_OK )
    {
        CPhidget_getErrorDescription( ret, &error );

        psync_log_error( "failed to zero the gyroscope -- %s\n", error );

        return DTC_UNAVAILABLE;
    }


    return DTC_NONE;
}
