/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   vehicle_control.h
 * Author: lbuckland
 *
 * Created on September 6, 2016, 3:55 PM
 */

#ifndef VEHICLE_CONTROL_H
#define VEHICLE_CONTROL_H

#include "math.h"

#include "gui.h"


#define RAD2DEG (180.0/M_PI)

#define DEG2RAD (M_PI/180.0)

#define STEERING_CONTROL_FACTOR 1000

#define STEERING_CMD_MSG_NAME "ps_platform_steering_command_msg" 

#define THROTTLE_CMD_MSG_NAME "ps_platform_throttle_command_msg"


void send_psync_messages_for_vehicle_control( node_data_s * user_data, waypoint_s * waypoints );


#endif /* VEHICLE_CONTROL_H */

