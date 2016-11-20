/*
 * Copyright (c) 2016 PolySync
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


/**
 * @file phidget_spatial_driver.h
 * @brief Phidget Spatial Hardware (HAL) Driver header file.
 *
 *
 * PUBLIC_HEADER
 */




#ifndef PHIDGET_SPATIAL_DRIVER_H
#define PHIDGET_SPATIAL_DRIVER_H




#include <phidget21.h>

#include "polysync_core.h"




/**
 * @brief Acceleration axes count.
 *
 * The number of available axes the Phidget Spatial can measure acceleration on.
 *
 */
#define PHIDGET_SPATIAL_ACCELERATION_AXIS_COUNT (3)


/**
 * @brief Gyroscope axes count supplied by the device.
 *
 */
#define PHIDGET_SPATIAL_GRYO_AXIS_COUNT (3)


/**
 * @brief Compass axes count supplied by the device.
 *
 */
#define PHIDGET_SPATIAL_COMPASS_AXIS_COUNT (3)


/**
 * @brief Amount of time to wait for attachment. [milliseconds]
 */
#define PHIDGET_SPATIAL_ATTACHMENT_WAIT_TIMEOUT (10000)


/**
 * @brief Acceleration max. [standard gravity]
 *
 * The maximum acceleration value that the axis will report.
 *
 * Acceleration can be accurately measured up to 8.0g - any value past this
 * will be reported as 8.1g, which represents saturation. If the acceleration
 * data is equal to AccelerationMax, it should be treated as suspect, as the
 * real acceleration could be far greater than the reported number.
 *
 */
#define PHIDGET_SPATIAL_ACCELERATION_MAX (8.1)


/**
 * @brief Acceleration min. [standard gravity]
 *
 * The minimum acceleration value that the axis will report.
 *
 * Negative acceleration can be accurately measured up to -8.0g - any value
 * past this will be reported as -8.1g, which represents saturation. If the
 * acceleration data is equal to AccelerationMin, it should be treated as
 * suspect, as the real acceleration could be far greater than the
 * reported number.
 *
 */
#define PHIDGET_SPATIAL_ACCELERATION_MIN (-8.1)


/**
 * @brief The maximum angular rate reported for any axis. [degrees/second]
 *
 */
#define PHIDGET_SPATIAL_ANGULAR_RATE_MAX (2000.1)


/**
 * @brief The minimum angular rate reported for any axis. [degrees/second]
 *
 */
#define PHIDGET_SPATIAL_ANGULAR_RATE_MIN (-2000.1)


/**
 * @brief The maximum magnetic field strength reported for any axis. [gauss]
 *
 */
#define PHIDGET_SPATIAL_MAGNETIC_FIELD_MAX (5.6)


/**
 * @brief The minimum magnetic field strength reported for any axis. [gauss]
 *
 */
#define PHIDGET_SPATIAL_MAGNETIC_FIELD_MIN (-5.6)


/**
 * @brief The maximum data rate attainable by the hardware device. [milliseconds]
 *
 */
#define PHIDGET_SPATIAL_DATARATE_MAX (1)


/**
 * @brief The minimum data rate attainable by the hardware device. [milliseconds]
 *
 */
#define PHIDGET_SPATIAL_DATARATE_MIN (1000)



/**
 * @brief Phidget Spatial hardware device handle.
 *
 * The handle is used to attach to the hardware device,
 * and for each call to the Phidgets API.
 *
 */
typedef CPhidgetSpatialHandle phidget_spatial_handle;



/**
 * @brief Function call and array index parameters for x,y,z coordinates.
 */
enum
{
    //
    //
    X_INDEX = 0,
    //
    //
    Y_INDEX = 1,
    //
    //
    Z_INDEX = 2,
    //
    //
    INDEX_COUNT = 3
};




/**
 * @brief Create and open the handle to communicate with the Phidget Spatial device.
 *
 * @param [out] handle A pointer to \ref phidget_spatial_handle which receives the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) on success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if communication with device through Phidget SDK failed.
 *
 */
int phidget_spatial_open_device(
        phidget_spatial_handle * const handle );


/**
 * @brief Close and clean up the handle to the Phidget Spatial device.
 *
 * @param [in] A pointer to \ref phidget_spatial_handle which specifies the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) on success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if communication with device through Phidget SDK failed.
 *
 */
int phidget_spatial_close_device(
        phidget_spatial_handle * const handle );


/**
 * @brief Get the acceleration of the three axes.
 *
 * @param [in] A pointer to \ref phidget_spatial_handle which specifies the configuration.
 * @param [out] x A pointer to double that receives the x acceleration value.
 * @param [out] y A pointer to double that receives the y acceleration value.
 * @param [out] z A pointer to double that receives the z acceleration value.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) on success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if communication with device through Phidget SDK failed.
 */
int phidget_spatial_get_acceleration(
        phidget_spatial_handle * const handle,
        double * const x,
        double * const y,
        double * const z );

/**
 * @brief Get the angular rate of the three axes.
 *
 * @param [in] A pointer to \ref phidget_spatial_handle which specifies the configuration.
 * @param [out] x A pointer to double that receives the x angular rate value.
 * @param [out] y A pointer to double that receives the y angular rate value.
 * @param [out] z A pointer to double that receives the z angular rate value.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) on success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if communication with device through Phidget SDK failed.
 *
 */
int phidget_spatial_get_angular_rate(
        phidget_spatial_handle * const handle,
        double * const x,
        double * const y,
        double * const z );


/**
 * @brief Get the magnetic field strength of the three axes.
 *
 * @param [in] A pointer to \ref phidget_spatial_handle which specifies the configuration.
 * @param [out] x A pointer to double that receives the x magnetic field strength value.
 * @param [out] y A pointer to double that receives the y magnetic field strength value.
 * @param [out] z A pointer to double that receives the z magnetic field strength value.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) on success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if communication with device through Phidget SDK failed.
 *
 */
int phidget_spatial_get_magnetic_field(
        phidget_spatial_handle * const handle,
        double * const x,
        double * const y,
        double * const z );


/**
 * @brief Get the spatial data that will execute at the device datarate.
 *
 * Callback function handler for Phidget Spatial on-data event.
 *
 * @param [in] handle A pointer to \ref phidget_spatial_handle which receives the configuration.
 * @param [in] user_data A pointer to user data, provided to the callback function.
 * @param [in] data An array of spatial event data structures that holds the spatial data packets that were sent in this event.
 * @param [in] count The number of spatial data event packets included in this event.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) on success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if communication with device through Phidget SDK failed.
 *
 */
int CCONV phidget_example_spatial_data_handler(
        phidget_spatial_handle handle,
        void * user_data,
        CPhidgetSpatial_SpatialEventDataHandle *data,
        int count );


/**
 * @brief Get the data rate for the Phidget Spatial device, in microseconds.
 *
 * @param [in] A pointer to \ref phidget_spatial_handle which specifies the configuration.
 * @param [out] data_rate A pointer to \ref ps_timestamp that receives the data rate value. [microseconds]
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) on success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if communication with device through Phidget SDK failed.
 *
 */
int phidget_spatial_get_data_rate(
        phidget_spatial_handle * const handle,
        ps_timestamp * const data_rate );


/**
 * @brief Set the data rate for the Phidget Spatial device, in microseconds.
 *
 * @param [in] A pointer to \ref phidget_spatial_handle which receives the configuration.
 * @param [out] data_rate A ps_timestamp that specifies the data rate value. [microseconds]
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) on success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if communication with device through Phidget SDK failed.
 *
 */
int phidget_spatial_set_data_rate(
        phidget_spatial_handle * handle,
        const ps_timestamp data_rate );


/**
 * @brief Zero the Phidget Spatial device gyroscope.
 *
 * Zeroes the gyroscope.
 * This takes about two seconds and the gyro axes will report zero
 * during the process.
 * This should only be called when the board is not moving.
 *
 * @param [in] A pointer to \ref phidget_spatial_handle which receives the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) on success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if communication with device through Phidget SDK failed.
 *
 */
int phidget_spatial_zero_gyroscope(
        phidget_spatial_handle * handle );




#endif	/* PHIDGET_SPATIAL_DRIVER_H */
