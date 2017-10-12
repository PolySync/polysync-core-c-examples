
#ifndef PS_VIDEO_LOG_UTILS
#define PS_VIDEO_LOG_UTILS

/*
 * Copyright (c) 2017 PolySync
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
 * \example video_log_utils.h
 *
 * Data structure and function prototyes used by a logfile iterator to convert
 * raw video data into images.
 */

#include <unistd.h>

// API headers
#include "polysync_core.h"
#include "polysync_node.h"
#include "polysync_message.h"

// Example specific headers
#include "libuvc/libuvc.h"
#include "libuvc/libuvc_config.h"

typedef enum {
    OUTPUT_BMP = 1,
    OUTPUT_PPM
} output_format_e;

typedef struct
{
    ps_node_ref node_ref;
    ps_msg_type image_data_msg_type;
    unsigned long long img_count;
    output_format_e output_format;
    unsigned int bytes_per_pixel;
    char output_dir[1024];
    char logfile_path[1024];
} context_s;

#pragma pack(push,1)
typedef struct {
    uint16_t bitmap_type;
    int32_t file_size;
    int16_t reserved1;
    int16_t reserved2;
    uint32_t offset_bits;
} bitmap_file_header;

typedef struct {
    uint32_t size_header;
    uint32_t width;
    uint32_t height;
    int16_t planes;
    int16_t bit_count;
    uint32_t compression;
    uint32_t image_size;
    uint32_t ppm_x;
    uint32_t ppm_y;
    uint32_t clr_used;
    uint32_t clr_important;
} bitmap_image_header;
#pragma pack(pop)

/**
 * @brief Convert ps_pixel_format_kind to uvc_frame_format.
 *
 * @param [in] ps_format PolySync image data pixel format.
 * @param [out] uvc_frame_format libuvc frame format.
 * @param [out] context format specific 'bytes_per_pixel' field is set.
 *
 * @return DTC code:
 * \li DTC_NONE (zero) if success.
*/
int set_uvc_frame_format(
        const ps_pixel_format_kind ps_format,
        enum uvc_frame_format * const uvc_format,
        context_s * const context);

/**
 * @brief Write ps_image_data_msg to ppm (portable pixmap) image file.
 *
 * @param [in] image_data_msg Valid non-null ps_image_data_msg whose values will populate the image file.
 * @param [in] context application context data.
 *
 * @return DTC code:
 * \li DTC_NONE (zero) if success.
*/
int output_ppm(
        const ps_image_data_msg * const image_data_msg,
        context_s * const context);

/**
 * @brief Write ps_image_data_msg to ppm (bitmap) image file.
 *
 * @param [in] image_data_msg Valid non-null ps_image_data_msg whose values will populate the image file.
 * @param [in] context application context data.
 *
 * @return DTC code:
 * \li DTC_NONE (zero) if success.
*/
int output_bmp(
        const ps_image_data_msg * const image_data_msg,
        context_s * const context);

/**
 * @brief Initialize application context.
 *
 * @param [in] context application context data.
 * @param [in] logfile_path Path to logfile. If NULL logfile path must be set by the user.
 *
 * @return DTC code:
 * \li DTC_NONE (zero) if success.
*/
int init_context(context_s * const context, const char * logfile_path);

#endif // PS_VIDEO_LOG_UTILS