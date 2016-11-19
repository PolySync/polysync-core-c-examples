/**
 * @file example/src/interface.c
 * @brief Example PolySync Interface Source.
 *
 * The example interface is used to provide an example PAL layer.
 *
 * It expects a simple SDF configuration similar to a simple CAN sensor, ie
 * \li Expects a valid node with some parameters
 * \li Expects a single sensor configuration under the node with some parameters
 * \li Expects a single IO configuration under the sensor with some parameters
 *
 * You can use the SDF configurator to configure a dynamic driver to use this example
 * after you run 'make install'.
 *
 * See \ref polysync_dynamic_interface.h for more information regarding the
 * interface.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "polysync_core.h"
#include "polysync_node.h"
#include "polysync_sdf.h"
#include "util.h"
#include "polysync_dynamic_interface.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief Custom parameter identifier used by the node.
 *
 */
#define CUSTOM_PARAM_ID (9999999)


/**
 * @brief Custom parameter description string used by the node.
 *
 */
#define CUSTOM_PARAM_DESCRIPTION "Custom parameter description string"




/**
 * @brief PAL interface options.
 *
 */
enum
{
    //
    // reserves '-t'
    PALOPT_TEST = 0,
    //
    //
    PALOPT_COUNT /*!< Number of PAL interface command line options. */
};




// *****************************************************
// static global data
// *****************************************************


/**
 * @brief PAL interface name.
 *
 * For PolySync provided nodes, this should match what the licensing framework uses.
 *
 */
static const char INTERFACE_NAME[] = "example-interface";


/**
 * @brief PAL interface description.
 *
 * Generic human readable description for the interface.
 *
 */
static const char INTERFACE_DESCRIPTION[] = "Example PAL Interface";


/**
 * @brief PAL interface options string.
 *
 */
static const char PALOPT_STRING[] = "t";


/**
 * @brief PAL interface options character list.
 *
 * @note Order defines the indexing scheme.
 *
 */
static const char PALOPT_CHARS[] =
{
    [PALOPT_TEST] = 't',
    '\0'
};


/**
 * @brief PAL interface options description list.
 *
 * @note Order must match the indexing scheme in \ref PALOPT_CHARS.
 *
 */
static const char * const STDOPT_DESCRIPTIONS[] =
{
    [PALOPT_TEST] = "test option in the test interface [optional]",
    NULL
};


/**
 * @brief PAL interface options help message list.
 *
 * @note Order must match the indexing scheme in \ref PALOPT_CHARS.
 *
 */
static const char * const STDOPT_HELP_STRINGS[] =
{
    [PALOPT_TEST] = "-t",
    NULL
};


/**
 * @brief Data model modules used by the PAL interface.
 *
 */
#define NUM_INTERFACE_MODULES (1)
static const char * const INTERFACE_MODULES[] =
{
    "core",
    NULL
};


/**
 * @brief Message type names used by the PAL interface.
 *
 * Normally this would be things not in the core module, but for the
 * test interface, it provides an example using a core message type.
 *
 */
#define NUM_MESSAGE_TYPE_NAMES (1)
static const char * const INTERFACE_MESSAGE_TYPE_NAMES[] =
{
    "ps_rnr_msg",
    NULL
};


/**
 * @brief Sensor descriptors used by the PAL interface.
 *
 * Provides a list of sensor descriptors this interface uses.
 * Also used by the node to re-generate missing SDF support.
 *
 */
#define NUM_INTERFACE_SENSOR_DESCRIPTORS (sizeof(INTERFACE_SENSOR_DESCRIPTORS)/sizeof(INTERFACE_SENSOR_DESCRIPTORS[0]))
static const ps_sensor_support_descriptor INTERFACE_SENSOR_DESCRIPTORS[] =
{
    {
        .name = INTERFACE_NAME,
        .type = PSYNC_SENSOR_KIND_NOT_AVAILABLE,
        .description = INTERFACE_DESCRIPTION,
        .support_link = NULL
    }
};


/**
 * @brief SDF IO configuration parameter values.
 *
 * This is a low-level structure used to organize the SDF parameter information
 * used by the IO configuration.
 *
 * Parameter ID's and value types used by the IO configuration.
 *
 */
#define NUM_INTERFACE_IO_PARAMETERS (sizeof(INTERFACE_IO_PARAMETERS)/sizeof(INTERFACE_IO_PARAMETERS[0]))
static const ps_sdf_parameter_value INTERFACE_IO_PARAMETERS[] =
{
    //
    //
    PSYNC_INIT_SDF_PARAMETER_TYPE(
            PSYNC_PARAM_ID_CAN0_HW_ID,
            PARAMETER_VALUE_ULONGLONG ),
    //
    //
    PSYNC_INIT_SDF_PARAMETER_TYPE(
            PSYNC_PARAM_ID_CAN0_CC_ID,
            PARAMETER_VALUE_ULONGLONG )
};


/**
 * @brief SDF IO configurations used by the node configuration.
 *
 * This is a mid-level structure used to organize the the SDF IO configurations
 * used by the sensor configuration given to the PAL.
 *
 * @note This structure is comprised of data from other static/const elements.
 *
 */
#define NUM_INTERFACE_IO_CONFIGS (sizeof(INTERFACE_IO_CONFIGS)/sizeof(INTERFACE_IO_CONFIGS[0]))
static const ps_sdf_io_configuration INTERFACE_IO_CONFIGS[] =
{
    //
    //
    PSYNC_INIT_SDF_IO_CONFIGURATION(
            NUM_INTERFACE_IO_PARAMETERS,
            (ps_sdf_parameter_value*) INTERFACE_IO_PARAMETERS )
};


/**
 * @brief SDF sensor configuration parameter values.
 *
 * This is a low-level structure used to organize the SDF parameter information
 * used by the sensor configuration.
 *
 * Parameter ID's and value types used by the sensor configuration.
 *
 */
#define NUM_INTERFACE_SENSOR_PARAMETERS (sizeof(INTERFACE_SENSOR_PARAMETERS)/sizeof(INTERFACE_SENSOR_PARAMETERS[0]))
static const ps_sdf_parameter_value INTERFACE_SENSOR_PARAMETERS[] =
{
    //
    //
    PSYNC_INIT_SDF_PARAMETER_TYPE(
            PSYNC_PARAM_ID_SENSOR0_ID,
            PARAMETER_VALUE_ULONGLONG )
};


/**
 * @brief SDF sensor configurations used by the node configuration.
 *
 * This is a mid-level structure used to organize the the SDF sensor configurations
 * used by the node configuration given to the PAL.
 *
 * @note This structure is comprised of data from other static/const elements.
 *
 */
#define NUM_INTERFACE_SENSOR_CONFIGS (sizeof(INTERFACE_SENSOR_CONFIGS)/sizeof(INTERFACE_SENSOR_CONFIGS[0]))
static const ps_sdf_sensor_configuration INTERFACE_SENSOR_CONFIGS[] =
{
    //
    //
    PSYNC_INIT_SDF_SENSOR_CONFIGURATION(
            NUM_INTERFACE_IO_CONFIGS,
            (ps_sdf_io_configuration*) INTERFACE_IO_CONFIGS,
            NUM_INTERFACE_SENSOR_PARAMETERS,
            (ps_sdf_parameter_value*) INTERFACE_SENSOR_PARAMETERS )
};


/**
 * @brief SDF node configuration parameter values.
 *
 * This is a low-level structure used to organize the SDF parameter information
 * used by the node configuration.
 *
 * Parameter ID's and value types used by the node configuration.
 *
 */
#define NUM_INTERFACE_NODE_PARAMETERS (sizeof(INTERFACE_NODE_PARAMETERS)/sizeof(INTERFACE_NODE_PARAMETERS[0]))
static const ps_sdf_parameter_value INTERFACE_NODE_PARAMETERS[] =
{
    //
    //
    PSYNC_INIT_SDF_PARAMETER_TYPE(
            PSYNC_PARAM_ID_COORDINATE_FRAME,
            PARAMETER_VALUE_ULONGLONG ),
    //
    //
    PSYNC_INIT_SDF_PARAMETER_TYPE(
            CUSTOM_PARAM_ID,
            PARAMETER_VALUE_ULONGLONG )
};


/**
 * @brief SDF node configuration expected by the PAL layer.
 *
 * This is the top-level structure provided to the node support mechanism.
 * It provides the node with a minimum SDF configuration required by the interface.
 *
 * This structure provides the relationship of elements and their parameters/other elements.
 *
 * The node will checks the following conditions:
 * \li Elements that are enabled here should be enabled in the SDF.
 * \li Elements with parameters here should be in the SDF, checks for ID and value type.
 *
 */
static const ps_sdf_node_configuration INTERFACE_NODE_CONFIG =
PSYNC_INIT_SDF_NODE_CONFIGURATION(
        NUM_INTERFACE_SENSOR_CONFIGS,
        (ps_sdf_sensor_configuration*) INTERFACE_SENSOR_CONFIGS,
        NUM_INTERFACE_NODE_PARAMETERS,
        (ps_sdf_parameter_value*) INTERFACE_NODE_PARAMETERS );


/**
 * @brief Initial/startup parameters used by the PAL interface.
 *
 * Provides a list of parameters (and their type/values) this interface expects on startup.
 * Also used by the node to re-generate missing SDF support.
 * This is NOT used to get/set runtime parameters.
 *
 * The use of \ref PSYNC_PARAMETER_FLAG_DISABLED, means that the node supports the parameter
 * but does not actively use it.
 *
 */
#define NUM_INTERFACE_SUPPORTED_PARAMETERS (sizeof(INTERFACE_SUPPORTED_PARAMETERS)/sizeof(INTERFACE_SUPPORTED_PARAMETERS[0]))
static const ps_parameter INTERFACE_SUPPORTED_PARAMETERS[] =
{
    //
    // default local coordinate frame identifier
    PSYNC_INIT_SDF_CORE_PARAMETER(
            PSYNC_PARAM_ID_COORDINATE_FRAME,
            0,
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL(PSYNC_COORDINATE_FRAME_PLATFORM),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA() ),
    //
    // sensor 0 ID, default is NA
    PSYNC_INIT_SDF_CORE_PARAMETER(
            PSYNC_PARAM_ID_SENSOR0_ID,
            0,
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL(PSYNC_IDENTIFIER_NOT_AVAILABLE),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA() ),
    //
    // CAN0 hardware ID
    PSYNC_INIT_SDF_CORE_PARAMETER(
            PSYNC_PARAM_ID_CAN0_HW_ID,
            0,
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL(0),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA() ),
    //
    // CAN0 circuit ID
    PSYNC_INIT_SDF_CORE_PARAMETER(
            PSYNC_PARAM_ID_CAN0_CC_ID,
            0,
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL(0),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA() ),
    //
    // custom parameter
    PSYNC_INIT_SDF_CUSTOM_PARAMETER(
            CUSTOM_PARAM_ID,
            0,
            PSYNC_INIT_SDF_PARAMETER_VALUE_ULL(0),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            PSYNC_INIT_SDF_PARAMETER_VALUE_NA(),
            CUSTOM_PARAM_DESCRIPTION )
};




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
int psync_interface_expose_options(
        const char **optstring,
        const char **optchars,
        const char * const **descriptions,
        const char * const **help_strings,
        unsigned long * const num_options )
{
    // set references
    (*optstring) = PALOPT_STRING;
    (*optchars) = PALOPT_CHARS;
    (*descriptions) = STDOPT_DESCRIPTIONS;
    (*help_strings) = STDOPT_HELP_STRINGS;
    (*num_options) = PALOPT_COUNT;


    return DTC_NONE;
}


//
int psync_interface_expose_parameters( const ps_parameter **parameters, unsigned long * const num_parameters )
{
    // set references
    (*parameters) = INTERFACE_SUPPORTED_PARAMETERS;
    (*num_parameters) = NUM_INTERFACE_SUPPORTED_PARAMETERS;


    return DTC_NONE;
}


//
int psync_interface_expose_type_support(
        const char **interface_name,
        const char * const **module_names,
        unsigned long * const num_modules,
        const char * const **message_type_names,
        unsigned long * const num_message_types )
{
    // set references
    (*interface_name) = INTERFACE_NAME;
    (*module_names) = INTERFACE_MODULES;
    (*num_modules) = NUM_INTERFACE_MODULES;
    (*message_type_names) = INTERFACE_MESSAGE_TYPE_NAMES;
    (*num_message_types) = NUM_MESSAGE_TYPE_NAMES;


    return DTC_NONE;
}


//
int psync_interface_expose_sensor_support(
        const ps_sensor_support_descriptor **sensor_support_descriptors,
        unsigned long * const num_descriptors )
{
    // set references
    (*sensor_support_descriptors) = INTERFACE_SENSOR_DESCRIPTORS;
    (*num_descriptors) = NUM_INTERFACE_SENSOR_DESCRIPTORS;


    return DTC_NONE;
}


//
int psync_interface_expose_sdf_support( const ps_sdf_node_configuration **sdf_node_configuration )
{
    // set references
    (*sdf_node_configuration) = &INTERFACE_NODE_CONFIG;


    return DTC_NONE;
}


//
int psync_interface_configure( ps_node_configuration_data * const node_config )
{
    psync_log_debug( "called psync_interface_configure" );


    return DTC_NONE;
}


//
int psync_interface_init( ps_dynamic_driver * const dynamic_driver )
{
    // local vars
    int ret = DTC_NONE;


    psync_log_debug( "called psync_interface_init" );

	// lookup SDF node configuration using node's environment
    if( (ret = psync_sdf_get_node_configuration(
            dynamic_driver->node_ref,
            dynamic_driver->node_config->sdf_key,
            &dynamic_driver->node_sdf_config,
            PSYNC_SDF_AUTOFILL_ON )) != DTC_NONE )
    {
        return ret;
    }


    return DTC_NONE;
}


//
int psync_interface_release( ps_dynamic_driver * const dynamic_driver )
{
    psync_log_debug( "called psync_interface_release" );


    return DTC_NONE;
}


//
int psync_interface_poll_for_data( ps_dynamic_driver * const dynamic_driver )
{
    // this is used to 'poll' for incoming/new data to be processed


    // return DTC_NONE when data is available for processing
    // return DTC_NONE;

    // return DTC_UNAVAILABLE when data is not available
    return DTC_UNAVAILABLE;
}


//
int psync_interface_log_data( ps_dynamic_driver * const dynamic_driver )
{
    // called when after psync_interface_poll_for_data if data is available

    // this is where you log incoming data if desired


    return DTC_NONE;
}


//
int psync_interface_process_data( ps_dynamic_driver * const dynamic_driver )
{
    // called after psync_interface_log_data, if there is data available

    // this is where you process and abstract your 'raw' data into PolySync message types


    return DTC_NONE;
}


//
int psync_interface_idle( ps_dynamic_driver * const dynamic_driver )
{
    // sleep for a little while so our example doesn't load the CPU
	(void) psync_sleep_micro( 10000 );


    return DTC_NONE;
}


//
int psync_interface_handle_parameter_command(
        ps_dynamic_driver * const dynamic_driver,
        const ps_parameter_message_kind command,
        const ps_parameter * const command_param,
        ps_parameter * const node_param )
{
    int ret = DTC_NONE;

    // handle get/set operations

    psync_log_debug( "called psync_interface_handle_parameter_command" );

    // check for our custom parameter
    if( command_param->id == CUSTOM_PARAM_ID )
    {
        psync_log_debug( "changing custom parameter from %llu to %llu",
                node_param->value._u.ull_value,
                command_param->value._u.ull_value );

        // apply change
        if( (ret = util_copy_parameter( command_param, node_param )) != DTC_NONE )
        {
            return ret;
        }
    }
    else
    {
        // don't handle anything else
        psync_log_debug( "ignoring update to parameter ID %llu", (unsigned long long) command_param->id );
    }


    // returning DTC_NONE means the get/set operation is accepted
    return DTC_NONE;
}


//
int psync_interface_handle_state( ps_dynamic_driver * const dynamic_driver )
{
    // handle node state changes

    psync_log_debug( "PAL handle state '%s'", PSYNC_STATE_KIND_STRING_TABLE[ dynamic_driver->current_state->node_state ] );


    return DTC_NONE;
}
