/**
 * @file gui.h
 * @brief GUI Interface.
 *
 */




#ifndef GUI_H
#define	GUI_H




#include "polysync_core.h"
#include "gl_headers.h"




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
 * @brief Reset view key.
 *
 */
#define         GUI_KEY_RESET_VIEW          'm'


/**
 * @brief Rotate view key.
 *
 */
#define         GUI_KEY_ROTATE_VIEW          'r'


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
 * @brief Default maximum render rate. [frames per second]
 *
 */
#define         GUI_DEFAULT_MAX_FPS         (33)




/**
 * @brief GUI configuration data.
 *
 */
typedef struct
{
    //
    //
    unsigned int                freeze_frame; /*!< Freeze-frame enabled/disabled.
                                               * Value zero means disabled. Value one means enabled. */
    //
    //
    unsigned int                help_visible; /*!< Help message visibility enabled/disabled.
                                               * Value zero means not visible. Value one means visisble. */
    //
    //
    GLdouble                    zoom_scale; /*!< Zoom scalar, 1.0 is no scale, 2.0 is twice as close. */
    //
    //
    GLdouble                    rotation; /*!< Image rotation. [degrees] */
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
    unsigned long               frame_cnt; /*!< Frame counter. */
    //
    //
    double                      rx_fps; /*!< Received/decoded image FPS. */
    //
    //
    ps_guid                     image_publisher_guid; /*!< Publisher GUID. */
    //
    //
    unsigned long               image_width; /*!< Image data width. [pixels] */
    //
    //
    unsigned long               image_height; /*!< Image data height. [pixels] */
    //
    //
    GLuint                      image_texture; /*!< Image data texture. */
} gui_context_s;




/**
 * @brief Initialize GUI resources.
 *
 * Sets up a rendering window and its resources.
 *
 * @param [in] win_title A pointer to char which specifies the window title string buffer
 * @param [in] win_width Window width.
 * @param [in] win_height Window height.
 *
 * @return A newly created GUI context on success, NULL on failure.
 *
 */
gui_context_s *gui_init( const char *win_title, const unsigned int win_width, const unsigned int win_height );


/**
 * @brief Release GUI resources.
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the context.
 *
 */
void gui_release( gui_context_s * const gui );


//
void gui_update_image_data( gui_context_s * const gui, const unsigned char * buffer, const unsigned long buffer_len );


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
