/**
 * @file phidget_spatial/src/type_support.c
 * @brief Phidget Spatial Generic Type Support Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "polysync_core.h"
#include "polysync_message.h"
#include "polysync_logfile.h"
#include "polysync_dynamic_interface.h"
#include "polysync_interface_util.h"
#include "interface_data.h"
#include "type_support.h"




// *****************************************************
// static global types/macros
// *****************************************************




// *****************************************************
// public global data
// *****************************************************


/**
 * @brief PAL interface name.
 *
 * For PolySync provided nodes, this should match what the licensing framework uses.
 *
 */
const char INTERFACE_NAME[] = "phidget-spatial";


/**
 * @brief PAL interface description.
 *
 * Generic human readable description for the interface.
 *
 */
const char INTERFACE_DESCRIPTION[] = "Phidget Spatial Interface";



/**
 * @brief PAL interface knowledge base link.
 *
 * Link to the online documentation for this device
 *
 */
const char INTERFACE_SUPPORT_LINK[] = "N/A";


/**
 * @brief Data model modules used by the PAL interface.
 *
 * See \ref NUM_MODULES for number of elements.
 *
 */
const char * const INTERFACE_MODULES[] =
{
    "core",
    "sensor",
    NULL
};


/**
 * @brief Message type names used by the PAL interface.
 *
 * See \ref NUM_MESSAGE_TYPE_NAMES for number of elements.
 *
 */
const char * const INTERFACE_MESSAGE_TYPE_NAMES[] =
{
    IMU_MSG_NAME,
    NULL
};


/**
 * @brief IMU message message type name.
 *
 */
const char IMU_MSG_NAME[] = "ps_imu_msg";




// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************

//
static int get_message_type(
        ps_node_ref node_ref,
        const char * const msg_name,
        ps_msg_type * const msg_type );


//
static int get_message_and_type(
        ps_node_ref node_ref,
        const char * const msg_name,
        ps_msg_type * const msg_type,
        ps_msg_ref * msg_ref_ptr );


//
static void set_default_imu_msg(
        const variables_s * const variables,
        ps_imu_msg * const msg );




// *****************************************************
// static definitions
// *****************************************************


//
static int get_message_type( ps_node_ref node_ref,
                             const char * const msg_name,
                             ps_msg_type * const msg_type )
{
    int ret = DTC_NONE;


    ret = psync_message_get_type_by_name( node_ref, msg_name, msg_type );

    if( ret == DTC_TYPESUPPORT )
    {
        psync_log_error( "missing data model support for message type '%s'", msg_name );
    }


    return ret;
}


//
static int get_message_and_type( ps_node_ref node_ref,
                                 const char * const msg_name,
                                 ps_msg_type * const msg_type,
                                 ps_msg_ref * msg_ref_ptr )
{
    int ret = DTC_NONE;


    ret = get_message_type( node_ref, msg_name, msg_type );

    if( ret == DTC_NONE )
    {
        ret = psync_message_alloc( node_ref, (*msg_type), msg_ref_ptr );
    }


    return ret;
}


//
static void set_default_sensor_descriptor(
        const variables_s * const variables,
        ps_sensor_descriptor * const sensor_descriptor )
{
    if( (sensor_descriptor != NULL) && (variables != NULL) )
    {
        sensor_descriptor->id = (ps_identifier) variables->sensor_0_id;
        sensor_descriptor->type = PHIDGET_SPATIAL_PS_SENSOR_KIND;
        sensor_descriptor->transform.parent_id = (ps_identifier) variables->coordinate_frame;
    }
}


//
static void set_default_imu_msg(
        const variables_s * const variables,
        ps_imu_msg * const msg )
{
    if( msg != NULL )
    {
        set_default_sensor_descriptor( variables, &msg->sensor_descriptor );

        msg->orientation[0] = PSYNC_ORIENTATION_NOT_AVAILABLE;
        msg->orientation[1] = PSYNC_ORIENTATION_NOT_AVAILABLE;
        msg->orientation[2] = PSYNC_ORIENTATION_NOT_AVAILABLE;
        msg->orientation[3] = PSYNC_ORIENTATION_NOT_AVAILABLE;

        msg->rotation_rate[0] = PSYNC_ROTATION_RATE_NOT_AVAILABLE;
        msg->rotation_rate[1] = PSYNC_ROTATION_RATE_NOT_AVAILABLE;
        msg->rotation_rate[2] = PSYNC_ROTATION_RATE_NOT_AVAILABLE;

        msg->velocity[0] = PSYNC_VELOCITY_NOT_AVAILABLE;
        msg->velocity[1] = PSYNC_VELOCITY_NOT_AVAILABLE;
        msg->velocity[2] = PSYNC_VELOCITY_NOT_AVAILABLE;

        msg->acceleration[0] = PSYNC_ACCELERATION_NOT_AVAILABLE;
        msg->acceleration[1] = PSYNC_ACCELERATION_NOT_AVAILABLE;
        msg->acceleration[2] = PSYNC_ACCELERATION_NOT_AVAILABLE;
    }
}




// *****************************************************
// public definitions
// *****************************************************


//
int init_type_support( ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;


    if( dynamic_driver == NULL )
    {
        ret = DTC_USAGE;
    }

    message_set_s * const message_set = GET_INTERFACE_MESSAGE_SET( dynamic_driver );

    if( message_set == NULL )
    {
        ret = DTC_USAGE;
    }

    if( ret == DTC_NONE )
    {
        ret = get_message_and_type( dynamic_driver->node_ref,
                                    IMU_MSG_NAME,
                                    &message_set->imu_msg_type,
                                    &message_set->imu_msg );
    }


    return ret;
}


//
int release_type_support( ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;


    if( dynamic_driver == NULL )
    {
        ret = DTC_USAGE;
    }

    message_set_s * const message_set = GET_INTERFACE_MESSAGE_SET( dynamic_driver );

    if( message_set == NULL )
    {
        ret = DTC_USAGE;
    }

    if( ret == DTC_NONE )
    {
        ret = psync_message_free( dynamic_driver->node_ref,
                                  &message_set->imu_msg );
    }


    return ret;
}


//
int init_rnr_support( ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;


    if( dynamic_driver == NULL )
    {
        ret = DTC_USAGE;
    }

    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );
    message_set_s * const message_set = GET_INTERFACE_MESSAGE_SET( dynamic_driver );

    if( (interface_data == NULL) || (message_set == NULL) )
    {
        ret = DTC_USAGE;
    }

    // replay publisher filter
    // we don't want the logfile API to publish the IMU messages contained in the log
    // instead we will have the API push them into a queue for our application to
    // handle
    ps_msg_type replay_filter_list[1] =
    {
        message_set->imu_msg_type
    };

    // get reference to replay message queue
    if( ret == DTC_NONE )
    {
        ret = psync_logfile_get_replay_msg_queue(
                dynamic_driver->node_ref,
                &interface_data->replay_msg_queue );
    }

    if( ret == DTC_NONE )
    {
        ret = psync_logfile_set_message_type_filters(
                dynamic_driver->node_ref,
                NULL,
                0,
                replay_filter_list,
                1 );
    }

    if( ret == DTC_NONE )
    {
        ret = psync_logfile_enable_output_queue(
                dynamic_driver->node_ref,
                1 );
    }


    return ret;
}


//
int init_message_meta_data( ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;


    if( dynamic_driver == NULL )
    {
        ret = DTC_USAGE;
    }

    message_set_s * const message_set = GET_INTERFACE_MESSAGE_SET( dynamic_driver );
    variables_s * const variables = GET_INTERFACE_VARIABLES( dynamic_driver );

    if( (message_set == NULL) || (variables == NULL) )
    {
        ret = DTC_USAGE;
    }

    if( ret == DTC_NONE )
    {
        if( message_set->imu_msg != PSYNC_MSG_REF_INVALID )
        {
            set_default_imu_msg(
                variables,
                (ps_imu_msg*) message_set->imu_msg );
        }
    }


    return ret;
}
