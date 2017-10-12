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
 * \example video_log_utils.c
 *
 * Utilites for generating image files from raw video frames produced
 * by the PolySync Logfile Iterator API.
 */

// System headers
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <popt.h>

// API headers
#include "polysync_core.h"
#include "polysync_node.h"
#include "polysync_message.h"
#include "polysync_logfile.h"

// Example specific headers
#include "libuvc/libuvc.h"
#include "video_log_utils.h"

int set_uvc_frame_format(
        const ps_pixel_format_kind ps_format,
        enum uvc_frame_format * const uvc_format,
        context_s * const context)
{
    int ret = DTC_NONE;

    if(uvc_format == NULL)
    {
        ret = DTC_USAGE;
    }

    if(ret == DTC_NONE)
    {
        if(ps_format == PIXEL_FORMAT_YUYV)
        {
            *uvc_format = UVC_FRAME_FORMAT_YUYV;
            context->bytes_per_pixel = 3;
        }
        else
        {
            psync_log_error(
                    "logged pixel format %d not supported by this tool",
                    (int)ps_format);
            ret = DTC_DATAERR;
        }
    }

    return ret;
}

int output_ppm(
        const ps_image_data_msg * const image_data_msg,
        context_s * const context)
{
    int ret = DTC_NONE;
    int print_ret = 0;
    int uvc_ret = 0;
    int image_size = image_data_msg->width *
            image_data_msg->height * context->bytes_per_pixel;
    const size_t name_max = 1024; // lots of room
    char img_name[name_max];
    FILE * img_file = NULL;
    uvc_frame_t in_format;
    uvc_frame_t * rgb = NULL;
    in_format.data = image_data_msg->data_buffer._buffer;
    in_format.data_bytes = image_data_msg->data_buffer._length;
    in_format.width = image_data_msg->width;
    in_format.height = image_data_msg->height;
    in_format.library_owns_data = 0;

    if((image_data_msg == NULL) || (context == NULL))
    {
        ret = DTC_USAGE;
    }

    if(ret == DTC_NONE)
    {
        uvc_ret = set_uvc_frame_format(
                image_data_msg->pixel_format,
                &in_format.frame_format,
                context);

        if(uvc_ret != 0)
        {
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        rgb = uvc_allocate_frame(image_size);

        uvc_ret = uvc_any2rgb(&in_format, rgb);

        if(uvc_ret != 0)
        {
            uvc_perror(uvc_ret, "uvc_any2rgb");
            uvc_free_frame(rgb);
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        print_ret = snprintf(
                img_name,
                name_max,
                "%s/img_%llu.ppm",
                context->output_dir,
                context->img_count);

        if(print_ret < 0)
        {
            psync_log_error(
                    "error setting output image name! snprintf returned %d",
                    print_ret);
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        ++context->img_count;

        img_file = fopen(img_name, "wb");

        if(img_file == NULL)
        {
            psync_log_error("failed to open %s for writing", img_name);
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        print_ret = fprintf(
                img_file,
                "P6\n%d %d\n255\n",
                image_data_msg->width,
                image_data_msg->height);

        if(print_ret < 0)
        {
            psync_log_error(
                    "error writing file header! fprintf returned %d",
                    print_ret);
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        fwrite(
            rgb->data,
            sizeof(uint8_t),
            image_size,
            img_file);

        if(ferror(img_file) != 0)
        {
            psync_log_error("failed to write to %s", img_name);
            ret = DTC_IOERR;
        }
    }

    if(ret == DTC_NONE)
    {
        ret = fclose(img_file);

        if(ret != 0)
        {
            psync_log_error(
                    "something is wrong! fclose failed on %s",
                    img_name);
            ret = DTC_IOERR;
        }
    }

    if(rgb != NULL)
    {
        uvc_free_frame(rgb);
    }

    return ret;
}

int output_bmp(
    const ps_image_data_msg * const image_data_msg,
    context_s * const context)
{
    int ret = DTC_NONE;
    int print_ret = 0;
    int uvc_ret = 0;
    int image_size = image_data_msg->width
            * image_data_msg->height * context->bytes_per_pixel;
    int file_size = 0;
    FILE * img_file = NULL;
    bitmap_file_header file_header;
    bitmap_image_header image_header;
    const size_t name_max = 1024; // lots of room
    char img_name[name_max];
    uvc_frame_t in_format;
    uvc_frame_t * bgr = NULL;
    in_format.data = image_data_msg->data_buffer._buffer;
    in_format.data_bytes = image_data_msg->data_buffer._length;
    in_format.width = image_data_msg->width;
    in_format.height = image_data_msg->height;
    in_format.library_owns_data = 0;

    if((image_data_msg == NULL) || (context == NULL))
    {
        ret = DTC_USAGE;
    }

    if(ret == DTC_NONE)
    {
        uvc_ret = set_uvc_frame_format(
                image_data_msg->pixel_format,
                &in_format.frame_format,
                context);

        if(uvc_ret != DTC_NONE)
        {
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        bgr = uvc_allocate_frame(image_size);

        ret = uvc_any2bgr(&in_format, bgr);

        if(uvc_ret != 0)
        {
            uvc_perror(uvc_ret, "uvc_any2bgr");
            uvc_free_frame(bgr);
            ret =  DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        file_size =
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

        print_ret = snprintf(
                img_name,
                name_max,
                "%s/img_%llu.ppm",
                context->output_dir,
                context->img_count);

        if(print_ret < 0)
        {
            psync_log_error(
                    "error setting output image name! snprintf returned %d",
                    print_ret);
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        ++context->img_count;

        img_file = fopen(img_name, "wb");

        if(img_file == NULL)
        {
            psync_log_error("failed to open %s for writing", img_name);
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        fwrite(&file_header, 1, sizeof(file_header), img_file);

        if(ferror(img_file) != 0)
        {
            psync_log_error("failed to write to %s", img_name);
            ret = DTC_IOERR;
        }
    }

    if(ret == DTC_NONE)
    {
        fwrite(&image_header, 1, sizeof(image_header), img_file);

        if(ferror(img_file) != 0)
        {
            psync_log_error("failed to write to %s", img_name);
            ret = DTC_IOERR;
        }
    }

    if(ret == DTC_NONE)
    {
        fwrite(
            bgr->data,
            sizeof(unsigned char),
            image_size,
            img_file);

        if(ferror(img_file) != 0)
        {
            psync_log_error("failed to write to %s", img_name);
            ret = DTC_IOERR;
        }
    }

    if(ret == DTC_NONE)
    {
        ret = fclose(img_file);

        if(ret != 0)
        {
            psync_log_error(
                    "something is wrong! fclose failed on %s",
                    img_name);
            ret = DTC_IOERR;
        }
    }

    if(bgr != NULL)
    {
        uvc_free_frame(bgr);
    }

    return ret;
}

int init_context(context_s * const context, const char * logfile_path)
{
    int ret = DTC_NONE;
    int print_ret = 0;
    char fmt[] = "/tmp/plog_images.XXXXXX";
    char *dir_name = NULL;

    context->img_count = 0;
    context->output_format = OUTPUT_PPM; // default

    if(logfile_path != NULL)
    {
        print_ret = snprintf(
                context->logfile_path,
                sizeof(context->logfile_path) - 1,
                "%s",
                logfile_path);

        if(print_ret < 0)
        {
            psync_log_error(
                    "error setting logfile_path! snprintf returned %d",
                    print_ret);
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        dir_name = mkdtemp(fmt);

        print_ret = snprintf(
                context->output_dir,
                sizeof(context->output_dir) - 1,
                "%s",
                dir_name);

        if(print_ret < 0)
        {
            psync_log_error(
                    "error setting default outdir! snprintf returned %d",
                    print_ret);
            ret = DTC_DATAERR;
        }
    }

    return ret;
}
