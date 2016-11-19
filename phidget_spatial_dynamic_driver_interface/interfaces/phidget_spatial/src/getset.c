/**
 * @file phidget_spatial/src/getset.c
 * @brief Phidget Spatial Get/Set Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "polysync_core.h"
#include "polysync_getset.h"

#include "interface_data.h"
#include "sdf.h"
#include "getset.h"




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
int register_getset_variables( ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;
    variables_s * const variables = GET_INTERFACE_VARIABLES( dynamic_driver );


    if( dynamic_driver == NULL )
    {
        ret = DTC_USAGE;
    }

    if( variables == NULL )
    {
        ret = DTC_USAGE;
    }

    if( ret == DTC_NONE )
    {
        ret = psync_getset_register_variable(
                dynamic_driver->node_ref,
                PSYNC_PARAM_ID_SERIAL0_NAME,
                0,
                GETSET_VARIABLE_TYPE_CSTRING,
                variables->serial_0_name,
                sizeof(variables->serial_0_name) );
    }

    if( ret == DTC_NONE )
    {
        ret = psync_getset_register_variable(
                dynamic_driver->node_ref,
                PSYNC_PARAM_ID_SENSOR0_ID,
                0,
                GETSET_VARIABLE_TYPE_UNSIGNED_LONG_LONG,
                &variables->sensor_0_id,
                sizeof(variables->sensor_0_id) );
    }

    if( ret == DTC_NONE )
    {
        ret = psync_getset_register_variable(
                dynamic_driver->node_ref,
                PSYNC_PARAM_ID_COORDINATE_FRAME,
                0,
                GETSET_VARIABLE_TYPE_UNSIGNED_LONG_LONG,
                &variables->coordinate_frame,
                sizeof(variables->coordinate_frame) );
    }

    if( ret == DTC_NONE )
    {
        ret = psync_getset_register_variable(
                dynamic_driver->node_ref,
                PHIDGET_SPATIAL_PARAM_ID_UPDATE_RATE,
                0,
                GETSET_VARIABLE_TYPE_UNSIGNED_LONG_LONG,
                &variables->update_rate,
                sizeof(variables->update_rate) );
    }

    if( ret == DTC_NONE )
    {
        ret = psync_getset_register_variable(
                dynamic_driver->node_ref,
                PARAM_ID_PUB_IMU,
                0,
                GETSET_VARIABLE_TYPE_BOOLEAN,
                &variables->publishes_imu,
                sizeof(variables->publishes_imu) );
    }

    if( ret == DTC_NONE )
    {
        ret = psync_getset_update_variable_values( dynamic_driver->node_ref );
    }


    return ret;
}
