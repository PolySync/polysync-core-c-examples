/**
 * \example video_encode_decode.c
 *
 * Video Device Data Encode and Decode Example.
 *
 * Shows how to use the Video API routines to communicate with a video device,
 * and encode/decode the data.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>

// API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_video.h"




// *****************************************************
// static global data
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
static const char NODE_NAME[] = "video-encode-decode";


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




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // node reference
    ps_node_ref node_ref = PSYNC_NODE_REF_INVALID;

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

	// init core API
    if( (ret = psync_init( NODE_NAME, PSYNC_NODE_TYPE_API_USER, PSYNC_DEFAULT_DOMAIN, PSYNC_SDF_KEY_INVALID, PSYNC_INIT_FLAG_STDOUT_LOGGING, &node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // nodes typically should shutdown after handling SIGINT
    // hook up the control-c signal handler, sets exit signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    // open device
    if( (ret = psync_video_open( &video_device, DEFAULT_VIDEO_DEVICE_PATH )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_open - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // check if desired configuration is available
    if( (ret = psync_video_check_format( &video_device, desired_device_format, DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_check_format - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set configuration
    if( (ret = psync_video_set_format( &video_device, desired_device_format, DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_set_format - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set framerate
    if( (ret = psync_video_set_frame_rate( &video_device, PSYNC_VIDEO_DEFAULT_FRAMES_PER_SECOND )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_set_frame_rate - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // enable streaming, this starts the capturing of data
    if( (ret = psync_video_enable_streaming( &video_device )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_enable_streaming - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set the decoded frame size, RGB
    decoded_frame_size = DEFAULT_VIDEO_WIDTH * DEFAULT_VIDEO_HEIGHT * 3;

    // allocate encoder buffer, enough space for a full raw frame
    if( (encoder_buffer = malloc( video_device.buffer_len )) == NULL )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- failed to allocate encoder buffer" );
        goto GRACEFUL_EXIT_STMNT;
    }

    // allocate decoder buffer, enough space for a full raw frame in the desired pixel format which is RGB in this example
    if( (decoder_buffer = malloc( decoded_frame_size )) == NULL )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- failed to allocate decoder buffer" );
        goto GRACEFUL_EXIT_STMNT;
    }

    // initialize h264 encoder
    if( (ret = psync_video_encoder_init(
            &video_encoder,
            desired_device_format,
            DEFAULT_VIDEO_WIDTH,
            DEFAULT_VIDEO_HEIGHT,
            PSYNC_VIDEO_DEFAULT_FRAMES_PER_SECOND )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_encoder_init - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // initialize h264 decoder, frame-rate will be determined by stream if possible, otherwise use default
    if( (ret = psync_video_decoder_init(
            &video_decoder,
            desired_decoder_format,
            DEFAULT_VIDEO_WIDTH,
            DEFAULT_VIDEO_HEIGHT,
            PSYNC_VIDEO_DEFAULT_FRAMES_PER_SECOND )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_decoder_init - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


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

            // encode the raw image data
            if( (ret = psync_video_encoder_encode( &video_encoder, rx_timestamp, video_device.buffer, video_device.buffer_len )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_encoder_encode - ret: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }

            // copy the encoded bytes into our local buffer, this is the encoded byte stream
            if( (ret = psync_video_encoder_copy_bytes( &video_encoder, encoder_buffer, video_device.buffer_len, &bytes_encoded )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_encoder_copy_bytes - ret: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }

            // if encoder has data available
            if( bytes_encoded != 0 )
            {
                printf( "    encoded %lu bytes\n", bytes_encoded );

                // decode the newly encoded data
                if( (ret = psync_video_decoder_decode( &video_decoder, rx_timestamp, encoder_buffer, bytes_encoded )) != DTC_NONE )
                {
                    psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_decoder_decode - ret: %d", ret );
                    goto GRACEFUL_EXIT_STMNT;
                }

                // copy the decoded bytes into our local buffer, this is the raw frame is our desired pixel format
                if( (ret = psync_video_decoder_copy_bytes( &video_decoder, decoder_buffer, decoded_frame_size, &bytes_decoded )) != DTC_NONE )
                {
                    psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_decoder_copy_bytes - ret: %d", ret );
                    goto GRACEFUL_EXIT_STMNT;
                }

                // if decoder has data available
                if( bytes_decoded != 0 )
                {
                    printf( "    decoded %lu bytes\n", bytes_decoded );
                }
            }
        }
        else if( ret != DTC_UNAVAILABLE )
        {
            // not no-data/timeout, report error
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_poll - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // wait a little
        psync_sleep_micro( 1000 );
    }


    // using 'goto' to allow for an easy example exit
    GRACEFUL_EXIT_STMNT:
    global_exit_signal = 1;

    // release and close video device
    if( (ret = psync_video_close( &video_device )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_close - ret: %d", ret );
    }

    // release video encoder
    if( (ret = psync_video_encoder_release( &video_encoder )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_encoder_release - ret: %d", ret );
    }

    // release video decoder
    if( (ret = psync_video_decoder_release( &video_decoder )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_decoder_release - ret: %d", ret );
    }

	// release core API
    if( (ret = psync_release( &node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }

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
