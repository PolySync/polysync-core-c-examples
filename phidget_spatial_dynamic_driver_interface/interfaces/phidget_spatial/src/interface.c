/**
 * @file phidget_spatial/src/interface.c
 * @brief Phidget Spatial Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "interface_data.h"
#include "type_support.h"
#include "options.h"
#include "sdf.h"
#include "getset.h"
#include "read.h"
#include "connect.h"
#include "log.h"
#include "process.h"
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
static void signal_interface_shutdown( void );




// *****************************************************
// static definitions
// *****************************************************


//
static void signal_interface_shutdown( void )
{
    // raise exit signal, wait a little for it to be handled by out node
    (void) raise( SIGINT );
    (void) psync_sleep_micro( 25000 );
}




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
    (*optstring) = PALOPT_STRING;
    (*optchars) = PALOPT_CHARS;
    (*descriptions) = STDOPT_DESCRIPTIONS;
    (*help_strings) = STDOPT_HELP_STRINGS;
    (*num_options) = PALOPT_COUNT;


    return DTC_NONE;
}


//
int psync_interface_expose_parameters(
        const ps_parameter **parameters,
        unsigned long * const num_parameters )
{
    (*parameters) = INTERFACE_SUPPORTED_PARAMETERS;
    (*num_parameters) = NUM_INITIAL_PARAMETERS;


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
    (*interface_name) = INTERFACE_NAME;
    (*module_names) = INTERFACE_MODULES;
    (*num_modules) = NUM_MODULES;
    (*message_type_names) = INTERFACE_MESSAGE_TYPE_NAMES;
    (*num_message_types) = NUM_MESSAGE_TYPE_NAMES;


    return DTC_NONE;
}


//
int psync_interface_expose_sensor_support(
        const ps_sensor_support_descriptor **sensor_support_descriptors,
        unsigned long * const num_descriptors )
{
    (*sensor_support_descriptors) = INTERFACE_SENSOR_DESCRIPTORS;
    (*num_descriptors) = NUM_SENSOR_DESCRIPTORS;


    return DTC_NONE;
}


//
int psync_interface_expose_sdf_support(
        const ps_sdf_node_configuration **sdf_node_configuration )
{
    (*sdf_node_configuration) = &INTERFACE_NODE_CONFIG;


    return DTC_NONE;
}


//
int psync_interface_configure(
        ps_node_configuration_data * const node_config )
{
    int ret = DTC_NONE;


    // create the top-level interface data structure
    interface_data_s * const interface_data = calloc( 1, sizeof(*interface_data) );
    if( interface_data == NULL )
    {
        psync_log_error( "failed to allocate interface data" );
        ret = DTC_MEMERR;
    }

    if( ret == DTC_NONE )
    {
        // set the node name to our interface name
        strncpy(
                node_config->node_name,
                INTERFACE_NAME,
                sizeof(node_config->node_name) );

        // set reference
        node_config->user_data = interface_data;
    }


    return ret;
}



//
int psync_interface_init(
        ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;
    bool interface_valid = PSYNC_FALSE;
    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );
    variables_s * const variables = GET_INTERFACE_VARIABLES( dynamic_driver );


    if( interface_data == NULL )
    {
        psync_log_error( "invalid interface data" );
        ret = DTC_CONFIG;
    }

    if( ret == DTC_NONE )
    {
        ret = parse_options( dynamic_driver );
    }

    if( ret == DTC_NONE  )
    {
        ret = reload_sdf_node_data( dynamic_driver );
    }

    if( ret == DTC_NONE )
    {
        ret = init_type_support( dynamic_driver );
    }

    if( ret == DTC_NONE )
    {
        ret = init_rnr_support( dynamic_driver );
    }

    if( ret == DTC_NONE )
    {
        ret = register_getset_variables( dynamic_driver );
    }

    if( ret == DTC_NONE )
    {
        ret = init_message_meta_data( dynamic_driver );
    }

    // 'test-interface' option output
    if( interface_data->palopts.options[ PALOPT_TEST ] != 0 )
    {
        if( dynamic_driver->standard_options[ STDOPT_NOHW ] == 0 )
        {
            printf( "testing interface '%s'\n", INTERFACE_NAME );
            printf( "opening device handle\n" );
        }
        else
        {
            printf( "cannot perform test on interface when replay mode (no-hardware) option is set\n" );
        }
    }

    // connect to the device if we're in record mode
    if( dynamic_driver->standard_options[ STDOPT_NOHW ] == 0 )
    {
        if( ret == DTC_NONE )
        {
            // get/set parameter is in milliseconds for convenience, convert to microseconds
            ret = connect_handle(
                    (ps_timestamp) (variables->update_rate * 1000ULL),
                    &interface_data->handle );
        }

        // reset return value after we activate a DTC
        if( ret != DTC_NONE )
        {
            psync_log_error(
                    "failed to open device handle - "
                    "check USB connection and SDF configuration" );

            (void) phidget_spatial_close_device( &interface_data->handle );

            // activate DTC
            psync_node_activate_fault(
                    dynamic_driver->node_ref,
                    DTC_NOINTERFACE,
                    NODE_STATE_ERROR );

            // reset return DTC
            ret = DTC_NONE;
        }
    }

    // validate interface if we're in record mode
    if( dynamic_driver->standard_options[ STDOPT_NOHW ] == 0 )
    {
        ret = validate_interface( dynamic_driver );

        // reset return value after we activate a DTC
        if( ret != DTC_NONE )
        {
            psync_log_error(
                    "failed to validate interface - "
                    "check USB connection and SDF configuration" );

            // local flag used for 'test-interface' option printing
            interface_valid = PSYNC_FALSE;

            // activate DTC
            psync_node_activate_fault(
                    dynamic_driver->node_ref,
                    DTC_NOINTERFACE,
                    NODE_STATE_ERROR );

            // reset return DTC
            ret = DTC_NONE;
        }
        else
        {
            // local flag used for 'test-interface' option printing
            interface_valid = PSYNC_TRUE;
        }
    }

    // 'test-interface' option output
    if( interface_data->palopts.options[ PALOPT_TEST ] != 0 )
    {
        if( interface_valid == PSYNC_TRUE )
        {
            printf( "interface test passed\n" );
        }
        else
        {
            printf( "interface test failed\n" );
        }

        signal_interface_shutdown();
    }


    return ret;
}


//
int psync_interface_release(
        ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;
    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );


    if( interface_data == NULL )
    {
        ret = DTC_USAGE;
    }

    if( ret == DTC_NONE)
    {
        (void) phidget_spatial_close_device( &interface_data->handle );

        (void) release_type_support( dynamic_driver );

        (void) psync_sdf_release_node_configuration( &dynamic_driver->node_sdf_config );

        free( interface_data );
        dynamic_driver->interface_data = NULL;
    }


    return ret;
}


//
int psync_interface_poll_for_data(
        ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;
    message_set_s * const message_set = GET_INTERFACE_MESSAGE_SET( dynamic_driver );


    if( dynamic_driver->logfile_mode == LOGFILE_MODE_READ )
    {
        ret = read_replay_data(
                dynamic_driver,
                (ps_imu_msg*) message_set->imu_msg );
    }
    else
    {
        ret = read_live_data(
                dynamic_driver,
                (ps_imu_msg*) message_set->imu_msg );
    }


    return ret;
}


//
int psync_interface_log_data(
        ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;
    message_set_s * const message_set = GET_INTERFACE_MESSAGE_SET( dynamic_driver );


    // log the IMU message if we're in record mode and actively recording
    ret = log_imu_msg(
            (ps_imu_msg*) message_set->imu_msg,
            dynamic_driver );


    return ret;
}


//
int psync_interface_process_data(
        ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;
    message_set_s * const message_set = GET_INTERFACE_MESSAGE_SET( dynamic_driver );


    ret = process_imu_msg(
            dynamic_driver,
            (ps_imu_msg*) message_set->imu_msg );


    return ret;
}


//
int psync_interface_idle(
        ps_dynamic_driver * const dynamic_driver )
{
    // do nothing
    return DTC_NONE;
}


//
int psync_interface_handle_parameter_command(
        ps_dynamic_driver * const dynamic_driver,
        const ps_parameter_message_kind command,
        const ps_parameter * const command_param,
        ps_parameter * const node_param )
{
    // do nothing
    return DTC_NONE;
}


//
int psync_interface_handle_state(
        ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;
    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );
    const ps_node_state_kind node_state = dynamic_driver->current_state->node_state;
    const ps_dtc dtc = dynamic_driver->current_state->dtc;


    // enforce sleep interval
    (void) psync_sleep_micro( PHIDGET_SPATIAL_DIAGNOSTIC_SLEEP_INTERVAL );

    if( (dynamic_driver != NULL) && (interface_data != NULL) )
    {
        // only recover in record-mode
        if( dynamic_driver->standard_options[ STDOPT_NOHW ] == 0 )
        {
            // ignore FATAL and OK state
            if( (node_state != NODE_STATE_OK) && (node_state != NODE_STATE_FATAL) )
            {
                if( dtc == DTC_NOINTERFACE )
                {
                    // attempt to recover, don't care about return since
                    // the call will do the recover calls
                    ret = recover_dtc_nointerface( dynamic_driver );
                }
            }
        }
    }

    // always return OK
    ret = DTC_NONE;


    return ret;
}
