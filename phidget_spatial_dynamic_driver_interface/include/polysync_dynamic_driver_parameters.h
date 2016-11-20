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
 * @file polysync_dynamic_driver_parameters.h
 * @brief PolySync Dynamic Driver Get/Set Parameters.
 *
 * PUBLIC_HEADER
 */




#ifndef POLYSYNC_DYNAMIC_DRIVER_PARAMETERS_H
#define POLYSYNC_DYNAMIC_DRIVER_PARAMETERS_H




#include "polysync_core.h"




/**
 * @brief Parameter ID - Publishes message type 'ps_diagnostic_trace_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_DIAGNOSTIC_TRACE (2000)


/**
 * @brief Parameter description - Publishes message type 'ps_diagnostic_trace_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_DIAGNOSTIC_TRACE "Publishes message type \"ps_diagnostic_trace_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_diagnostic_trace_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_DIAGNOSTIC_TRACE (2001)


/**
 * @brief Parameter description - Subscribes to message type 'ps_diagnostic_trace_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_DIAGNOSTIC_TRACE "Subscribes to message type \"ps_diagnostic_trace_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_byte_array_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_BYTE_ARRAY (2002)


/**
 * @brief Parameter description - Publishes message type 'ps_byte_array_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_BYTE_ARRAY "Publishes message type \"ps_byte_array_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_byte_array_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_BYTE_ARRAY (2003)


/**
 * @brief Parameter description - Subscribes to message type 'ps_byte_array_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_BYTE_ARRAY "Subscribes to message type \"ps_byte_array_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_can_frame_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_CAN_FRAME (2004)


/**
 * @brief Parameter description - Publishes message type 'ps_can_frame_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_CAN_FRAME "Publishes message type \"ps_can_frame_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_can_frame_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_CAN_FRAME (2005)


/**
 * @brief Parameter description - Subscribes to message type 'ps_can_frame_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_CAN_FRAME "Subscribes to message type \"ps_can_frame_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_event_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_EVENT (2006)


/**
 * @brief Parameter description - Publishes message type 'ps_event_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_EVENT "Publishes message type \"ps_event_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_event_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_EVENT (2007)


/**
 * @brief Parameter description - Subscribes to message type 'ps_event_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_EVENT "Subscribes to message type \"ps_event_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_parameters_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PARAMETERS (2008)


/**
 * @brief Parameter description - Publishes message type 'ps_parameters_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PARAMETERS "Publishes message type \"ps_parameters_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_parameters_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PARAMETERS (2009)


/**
 * @brief Parameter description - Subscribes to message type 'ps_parameters_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PARAMETERS "Subscribes to message type \"ps_parameters_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_rnr_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_RNR (2010)


/**
 * @brief Parameter description - Publishes message type 'ps_rnr_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_RNR "Publishes message type \"ps_rnr_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_rnr_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_RNR (2011)


/**
 * @brief Parameter description - Subscribes to message type 'ps_rnr_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_RNR "Subscribes to message type \"ps_rnr_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_sdf_state_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_SDF_STATE (2012)


/**
 * @brief Parameter description - Publishes message type 'ps_sdf_state_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_SDF_STATE "Publishes message type \"ps_sdf_state_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_sdf_state_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_SDF_STATE (2013)


/**
 * @brief Parameter description - Subscribes to message type 'ps_sdf_state_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_SDF_STATE "Subscribes to message type \"ps_sdf_state_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_rnr_sessions_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_RNR_SESSIONS (2014)


/**
 * @brief Parameter description - Publishes message type 'ps_rnr_sessions_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_RNR_SESSIONS "Publishes message type \"ps_rnr_sessions_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_rnr_sessions_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_RNR_SESSIONS (2015)


/**
 * @brief Parameter description - Subscribes to message type 'ps_rnr_sessions_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_RNR_SESSIONS "Subscribes to message type \"ps_rnr_sessions_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_file_transfer_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_FILE_TRANSFER (2016)


/**
 * @brief Parameter description - Publishes message type 'ps_file_transfer_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_FILE_TRANSFER "Publishes message type \"ps_file_transfer_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_file_transfer_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_FILE_TRANSFER (2017)


/**
 * @brief Parameter description - Subscribes to message type 'ps_file_transfer_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_FILE_TRANSFER "Subscribes to message type \"ps_file_transfer_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_file_transfer_ext_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_FILE_TRANSFER_EXT (2018)


/**
 * @brief Parameter description - Publishes message type 'ps_file_transfer_ext_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_FILE_TRANSFER_EXT "Publishes message type \"ps_file_transfer_ext_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_file_transfer_ext_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_FILE_TRANSFER_EXT (2019)


/**
 * @brief Parameter description - Subscribes to message type 'ps_file_transfer_ext_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_FILE_TRANSFER_EXT "Subscribes to message type \"ps_file_transfer_ext_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_file_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_FILE (2020)


/**
 * @brief Parameter description - Publishes message type 'ps_file_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_FILE "Publishes message type \"ps_file_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_file_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_FILE (2021)


/**
 * @brief Parameter description - Subscribes to message type 'ps_file_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_FILE "Subscribes to message type \"ps_file_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_file_ext_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_FILE_EXT (2022)


/**
 * @brief Parameter description - Publishes message type 'ps_file_ext_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_FILE_EXT "Publishes message type \"ps_file_ext_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_file_ext_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_FILE_EXT (2023)


/**
 * @brief Parameter description - Subscribes to message type 'ps_file_ext_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_FILE_EXT "Subscribes to message type \"ps_file_ext_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_COMMAND (2024)


/**
 * @brief Parameter description - Publishes message type 'ps_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_COMMAND "Publishes message type \"ps_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_COMMAND (2025)


/**
 * @brief Parameter description - Subscribes to message type 'ps_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_COMMAND "Subscribes to message type \"ps_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_response_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_RESPONSE (2026)


/**
 * @brief Parameter description - Publishes message type 'ps_response_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_RESPONSE "Publishes message type \"ps_response_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_response_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_RESPONSE (2027)


/**
 * @brief Parameter description - Subscribes to message type 'ps_response_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_RESPONSE "Subscribes to message type \"ps_response_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_manager_status_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_MANAGER_STATUS (2028)


/**
 * @brief Parameter description - Publishes message type 'ps_manager_status_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_MANAGER_STATUS "Publishes message type \"ps_manager_status_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_manager_status_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_MANAGER_STATUS (2029)


/**
 * @brief Parameter description - Subscribes to message type 'ps_manager_status_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_MANAGER_STATUS "Subscribes to message type \"ps_manager_status_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_image_data_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_IMAGE_DATA (2030)


/**
 * @brief Parameter description - Publishes message type 'ps_image_data_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_IMAGE_DATA "Publishes message type \"ps_image_data_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_image_data_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_IMAGE_DATA (2031)


/**
 * @brief Parameter description - Subscribes to message type 'ps_image_data_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_IMAGE_DATA "Subscribes to message type \"ps_image_data_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_motion_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_MOTION (2032)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_motion_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_MOTION "Publishes message type \"ps_platform_motion_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_motion_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_MOTION (2033)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_motion_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_MOTION "Subscribes to message type \"ps_platform_motion_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_imu_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_IMU (2034)


/**
 * @brief Parameter description - Publishes message type 'ps_imu_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_IMU "Publishes message type \"ps_imu_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_imu_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_IMU (2035)


/**
 * @brief Parameter description - Subscribes to message type 'ps_imu_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_IMU "Subscribes to message type \"ps_imu_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_gps_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_GPS (2036)


/**
 * @brief Parameter description - Publishes message type 'ps_gps_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_GPS "Publishes message type \"ps_gps_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_gps_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_GPS (2037)


/**
 * @brief Parameter description - Subscribes to message type 'ps_gps_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_GPS "Subscribes to message type \"ps_gps_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_radar_targets_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_RADAR_TARGETS (2038)


/**
 * @brief Parameter description - Publishes message type 'ps_radar_targets_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_RADAR_TARGETS "Publishes message type \"ps_radar_targets_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_radar_targets_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_RADAR_TARGETS (2039)


/**
 * @brief Parameter description - Subscribes to message type 'ps_radar_targets_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_RADAR_TARGETS "Subscribes to message type \"ps_radar_targets_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_lidar_points_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_LIDAR_POINTS (2040)


/**
 * @brief Parameter description - Publishes message type 'ps_lidar_points_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_LIDAR_POINTS "Publishes message type \"ps_lidar_points_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_lidar_points_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_LIDAR_POINTS (2041)


/**
 * @brief Parameter description - Subscribes to message type 'ps_lidar_points_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_LIDAR_POINTS "Subscribes to message type \"ps_lidar_points_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_objects_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_OBJECTS (2042)


/**
 * @brief Parameter description - Publishes message type 'ps_objects_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_OBJECTS "Publishes message type \"ps_objects_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_objects_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_OBJECTS (2043)


/**
 * @brief Parameter description - Subscribes to message type 'ps_objects_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_OBJECTS "Subscribes to message type \"ps_objects_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_lane_model_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_LANE_MODEL (2044)


/**
 * @brief Parameter description - Publishes message type 'ps_lane_model_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_LANE_MODEL "Publishes message type \"ps_lane_model_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_lane_model_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_LANE_MODEL (2045)


/**
 * @brief Parameter description - Subscribes to message type 'ps_lane_model_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_LANE_MODEL "Subscribes to message type \"ps_lane_model_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_traffic_sign_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_TRAFFIC_SIGN (2046)


/**
 * @brief Parameter description - Publishes message type 'ps_traffic_sign_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_TRAFFIC_SIGN "Publishes message type \"ps_traffic_sign_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_traffic_sign_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_TRAFFIC_SIGN (2047)


/**
 * @brief Parameter description - Subscribes to message type 'ps_traffic_sign_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_TRAFFIC_SIGN "Subscribes to message type \"ps_traffic_sign_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_gps_accuracy_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_GPS_ACCURACY (2048)


/**
 * @brief Parameter description - Publishes message type 'ps_gps_accuracy_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_GPS_ACCURACY "Publishes message type \"ps_gps_accuracy_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_gps_accuracy_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_GPS_ACCURACY (2049)


/**
 * @brief Parameter description - Subscribes to message type 'ps_gps_accuracy_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_GPS_ACCURACY "Subscribes to message type \"ps_gps_accuracy_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_imu_accuracy_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_IMU_ACCURACY (2050)


/**
 * @brief Parameter description - Publishes message type 'ps_imu_accuracy_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_IMU_ACCURACY "Publishes message type \"ps_imu_accuracy_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_imu_accuracy_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_IMU_ACCURACY (2051)


/**
 * @brief Parameter description - Subscribes to message type 'ps_imu_accuracy_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_IMU_ACCURACY "Subscribes to message type \"ps_imu_accuracy_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_control_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_CONTROL (2052)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_control_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_CONTROL "Publishes message type \"ps_platform_control_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_control_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_CONTROL (2053)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_control_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_CONTROL "Subscribes to message type \"ps_platform_control_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_brake_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_BRAKE_COMMAND (2054)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_brake_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_BRAKE_COMMAND "Publishes message type \"ps_platform_brake_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_brake_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_BRAKE_COMMAND (2055)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_brake_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_BRAKE_COMMAND "Subscribes to message type \"ps_platform_brake_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_brake_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_BRAKE_REPORT (2056)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_brake_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_BRAKE_REPORT "Publishes message type \"ps_platform_brake_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_brake_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_BRAKE_REPORT (2057)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_brake_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_BRAKE_REPORT "Subscribes to message type \"ps_platform_brake_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_throttle_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_THROTTLE_COMMAND (2058)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_throttle_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_THROTTLE_COMMAND "Publishes message type \"ps_platform_throttle_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_throttle_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_THROTTLE_COMMAND (2059)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_throttle_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_THROTTLE_COMMAND "Subscribes to message type \"ps_platform_throttle_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_throttle_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_THROTTLE_REPORT (2060)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_throttle_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_THROTTLE_REPORT "Publishes message type \"ps_platform_throttle_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_throttle_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_THROTTLE_REPORT (2061)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_throttle_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_THROTTLE_REPORT "Subscribes to message type \"ps_platform_throttle_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_steering_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_STEERING_COMMAND (2062)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_steering_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_STEERING_COMMAND "Publishes message type \"ps_platform_steering_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_steering_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_STEERING_COMMAND (2063)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_steering_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_STEERING_COMMAND "Subscribes to message type \"ps_platform_steering_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_steering_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_STEERING_REPORT (2064)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_steering_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_STEERING_REPORT "Publishes message type \"ps_platform_steering_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_steering_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_STEERING_REPORT (2065)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_steering_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_STEERING_REPORT "Subscribes to message type \"ps_platform_steering_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_gear_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_GEAR_COMMAND (2066)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_gear_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_GEAR_COMMAND "Publishes message type \"ps_platform_gear_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_gear_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_GEAR_COMMAND (2067)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_gear_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_GEAR_COMMAND "Subscribes to message type \"ps_platform_gear_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_gear_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_GEAR_REPORT (2068)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_gear_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_GEAR_REPORT "Publishes message type \"ps_platform_gear_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_gear_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_GEAR_REPORT (2069)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_gear_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_GEAR_REPORT "Subscribes to message type \"ps_platform_gear_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_turn_signal_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_TURN_SIGNAL_COMMAND (2070)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_turn_signal_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_TURN_SIGNAL_COMMAND "Publishes message type \"ps_platform_turn_signal_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_turn_signal_command_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_TURN_SIGNAL_COMMAND (2071)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_turn_signal_command_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_TURN_SIGNAL_COMMAND "Subscribes to message type \"ps_platform_turn_signal_command_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_cabin_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_CABIN_REPORT (2072)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_cabin_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_CABIN_REPORT "Publishes message type \"ps_platform_cabin_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_cabin_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_CABIN_REPORT (2073)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_cabin_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_CABIN_REPORT "Subscribes to message type \"ps_platform_cabin_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_suspension_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_SUSPENSION_REPORT (2074)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_suspension_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_SUSPENSION_REPORT "Publishes message type \"ps_platform_suspension_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_suspension_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_SUSPENSION_REPORT (2075)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_suspension_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_SUSPENSION_REPORT "Subscribes to message type \"ps_platform_suspension_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_tire_pressure_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_TIRE_PRESSURE_REPORT (2076)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_tire_pressure_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_TIRE_PRESSURE_REPORT "Publishes message type \"ps_platform_tire_pressure_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_tire_pressure_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_TIRE_PRESSURE_REPORT (2077)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_tire_pressure_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_TIRE_PRESSURE_REPORT "Subscribes to message type \"ps_platform_tire_pressure_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_wheel_speed_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_WHEEL_SPEED_REPORT (2078)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_wheel_speed_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_WHEEL_SPEED_REPORT "Publishes message type \"ps_platform_wheel_speed_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_wheel_speed_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_WHEEL_SPEED_REPORT (2079)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_wheel_speed_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_WHEEL_SPEED_REPORT "Subscribes to message type \"ps_platform_wheel_speed_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_obd_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_OBD (2080)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_obd_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_OBD "Publishes message type \"ps_platform_obd_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_obd_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_OBD (2081)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_obd_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_OBD "Subscribes to message type \"ps_platform_obd_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_zones_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_ZONES (2082)


/**
 * @brief Parameter description - Publishes message type 'ps_zones_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_ZONES "Publishes message type \"ps_zones_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_zones_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_ZONES (2083)


/**
 * @brief Parameter description - Subscribes to message type 'ps_zones_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_ZONES "Subscribes to message type \"ps_zones_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_cabin_ext_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_CABIN_EXT_REPORT (2084)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_cabin_ext_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_CABIN_EXT_REPORT "Publishes message type \"ps_platform_cabin_ext_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_cabin_ext_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_CABIN_EXT_REPORT (2085)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_cabin_ext_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_CABIN_EXT_REPORT "Subscribes to message type \"ps_platform_cabin_ext_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_brake_information_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_BRAKE_INFORMATION (2086)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_brake_information_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_BRAKE_INFORMATION "Publishes message type \"ps_platform_brake_information_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_brake_information_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_BRAKE_INFORMATION (2087)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_brake_information_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_BRAKE_INFORMATION "Subscribes to message type \"ps_platform_brake_information_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Publishes message type 'ps_platform_surround_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_PUB_PLATFORM_SURROUND_REPORT (2088)


/**
 * @brief Parameter description - Publishes message type 'ps_platform_surround_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_PUB_PLATFORM_SURROUND_REPORT "Publishes message type \"ps_platform_surround_report_msg\" (value 0 means disabled)"


/**
 * @brief Parameter ID - Subscribes to message type 'ps_platform_surround_report_msg'.
 *
 * \li value type: \ref PARAMETER_VALUE_ULONGLONG
 * \li type: boolean
 * \li units: NA
 *
 * Custom parameter ID.
 *
 */
#define PARAM_ID_SUB_PLATFORM_SURROUND_REPORT (2089)


/**
 * @brief Parameter description - Subscribes to message type 'ps_platform_surround_report_msg'.
 *
 * Used in the static parameter construction.
 *
 */
#define PARAM_DESC_SUB_PLATFORM_SURROUND_REPORT "Subscribes to message type \"ps_platform_surround_report_msg\" (value 0 means disabled)"




#endif	/* POLYSYNC_DYNAMIC_DRIVER_PARAMETERS_H */
