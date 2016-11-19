/**
 * @file phidget_spatial/src/read.c
 * @brief Phidget Spatial Data Reader Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "polysync_core.h"
#include "polysync_can.h"

#include "interface_data.h"
#include "polysync_dynamic_interface.h"
#include "polysync_interface_util.h"
#include "phidget_spatial_driver.h"
#include "read.h"




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


//
static int read_data_on_interval(
        ps_dynamic_driver * const dynamic_driver,
        ps_imu_msg * const msg )
{
    int ret = DTC_NONE;
    ps_timestamp time_diff = 0;
    bool read_new_sample = PSYNC_FALSE;
    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );
    variables_s * const variables = GET_INTERFACE_VARIABLES( dynamic_driver );
    const ps_timestamp interval = (variables->update_rate * 1000ULL);

    // get time since last update
    ret = util_get_time_since( interface_data->last_sample_update, &time_diff );

    if( ret == DTC_NONE )
    {
        if( time_diff >= interval )
        {
            read_new_sample = PSYNC_TRUE;
        }
    }

    if( read_new_sample == PSYNC_TRUE )
    {
        // update rx timestamp
        if( ret == DTC_NONE )
        {
            ret = psync_get_timestamp( &msg->timestamp );

            // we use the rx timestamp for replaying the data in time
            // the replay module will look at the header timestamp by default
            msg->header.timestamp = msg->timestamp;

            // update our local sample rate variable
            interface_data->last_sample_update = msg->timestamp;
        }

        // read acceleration vector
        if( ret == DTC_NONE )
        {
            ret = phidget_spatial_get_acceleration(
                    &interface_data->handle,
                    &msg->acceleration[ X_INDEX ],
                    &msg->acceleration[ Y_INDEX ],
                    &msg->acceleration[ Z_INDEX ] );

            // convert g to meter/second^2, aligned to the PolySync coordinate system
            const double g1 = 9.80665;
            msg->acceleration[ X_INDEX ] *= g1;
            msg->acceleration[ Y_INDEX ] *= -g1;
            msg->acceleration[ Z_INDEX ] *= -g1;
        }

        // read gyroscope vector
        if( ret == DTC_NONE )
        {
            ret = phidget_spatial_get_angular_rate(
                    &interface_data->handle,
                    &msg->rotation_rate[ X_INDEX ],
                    &msg->rotation_rate[ Y_INDEX ],
                    &msg->rotation_rate[ Z_INDEX ] );

            // convert degrees/second to radians/second, aligned to the PolySync coordinate system
            msg->rotation_rate[ X_INDEX ] *= -(M_PI/180.0);
            msg->rotation_rate[ Y_INDEX ] *= (M_PI/180.0);
            msg->rotation_rate[ Z_INDEX ] *= -(M_PI/180.0);
        }
    }
    else
    {
        // did not read data
        if( ret == DTC_NONE )
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
int read_live_data(
        ps_dynamic_driver * const dynamic_driver,
        ps_imu_msg * const msg )
{
    int ret = DTC_NONE;
    bool data_ready = PSYNC_FALSE;
    ps_timestamp time_diff = 0;
    diagnostics_s * const diagnostics = GET_INTERFACE_DIAGNOSTICS( dynamic_driver );


    if( (dynamic_driver->interface_data == NULL) || (msg == NULL) )
    {
        ret = DTC_USAGE;
    }

    // replay-mode returns DTC_UNAVAILABLE
    if( dynamic_driver->standard_options[ STDOPT_NOHW ] != 0 )
    {
        ret = DTC_UNAVAILABLE;
    }
    else
    {
        // read new sample
        if( ret == DTC_NONE )
        {
            ret = read_data_on_interval( dynamic_driver, msg );
        }

        if( ret == DTC_NONE )
        {
            // update diagnostics using current UTC time domain
            ret = psync_get_timestamp( &diagnostics->last_rx_timestamp );

            // data ready
            data_ready = PSYNC_TRUE;
        }

        // update diagnostic rx timeout
        if( ret != DTC_NONE )
        {
            ret = util_get_time_since( diagnostics->last_rx_timestamp, &time_diff );
            if( time_diff > PHIDGET_SPATIAL_RX_TIMEOUT )
            {
                ret = DTC_UNAVAILABLE;
                psync_node_activate_fault(
                        dynamic_driver->node_ref,
                        DTC_NOINTERFACE,
                        NODE_STATE_ERROR );
            }
        }
    }

    // DTC handling deals with issue, just return data/no-data
    if( (ret != DTC_NONE) || (data_ready == PSYNC_FALSE) )
    {
        ret = DTC_UNAVAILABLE;
    }


    return ret;
}


//
int read_replay_data(
        ps_dynamic_driver * const dynamic_driver,
        ps_imu_msg * const msg )
{
    int ret = DTC_NONE;
    bool data_ready = PSYNC_FALSE;
    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );
    message_set_s * const message_set = GET_INTERFACE_MESSAGE_SET( dynamic_driver );
    diagnostics_s * const diagnostics = GET_INTERFACE_DIAGNOSTICS( dynamic_driver );


    if( (dynamic_driver->interface_data == NULL) || (msg == NULL) )
    {
        ret = DTC_USAGE;
    }

    // update diagnostics using current UTC time domain
    // prevents activating a fault when doing a transition back to record-mode
    if( ret == DTC_NONE )
    {
        ret = psync_get_timestamp( &diagnostics->last_rx_timestamp );
    }

    // check for a message in the queue that we are expecting
    if( (ret == DTC_NONE) && (interface_data->replay_msg_queue != NULL) )
    {
        ps_msg_ref replay_msg = (ps_msg_ref) g_async_queue_try_pop( interface_data->replay_msg_queue );
        if( replay_msg != PSYNC_MSG_REF_INVALID )
        {
            ps_msg_type msg_type = PSYNC_MSG_TYPE_INVALID;

            (void) psync_message_get_type( replay_msg, &msg_type );

            // copy IMU message
            if( msg_type == message_set->imu_msg_type )
            {
                if( ret == DTC_NONE )
                {
                    ret = psync_message_copy(
                            dynamic_driver->node_ref,
                            replay_msg,
                            (ps_msg_ref) msg );

                    if( ret == DTC_NONE )
                    {
                        data_ready = PSYNC_TRUE;
                    }
                }
            }

            (void) psync_message_free( dynamic_driver->node_ref, &replay_msg );
        }
    }

    // DTC handling deals with issue, just return data/no-data
    if( (ret != DTC_NONE) || (data_ready != PSYNC_TRUE) )
    {
        ret = DTC_UNAVAILABLE;
    }


    return ret;
}
