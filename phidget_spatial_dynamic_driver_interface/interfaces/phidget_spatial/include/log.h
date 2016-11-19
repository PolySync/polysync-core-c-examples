/**
 * @file phidget_spatial/include/data_logger.h
 * @brief Phidget Spatial Data Logger Interface.
 *
 */




#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H




#include "polysync_core.h"
#include "polysync_dynamic_driver.h"




//
int log_imu_msg(
        const ps_imu_msg * const msg,
        ps_dynamic_driver * const dynamic_driver );




#endif	/* DATA_LOGGER_H */
