/**
 * @file phidget_spatial/include/connect.h
 * @brief Phidget Spatial Connection Utilities Interface.
 *
 */




#ifndef CONNECT_H
#define CONNECT_H




#include "polysync_core.h"
#include "phidget_spatial_driver.h"




//
int connect_handle(
        const ps_timestamp data_rate,
        phidget_spatial_handle * const handle );




#endif	/* CONNECT_H */
