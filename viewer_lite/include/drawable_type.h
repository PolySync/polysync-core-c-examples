/**
 * @file drawable_type.h
 * @brief Drawable Types and Resources.
 *
 */




#ifndef DRAWABLE_TYPE_H
#define	DRAWABLE_TYPE_H




#include <glib-2.0/glib.h>
#include "polysync_core.h"
#include "gl_headers.h"




/**
 * @brief Color mode kinds.
 *
 */
typedef enum
{
    //
    //
    COLOR_MODE_OBJECT_ID = 0, /*!< Objects are colored based on their object identifier. */
    //
    //
    COLOR_MODE_CONTAINER_ID, /*!< Objects are colored based on their container identifier. */
    //
    //
    COLOR_MODE_PARENT_ID, /*!< Objects are colored based on their parent identifier. */
    //
    //
    COLOR_MODE_KIND_COUNT, /*!< Number of \ref color_mode_kind values. */
} color_mode_kind;


/**
 * @brief Drawable primitive kinds.
 *
 */
typedef enum
{
    //
    //
    PRIMITIVE_CIRCLE = 0, /*!< Circle with radius in 2D, sphere with radius in 3D. */
    //
    //
    PRIMITIVE_CROSS, /*!< Cross with length and width in 2D, cross with length and width above the ground plane in 3D. */
    //
    //
    PRIMITIVE_RECTANGLE, /*!< Rectangle with length and width in 2D, rectangle with length, width, and height in 3D. */
    //
    //
    PRIMITIVE_TRIANGLE, /*!< Triangle with length and width in 2D, triangle with length and width above the ground plane in 3D. */
    //
    //
    PRIMITIVE_ELLIPSE, /*!< Ellipse with length and width in 2D, ellipse with length and width above the ground plane in 3D. */
    //
    //
    PRIMITIVE_KIND_COUNT, /*!< Number of \ref primitive_kind values. */
} primitive_kind;


/**
 * @brief View mode kinds.
 *
 */
typedef enum
{
    //
    //
    VIEW_MODE_BIRDSEYE, /*!< 2D "birdseye" view mode. */
    //
    //
    VIEW_MODE_PERSPECTIVE, /*!< 3D "perspective" view mode. */
    //
    //
    VIEW_MODE_SIDE, /*!< 2D "side" view mode. */
    //
    //
    VIEW_MODE_KIND_COUNT /*!< Number of \ref view_mode_kind values. */
} view_mode_kind;




/**
 * @brief Ruler data.
 *
 */
typedef struct
{
    //
    //
    GLdouble                color_rgba[4];
    //
    //
    unsigned int            p1_set;
    //
    //
    GLdouble                x1;
    //
    //
    GLdouble                y1;
    //
    //
    GLdouble                z1;
    //
    //
    unsigned int            p2_set;
    //
    //
    GLdouble                x2;
    //
    //
    GLdouble                y2;
    //
    //
    GLdouble                z2;
    //
    //
    double                  distance;
    //
    //
    char                    p1_string[PSYNC_DEFAULT_STRING_LEN];
    //
    //
    char                    p2_string[PSYNC_DEFAULT_STRING_LEN];
    //
    //
    char                    distance_string[PSYNC_DEFAULT_STRING_LEN];
    //
    //
    double                  xy_angle;
} ruler_data_s;


/**
 * @brief Platform data.
 *
 */
typedef struct
{
    //
    //
    unsigned long long      id;
    //
    //
    unsigned long long      container_id;
    //
    //
    unsigned long long      parent_id;
    //
    //
    ps_timestamp            update_time;
    //
    //
    ps_timestamp            timeout_interval;
    //
    //
    GLdouble                color_rgba[4];
    //
    //
    double                  x;
    //
    //
    double                  y;
    //
    //
    double                  z;
	//
    //
    double                  length;
    //
    //
    double                  width;
    //
    //
    double                  height;
    //
    //
    unsigned int            rotation_rate_valid;
    //
    //
    double                  wx;
    //
    //
    double                  wy;
    //
    //
    double                  wz;
    //
    //
    unsigned int            velocity_valid;
    //
    //
    double                  speed;
    //
    //
    double                  vx;
    //
    //
    double                  vy;
    //
    //
    double                  vz;
    //
    //
    double                  adjusted_vx;
    //
    //
    double                  adjusted_vy;
    //
    //
    double                  adjusted_vz;
} platform_data_s;


/**
 * @brief Object data.
 *
 */
typedef struct
{
    //
    //
    unsigned long long      id;
    //
    //
    unsigned long long      container_id;
    //
    //
    unsigned long long      parent_id;
    //
    //
    primitive_kind          primitive;
    //
    //
    ps_timestamp            update_time;
    //
    //
    ps_timestamp            timeout_interval;
    //
    //
    GLdouble                color_rgba[4];
    //
    //
    double                  x;
    //
    //
    double                  y;
    //
    //
    double                  z;
	//
    //
    double                  sigma_x;
    //
    //
    double                  sigma_y;
    //
    //
    double                  sigma_z;
    //
    //
    double                  vx;
    //
    //
    double                  vy;
    //
    //
    double                  vz;
	//
    //
    double                  gate_x;
    //
    //
    double                  gate_y;
    //
    //
    double                  gate_z;
    //
    //
    double                  length;
    //
    //
    double                  width;
    //
    //
    double                  height;
    //
    //
    double                  radius;
    //
    //
    double                  adjusted_radius;
    //
    //
    double                  orientation;
} object_s;


/**
 * @brief Object container data.
 *
 */
typedef struct
{
    //
    //
    unsigned long long      id;
    //
    //
    ps_timestamp            update_time;
    //
    //
    ps_timestamp            timeout_interval;
    //
    //
    GLdouble                color_rgba[4];
    //
    //
    GList                   *objects;
} object_container_s;


/**
 * @brief Object container parent data.
 *
 */
typedef struct
{
    //
    //
    unsigned long long      id;
    //
    //
    ps_timestamp            update_time;
    //
    //
    ps_timestamp            timeout_interval;
    //
    //
    GLdouble                color_rgba[4];
    //
    //
    GList                   *containers;
} object_container_parent_s;




#endif	/* DRAWABLE_TYPE_H */
