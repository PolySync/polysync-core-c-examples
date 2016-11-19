/**
 * @file phidget_spatial/include/read.h
 * @brief Phidget Spatial Data Reader Interface.
 *
 */




#ifndef READ_H
#define READ_H




#include "polysync_core.h"
#include "polysync_dynamic_driver.h"




//
int read_live_data(
        ps_dynamic_driver * const dynamic_driver,
        ps_imu_msg * const msg );


//
int read_replay_data(
        ps_dynamic_driver * const dynamic_driver,
        ps_imu_msg * const msg );




#endif	/* READ_H */
