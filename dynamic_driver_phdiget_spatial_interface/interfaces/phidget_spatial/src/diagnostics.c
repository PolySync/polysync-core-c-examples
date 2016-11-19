/**
 * @file phidget_spatial/src/diagnostics.c
 * @brief Phidget Spatial Diagnostic Utilities Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "polysync_core.h"
#include "polysync_node.h"

#include "phidget_spatial_driver.h"
#include "interface_data.h"
#include "polysync_dynamic_interface.h"
#include "polysync_interface_util.h"
#include "connect.h"
#include "diagnostics.h"




// *****************************************************
// static global types/macros
// *****************************************************




// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************


//
static int wait_for_gyro_data(
        const ps_timestamp timeout,
        phidget_spatial_handle * const handle );




// *****************************************************
// static definitions
// *****************************************************


//
static int wait_for_gyro_data(
        const ps_timestamp timeout,
        phidget_spatial_handle * const handle )
{
    int ret = DTC_NONE;
    bool is_valid = PSYNC_FALSE;
    ps_timestamp start_time = 0;
    ps_timestamp end_time = 0;


    // get start time
    ret = psync_get_timestamp( &start_time );

    // wait for timeout or valid frame
    while( (is_valid == PSYNC_FALSE) && (ret == DTC_NONE) )
    {
        if( ret == DTC_NONE )
        {
            double acceleration[ INDEX_COUNT ];

            ret = phidget_spatial_get_acceleration(
                    handle,
                    &acceleration[ X_INDEX ],
                    &acceleration[ Y_INDEX ],
                    &acceleration[ Z_INDEX ] );
        }

        if( ret == DTC_NONE )
        {
            is_valid = PSYNC_TRUE;
        }

        // check for timeout
        if( is_valid == PSYNC_FALSE )
        {
            ret = psync_get_timestamp( &end_time );

            if( (end_time - start_time) > timeout )
            {
                ret = DTC_UNAVAILABLE;
            }
        }
    }

    if( ret == DTC_NONE )
    {
        if( is_valid == PSYNC_FALSE )
        {
            ret = DTC_UNAVAILABLE;
        }
    }


    return ret;
}




// *****************************************************
// public definitions
// *****************************************************


//
int validate_interface(
        ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;
    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );
    diagnostics_s * const diagnostics = GET_INTERFACE_DIAGNOSTICS( dynamic_driver );


    // test pointers
    if( (dynamic_driver == NULL) || (interface_data == NULL) )
    {
        ret = DTC_USAGE;
    }

    // test handle
    if( ret == DTC_NONE )
    {
        if( interface_data->handle == NULL )
        {
            ret = DTC_CONFIG;
        }
    }

    // wait for valid IMU data
    if( ret == DTC_NONE )
    {
        ret = wait_for_gyro_data(
                PHIDGET_SPATIAL_VALIDATION_TIMEOUT,
                &interface_data->handle );
    }

    // update diagnostics if succeeded
    if( ret == DTC_NONE )
    {
        ret = psync_get_timestamp( &diagnostics->last_rx_timestamp );
    }


    return ret;
}


//
int recover_dtc_nointerface(
        ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;
    ps_timestamp time_diff = 0;
    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );
    diagnostics_s * const diagnostics = GET_INTERFACE_DIAGNOSTICS( dynamic_driver );
    variables_s * const variables = GET_INTERFACE_VARIABLES( dynamic_driver );
    const ps_dtc dtc = dynamic_driver->current_state->dtc;


    // get time since last retry
    if( diagnostics->last_recovery_attempt == 0 )
    {
        time_diff = PHIDGET_SPATIAL_DIAGNOSTIC_RETRY_INTERVAL;
    }
    else
    {
        ret = util_get_time_since( diagnostics->last_recovery_attempt, &time_diff );
        if( ret != DTC_NONE )
        {
            diagnostics->last_recovery_attempt = 0;
            time_diff = PHIDGET_SPATIAL_DIAGNOSTIC_RETRY_INTERVAL;
        }
    }

    // only retry at our interval
    if( time_diff >= PHIDGET_SPATIAL_DIAGNOSTIC_RETRY_INTERVAL )
    {
        (void) phidget_spatial_close_device( &interface_data->handle );

        // open and configure device handle
        if( ret == DTC_NONE )
        {
            // get/set parameter is in milliseconds for convenience, convert to microseconds
            ret = connect_handle(
                    (ps_timestamp) (variables->update_rate * 1000ULL),
                    &interface_data->handle );
        }

        // validate
        if( ret == DTC_NONE )
        {
            ret = validate_interface( dynamic_driver );
        }

        // recover if we succeeded
        if( ret == DTC_NONE )
        {
            psync_node_recover_fault( dynamic_driver->node_ref, dtc, NODE_STATE_OK );
        }

        // update recovery attempt timestamp
        (void) psync_get_timestamp( &diagnostics->last_recovery_attempt );
    }


    return ret;
}
