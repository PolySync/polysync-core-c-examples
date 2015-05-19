/******************************************************
** FILE: vsv_structures.h
**
** ABSTRACT:
**
** DOCUMENTS:
**
** AUTHOR:
** JL
**
** NOTES:
**
*******************************************************/




/**
 * @example vsv_structures.h
 *
 * Example Project Data structures.
 *
 */



#ifndef VSV_STRUCTURES_H
#define	VSV_STRUCTURES_H




/**
 * @brief General context for viewer data.
 *
 */
typedef struct
{
    //
    //
    char                        window_title[ PSYNC_DEFAULT_STRING_LEN ]; /**< window title. */
    //
    //
    unsigned short              is_init; /**< init flag. */
    //
    //
    unsigned int                width; /**< frame width. */
    //
    //
    unsigned int                height; /**< frame height. */
    //
    //
    unsigned long               frame_cnt; /**< frame count. */
    //
    //
    SDL_Surface                 *surface; /**< main window surface. */
    //
    //
    unsigned int                flip_image; /**< rotate flag. */
    //
    //
    ps_guid                     src_guid; /**< GUID of image data source. */
} _vsv_data;


#endif	/* VSV_STRUCTURES_H */

