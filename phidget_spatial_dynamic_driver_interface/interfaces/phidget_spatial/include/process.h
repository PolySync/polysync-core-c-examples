/**
 * @file phidget_spatial/include/process.h
 * @brief Phidget Spatial Data Processing Interface.
 *
 */




#ifndef PROCESS_H
#define PROCESS_H




#include "polysync_core.h"
#include "polysync_dynamic_driver.h"




//
int process_imu_msg(
        ps_dynamic_driver * const dynamic_driver,
        ps_imu_msg * const msg );




#endif	/* PROCESS_H */
