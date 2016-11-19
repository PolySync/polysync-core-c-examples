/**
 * @file phidget_spatial/include/interface_data.h
 * @brief Phidget Spatial Interface Data.
 *
 */




#ifndef INTERFACE_DATA_H
#define INTERFACE_DATA_H




#include <glib-2.0/glib.h>

#include "polysync_core.h"
#include "polysync_logfile.h"
#include "polysync_getset.h"

#include "polysync_dynamic_interface.h"
#include "polysync_dynamic_driver.h"

#include "phidget_spatial_driver.h"




/**
 * @brief Get the \ref interface_data_s pointer from a Dynamic Driver.
 *
 */
#define GET_INTERFACE_DATA(dd) ((interface_data_s*) dd->interface_data)


/**
 * @brief Get the \ref message_set_s pointer from a Dynamic Driver's \ref interface_data_s data.
 *
 */
#define GET_INTERFACE_MESSAGE_SET(dd) (&((interface_data_s*) dd->interface_data)->messages)


/**
 * @brief Get the \ref diagnostics_s pointer from a Dynamic Driver's \ref interface_data_s data.
 *
 */
#define GET_INTERFACE_DIAGNOSTICS(dd) (&((interface_data_s*) dd->interface_data)->diagnostics)


/**
 * @brief Get the \ref variables_s pointer from a Dynamic Driver's \ref interface_data_s data.
 *
 */
#define GET_INTERFACE_VARIABLES(dd) (&((interface_data_s*) dd->interface_data)->variables)


/**
 * @brief Amount of time the diagnostic handler will sleep before retries. [microseconds]
 *
 */
#define PHIDGET_SPATIAL_DIAGNOSTIC_SLEEP_INTERVAL (1000)


/**
 * @brief Amount of time the diagnostic framework will wait before another recovery attempt. [microseconds]
 *
 */
#define PHIDGET_SPATIAL_DIAGNOSTIC_RETRY_INTERVAL (200000)


/**
 * @brief Maximum amount of time between valid received data allowed before activating a DTC. [microseconds]
 *
 */
#define PHIDGET_SPATIAL_RX_TIMEOUT (2000000)


/**
 * @brief Length of time to attempt to validate device. [microseconds]
 *
 */
#define PHIDGET_SPATIAL_VALIDATION_TIMEOUT (1000000)


/**
 * @brief Number of data model modules used by the PAL interface.
 *
 * Defines the number of elements in \ref INTERFACE_MODULES.
 *
 */
#define NUM_MODULES (2)


/**
 * @brief Number of message types (by name) used by the PAL interface.
 *
 * Defines the number of elements in \ref INTERFACE_MESSAGE_TYPE_NAMES.
 *
 */
#define NUM_MESSAGE_TYPE_NAMES (1)


/**
 * @brief PolySync sensor kind value for Phidget Spatial.
 *
 * @warning This MUST stay constant, and should match the design documentation.
 *
 */
#define PHIDGET_SPATIAL_PS_SENSOR_KIND (360)




/**
 * @brief PAL interface options.
 *
 */
enum
{
    //
    //
    PALOPT_DEBUG = 0, /*!< Enable additional debugging output, reserves -d */
    //
    //
    PALOPT_TEST, /*!< Perform a validation test on the interface, reserves -t */
    //
    //
    PALOPT_COUNT /*!< Number of PAL interface command line options. */
};


/**
 * @brief Command-line options and their arguments (if present).
 *
 */
typedef struct
{
    //
    //
    unsigned int options[PALOPT_COUNT]; /*!< Enabled flags for the interface options.
                                         * Value zero means option at index is not set. */
}palopts_s;


/**
 * @brief Message set.
 *
 * Container for PolySync messages and message-type identifiers.
 *
 */
typedef struct
{
    //
    //
    ps_msg_type imu_msg_type; /*!< IMU message type. */
    //
    //
    ps_msg_ref imu_msg; /*!< IMU message. */
} message_set_s;


/**
 * @brief Diagnostics data.
 *
 */
typedef struct
{
    //
    //
    ps_timestamp last_rx_timestamp; /*!< Last valid data-recieved
                                     * timestamp, used by the diagnostics framework. [UTC microseconds] */
    //
    //
    ps_timestamp last_recovery_attempt; /*!< Last DTC recovery attempt timestamp. [UTC microseconds] */
} diagnostics_s;


/**
 * @brief State and get/set variables.
 *
 */
typedef struct
{
    //
    //
    char serial_0_name[PSYNC_DEFAULT_STRING_LEN]; /*!< Serial 0 name.
                                                   *
                                                   * Registered to get/set parameter \ref PSYNC_PARAM_ID_SERIAL0_NAME. */
    //
    //
    unsigned long long sensor_0_id; /*!< Sensor 0 ID.
                                     *
                                     * Registered to get/set parameter \ref PSYNC_PARAM_ID_SENSOR0_ID. */
    //
    //
    unsigned long long coordinate_frame; /*!< Node coordinate frame.
                                          *
                                          * Registered to get/set parameter \ref PSYNC_PARAM_ID_COORDINATE_FRAME. */
    //
    //
    unsigned long long update_rate; /*!< Device update rate.
                                     *
                                     * Registered to get/set parameter \ref PHIDGET_SPATIAL_PARAM_ID_UPDATE_RATE. */
    //
    //
    bool publishes_imu; /*!< Flag indicating whether or not the
                         * interface should publish
                         * IMU messages.
                         *
                         * Registered to get/set parameter \ref PARAM_ID_PUB_IMU. */
} variables_s;


/**
 * @brief Interface data.
 *
 */
typedef struct
{
    //
    //
    diagnostics_s diagnostics; /*!< Diagnostics data. */
    //
    //
    message_set_s messages; /*!< PolySync message set/support data. */
    //
    //
    variables_s variables; /*!< State/get-set variables. */
    //
    //
    palopts_s palopts; /*!< Enabled flags for the interface options.
                        * Value zero means option at index is not set. */
    //
    //
    GAsyncQueue *replay_msg_queue; /*!< Reference to the logfile replay data queue.
                                    * Used to get the replay UDP byte array messages to the
                                    * node without using a subscriber. */
    //
    //
    phidget_spatial_handle handle; /*!< Device handle. */
    //
    //
    ps_timestamp last_sample_update; /*!< Last sample/measurement update. [microseconds]
                                      *
                                      * Used to control how often we poll for a new sample. */
} interface_data_s;




#endif	/* INTERFACE_DATA_H */
