/**
 * @file phidget_spatial/src/connect.c
 * @brief Phidget Spatial Connection Utilities Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "polysync_core.h"

#include "interface_data.h"
#include "phidget_spatial_driver.h"
#include "connect.h"




// *****************************************************
// static global types/macros
// *****************************************************




// *****************************************************
// static global data
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


//
int connect_handle(
        const ps_timestamp data_rate,
        phidget_spatial_handle * const handle )
{
    int ret = DTC_NONE;


    if( handle == NULL )
    {
        ret = DTC_USAGE;
    }

    if( ret == DTC_NONE )
    {
        ret = phidget_spatial_open_device( handle );
    }

    if( ret == DTC_NONE )
    {
        ret = phidget_spatial_set_data_rate(
                handle,
                data_rate );
    }

    if( ret == DTC_NONE )
    {
        ret = phidget_spatial_zero_gyroscope( handle );
    }

    if( ret == DTC_NONE )
    {
        const ps_timestamp first_sample_wait = MIN( data_rate, (PHIDGET_SPATIAL_DATARATE_MIN * 1000ULL) );

        ret = psync_sleep_micro( first_sample_wait );
    }


    return ret;
}
