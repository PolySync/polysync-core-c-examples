/**
 * @file phidget_spatial/src/sdf.c
 * @brief Phidget Spatial SDF Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_dynamic_interface.h"
#include "polysync_interface_util.h"

#include "interface_data.h"
#include "type_support.h"
#include "options.h"
#include "sdf.h"




// *****************************************************
// static global types/macros
// *****************************************************

//
#define SDF_ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))




// *****************************************************
// public global data
// *****************************************************


/**
 * @brief Sensor descriptors used by the PAL interface.
 *
 * Provides a list of sensor descriptors this interface uses.
 * Also used by the node to re-generate missing SDF support.
 *
 * See \ref NUM_SENSOR_DESCRIPTORS for number of elements.
 *
 */
const ps_sensor_support_descriptor INTERFACE_SENSOR_DESCRIPTORS[] =
{
    {
        .name = INTERFACE_NAME,
        .type = PHIDGET_SPATIAL_PS_SENSOR_KIND,
        .description = INTERFACE_DESCRIPTION,
        .support_link = INTERFACE_SUPPORT_LINK
    }
};


/**
 * @brief Number of sensor descriptors used by the PAL interface.
 *
 * Defines the number of elements in \ref INTERFACE_SENSOR_DESCRIPTORS.
 *
 */
const unsigned long NUM_SENSOR_DESCRIPTORS =
        (unsigned long) SDF_ARRAY_LENGTH( INTERFACE_SENSOR_DESCRIPTORS );


/**
 * @brief SDF IO configuration parameter values.
 *
 * This is a low-level structure used to organize the SDF parameter information
 * used by the IO configuration.
 *
 * Parameter ID's and value types used by the IO configuration.
 *
 * @note Only parameter ID and value type are required.
 *
 */
const ps_sdf_parameter_value INTERFACE_IO_PARAMETERS[] =
{
    //
    //
    PSYNC_INIT_SDF_PARAMETER_TYPE(
            PSYNC_PARAM_ID_SERIAL0_NAME,
            PARAMETER_VALUE_STRING ),
};


//
const ps_sdf_io_configuration INTERFACE_IO_CONFIGS[] =
{
    //
    //
    PSYNC_INIT_SDF_IO_CONFIGURATION(
            SDF_ARRAY_LENGTH( INTERFACE_IO_PARAMETERS ),
            (ps_sdf_parameter_value*) INTERFACE_IO_PARAMETERS )
};


//
const ps_sdf_parameter_value INTERFACE_SENSOR_PARAMETERS[] =
{
    //
    //
    PSYNC_INIT_SDF_PARAMETER_TYPE(
            PSYNC_PARAM_ID_SENSOR0_ID,
            PARAMETER_VALUE_ULONGLONG ),
    //
    //
    PSYNC_INIT_SDF_PARAMETER_TYPE(
            PHIDGET_SPATIAL_PARAM_ID_UPDATE_RATE,
            PARAMETER_VALUE_ULONGLONG )
};


//
const ps_sdf_sensor_configuration INTERFACE_SENSOR_CONFIGS[] =
{
    //
    //
    PSYNC_INIT_SDF_SENSOR_CONFIGURATION(
            SDF_ARRAY_LENGTH( INTERFACE_IO_CONFIGS ),
            (ps_sdf_io_configuration*) INTERFACE_IO_CONFIGS,
            SDF_ARRAY_LENGTH( INTERFACE_SENSOR_PARAMETERS ),
            (ps_sdf_parameter_value*) INTERFACE_SENSOR_PARAMETERS )
};


//
const ps_sdf_parameter_value INTERFACE_NODE_PARAMETERS[] =
{
    //
    //
    PSYNC_INIT_SDF_PARAMETER_TYPE(
            PSYNC_PARAM_ID_COORDINATE_FRAME,
            PARAMETER_VALUE_ULONGLONG ),
    //
    //
    PSYNC_INIT_SDF_PARAMETER_TYPE(
            PARAM_ID_PUB_IMU,
            PARAMETER_VALUE_ULONGLONG )
};


//
const ps_sdf_node_configuration INTERFACE_NODE_CONFIG =
PSYNC_INIT_SDF_NODE_CONFIGURATION(
        SDF_ARRAY_LENGTH( INTERFACE_SENSOR_CONFIGS ),
        (ps_sdf_sensor_configuration*) INTERFACE_SENSOR_CONFIGS,
        SDF_ARRAY_LENGTH( INTERFACE_NODE_PARAMETERS ),
        (ps_sdf_parameter_value*) INTERFACE_NODE_PARAMETERS );


//
const ps_parameter INTERFACE_SUPPORTED_PARAMETERS[] =
{
    //
    //
    PSYNC_INIT_SDF_CORE_PARAMETER(
            PSYNC_PARAM_ID_SERIAL0_NAME,
            PSYNC_PARAMETER_FLAG_READONLY_VALUE,
            PSYNC_INIT_SDF_PARAMETER_VALUE_STRING( "/dev/ttyUSB0" ),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA() ),
    //
    //
    PSYNC_INIT_SDF_CORE_PARAMETER(
            PSYNC_PARAM_ID_SENSOR0_ID,
            PSYNC_PARAMETER_FLAG_READONLY_VALUE,
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL( PSYNC_IDENTIFIER_NOT_AVAILABLE ),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA() ),
    //
    //
    PSYNC_INIT_SDF_CORE_PARAMETER(
            PSYNC_PARAM_ID_COORDINATE_FRAME,
            0,
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL( PSYNC_COORDINATE_FRAME_PLATFORM ),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA() ),
    //
    //
    PSYNC_INIT_SDF_CUSTOM_PARAMETER(
            PARAM_ID_PUB_IMU,
            0,
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL(1),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PARAM_DESC_PUB_IMU ),
    //
    // max/min refer to the update rate, not the value
    PSYNC_INIT_SDF_CUSTOM_PARAMETER(
            PHIDGET_SPATIAL_PARAM_ID_UPDATE_RATE,
            PSYNC_PARAMETER_FLAG_READONLY_VALUE,
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL( 56 ),
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL( PHIDGET_SPATIAL_DATARATE_MAX ),
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL( PHIDGET_SPATIAL_DATARATE_MIN ),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PARAM_DESC_PHIDGET_SPATIAL_UPDATE_RATE )
};


//
const unsigned long NUM_INITIAL_PARAMETERS =
        (unsigned long) sizeof(INTERFACE_SUPPORTED_PARAMETERS) / sizeof(INTERFACE_SUPPORTED_PARAMETERS[0]);




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
int reload_sdf_node_data( ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;


    ret = psync_sdf_get_node_configuration(
            dynamic_driver->node_ref,
            dynamic_driver->node_config->sdf_key,
            &dynamic_driver->node_sdf_config,
            PSYNC_SDF_AUTOFILL_ON );


    return ret;
}
