/**
 * @file phidget_spatial/src/data_logger.c
 * @brief Phidget Spatial Data Logger Interface Source.
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
#include "log.h"




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
int log_imu_msg(
        const ps_imu_msg * const msg,
        ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;
    bool is_recording = PSYNC_FALSE;


    if( (msg == NULL) || (dynamic_driver == NULL) )
    {
        ret = DTC_CONFIG;
    }

    if( ret == DTC_NONE )
    {
        if(
            (dynamic_driver->logfile_mode == LOGFILE_MODE_WRITE)
            && (dynamic_driver->logfile_state == LOGFILE_STATE_ENABLED) )
        {
            is_recording = PSYNC_TRUE;
        }
    }

    if( (ret == DTC_NONE) && (is_recording == PSYNC_TRUE) )
    {
        ret = psync_logfile_write_message(
                dynamic_driver->node_ref,
                (ps_msg_ref) msg );
    }


    return ret;
}
