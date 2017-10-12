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
 * \example main.c
 *
 * Shows how to use the Logfile API routines to iterate over a video device
 * PolySync logfile and convert raw image data to image files,
 * outside the normal replay time domain.
 *
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
#include "polysync_sdf.h"
#include "polysync_node.h"
#include "polysync_message.h"
#include "polysync_logfile.h"

#include "video_log_utils.h"

enum
{
    OPT_OUTPUT_FORMAT = 1,
    OPT_LOGFILE_PATH,
    OPT_OUTDIR_PATH,
    OPT_SHOW_HELP
};

/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "polysync-logfile-iterator-for-video-device-c";

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
    void * const user_data)
{
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
            "specifiy image file output directory path, defaults to "
            "/tmp/plog_images.XXXXXX where `XXXXXX` represents a random string",
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
                psync_log_error(
                    "unsupported output format, "
                    "supported formats are bmp and ppm");
                ret = DTC_USAGE;
                poptPrintUsage(opt_ctx, stderr, 0);
            }
        }
    }

    if(ret == DTC_NONE)
    {
        if(logfilepath_set == 0)
        {
            psync_log_error("path to logfile required");
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
                psync_log_error(
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
                psync_log_error(
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
        psync_log_info("writing image files to %s", context.output_dir);
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
                psync_log_error(
                    "failed to initialize PolySync - ret: %d", ret);
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
            psync_log_error(
                "failed to get %s message type - ret: %d",
                IMAGE_DATA_MSG_NAME,
                ret);
            ret = DTC_CONFIG;
        }
    }

    if(ret == DTC_NONE)
    {
        // initialize logfile API resources
        ret = psync_logfile_init(context.node_ref);

        if(ret != DTC_NONE)
        {
            psync_log_error("failed in logfile intialization - ret: %d", ret);
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
            psync_log_error("failed to create logfile iterator - ret: %d", ret);
            ret = DTC_CONFIG;
        }
    }

    if(ret == DTC_NONE)
    {
        // release logfile API resources
        ret = psync_logfile_release(context.node_ref);

        if(ret != DTC_NONE)
        {
            psync_log_error("failed to release logfile - ret: %d", ret);
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
            psync_log_error("failed to release PolySync - ret: %d", ret);
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
