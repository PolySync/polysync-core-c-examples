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
 * @file polysync_dynamic_interface.h
 * @brief PolySync Dynamic Interface.
 *
 * The PolySync Dynamic Interface defines the interfaces to a PolySync Abstraction Layer.
 *
 * PUBLIC_HEADER
 */




#ifndef POLYSYNC_DYNAMIC_INTERFACE_H
#define	POLYSYNC_DYNAMIC_INTERFACE_H


#ifdef	__cplusplus
extern "C" {
#endif



#include "polysync_core.h"
#include "polysync_node.h"
#include "polysync_parameter_group.h"
#include "polysync_node_template.h"
#include "polysync_dynamic_driver.h"




/**
 * @brief Expose interface options, function type.
 *
 * Expose command line options used by the interface.
 *
 * @note Returning a DTC other than DTC_NONE will cause the node to transition
 * into the fatal state and terminate immediately.
 *
 * @note PolySync is not initialized at this point, and thus no node related
 * PolySync routines should be called in this context.
 *
 * @warning Expects symbol name to be "psync_interface_expose_options" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [out] optstring A pointer to char buffer which receives a pointer to the interface options string.
 * @param [out] optchars A pointer to char buffer which receives a pointer to the interface option characters string.
 * @param [out] descriptions A pointer to char buffer array which receives a pointer to the interface option description strings array.
 * @param [out] help_strings A pointer to char buffer array which receives a pointer to the interface option help message strings array.
 * @param [out] num_options A pointer to unsigned long which receives the number of interface options value.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_expose_options_function)(
        const char **optstring,
        const char **optchars,
        const char * const **descriptions,
        const char * const **help_strings,
        unsigned long * const num_options );


/**
 * @brief Expose interface startup parameters, function type.
 *
 * Expose startup parameters used by the interface.
 *
 * @note Returning a DTC other than DTC_NONE will cause the node to transition
 * into the fatal state and terminate immediately.
 *
 * @note PolySync is not initialized at this point, and thus no node related
 * PolySync routines should be called in this context.
 *
 * @warning Expects symbol name to be "psync_interface_expose_parameters" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [out] parameters A pointer to \ref ps_parameter array which receives a pointer to the interface startup parameters array.
 * @param [out] num_parameters A pointer to unsigned long which receives the number of interface startup parameters value.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_expose_parameters_function)(
        const ps_parameter **parameters,
        unsigned long * const num_parameters );


/**
 * @brief Expose interface type support information, function type.
 *
 * Expose type support information used by the interface.
 *
 * This includes:
 * \li data model resources (what modules are used by the interface).
 * \li message types used by the interface (by string name).
 *
 * The interface name is usually the same name used by the PolySync licensing framework.
 *
 * Version enforcement is left up to the interface, the node will check that the resources exist.
 *
 * @note Returning a DTC other than DTC_NONE will cause the node to transition
 * into the fatal state and terminate immediately.
 *
 * @note PolySync is not initialized at this point, and thus no node related
 * PolySync routines should be called in this context.
 *
 * @warning Expects symbol name to be "psync_interface_expose_type_support" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [out] interface_name A pointer to char buffer which receives a pointer to the interface name string.
 * @param [out] module_names A pointer to char buffer array which receives a pointer to the interface data model module name strings array.
 * @param [out] num_modules A pointer to unsigned long which receives the number of interface data model modules value.
 * @param [out] message_type_names A pointer to char buffer array which receives a pointer to the interface message type name strings array.
 * @param [out] num_message_types A pointer to unsigned long which receives the number of interface message types value.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_expose_type_support_function)(
        const char **interface_name,
        const char * const **module_names,
        unsigned long * const num_modules,
        const char * const **message_type_names,
        unsigned long * const num_message_types );


/**
 * @brief Expose interface sensor support information, function type.
 *
 * Expose sensor support information used by the interface.
 *
 * This includes:
 * \li Sensor name as seen by the license model.
 * \li Sensor type identifier, used to populate the data model messages.
 * \li Sensor description string.
 *
 * @note Returning a DTC other than DTC_NONE will cause the node to transition
 * into the fatal state and terminate immediately.
 *
 * @note PolySync is not initialized at this point, and thus no node related
 * PolySync routines should be called in this context.
 *
 * @warning Expects symbol name to be "psync_interface_expose_sensor_support" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [out] sensor_support_descriptors A pointer
 * to \ref ps_sensor_support_descriptor array which receives a pointer to the interface sensor support descriptor array.
 * @param [out] num_descriptors A pointer to unsigned long which receives the number of sensor support descriptors value.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_expose_sensor_support_function)(
        const ps_sensor_support_descriptor **sensor_support_descriptors,
        unsigned long * const num_descriptors );


/**
 * @brief Expose interface SDF support information, function type.
 *
 * Expose node-level SDF support information used by the interface.
 *
 * @note Returning a DTC other than DTC_NONE will cause the node to transition
 * into the fatal state and terminate immediately.
 *
 * @note PolySync is not initialized at this point, and thus no node related
 * PolySync routines should be called in this context.
 *
 * @warning Expects symbol name to be "psync_interface_expose_sdf_support" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [out] sdf_node_configuration A pointer
 * to \ref ps_sdf_node_configuration which receives a pointer to the interface node SDF configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_expose_sdf_support_function)(
        const ps_sdf_node_configuration **sdf_node_configuration );


/**
 * @brief Expose interface parameter groups, function type.
 *
 * Expose parameter groups used by the interface.
 *
 * @note Returning a DTC other than DTC_NONE will cause the node to transition
 * into the fatal state and terminate immediately.
 *
 * @note PolySync is not initialized at this point, and thus no node related
 * PolySync routines should be called in this context.
 *
 * @warning Expects symbol name to be "psync_interface_expose_parameter_groups" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [out] parameter_groups A pointer to \ref ps_parameter_group array which receives a pointer to the interface parameter groups array.
 * @param [out] group_mappings A pointer to \ref ps_parameter_group_sdf_exposure_map array which receives a pointer to the interface parameter groups SDF exposure map array.
 * @param [out] num_parameter_groups A pointer to unsigned long which receives the number of interface parameter groups.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_expose_parameter_groups_function)(
        const ps_parameter_group **parameter_groups,
        const ps_parameter_group_sdf_exposure_map **group_mappings,
        unsigned long * const num_parameter_groups );


/**
 * @brief Set interface configuration, function type.
 *
 * Called once after the driver transitions into the AUTH state, early near
 * the start of the program before any PolySync related routines.
 *
 * Early/low-level/static configurations and options are handled in this routine.
 *
 * If the host provides command line arguments they will be set, and available
 * for parsing (ie getopts).
 *
 * The node will set default values in the node_config, which can be changed if desired.
 *
 * The user data field is to be set by the interface, the pointer
 * in \ref ps_node_configuration_data.user_data gets copied over to
 * the \ref ps_dynamic_driver.interface_data field for access in the other routines.
 *
 * @note Returning a DTC other than DTC_NONE will cause the node to transition
 * into the fatal state and terminate immediately.
 *
 * @note PolySync is not initialized at this point, and thus no node related
 * PolySync routines should be called in this context.
 *
 * @warning Expects symbol name to be "psync_interface_configure" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [in] node_config A pointer to \ref ps_node_configuration_data which stores the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_configure_function)(
        ps_node_configuration_data * const node_config );


/**
 * @brief Initialize interface, function type.
 *
 * Called once after the node transitions into the INIT state from the AUTH state on startup, and while in the
 * INIT state for the rest of the application's runtime.
 *
 * Fault handling mechanisms may call this routine arbitrarily to re-initialize.
 *
 * Runtime configuration logic is handled in this routine. This includes elements
 * such as parameters, hardware-driver initialization, data model resource initialization, etc.
 *
 * @note Returning a DTC other than DTC_NONE will cause
 * the node to immediately return execution to the Node Template API for state management.
 *
 * @warning Expects symbol name to be "psync_interface_init" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which receives the initialization.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_init_function)(
        ps_dynamic_driver * const dynamic_driver );


/**
 * @brief Release interface, function type.
 *
 * Called once when the node exits.
 *
 * This routine should release any resources owned by the interface, that are not handled
 * by the node.
 *
 * @note Returning a DTC other than DTC_NONE will cause
 * the node to immediately return execution to the Node Template API for state management.
 *
 * @warning Expects symbol name to be "psync_interface_release" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which specifies the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_release_function)(
        ps_dynamic_driver * const dynamic_driver );


/**
 * @brief Check for interface data, function type.
 *
 * Called once every iteration in the node's main event loop.
 *
 * This routine checks for interface data availability.
 * If data is available, the node will call the interface for logging and
 * data processing.
 *
 * @note Returning a DTC other than DTC_NONE or DTC_UNAVAILABLE will cause
 * the node to immediately return execution to the Node Template API for state management.
 *
 * @warning Expects symbol name to be "psync_interface_poll_for_data" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which specifies the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if data is available for processing.
 * \li \ref DTC_UNAVAILABLE if no data is available.
 * \li DTC code on error.
 *
 */
typedef int (*psync_interface_poll_for_data_function)(
        ps_dynamic_driver * const dynamic_driver );


/**
 * @brief Log interface data, function type.
 *
 * Called after "psync_interface_poll_for_data", only if data is available.
 *
 * This routine log's interface data as needed by the interface.
 *
 * @note Returning a DTC other than DTC_NONE or DTC_UNAVAILABLE will cause
 * the node to immediately return execution to the Node Template API for state management.
 *
 * @warning Expects symbol name to be "psync_interface_log_data" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which specifies the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_log_data_function)(
        ps_dynamic_driver * const dynamic_driver );


/**
 * @brief Process interface data, function type.
 *
 * Called after "psync_interface_log_data", only if data is available.
 *
 * This routine processes the interface data as needed by the interface.
 *
 * @note Returning a DTC other than DTC_NONE or DTC_UNAVAILABLE will cause
 * the node to immediately return execution to the Node Template API for state management.
 *
 * @warning Expects symbol name to be "psync_interface_process_data" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which specifies the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_process_data_function)(
        ps_dynamic_driver * const dynamic_driver );


/**
 * @brief Idle-node interface logic, function type.
 *
 * Called once every iteration in the node's main event loop.
 *
 * This routine can be used by the interface to do any addition logic.
 *
 * @note Returning a DTC other than DTC_NONE or DTC_UNAVAILABLE will cause
 * the node to immediately return execution to the Node Template API for state management.
 *
 * @note Interfaces can return DTC_UNAVAILABLE to reset the dynamic driver's idle mechanism.
 *
 * @note This is an optional function and is not required by the dynamic driver to function.
 *
 * @warning Expects symbol name to be "psync_interface_idle" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which specifies the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 * \li \ref DTC_UNAVAILABLE to reset the dynamic driver idle sleep mechanism.
 *
 */
typedef int (*psync_interface_idle_function)(
        ps_dynamic_driver * const dynamic_driver );


/**
 * @brief Handle get/set operation, function type.
 *
 * Called by the dynamic driver whenever a get/set operation is requested.
 *
 * Typically this is only called for get* operations, but checking
 * the type is recommended.
 *
 * If the operation is allowed, the Get/Set API is used to update any
 * mapped variables and optionally the SDF.
 *
 * @warning Expects symbol name to be "psync_interface_handle_parameter_command" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which specifies the configuration.
 * @param [in] command Get/set operation command type.
 * @param [in] command_parameter A pointer to \ref ps_parameter which specifies the requested parameter state.
 * @param [in] node_parameter A pointer to \ref ps_parameter which specifies the current parameter state.
 *
 * * @return DTC code:
 * \li \ref DTC_NONE (zero) if success - this means the dynamic driver will allow the operation.
 * \li other DTC code means dynamic driver will not allow the operation.
 *
 */
typedef int (*psync_interface_handle_parameter_command_function)(
        ps_dynamic_driver * const dynamic_driver,
        const ps_parameter_message_kind command,
        const ps_parameter * const command_param,
        ps_parameter * const node_param );


/**
 * @brief Handle non-OK state, function type.
 *
 * Called once every iteration in the node's main event loop if current state
 * is not \ref NODE_STATE_OK.
 *
 * This routine can be used by the interface to do any recovery logic needed
 * to get back into the OK state.
 *
 * The current state of the node is visible in \ref ps_dynamic_driver.current_state.
 *
 * @note Returning a DTC other than DTC_NONE will cause
 * the node to immediately return execution to the Node Template API for state management.
 *
 * @warning Expects symbol name to be "psync_interface_handle_state" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which specifies the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_handle_state_function)(
        ps_dynamic_driver * const dynamic_driver );


/**
 * @brief Handle command message, function type.
 *
 * Called by the dynamic driver whenever a PolySync command message is received.
 *
 * This routine can be used by the interface to do any command
 * handling logic if needed. The return value is provided in the
 * response DTC field of the response message publish by the dynamic driver.
 *
 * The response message being managed by the dynamic driver is provided so
 * the interface can modify the response data if needed.
 *
 * This is called after dynamic driver level commands are handled.
 *
 * @note This is an optional function and is not required by the dynamic driver to function.
 *
 * @warning Expects symbol name to be "psync_interface_handle_command" if not
 * using the \ref ps_dynamic_driver_interface loading interface.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which specifies the configuration.
 * @param [in] command A pointer to \ref ps_command_msg which specifies the command data to be handled.
 * @param [out] response A pointer to \ref ps_response_msg which specifies the response data to be sent by the dynamic driver.
 *
 * @return Response DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_handle_command_function)(
        ps_dynamic_driver * const dynamic_driver,
        const ps_command_msg * const command,
        ps_response_msg * const response );




/**
 * @brief PolySync dynamic driver interface.
 *
 * Provides the interface for a dynamic driver, used by PAL interfaces.
 *
 * PAL interfaces can use this structure and
 * implement the \ref psync_interface_load_function function instead of
 * exporting each of the functions explicitly by symbol name.
 *
 */
typedef struct
{
    //
    //
    psync_interface_expose_options_function expose_options; /*!< See \ref psync_interface_expose_options_function for information. */
    //
    //
    psync_interface_expose_type_support_function expose_type_support; /*!< See \ref psync_interface_expose_type_support_function for information. */
    //
    //
    psync_interface_expose_parameters_function expose_parameters; /*!< See \ref psync_interface_expose_parameters_function for information. */
    //
    //
    psync_interface_expose_sensor_support_function expose_sensor_support; /*!< See \ref psync_interface_expose_sensor_support_function for information. */
    //
    //
    psync_interface_expose_sdf_support_function expose_sdf_support; /*!< See \ref psync_interface_expose_sdf_support_function for information. */
    //
    //
    psync_interface_expose_parameter_groups_function expose_parameter_groups; /*!< Optional, see \ref psync_interface_expose_parameter_groups_function for information. */
    //
    //
    psync_interface_configure_function configure; /*!< See \ref psync_interface_configure_function for information. */
    //
    //
    psync_interface_init_function init; /*!< See \ref psync_interface_init_function for information. */
    //
    //
    psync_interface_release_function release; /*!< See \ref psync_interface_release_function for information. */
    //
    //
    psync_interface_poll_for_data_function poll_for_data; /*!< See \ref psync_interface_poll_for_data_function for information. */
    //
    //
    psync_interface_log_data_function log_data; /*!< See \ref psync_interface_log_data_function for information. */
    //
    //
    psync_interface_process_data_function process_data; /*!< See \ref psync_interface_process_data_function for information. */
    //
    //
    psync_interface_idle_function idle; /*!< Optional, see \ref psync_interface_idle_function for information. */
    //
    //
    psync_interface_handle_parameter_command_function handle_parameter_command; /*!< Optional, see \ref psync_interface_handle_parameter_command_function for information. */
    //
    //
    psync_interface_handle_state_function handle_state; /*!< See \ref psync_interface_handle_state_function for information. */
    //
    //
    psync_interface_handle_command_function handle_command; /*!< Optional, see \ref psync_interface_handle_command_function for information. */
} ps_dynamic_driver_interface;




/**
 * @brief Load dynamic driver interface functions, function type.
 *
 * Provide all of the interface functions used by dynamic driver using a
 * single load interface instead of the older explicit method.
 *
 * @note Returning a DTC other than DTC_NONE will cause the node to transition
 * into the fatal state and terminate immediately.
 *
 * @note PolySync is not initialized at this point, and thus no node related
 * PolySync routines should be called in this context.
 *
 * @warning Expects symbol name to be "psync_interface_load_pal".
 *
 * @param [in/out] dynamic_driver_interface A pointer to \ref ps_dynamic_driver_interface which receives all of the interface's function pointers.
 *
 * @return Response DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
typedef int (*psync_interface_load_pal_function)(
        ps_dynamic_driver_interface * const dynamic_driver_interface );




#ifdef	__cplusplus
}
#endif


#endif	/* POLYSYNC_DYNAMIC_INTERFACE_H */
