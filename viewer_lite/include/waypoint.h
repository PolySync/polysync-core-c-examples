/* 
 * File:   waypoint.h
 * Author: lbuckland
 *
 * Created on September 2, 2016, 4:37 PM
 */

#ifndef WAYPOINT_H
#define WAYPOINT_H

#define MAX_WAYPOINTS 10

#define REACHED_WAYPOINT_DISTANCE 1


/**
 * @brief Waypoint.
 *
 */
typedef struct
{
    //
    //
    double                      x; /*!< Waypoint X coordinate. */
    //
    //
    double                      y; /*!< Waypoint Y coordinate. */
    //
    //
    double                      heading; /*!< Waypoint heading. */
    //
    //
    int                         valid; /*!< Is Waypoint valid */
} waypoint_s;

#endif /* WAYPOINT_H */

