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
 * @file vehicle_control.h
 * @brief Simple control of a vehicle.
 *
 */




#ifndef VEHICLE_CONTROL_H
#define VEHICLE_CONTROL_H




#include "math.h"

#include "gui.h"



/**
 * @brief Proportional control factor used to do steering control. [int]
 *
 */
#define STEERING_CONTROL_FACTOR (2000)

/**
 * @brief Default throttle for vehicle control. [int]
 *
 */
#define DEFAULT_THROTTLE (5)

/**
 * @brief String name for steering command msg. [int]
 *
 */
#define STEERING_CMD_MSG_NAME "ps_platform_steering_command_msg" 

/**
 * @brief String name for throttle command msg. [int]
 *
 */
#define THROTTLE_CMD_MSG_NAME "ps_platform_throttle_command_msg"



/**
 * @brief Send steering and throttle messages for vehicle control.
 * 
 * @param [in] user_data Pointer to struct which contains global variables for this application.
 * @param [out] waypoints Pointer to list of waypoints which have been selected by user.
 *
 */
void send_psync_messages_for_vehicle_control( const node_data_s * const user_data, waypoint_s * const waypoints );


#endif /* VEHICLE_CONTROL_H */

