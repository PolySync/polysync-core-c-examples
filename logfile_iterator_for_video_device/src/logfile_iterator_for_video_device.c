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
 * \example logfile_iterator_for_velodyne_hdl.c
 *
 * Shows how to use the Logfile API routines to iterate over a Velodyne HDL
 * PolySync logfile and access the raw data, outside the normal replay time domain.
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_node.h"
#include "polysync_message.h"
#include "polysync_logfile.h"
#include "polysync_video.h"

#include "libuvc/libuvc.h"
#include "video_log_utils.h"


// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "polysync-logfile-iterator-for-video-device-c";


/**
 * @brief Logfile path we'll use instead of the automatic API-generated name.
 *
 */
static const char LOGFILE_PATH[] = "/home/snewton/.local/share/polysync/rnr_logs/555/video-device.1688854689402078.plog";

/**
 * @brief PolySync 'ps_byte_array_msg' type name.
 *
 */
static const char IMAGE_DATA_MSG_NAME[] = "ps_image_data_msg";


/**
 * @brief Logfile iterator callback.
 *
 * @warning When logfile is empty, expect:
 * \li \ref ps_logfile_attributes.data_count == 0
 * \li msg_type == \ref PSYNC_MSG_TYPE_INVALID
 * \li log_record == NULL
 *
 * @param [in] file_attributes Logfile attributes loaded by the logfile API.
 * @param [in] msg_type Message type identifier for the message in \ref ps_rnr_log_record.data, as seen by this data model.
 * @param [in] log_record Logfile record loaded by the logfile API.
 * @param [in] user_data A pointer to user data, provided by \ref psync_logfile_foreach_iterator.
 *
 */
static void logfile_iterator_callback(
        const ps_logfile_attributes * const file_attributes,
        const ps_msg_type msg_type,
        const ps_rnr_log_record * const log_record,
        void * const user_data);


static void logfile_iterator_callback(
    const ps_logfile_attributes * const file_attributes,
    const ps_msg_type msg_type,
    const ps_rnr_log_record * const log_record,
    void * const user_data)
{

    // int ret = DTC_NONE;
    context_s * const context = (context_s*) user_data;

    // if logfile is empty, only attributes are provided
    if(log_record != NULL)
    {
        // we only want to read image data messages
        if(msg_type == context->image_data_msg_type)
        {
            const ps_msg_ref msg = (ps_msg_ref) log_record->data;
            const ps_image_data_msg * const image_data_msg = (ps_image_data_msg*) msg;

            (void) output_bmp(image_data_msg, context);
            (void) output_ppm(image_data_msg, context);
        }
    }
}


int set_uvc_frame_format(
    const ps_pixel_format_kind ps_format,
    enum uvc_frame_format * uvc_format)
{
    int ret = DTC_NONE;

    if(ps_format == PIXEL_FORMAT_YUYV)
    {
        *uvc_format = UVC_FRAME_FORMAT_YUYV;
    }
    else
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "logged pixel format %d unsupported by this tool", ps_format);
        ret = DTC_DATAERR;
    }

    return ret;
}

int output_ppm(
    const ps_image_data_msg * const image_data_msg,
    context_s * const context)
{
    int ret = DTC_NONE;
    int image_size = image_data_msg->width * image_data_msg->height * 3;

    uvc_frame_t yuyv;
    yuyv.data = image_data_msg->data_buffer._buffer;
    yuyv.data_bytes = image_data_msg->data_buffer._length;
    yuyv.width = image_data_msg->width;
    yuyv.height = image_data_msg->height;
    yuyv.library_owns_data = 0;
    ret = set_uvc_frame_format(
        image_data_msg->pixel_format,
        &yuyv.frame_format);

    if(ret != DTC_NONE)
    {
        return ret;
    }

    uvc_frame_t *rgb = uvc_allocate_frame(image_size);

    ret = uvc_any2rgb(&yuyv, rgb);

    if(ret != 0)
    {
        uvc_perror(ret, "uvc_any2rgb");
        uvc_free_frame(rgb);
        return DTC_DATAERR;
    }

    const size_t name_max = 1024; // lots of room
    char img_name[name_max];
    ret = snprintf(
        img_name,
        name_max,
        "images/img_%llu.ppm",
        context->img_count);

    if(ret < 0)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "error setting output image name! snprintf returned %d", ret);
        return DTC_DATAERR;
    }

    ++context->img_count;

    FILE * img_file = fopen(img_name, "wb");

    if(img_file == NULL)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "failed to open %s for writing", img_name);
        return DTC_DATAERR;
    }

    fprintf(
        img_file,
        "P6\n%d %d\n255\n",
        image_data_msg->width,
        image_data_msg->height);

    fwrite(
        rgb->data,
        sizeof(uint8_t),
        image_size,
        img_file);

    if(ferror(img_file) != 0)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "failed to write to %s", img_name);
        return DTC_IOERR;
    }

    ret = fclose(img_file);

    if(ret != 0)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "something is wrong! fclose failed on %s", img_name);
        return DTC_IOERR;
    }

    return ret;
}

int output_bmp(
    const ps_image_data_msg * const image_data_msg,
    context_s * const context)
{
    int ret = DTC_NONE;
    int image_size = image_data_msg->width * image_data_msg->height * 3;
    uvc_frame_t yuyv;
    yuyv.data = image_data_msg->data_buffer._buffer;
    yuyv.data_bytes = image_data_msg->data_buffer._length;
    yuyv.width = image_data_msg->width;
    yuyv.height = image_data_msg->height;
    yuyv.library_owns_data = 0;
    ret = set_uvc_frame_format(
        image_data_msg->pixel_format,
        &yuyv.frame_format);

    if(ret != DTC_NONE)
    {
        return ret;
    }

    uvc_frame_t *bgr = uvc_allocate_frame(image_size);

    ret = uvc_any2bgr(&yuyv, bgr);

    if(ret != 0)
    {
        uvc_perror(ret, "uvc_any2bgr");
        uvc_free_frame(bgr);
        return DTC_DATAERR;
    }

    bitmap_file_header file_header;
    bitmap_image_header image_header;
    int file_size =
        sizeof(file_header) + sizeof(image_header) + image_size;

    file_header.bitmap_type = 0x4d42; //"BM"
    file_header.file_size = file_size;
    file_header.reserved1 = 0; // unused
    file_header.reserved2 = 0; // unused
    file_header.offset_bits = 0; // unused

    image_header.size_header = sizeof(image_header);
    image_header.width = image_data_msg->width;
    image_header.height = image_data_msg->height;
    image_header.planes = 1;
    image_header.bit_count = 24; // The 24-bit pixel
    image_header.compression = 0; // none
    image_header.image_size = image_size;
    image_header.ppm_x = 0; // unused
    image_header.ppm_y = 0; // unused
    image_header.clr_used = 0; // unused
    image_header.clr_important = 0; // unused

    const size_t name_max = 1024; // lots of room
    char img_name[name_max];
    ret = snprintf(
        img_name,
        name_max,
        "images/img_%llu.bmp",
        context->img_count);

    if(ret < 0)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "error setting output image name! snprintf returned %d", ret);
        return DTC_DATAERR;
    }

    ++context->img_count;

    FILE * img_file = fopen(img_name, "wb");

    if(img_file == NULL)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "failed to open %s for writing", img_name);
        return DTC_DATAERR;
    }

    fwrite(&file_header, 1, sizeof(file_header), img_file);

    if(ferror(img_file) != 0)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "failed to write to %s", img_name);
        return DTC_IOERR;
    }

    fwrite(&image_header, 1, sizeof(image_header), img_file);

    if(ferror(img_file) != 0)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "failed to write to %s", img_name);
        return DTC_IOERR;
    }

    fwrite(
        bgr->data,
        sizeof(unsigned char),
        image_size,
        img_file);

    if(ferror(img_file) != 0)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "failed to write to %s", img_name);
        return DTC_IOERR;
    }

    ret = fclose(img_file);

    if(ret != 0)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "something is wrong! fclose failed on %s", img_name);
        return DTC_IOERR;
    }

    uvc_free_frame(bgr);

    return ret;
}

#ifndef NOMAIN

int main(int argc, char **argv)
{
    int ret = DTC_NONE;
    context_s context;
    memset(&context, 0, sizeof(context));


    context.img_count = 0;
    context.initialized = 0;

    // init core API
    ret = psync_init(
            NODE_NAME,
            PSYNC_NODE_TYPE_API_USER,
            PSYNC_DEFAULT_DOMAIN,
            PSYNC_SDF_ID_INVALID,
            PSYNC_INIT_FLAG_STDOUT_LOGGING,
            &context.node_ref);

    if(ret != DTC_NONE)
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_init - ret: %d",
                ret);
        ret = DTC_CONFIG;
    }

    if(ret == DTC_NONE)
    {
        // get the message type for 'ps_image_data_msg'
        ret = psync_message_get_type_by_name(
            context.node_ref,
            IMAGE_DATA_MSG_NAME,
            &context.image_data_msg_type);
    }

    if(ret != DTC_NONE)
    {
        psync_log_error("psync_message_get_type_by_name - ret: %d", ret);
        ret = DTC_CONFIG;
    }

    if(ret == DTC_NONE)
    {
        // initialize logfile API resources
        ret = psync_logfile_init(context.node_ref);
    }

    if(ret != DTC_NONE)
    {
        psync_log_message(
            LOG_LEVEL_ERROR,
            "main -- psync_logfile_init - ret: %d",
            ret);
        ret = DTC_CONFIG;
    }

    if(ret == DTC_NONE)
    {
        // initialize logfile API resources
        ret = psync_logfile_foreach_iterator(
            context.node_ref,
            LOGFILE_PATH,
            logfile_iterator_callback,
            &context);
    }

    // iterate over the logfile data
    if(ret != DTC_NONE)
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_foreach_iterator - ret: %d",
                ret);
        ret = DTC_CONFIG;
    }

    if(ret == DTC_NONE)
    {
        // release logfile API resources
        ret = psync_logfile_release(context.node_ref);
    }

    // iterate over the logfile data
    if(ret != DTC_NONE)
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_release - ret: %d",
                ret);
        ret = DTC_CONFIG;
    }

    if(ret == DTC_NONE)
    {
        // release logfile API resources
        ret = psync_release(&context.node_ref);
    }
	// release core API
    if(ret != DTC_NONE)
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_release - ret: %d",
                ret);
        ret = DTC_CONFIG;
    }

    if(ret == DTC_NONE)
    {
        ret = EXIT_SUCCESS;
    }
    else
    {
        ret = EXIT_FAILURE;
    }

    return ret;
}

#endif // NOMAIN
