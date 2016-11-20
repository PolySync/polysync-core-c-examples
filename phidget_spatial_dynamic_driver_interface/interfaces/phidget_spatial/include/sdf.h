/**
 * @file phidget_spatial/include/sdf.h
 * @brief Phidget Spatial SDF Interface.
 *
 */




#ifndef SDF_H
#define SDF_H




#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_dynamic_driver.h"
#include "polysync_dynamic_driver_parameters.h"




/**
 * @brief Parameter ID - Update rate. Supported update rates are: 1, 2, 4, 8, and every multiple of 8 in the range of 1ms to 1000ms. [milliseconds]
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: unsigned long long
 * \li units: milliseconds
 *
 * Custom parameter ID.
 *
 */
#define PHIDGET_SPATIAL_PARAM_ID_UPDATE_RATE (5400)


/**
 * @brief Parameter description - Update rate. Supported update rates are: 1, 2, 4, 8, and every multiple of 8 in the range of 1ms to 1000ms. [milliseconds]
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PHIDGET_SPATIAL_UPDATE_RATE "Update rate. Supported update rates are: 1, 2, 4, 8, and every multiple of 8 in the range of 1ms to 1000ms. [milliseconds]"




/**
 * @brief Sensor descriptors used by the PAL interface.
 *
 * Provides a list of sensor descriptors this interface uses.
 * Also used by the node to re-generate missing SDF support.
 *
 * See \ref NUM_SENSOR_DESCRIPTORS for number of elements.
 *
 */
extern const ps_sensor_support_descriptor INTERFACE_SENSOR_DESCRIPTORS[];


/**
 * @brief Number of sensor descriptors used by the PAL interface.
 *
 * Defines the number of elements in \ref INTERFACE_SENSOR_DESCRIPTORS.
 *
 */
extern const unsigned long NUM_SENSOR_DESCRIPTORS;


/**
 * @brief SDF IO configuration parameter values.
 *
 * This is a low-level structure used to organize the SDF parameter information
 * used by the IO configuration.
 *
 * Parameter ID's and value types used by the IO configuration.
 *
 * @note Only parameter ID and value type are required.
 *
 */
extern const ps_sdf_parameter_value INTERFACE_IO_PARAMETERS[];


/**
 * @brief SDF IO configurations used by the node configuration.
 *
 * This is a mid-level structure used to organize the the SDF IO configurations
 * used by the sensor configuration given to the PAL.
 *
 * @note This structure is comprised of data from other static/const elements.
 *
 */
extern const ps_sdf_io_configuration INTERFACE_IO_CONFIGS[];


/**
 * @brief SDF sensor configuration parameter values.
 *
 * This is a low-level structure used to organize the SDF parameter information
 * used by the sensor configuration.
 *
 * Parameter ID's and value types used by the sensor configuration.
 *
 * @note Only parameter ID and value type are required.
 *
 */
extern const ps_sdf_parameter_value INTERFACE_SENSOR_PARAMETERS[];


/**
 * @brief SDF sensor configurations used by the node configuration.
 *
 * This is a mid-level structure used to organize the the SDF sensor configurations
 * used by the node configuration given to the PAL.
 *
 * @note This structure is comprised of data from other static/const elements.
 *
 */
extern const ps_sdf_sensor_configuration INTERFACE_SENSOR_CONFIGS[];


/**
 * @brief SDF node configuration parameter values.
 *
 * This is a low-level structure used to organize the SDF parameter information
 * used by the node configuration.
 *
 * Parameter ID's and value types used by the node configuration.
 *
 * @note Only parameter ID and value type are required.
 *
 */
extern const ps_sdf_parameter_value INTERFACE_NODE_PARAMETERS[];


/**
 * @brief SDF node configuration expected by the PAL layer.
 *
 * This is the top-level structure provided to the node support mechanism.
 * It provides the node with a minimum SDF configuration required by the interface.
 *
 * This structure provides the relationship of elements and their parameters/other elements.
 *
 * The node will checks the following conditions:
 * \li Elements that are enabled here should be enabled in the SDF.
 * \li Elements with parameters here should be in the SDF, checks for ID and value type.
 *
 * @note This structure is comprised of data from other static/const elements.
 *
 */
extern const ps_sdf_node_configuration INTERFACE_NODE_CONFIG;


/**
 * @brief Initial/startup parameters used by the PAL interface.
 *
 * Provides a list of parameters (and their type/values) this interface expects on startup.
 * Also used by the node to re-generate missing SDF support.
 * This is NOT used to get/set runtime parameters.
 *
 * The use of \ref PSYNC_PARAMETER_FLAG_DISABLED, means that the node supports the parameter
 * but does not actively use it.
 *
 * See \ref NUM_INITIAL_PARAMETERS for number of elements.
 *
 */
extern const ps_parameter INTERFACE_SUPPORTED_PARAMETERS[];


/**
 * @brief Number of startup parameters used by the PAL interface.
 *
 * Defines the number of elements in \ref INTERFACE_SUPPORTED_PARAMETERS.
 *
 */
extern const unsigned long NUM_INITIAL_PARAMETERS;




/**
 * @brief Reload SDF node data from SDF file.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which receives the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_CONFIG if SDF configuration invalid.
 *
 */
int reload_sdf_node_data( ps_dynamic_driver * const dynamic_driver );




#endif	/* SDF_H */
