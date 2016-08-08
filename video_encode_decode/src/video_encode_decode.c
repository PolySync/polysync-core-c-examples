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
 * \example video_encode_decode.c
 *
 * Shows how to use the Video API routines to communicate with a video device,
 * and encode/decode the data.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_video.h"




// *****************************************************
// static global types/macros
// *****************************************************


/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static sig_atomic_t global_exit_signal = 0;


/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "polysync-video-encode-decode-c";


/**
 * @brief Default video path.
 *
 */
static const char DEFAULT_VIDEO_DEVICE_PATH[] = "/dev/video0";


/**
 * @brief Default video width.
 *
 */
static unsigned long DEFAULT_VIDEO_WIDTH = 640;


/**
 * @brief Default video height.
 *
 */
static unsigned long DEFAULT_VIDEO_HEIGHT = 480;




// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief Signal handler.
 *
 */
static void sig_handler( int signal );


/**
 * @brief Example error/DTC handler.
 *
 */
static void error_exit(
        const char * const function,
        const ps_dtc dtc,
        const char * const filename,
        const unsigned int line_num );




// *****************************************************
// static definitions
// *****************************************************


//
static void sig_handler( int sig )
{
	if( sig == SIGINT )
    {
        global_exit_signal = 1;
    }
}


//
static void error_exit(
        const char * const function,
        const ps_dtc dtc,
        const char * const filename,
        const unsigned int line_num )
{
    // exit if failed
    if( dtc != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- '%s' returned DTC %llu",
                filename,
                line_num,
                function,
                (unsigned long long) dtc );

        exit(EXIT_FAILURE);
    }
}




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;


    // timestamp for the incoming raw image data
    ps_timestamp rx_timestamp = 0;

    // desired video device pixel format, also known as YUY2
    ps_pixel_format_kind desired_device_format = PIXEL_FORMAT_YUYV;

    // desired decoder output pixel format, RGB
    ps_pixel_format_kind desired_decoder_format = PIXEL_FORMAT_RGB24;

    // size in bytes of the decoded image data
    unsigned long decoded_frame_size = 0;

    // number of bytes encoded per frame
    unsigned long bytes_encoded = 0;

    // number of bytes decoded per frame
    unsigned long bytes_decoded = 0;

    // frame counter
    unsigned long frame_counter = 0;

    // video device - provides access to device like '/dev/video0'
    ps_video_device video_device;

    // video encoder - converts raw image device data into compressed stream
    ps_video_encoder video_encoder;

    // video decoder - converts compressed stream into raw image
    ps_video_decoder video_decoder;

    // local encoder data buffer - filled by the encoder
    unsigned char *encoder_buffer = NULL;

    // local decoder data buffer - filled by the decoder
    unsigned char *decoder_buffer = NULL;


    // zero
    memset( &video_device, 0, sizeof(video_device) );
    memset( &video_encoder, 0, sizeof(video_encoder) );
    memset( &video_decoder, 0, sizeof(video_decoder) );

    // nodes typically should shutdown after handling SIGINT
    // hook up the control-c signal handler, sets exit signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    // open video device
    ret = psync_video_open( &video_device, DEFAULT_VIDEO_DEVICE_PATH );

    // exit if failed
    error_exit( "psync_video_open", ret, __FILE__, __LINE__ );

    // check if desired configuration is available
    ret = psync_video_check_format(
            &video_device,
            desired_device_format,
            DEFAULT_VIDEO_WIDTH,
            DEFAULT_VIDEO_HEIGHT );

    // exit if failed
    error_exit( "psync_video_check_format", ret, __FILE__, __LINE__ );

    // set configuration
    ret = psync_video_set_format(
            &video_device,
            desired_device_format,
            DEFAULT_VIDEO_WIDTH,
            DEFAULT_VIDEO_HEIGHT );

    // exit if failed
    error_exit( "psync_video_set_format", ret, __FILE__, __LINE__ );

    // set frame rate
    ret = psync_video_set_frame_rate(
            &video_device,
            PSYNC_VIDEO_DEFAULT_FRAMES_PER_SECOND );

    // exit if failed
    error_exit( "psync_video_set_frame_rate", ret, __FILE__, __LINE__ );

    // enable streaming, this starts the capturing of data
    ret = psync_video_enable_streaming( &video_device );

    // exit if failed
    error_exit( "psync_video_enable_streaming", ret, __FILE__, __LINE__ );

    // set the decoded frame size, RGB
    decoded_frame_size = DEFAULT_VIDEO_WIDTH * DEFAULT_VIDEO_HEIGHT * 3;

    // allocate encoder buffer, enough space for a full raw frame
    if( (encoder_buffer = malloc( video_device.buffer_len )) == NULL )
    {
        ret = DTC_MEMERR;
    }

    // exit if failed
    error_exit( "malloc", ret, __FILE__, __LINE__ );

    // allocate decoder buffer, enough space for a full raw frame in the desired pixel format which is RGB in this example
    if( (decoder_buffer = malloc( decoded_frame_size )) == NULL )
    {
        ret = DTC_MEMERR;
    }

    // exit if failed
    error_exit( "malloc", ret, __FILE__, __LINE__ );

    // initialize h264 encoder
    ret = psync_video_encoder_init(
            &video_encoder,
            desired_device_format,
            DEFAULT_VIDEO_WIDTH,
            DEFAULT_VIDEO_HEIGHT,
            PSYNC_VIDEO_DEFAULT_FRAMES_PER_SECOND,
            PIXEL_FORMAT_H264,
            DEFAULT_VIDEO_WIDTH,
            DEFAULT_VIDEO_HEIGHT );

    // exit if failed
    error_exit( "psync_video_encoder_init", ret, __FILE__, __LINE__ );

    // initialize h264 decoder, frame-rate will be determined by stream if possible, otherwise use default
    ret = psync_video_decoder_init(
            &video_decoder,
            PIXEL_FORMAT_H264,
            DEFAULT_VIDEO_WIDTH,
            DEFAULT_VIDEO_HEIGHT,
            desired_decoder_format,
            DEFAULT_VIDEO_WIDTH,
            DEFAULT_VIDEO_HEIGHT,
            PSYNC_VIDEO_DEFAULT_FRAMES_PER_SECOND );

    // exit if failed
    error_exit( "psync_video_decoder_init", ret, __FILE__, __LINE__ );


    // main event loop
    // loop until signaled (control-c)
    while( global_exit_signal == 0 )
    {
        // zero
        bytes_encoded = 0;
        bytes_decoded = 0;

        // wait up to 20 ms for image data
        ret = psync_video_poll( &video_device, 20000, &rx_timestamp );

        // error check
        if( ret == DTC_NONE )
        {
            // data is available
            // we can access the data in \ref ps_video_device.buffer
            frame_counter += 1;

            printf( "frame[%lu] - rx: %llu\n", frame_counter, rx_timestamp );

            // encode the raw image data stored in the video device handle
            ret = psync_video_encoder_encode(
                    &video_encoder,
                    rx_timestamp,
                    video_device.buffer,
                    video_device.buffer_len );

            // exit if failed
            error_exit( "psync_video_encoder_encode", ret, __FILE__, __LINE__ );

            // copy the encoded bytes into our local buffer, this is the encoded byte stream
            ret = psync_video_encoder_copy_bytes(
                    &video_encoder,
                    encoder_buffer,
                    video_device.buffer_len,
                    &bytes_encoded );

            // exit if failed
            error_exit( "psync_video_encoder_copy_bytes", ret, __FILE__, __LINE__ );

            // if encoder has data available
            if( bytes_encoded != 0 )
            {
                printf( "    number of bytes in encoder buffer: %lu\n", bytes_encoded );

                // decode the newly encoded data
                ret = psync_video_decoder_decode(
                        &video_decoder,
                        rx_timestamp,
                        encoder_buffer,
                        bytes_encoded );

                // exit if failed
                error_exit( "psync_video_decoder_decode", ret, __FILE__, __LINE__ );

                // copy the decoded bytes into our local buffer, this is the raw frame is our desired pixel format
                ret = psync_video_decoder_copy_bytes(
                        &video_decoder,
                        decoder_buffer,
                        decoded_frame_size,
                        &bytes_decoded );

                // exit if failed
                error_exit( "psync_video_decoder_copy_bytes", ret, __FILE__, __LINE__ );

                // if decoder has data available
                if( bytes_decoded != 0 )
                {
                    printf( "    number of bytes in encoder buffer: %lu\n", bytes_decoded );
                }
            }
        }
        else if( ret != DTC_UNAVAILABLE )
        {
            // not no-data/timeout, this is an error
            // exit if failed
            error_exit( "psync_video_poll", ret, __FILE__, __LINE__ );
        }
    }


    // release and close video device
    (void) psync_video_close( &video_device );

    // release video encoder
    (void) psync_video_encoder_release( &video_encoder );

    // release video decoder
    (void) psync_video_decoder_release( &video_decoder );

    // free encoder buffer
    if( encoder_buffer != NULL )
    {
        free(encoder_buffer);
    }

    // free decoder buffer
    if( decoder_buffer != NULL )
    {
        free(decoder_buffer);
    }


	return EXIT_SUCCESS;
}
