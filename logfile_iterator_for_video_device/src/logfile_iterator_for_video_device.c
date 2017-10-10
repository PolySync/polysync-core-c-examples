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
#include <sys/stat.h>
#include <popt.h>

// API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_node.h"
#include "polysync_message.h"
#include "polysync_logfile.h"
#include "polysync_video.h"

// Example specific headers
#include "libuvc/libuvc.h"
#include "video_log_utils.h"


/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "polysync-logfile-iterator-for-video-device-c";


// static const char LOGFILE_PATH[] = "/home/snewton/.local/share/polysync/rnr_logs/555/video-device.1688854689402078.plog";

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
    if((log_record != NULL) && (context != NULL))
    {
        // we only want to read image data messages
        if(msg_type == context->image_data_msg_type)
        {
            const ps_msg_ref msg = (ps_msg_ref) log_record->data;
            const ps_image_data_msg * const image_data_msg = (ps_image_data_msg*) msg;

            if(context->output_format == OUTPUT_BMP)
            {
                (void) output_bmp(image_data_msg, context);
            }
            else if(context->output_format == OUTPUT_PPM)
            {
                (void) output_ppm(image_data_msg, context);
            }
        }
    }
}


int set_uvc_frame_format(
    const ps_pixel_format_kind ps_format,
    enum uvc_frame_format * const uvc_format)
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
        }
        else
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "logged pixel format %d unsupported by this tool", ps_format);
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
    int image_size = image_data_msg->width * image_data_msg->height * 3;
    const size_t name_max = 1024; // lots of room
    char img_name[name_max];
    FILE * img_file = NULL;
    uvc_frame_t yuyv;
    uvc_frame_t * rgb = NULL;
    yuyv.data = image_data_msg->data_buffer._buffer;
    yuyv.data_bytes = image_data_msg->data_buffer._length;
    yuyv.width = image_data_msg->width;
    yuyv.height = image_data_msg->height;
    yuyv.library_owns_data = 0;

    if((image_data_msg == NULL) || (context == NULL))
    {
        ret = DTC_USAGE;
    }

    if(ret == DTC_NONE)
    {
        uvc_ret = set_uvc_frame_format(
            image_data_msg->pixel_format,
            &yuyv.frame_format);

        if(uvc_ret != 0)
        {
            ret = DTC_DATAERR;
        }
    }
    if(ret == DTC_NONE)
    {
        rgb = uvc_allocate_frame(image_size);

        uvc_ret = uvc_any2rgb(&yuyv, rgb);

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
            psync_log_message(
                LOG_LEVEL_ERROR,
                "error setting output image name! snprintf returned %d", print_ret);
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        ++context->img_count;

        img_file = fopen(img_name, "wb");

        if(img_file == NULL)
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "failed to open %s for writing", img_name);
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
            psync_log_message(
                LOG_LEVEL_ERROR,
                "error writing file header! fprintf returned %d", print_ret);
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
            psync_log_message(
                LOG_LEVEL_ERROR,
                "failed to write to %s", img_name);
            ret = DTC_IOERR;
        }
    }

    if(ret == DTC_NONE)
    {
        ret = fclose(img_file);

        if(ret != 0)
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "something is wrong! fclose failed on %s", img_name);
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
    int image_size = image_data_msg->width * image_data_msg->height * 3;
    int file_size = 0;
    FILE * img_file = NULL;
    bitmap_file_header file_header;
    bitmap_image_header image_header;
    const size_t name_max = 1024; // lots of room
    char img_name[name_max];
    uvc_frame_t yuyv;
    uvc_frame_t * bgr = NULL;
    yuyv.data = image_data_msg->data_buffer._buffer;
    yuyv.data_bytes = image_data_msg->data_buffer._length;
    yuyv.width = image_data_msg->width;
    yuyv.height = image_data_msg->height;
    yuyv.library_owns_data = 0;

    if((image_data_msg == NULL) || (context == NULL))
    {
        ret = DTC_USAGE;
    }

    if(ret == DTC_NONE)
    {
        uvc_ret = set_uvc_frame_format(
            image_data_msg->pixel_format,
            &yuyv.frame_format);

        if(uvc_ret != DTC_NONE)
        {
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        bgr = uvc_allocate_frame(image_size);

        ret = uvc_any2bgr(&yuyv, bgr);

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
            psync_log_message(
                LOG_LEVEL_ERROR,
                "error setting output image name! snprintf returned %d", print_ret);
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        ++context->img_count;

        img_file = fopen(img_name, "wb");

        if(img_file == NULL)
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "failed to open %s for writing", img_name);
            ret = DTC_DATAERR;
        }
    }

    if(ret == DTC_NONE)
    {
        fwrite(&file_header, 1, sizeof(file_header), img_file);

        if(ferror(img_file) != 0)
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "failed to write to %s", img_name);
            ret = DTC_IOERR;
        }
    }

    if(ret == DTC_NONE)
    {
        fwrite(&image_header, 1, sizeof(image_header), img_file);

        if(ferror(img_file) != 0)
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "failed to write to %s", img_name);
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
            psync_log_message(
                LOG_LEVEL_ERROR,
                "failed to write to %s", img_name);
            ret = DTC_IOERR;
        }
    }

    if(ret == DTC_NONE)
    {
        ret = fclose(img_file);

        if(ret != 0)
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "something is wrong! fclose failed on %s", img_name);
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
            psync_log_message(
                LOG_LEVEL_ERROR,
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
            psync_log_message(
                LOG_LEVEL_ERROR,
                "error setting default outdir! snprintf returned %d",
                print_ret);
            ret = DTC_DATAERR;
        }
    }

    return ret;
}

#ifndef NOMAIN

enum
{
    OPT_OUTPUT_FORMAT = 1,
    OPT_LOGFILE_PATH,
    OPT_OUTDIR_PATH,
    OPT_SHOW_HELP
};

static int parse_options(
    const int argc,
    char ** const argv,
    context_s * const context)
{
    int ret = DTC_NONE;
    poptContext opt_ctx;
    char * outputformat = NULL;
    char * logfilepath = NULL;
    char * outdir = NULL;
    int outputformat_set = 0;
    int logfilepath_set = 0;
    int outdir_set = 0;

    const struct poptOption OPTIONS_TABLE[] =
    {
        {
            "path",
            'p',
            POPT_ARG_STRING,
            &logfilepath,
            OPT_LOGFILE_PATH,
            "required, path to video-device plog",
            "PATH"
        },
        {
            "outdir",
            'o',
            POPT_ARG_STRING,
            &outdir,
            OPT_OUTDIR_PATH,
            "specifiy image file output directory path, defaults to /tmp",
            "PATH"
        },
        {
            "format",
            'f',
            POPT_ARG_STRING,
            &outputformat,
            OPT_OUTPUT_FORMAT,
            "specifiy file format 'bmp' or 'ppm', defaults to 'ppm'",
            "FORMAT"
        },
        POPT_AUTOHELP
        POPT_TABLEEND
    };

    if(ret == DTC_NONE)
    {
        opt_ctx = poptGetContext(
            NULL,
            argc,
            (const char**) argv,
            OPTIONS_TABLE,
            0);

        if(argc < 2)
        {
            ret = DTC_USAGE;
            poptPrintUsage(opt_ctx, stderr, 0);
            poptFreeContext(opt_ctx);
        }
    }

    if(ret == DTC_NONE)
    {
        int opt = 0;

        while((opt = poptGetNextOpt(opt_ctx)) >= 0)
        {
            if(opt == OPT_LOGFILE_PATH)
            {
                logfilepath_set = 1;
            }
            else if(opt == OPT_OUTDIR_PATH)
            {
                outdir_set = 1;
            }
            else if(opt == OPT_OUTPUT_FORMAT)
            {
                outputformat_set = 1;
            }
            else if(opt == OPT_SHOW_HELP)
            {
                poptPrintHelp(opt_ctx, stdout, 0);
                poptFreeContext(opt_ctx);
                ret = DTC_NOINTERFACE;
                break;
            }
        }

        if(opt < -1)
        {
            (void) fprintf(
                    stderr,
                    "argument error '%s': %s\n\n",
                    poptBadOption(opt_ctx, POPT_BADOPTION_NOALIAS),
                    poptStrerror(ret));
            ret = DTC_USAGE;
            poptPrintUsage(opt_ctx, stderr, 0);
        }
    }

    if(ret == DTC_NONE)
    {
        if(outputformat_set != 0)
        {
            if(strncmp(outputformat, "bmp", 3) == 0)
            {
                context->output_format = OUTPUT_BMP;
            }
            else if(strncmp(outputformat, "ppm", 3) != 0)
            {
                psync_log_message(
                    LOG_LEVEL_ERROR,
                    "unsupported output format, supported formats are bmp and ppm");
                ret = DTC_USAGE;
                poptPrintUsage(opt_ctx, stderr, 0);
            }
        }
    }

    if(ret == DTC_NONE)
    {
        if(logfilepath_set == 0)
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "path to logfile required");
            poptPrintUsage(opt_ctx, stderr, 0);
        }
        else
        {
            int print_ret = 0;

            print_ret = snprintf(
                context->logfile_path,
                sizeof(context->logfile_path),
                "%s",
                logfilepath);

                if(print_ret < 0)
            {
                psync_log_message(
                    LOG_LEVEL_ERROR,
                    "error setting logfile path! snprintf returned %d",
                    print_ret);
                ret = DTC_DATAERR;
            }
        }
    }

    if(ret == DTC_NONE)
    {
        if(outdir_set != 0)
        {
            int print_ret = 0;
            print_ret = snprintf(
                context->output_dir,
                sizeof(context->output_dir) - 1,
                "%s", outdir);

            // Minimal effort path cleanup
            size_t len = strlen(context->output_dir);

            if(context->output_dir[len-1] == '/')
            {
                context->output_dir[len-1] = '\0';
            }

            if(print_ret < 0)
            {
                psync_log_message(
                    LOG_LEVEL_ERROR,
                    "error setting outdir! snprintf returned %d",
                    print_ret);
                ret = DTC_DATAERR;
            }
        }
    }

    return ret;
}

int main(int argc, char **argv)
{
    int ret = DTC_NONE;
    context_s context;
    memset(&context, 0, sizeof(context));

    ret = init_context(&context, NULL);

    if(ret == DTC_NONE)
    {
        ret = parse_options(argc, argv, &context);
    }

    if(ret == DTC_NONE)
    {
        psync_log_message(
            LOG_LEVEL_INFO,
            "writing image files to %s", context.output_dir);
    }

    if(ret == DTC_NONE)
    {
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
        }

    if(ret == DTC_NONE)
    {
        // get the message type for 'ps_image_data_msg'
        ret = psync_message_get_type_by_name(
            context.node_ref,
            IMAGE_DATA_MSG_NAME,
            &context.image_data_msg_type);

        if(ret != DTC_NONE)
        {
            psync_log_error("psync_message_get_type_by_name - ret: %d", ret);
            ret = DTC_CONFIG;
        }
    }

    if(ret == DTC_NONE)
    {
        // initialize logfile API resources
        ret = psync_logfile_init(context.node_ref);

        if(ret != DTC_NONE)
        {
            psync_log_message(
                LOG_LEVEL_ERROR,
                "main -- psync_logfile_init - ret: %d",
                ret);
            ret = DTC_CONFIG;
        }
    }

    if(ret == DTC_NONE)
    {
        // iterate over logfile
        ret = psync_logfile_foreach_iterator(
            context.node_ref,
            context.logfile_path,
            logfile_iterator_callback,
            &context);

        if(ret != DTC_NONE)
        {
            psync_log_message(
                    LOG_LEVEL_ERROR,
                    "main -- psync_logfile_foreach_iterator - ret: %d",
                    ret);
            ret = DTC_CONFIG;
        }
    }

    if(ret == DTC_NONE)
    {
        // release logfile API resources
        ret = psync_logfile_release(context.node_ref);

        if(ret != DTC_NONE)
        {
            psync_log_message(
                    LOG_LEVEL_ERROR,
                    "main -- psync_logfile_release - ret: %d",
                    ret);
            ret = DTC_CONFIG;
        }
    }

    if(ret == DTC_NONE)
    {
        // release logfile API resources
        ret = psync_release(&context.node_ref);

        // release core API
        if(ret != DTC_NONE)
        {
            psync_log_message(
                    LOG_LEVEL_ERROR,
                    "main -- psync_release - ret: %d",
                    ret);
            ret = DTC_CONFIG;
        }
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

