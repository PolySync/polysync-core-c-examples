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
 * @file polysync_dynamic_driver_commands.h
 * @brief PolySync Dynamic Driver Commands.
 *
 * PUBLIC_HEADER
 */




#ifndef POLYSYNC_DYNAMIC_DRIVER_COMMANDS_H
#define POLYSYNC_DYNAMIC_DRIVER_COMMANDS_H




#include "polysync_core.h"




/**
 * @brief Enumerate all dynamic driver interfaces.
 *
 * Request all dynamic driver interfaces to respond.
 *
 * Command Data: NA
 *
 * Response:
 * \li Message type: \ref ps_response_msg
 *
 * Response Data: NA
 *
 */
#define COMMAND_ID_ENUMERATE_DYNAMIC_DRIVER_INTERFACE (1000)


/**
 * @brief Enumerate all dynamic driver interfaces supporting the requested sensor type.
 *
 * Request all dynamic driver interfaces who support the provided ps_sensor_kind value to respond.
 *
 * Command Data:
 * \li Expects data element zero to be type unsigned long long - which is the \ref ps_sensor_kind to request
 *
 * Response:
 * \li Message type: \ref ps_response_msg
 *
 * Response Data:
 * \li Copy of the requested \ref ps_sensor_kind value in data element zero
 *
 */
#define COMMAND_ID_ENUMERATE_DYNAMIC_DRIVER_INTERFACE_BY_SENSOR (1001)


/**
 * @brief Request the process identifier (PID) from a dynamic driver interface's node.
 *
 * To request all dynamic driver interfaces to respond:
 *   - ps_command_msg.dest_guid = PSYNC_GUID_INVALID
 *   - OR
 *   - psync_guid_set_node_type( PSYNC_NODE_TYPE_ALL, &dest_guid )
 *
 * Command Data: NA
 *
 * Response:
 * \li Message type: \ref ps_response_msg
 *
 * Response Data:
 * \li PID of the responding node in data element zero if command was handled successfully
 *   - ps_response_msg.dtc == DTC_NONE
 *   - ps_response_msg.data._buffer[0]._u.ull_value == PID
 *   - ps_response_msg.data._buffer[0]._d == PARAMETER_VALUE_ULONGLONG
 *   - ps_response_msg.data._length == 1
 *
 */
#define COMMAND_ID_GET_DYNAMIC_DRIVER_PROCESS_ID (1010)


/**
 * @brief Request a dynamic driver to publish its RnR status message.
 *
 * To request all dynamic driver interfaces to respond:
 *   - ps_command_msg.dest_guid = PSYNC_GUID_INVALID
 *   - OR
 *   - psync_guid_set_node_type( PSYNC_NODE_TYPE_ALL, &dest_guid )
 *
 * Command Data: NA
 *
 * Response:
 * \li Message type: \ref ps_response_msg
 * \li Message type: \ref ps_rnr_msg
 *   - contains the current RnR status information
 *
 * Response Data: NA
 *
 */
#define COMMAND_ID_GET_DYNAMIC_DRIVER_RNR_STATUS (1011)


/**
 * @brief Request the current SDF path from a dynamic driver.
 *
 * To request all dynamic driver interfaces to respond:
 *   - ps_command_msg.dest_guid = PSYNC_GUID_INVALID
 *   - OR
 *   - psync_guid_set_node_type( PSYNC_NODE_TYPE_ALL, &dest_guid )
 *
 * Command Data: NA
 *
 * Response:
 * \li Message type: \ref ps_response_msg
 *
 * Response Data:
 * \li NULL terminated SDF path string in data element zero if command was handled successfully
 *   - ps_response_msg.dtc == DTC_NONE
 *   - ps_response_msg.data._buffer[0]._u.str_value._buffer == SDF path
 *   - ps_response_msg.data._buffer[0]._u.str_value._length == string length including NULL terminator
 *   - ps_response_msg.data._buffer[0]._d == PARAMETER_VALUE_STRING
 *   - ps_response_msg.data._length == 1
 *
 */
#define COMMAND_ID_GET_DYNAMIC_DRIVER_SDF_PATH (1012)


/**
 * @brief Request the interface name from a dynamic driver.
 *
 * To request all dynamic driver interfaces to respond:
 *   - ps_command_msg.dest_guid = PSYNC_GUID_INVALID
 *   - OR
 *   - psync_guid_set_node_type( PSYNC_NODE_TYPE_ALL, &dest_guid )
 *
 * Command Data: NA
 *
 * Response:
 * \li Message type: \ref ps_response_msg
 *
 * Response Data:
 * \li NULL terminated name string in data element zero if command was handled successfully
 *   - ps_response_msg.dtc == DTC_NONE
 *   - ps_response_msg.data._buffer[0]._u.str_value._buffer == interface name
 *   - ps_response_msg.data._buffer[0]._u.str_value._length == string length including NULL terminator
 *   - ps_response_msg.data._buffer[0]._d == PARAMETER_VALUE_STRING
 *   - ps_response_msg.data._length == 1
 *
 */
#define COMMAND_ID_GET_DYNAMIC_DRIVER_INTERFACE_NAME (1013)


/**
 * @brief Request the SDF node name from a dynamic driver.
 *
 * To request all dynamic driver interfaces to respond:
 *   - ps_command_msg.dest_guid = PSYNC_GUID_INVALID
 *   - OR
 *   - psync_guid_set_node_type( PSYNC_NODE_TYPE_ALL, &dest_guid )
 *
 * Command Data: NA
 *
 * Response:
 * \li Message type: \ref ps_response_msg
 *
 * Response Data:
 * \li NULL terminated name string in data element zero if command was handled successfully
 *   - ps_response_msg.dtc == DTC_NONE
 *   - ps_response_msg.data._buffer[0]._u.str_value._buffer == SDF node name
 *   - ps_response_msg.data._buffer[0]._u.str_value._length == string length including NULL terminator
 *   - ps_response_msg.data._buffer[0]._d == PARAMETER_VALUE_STRING
 *   - ps_response_msg.data._length == 1
 *
 */
#define COMMAND_ID_GET_DYNAMIC_DRIVER_SDF_NODE_NAME (1014)


/**
 * @brief Request a dynamic driver to publish its SDF state message.
 *
 * To request all dynamic driver interfaces to respond:
 *   - ps_command_msg.dest_guid = PSYNC_GUID_INVALID
 *   - OR
 *   - psync_guid_set_node_type( PSYNC_NODE_TYPE_ALL, &dest_guid )
 *
 * Command Data: NA
 *
 * Response:
 * \li Message type: \ref ps_response_msg
 * \li Message type: \ref ps_sdf_state_msg
 *   - contains the SDF state information loaded on startup
 *
 * Response Data: NA
 *
 */
#define COMMAND_ID_GET_DYNAMIC_DRIVER_SDF_STATE (1015)


/**
 * @brief Request the interface address string from a dynamic driver.
 *
 * To request all dynamic driver interfaces to respond:
 *   - ps_command_msg.dest_guid = PSYNC_GUID_INVALID
 *   - OR
 *   - psync_guid_set_node_type( PSYNC_NODE_TYPE_ALL, &dest_guid )
 *
 * Command Data: NA
 *
 * Response:
 * \li Message type: \ref ps_response_msg
 *
 * Response Data:
 * \li NULL terminated interface address string in data element zero if command was handled successfully
 *   - ps_response_msg.dtc == DTC_NONE
 *   - ps_response_msg.data._buffer[0]._u.str_value._buffer == interface address
 *   - ps_response_msg.data._buffer[0]._u.str_value._length == string length including NULL terminator
 *   - ps_response_msg.data._buffer[0]._d == PARAMETER_VALUE_STRING
 *   - ps_response_msg.data._length == 1
 *
 */
#define COMMAND_ID_GET_DYNAMIC_DRIVER_INTERFACE_ADDRESS (1016)




#endif	/* POLYSYNC_DYNAMIC_DRIVER_COMMANDS_H */
