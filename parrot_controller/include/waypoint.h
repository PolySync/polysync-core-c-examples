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
 * @file waypoint.h
 * @brief Waypoint storage.
 *
 */




#ifndef WAYPOINT_H
#define WAYPOINT_H


#include "gui.h"



/**
 * @brief Max number of waypoints stored and displayed on screen. [int]
 *
 */
#define MAX_WAYPOINTS (10)

/**
 * @brief Distance at which the parrot is considered to have arrived at its destination. [int]
 *
 */
#define REACHED_WAYPOINT_DISTANCE (10)




/**
 * @brief Waypoint.
 *
 */
typedef struct
{
    //
    //
    double x; /*!< Waypoint X coordinate. */
    //
    //
    double y; /*!< Waypoint Y coordinate. */
    //
    //
    double heading; /*!< Waypoint heading. */
    //
    //
    int valid; /*!< Is Waypoint valid */
} waypoint_s;



/**
 * @brief Invalidate all waypoints in waypoint array. [void]
 * 
 * @param [out] waypoints Pointer to list of waypoints which have been selected by user.
 *
 */
void zero_waypoints( waypoint_s * const waypoints );

/**
 * @brief Add new waypoint to waypoint array. [void]
 * 
 * @param [in] x Coordinate used to add the new waypoint to the array. 
 * @param [in] y Coordinate used to add the new waypoint to the array. 
 * @param [out] waypoints Pointer to list of waypoints which have been selected by user.
 *
 */
void add_new_waypoint( const double x, const double y, waypoint_s * const waypoints );



#endif /* WAYPOINT_H */

