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
 * @file polysync_dynamic_driver.h
 * @brief PolySync Dynamic Driver Interface Data.
 *
 * PUBLIC_HEADER
 */




#ifndef POLYSYNC_DYNAMIC_DRIVER_H
#define	POLYSYNC_DYNAMIC_DRIVER_H


#ifdef	__cplusplus
extern "C" {
#endif




#include "polysync_core.h"
#include "polysync_node.h"
#include "polysync_node_template.h"
#include "polysync_message.h"
#include "polysync_sdf.h"




/**
 * @brief Invalid parameter group SDF exposure map index value.
 *
 */
#define PSYNC_PARAMETER_GROUP_SDF_INDEX_INVALID (0xFFFFFFFF)


/**
 * @brief Maximum number of dynamic driver standard options.
 *
 * Maximum number of dynamic driver standard options supported by this implementation.
 *
 * See \ref STDOPT_COUNT for the actual number of standard options used.
 *
 */
#define PSYNC_DYNAMIC_DRIVER_STDOPT_MAX (64)




/**
 * @brief Dynamic driver standard options.
 *
 * These are options supported by the dynamic driver, the option characters are
 * reserved and should not be used by the interfaces.
 *
 * These can be used to index the dynamic driver
 * data options array (\ref ps_dynamic_driver.standard_options):
 * @code
 * // option is set if the value is equal to 1
 * unsigned int opt-is-set = standard_options[ STDOPT_HELP ];
 * @endcode
 *
 */
enum
{
    //
    //
    STDOPT_HELP = 0, /*!< Output the help messages option, reserves '-h'. */
    //
    //
    STDOPT_STDOUT, /*!< Enable standard output logging option, reserves '-o'. */
    //
    //
    STDOPT_NOHW, /*!< Start in replay mode (no-hardware context) option, reserves '-w'. */
    //
    //
    STDOPT_SDF_RUNTIME_KEY, /*!< Use the specified SDF runtime option, reserves '-r'. */
    //
    //
    STDOPT_SDF_NODE_KEY, /*!< Node/SDF configuration key/identifier option, reserves '-n'. */
    //
    //
    STDOPT_MANUAL_INTERFACE, /*!< Manually specify a PAL interface file option, reserves '-i'. */
    //
    //
    STDOPT_EXPORT_SUPPORT, /*!< Export SDF configurator support string option, reserves '-e'. */
    //
    //
    STDOPT_ALLOW_SDF_UPDATES, /*!< Enable the ability to update SDF configuration as needed option, reserves '-u'. */
    //
    //
    STDOPT_LOGFILE_PATH, /*!< Use provided logfile in Record and Replay operations instead of the default, reserves '-p'. */
    //
    //
    STDOPT_SDF_PATH, /*!< Use provided SDF instead of the default, reserves '-s'. */
    //
    //
    STDOPT_UPDATE_NODE_SDF, /*!< Update the node SDF configuration and exit option, reserves '-U'. */
    //
    //
    STDOPT_CHECK_NODE_SDF, /*!< Check the node SDF configuration for required updates and exit option, reserves '-O'. */
    //
    //
    STDOPT_COUNT /*!< Number of standard command line options. */
};




/**
 * @brief PolySync sensor support descriptor.
 *
 * Used to provide the dynamic driver with information about the sensor support in the PAL interface.
 *
 */
typedef struct
{
    //
    //
    const char *name; /*!< Pointer to interface name. */
    //
    //
    ps_sensor_kind type; /*!< PolySync sensor type ID.
                          * Value \ref PSYNC_SENSOR_ID_NOT_AVAILABLE means not available. */
    //
    //
    const char *description; /*!< Pointer to interface description. */
    //
    //
    const char *support_link; /*!< Pointer to interface support link URL. NULL and empty-strings are allowed. */
} ps_sensor_support_descriptor;


/**
 * @brief Parameter group SDF exposure mapping data.
 *
 * Describes the location of a group within an
 * exposed SDF node configuration.
 *
 */
typedef struct
{
    //
    //
    unsigned long group_id; /*!< Group identifier. */
    //
    //
    unsigned long node_config_index; /*!< Index of the group in the node configuration array.
                                      * Value \ref PSYNC_PARAMETER_GROUP_SDF_INDEX_INVALID means invalid. */
    //
    //
    unsigned long sensor_config_index; /*!< Index of the group in the sensor configuration array (within the parent node configuration).
                                        * Value \ref PSYNC_PARAMETER_GROUP_SDF_INDEX_INVALID means invalid. */
    //
    //
    unsigned long io_config_index; /*!< Index of the group in the IO configuration array (within the parent sensor configuration).
                                    * Value \ref PSYNC_PARAMETER_GROUP_SDF_INDEX_INVALID means invalid. */
} ps_parameter_group_sdf_exposure_map;


/**
 * @brief PolySync Dynamic Driver.
 *
 * This is the top-level data structure provided to all of the
 * interface callbacks. It contains a user-data pointer (\ref ps_dynamic_driver.interface_data)
 * that is set by the interface (optionally) in \ref psync_interface_configure_function.
 *
 */
typedef struct
{
    //
    //
    ps_node_ref node_ref; /*!< Node reference used by the dynamic driver and interface.
                           * Constructed and validated by the dynamic driver. */
    //
    //
    unsigned long sdf_runtime_key; /*!< SDF runtime identifier used to configure the node. */
    //
    //
    ps_sdf_node_configuration node_sdf_config; /*!< Node's SDF node configuration data. */
    //
    //
    const ps_node_configuration_data * const node_config; /*!< A copy of the node-template configuration data set by the interface. */
    //
	//
    const ps_diagnostic_state * const current_state; /*!< A copy of the node's current diagnostic state. */
    //
    //
    void *interface_data; /*!< Interface data, set and used by the PolySync Abstraction Layer. */
    //
    //
    const ps_msg_type rnr_msg_type; /*!< RnR message type used by the dynamic driver. */
    //
    //
    ps_msg_ref rnr_msg; /*!< RnR message used by the dynamic driver. */
    //
    //
    const ps_msg_type sdf_state_msg_type; /*!< SDF state message type used by the dynamic driver. */
    //
    //
    ps_msg_ref sdf_state_msg; /*!< SDF state message used by the dynamic driver. */
    //
    //
    const ps_msg_type parameters_msg_type; /*!< Get/Set parameters message type used by the dynamic driver. */
    //
    //
    ps_msg_ref parameters_msg; /*!< Get/Set parameters message used by the dynamic driver. */
    //
    //
    const ps_msg_type command_msg_type; /*!< Command message type used by the dynamic driver. */
    //
    //
    const ps_msg_type response_msg_type; /*!< Response message type used by the dynamic driver. */
    //
    //
    ps_msg_ref response_msg; /*!< Response message used by the dynamic driver. */
    //
    //
    const ps_logfile_mode_kind logfile_mode; /*!< Current logfile mode.
                                              * Can be used to quickly check the status of the logfile mechanisms. */
    //
    //
    const ps_logfile_state_kind logfile_state; /*!< Current logfile state.
                                                * Can be used to quickly check the status of the logfile mechanisms. */
    //
    //
    char *manual_logfile_path; /*!< Manual logfile path provided with option \ref STDOPT_LOGFILE_PATH. */
    //
    //
    char *manual_sdf_path; /*!< Manual SDF path provided with option \ref STDOPT_SDF_PATH. */
    //
    //
    unsigned int standard_options[PSYNC_DYNAMIC_DRIVER_STDOPT_MAX]; /*!< Array of dynamic driver standard options.
                                                                     *
                                                                     * These are set by the dynamic driver application, and are
                                                                     * visible to the PAL interface.
                                                                     *
                                                                     * Value 0 means option not set.
                                                                     *
                                                                     * See \ref STDOPT_COUNT for the number of available standard options.
                                                                     *
                                                                     * The maximum length of this array will stay constant acording to the versioning scheme.
                                                                     * See \ref PSYNC_DYNAMIC_DRIVER_STDOPT_MAX. */
} ps_dynamic_driver;




#ifdef	__cplusplus
}
#endif


#endif	/* POLYSYNC_DYNAMIC_DRIVER_H */
