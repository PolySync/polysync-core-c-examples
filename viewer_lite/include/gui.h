/*
 * Copyright (c) 2015 HARBRICK TECHNOLOGIES, INC
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
 * @file gui.h
 * @brief GUI Interface.
 *
 */




#ifndef GUI_H
#define	GUI_H




#include <glib-2.0/glib.h>
//#include <signal.h>
#include "polysync_core.h"

#include "gl_headers.h"
#include "drawable_type.h"




/**
 * @brief Exit application key.
 *
 */
#define         GUI_KEY_EXIT                'q'


/**
 * @brief Show help key.
 *
 */
#define         GUI_KEY_HELP                'h'


/**
 * @brief Freeze frame key.
 *
 */
#define         GUI_KEY_FREE_FRAME          'f'


/**
 * @brief Toggle radial grid lines key.
 *
 */
#define         GUI_KEY_RADIAL_GRID         'g'


/**
 * @brief Show velocity vectors key.
 *
 */
#define         GUI_KEY_VEL_VECTORS         'v'


/**
 * @brief Toggle color mode key.
 *
 */
#define         GUI_KEY_COLOR_MODE          'c'


/**
 * @brief Toggle radar amplitude mapper key.
 *
 */
#define         GUI_KEY_RADAR_AMP_MAP       'a'


/**
 * @brief Toggle ruler key.
 *
 */
#define         GUI_KEY_RULER               'r'


/**
 * @brief Show platform motion vectors key.
 *
 */
#define         GUI_KEY_PLATFORM_VECTORS    'p'


/**
 * @brief Toggle view mode key.
 *
 */
#define         GUI_KEY_VIEW_MODE           'm'


/**
 * @brief Toggle circle primitive visibility key.
 *
 */
#define         GUI_KEY_CIRCLE_VISIBLE      '9'


/**
 * @brief Toggle rectangle primitive visibility key.
 *
 */
#define         GUI_KEY_RECTANGLE_VISIBLE   '8'


/**
 * @brief Toggle ellipse visibility key.
 *
 */
#define         GUI_KEY_ELLIPSE_VISIBLE     '7'

/**
 * @brief Toggle points visibility key.
 *
 */
#define         GUI_KEY_POINTS_VISIBLE      '6'


/**
 * @brief Invalid \ref gui_context_s.win_id value.
 *
 */
#define         GUI_WINDOW_ID_INVALID       (-1)


/**
 * @brief Flag value indicating an immediate redraw. [\ref ps_timestamp]
 *
 */
#define         GUI_FORCE_REDRAW            (1ULL)


/**
 * @brief Default window width. [pixels]
 *
 */
#define         GUI_DEFAULT_WIDTH           (800)


/**
 * @brief Default window height. [pixels]
 *
 */
#define         GUI_DEFAULT_HEIGHT          (800)


/**
 * @brief Default maximum render rate. [frames per second]
 *
 */
#define         GUI_DEFAULT_MAX_FPS         (33)


/**
 * @brief Default grid surface scale. [meters]
 *
 */
#define         GUI_DEFAULT_GRID_SCALE      (400.0)


/**
 * @brief Default line width. [meters]
 *
 */
#define         GUI_DEFAULT_LINE_WIDTH      (1.1)


/**
 * @brief Default wire frame line width. [meters]
 *
 */
#define         GUI_DEFAULT_WIRE_LINE_WIDTH  (1.35)




/**
 * @brief GUI context data.
 *
 */
typedef struct
{
    //
    //
    view_mode_kind              view_mode; /*!< View mode. */
    //
    //
    color_mode_kind             color_mode; /*!< Color mode. */
    //
    //
    unsigned int                freeze_frame; /*!< Freeze-frame enabled/disabled.
                                               * Value zero means disabled. Value one means enabled. */
    //
    //
    unsigned int                ruler; /*!< Ruler enabled/disabled.
                                        * Value zero means disabled. Value one means enabled. */
    //
    //
    double                      wireframe_width; /*!< Width of wire frame lines. [meters] */
    //
    //
    unsigned int                velocity_vectors_visible; /*!< Entity velocity vector visibility enabled/disabled.
                                                           * Value zero means not visible. Value one means visisble. */
    //
    //
    unsigned int                platform_vectors_visible; /*!< Platform vector visibility enabled/disabled.
                                                           * Value zero means not visible. Value one means visisble. */
    //
    //
    unsigned int                adjusted_circle_radius; /*!< Adjusted circle radius enabled/disabled. Enables the use of radius mapping.
                                                             * Value zero means disabled. Value one means enabled. */
    //
    //
    unsigned int                circle_visible; /*!< Circle primitive visibility enabled/disabled.
                                                * Value zero means not visible. Value one means visisble. */
    //
    //
    unsigned int                rectangle_visible; /*!< Rectangle primitive visibility enabled/disabled.
                                                  * Value zero means not visible. Value one means visisble. */
    //
    //
    unsigned int                ellipse_visible; /*!< Ellilpse primitive visibility enabled/disabled.
                                                         * Value zero means not visible. Value one means visisble. */
    //
    //
    unsigned int                points_visible; /*!< Points primitive visibility enabled/disabled.
                                                 * Value zero means not visible. Value one means visisble. */
    //
    //
    unsigned int                help_visible; /*!< Help message visibility enabled/disabled.
                                               * Value zero means not visible. Value one means visisble. */
    //
    //
    unsigned int                radial_grid_visible; /*!< Radial grid lines visibility enabled/disabled.
                                                      * Value zero means not visible. Value one means visisble. */
    //
    //
    GLdouble                    zoom_scale; /*!< Zoom scalar, 1.0 is no scale, 2.0 is twice as close. */
    //
    //
    GLdouble                    camera_pos[3]; /*!< Position of the camera. [xyz] */
    //
    //
    GLdouble                    view_pos[3]; /*!< Position of the viewing center. [xyz] */
} gui_configuration_s;


/**
 * @brief GUI context data.
 *
 */
typedef struct
{
    //
    //
    int                         gl_argc; /*!< GL argument count. */
    //
    //
    char                        **gl_argv; /*!< GL arugment list. */
    //
    //
    GLenum                      gl_error; /*!< GL error value. */
    //
    //
    char                        win_title[ PSYNC_DEFAULT_STRING_LEN ]; /*!< Window title string. */
    //
    //
    int                         win_id; /*!< Window identifier.
                                         * Value \ref GUI_WINDOW_ID_INVALID means invalid. */
    //
    //
    unsigned int                win_width; /*!< Window width. [pixels] */
    //
    //
    unsigned int                win_height; /*!< Window height. [pixels] */
    //
    //
    double                      grid_scale; /*!< Scale of the grid units (ie 400.0 for a 400 by 400 grid surface). */
    //
    //
    unsigned int                max_fps; /*!< Maximum render frames per second. */
    //
    //
    double                      rendered_fps; /*!< Rendered frames per second. */
    //
    //
    ps_timestamp                last_render_time; /*!< Last render timestamp. [microseconds] */
    //
    //
    double                      mouse_x; /*!< Last mouse X coordinate. */
    //
    //
    double                      mouse_y; /*!< Last mouse Y coordinate. */
    //
    //
    unsigned int                mouse_state; /*!< Last mouse button state. */
    //
    //
    unsigned int                mouse_button; /*!< Last mouse button. */
    //
    //
    gui_configuration_s         config; /*!< Configuration data. */
    //
    //
    platform_data_s             platform; /*!< Platform data. */
    //
    //
    ruler_data_s                ruler; /*!< Ruler data. */
    //
    //
    GList                       *entity_list; /*!< GList of on-bus parent entities and their children (\ref object_container_parent_s). */
} gui_context_s;




/**
 * @brief Initialize GUI resources.
 *
 * Sets up a rendering window and its resources.
 *
 * @param [in] win_title A pointer to char which specifies the window title string buffer
 * @param [in] win_width Window width.
 * @param [in] win_height Window height.
 * @param [in] grid_scale Grid surface scale, see \ref GUI_DEFAULT_GRID_SCALE. [meters]
 *
 * @return A newly created GUI context on success, NULL on failure.
 *
 */
gui_context_s *gui_init( const char *win_title, const unsigned int win_width, const unsigned int win_height, const double grid_scale );


/**
 * @brief Release GUI resources.
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the context.
 *
 */
void gui_release( gui_context_s * const gui );


/**
 * @brief Update GUI and possibly redraw.
 *
 * Triggers a redraw based on the configured frame rate (see \ref gui_context_s.max_fps).
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the context.
 * @param [in] update_time Current timestamp used to update rendering with limiter.
 * @param [out] time_to_redraw A pointer to \ref ps_timestamp which receives the time remaining until the last redraw.
 * If set to value \ref GUI_FORCE_REDRAW, forces a redraw.
 *
 */
void gui_update( gui_context_s * const gui, const ps_timestamp update_time, ps_timestamp * const time_to_redraw );




#endif	/* GUI_H */
