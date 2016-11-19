/**
 * @file phidget_spatial/src/process.c
 * @brief Phidget Spatial Data Processing Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "polysync_core.h"
#include "polysync_message.h"

#include "interface_data.h"
#include "debug.h"
#include "process.h"




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
int process_imu_msg(
        ps_dynamic_driver * const dynamic_driver,
        ps_imu_msg * const msg )
{
    int ret = DTC_NONE;
    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );
    variables_s * const variables = GET_INTERFACE_VARIABLES( dynamic_driver );


    if( msg == NULL )
    {
        ret = DTC_USAGE;
    }

    if( dynamic_driver == NULL )
    {
        ret = DTC_USAGE;
    }

    // update publish timestamp
    if( ret == DTC_NONE )
    {
        ret = psync_get_timestamp( &msg->header.timestamp );
    }

    // publish if enabled
    if( ret == DTC_NONE )
    {
        if( variables->publishes_imu == PSYNC_TRUE )
        {
            ret = psync_message_publish(
                    dynamic_driver->node_ref,
                    (ps_msg_ref) msg );
        }
    }

    // debug output
    if( ret == DTC_NONE )
    {
        if( interface_data->palopts.options[ PALOPT_DEBUG ] != 0 )
        {
            debug_print_imu_msg( msg );
        }
    }


    return ret;
}
