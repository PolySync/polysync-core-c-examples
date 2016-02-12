/**
 * \example image_data_viewer.c
 *
 * Image Data Viewer Example.
 *
 * Shows how to decode and view encoded image data received over the PolySync bus.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <glib-2.0/glib.h>

// API headers
#include "polysync_core.h"
#include "polysync_message.h"
#include "polysync_sdf.h"
#include "polysync_video.h"

// GUI headers
#include "gui.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief Node reference used by the example.
 *
 */
static ps_node_ref global_node_ref = PSYNC_NODE_REF_INVALID;

/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static sig_atomic_t global_exit_signal = 0;


/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "image-data-viewer";


/**
 * @brief Image data message type name.
 *
 */
static const char IMAGE_DATA_MSG_NAME[] = "ps_image_data_msg";




// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief Signal handler.
 *
 * @param [in] signal Signal value to handle.
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


//
static void flush_queue( GAsyncQueue * const msg_queue )
{
    if( msg_queue == NULL )
    {
        return;
    }

    // local vars
    ps_msg_ref msg = PSYNC_MSG_REF_INVALID;


    // flush
    while( (msg = g_async_queue_try_pop(msg_queue)) != NULL )
    {
        // release
        (void) psync_message_free( global_node_ref, &msg );
    }
}


//
static void ps_image_data_msg__handler( const ps_msg_type msg_type, const ps_msg_ref const message, void * const user_data )
{
    if( user_data == NULL )
    {
        return;
    }

    // local vars
    int ret = DTC_NONE;
    GAsyncQueue *msg_queue = NULL;
    ps_msg_ref msg_copy = PSYNC_MSG_REF_INVALID;


    // cast
    msg_queue = (GAsyncQueue*) user_data;

    // create copy
    if( (ret = psync_message_alloc( global_node_ref, msg_type, &msg_copy )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "ps_image_data_msg__handler : (%u) -- psync_message_alloc: %d", __LINE__, ret );
        return;
    }

    // copy
    if( (ret = psync_message_copy( global_node_ref, message, msg_copy )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "ps_image_data_msg__handler : (%u) -- psync_message_copy: %d", __LINE__, ret );
        return;
    }

    // enqueue
    g_async_queue_push( msg_queue, (gpointer) msg_copy );
}




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // GUI data
    gui_context_s *gui = NULL;

    // GUID of first seen node to subscribe to
    ps_guid publisher_guid = PSYNC_GUID_INVALID;

    // subscriber message queue
    GAsyncQueue *msg_queue = NULL;

    // image data message type
    ps_msg_type image_data_msg_type = PSYNC_MSG_TYPE_INVALID;

    // desired decoder output pixel format, RGB
    ps_pixel_format_kind desired_decoder_format = PIXEL_FORMAT_RGB24;

    // publisher image width/height
    unsigned long publisher_width = 0;
    unsigned long publisher_height = 0;

    // publisher pixel format
    ps_pixel_format_kind publisher_format = PIXEL_FORMAT_INVALID;

    // video decoder - converts compressed stream into raw image
    ps_video_decoder video_decoder;

    // local decoder data buffer - filled by the decoder
    unsigned char *decoder_buffer = NULL;

    // size in bytes of the decoded image data
    unsigned long decoded_frame_size = 0;

    // number of bytes decoded per frame
    unsigned long bytes_decoded = 0;

    // current time
    ps_timestamp timestamp = 0;

    // time to redraw
    ps_timestamp time_to_draw = 0;

    // sleep counter
    unsigned long sleep_tick = 0;

    // last image rx/decoded timestamp
    ps_timestamp last_rx_time = 0;


    // zero
    memset( &video_decoder, 0, sizeof(video_decoder) );

	// init core API
    if( (ret = psync_init(
            NODE_NAME,
            PSYNC_NODE_TYPE_API_USER,
            PSYNC_DEFAULT_DOMAIN,
            PSYNC_SDF_ID_INVALID,
            PSYNC_INIT_FLAG_STDOUT_LOGGING,
            &global_node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // create message queue
    if( (msg_queue = g_async_queue_new()) == NULL )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- failed to create message queue" );
        goto GRACEFUL_EXIT_STMNT;
    }

    // nodes typically should shutdown after handling SIGINT
    // hook up the control-c signal handler, sets exit signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    // get image data message type
    if( (ret = psync_message_get_type_by_name( global_node_ref, IMAGE_DATA_MSG_NAME, &image_data_msg_type )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_get_type_by_name - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // register listener, provide our message queue pointer
    if( (ret = psync_message_register_listener( global_node_ref, image_data_msg_type, ps_image_data_msg__handler, msg_queue )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_register_listener - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    printf( "waiting for first image data publisher\n" );

    // while we haven't seen a valid image data publisher
    while( publisher_guid == PSYNC_GUID_INVALID )
    {
        // get message if one exist
        ps_msg_ref msg = g_async_queue_try_pop( msg_queue );

        // check if valid
        if( msg != PSYNC_MSG_REF_INVALID )
        {
            // cast
            const ps_image_data_msg * const image_data_msg = (ps_image_data_msg*) msg;

            // check for supported pixel format
            if( (image_data_msg->pixel_format == PIXEL_FORMAT_H264)
                    || (image_data_msg->pixel_format == PIXEL_FORMAT_MJPEG) )
            {
                // found
                printf( "found publisher GUID 0x%016llX (%llu) - pixel_format: '%s'\n",
                        (unsigned long long) image_data_msg->header.src_guid,
                        (unsigned long long) image_data_msg->header.src_guid,
                        (image_data_msg->pixel_format == PIXEL_FORMAT_H264) ? "H264" : "MJPEG" );

                // set publisher format
                publisher_format = image_data_msg->pixel_format;

                // set publisher GUID
                publisher_guid = image_data_msg->header.src_guid;

                // set publisher pixel format and w/h
                publisher_width = image_data_msg->width;
                publisher_height = image_data_msg->height;
            }
        }

        // check for exit
        if( global_exit_signal != 0 )
        {
            goto GRACEFUL_EXIT_STMNT;
        }
    }

    // initialize decoder, frame-rate will be determined by stream if possible, otherwise use default
    if( (ret = psync_video_decoder_init(
            &video_decoder,
            publisher_format,
            publisher_width,
            publisher_height,
            desired_decoder_format,
            publisher_width,
            publisher_height,
            PSYNC_VIDEO_DEFAULT_FRAMES_PER_SECOND )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_decoder_init - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set the decoded frame size, RGB
    decoded_frame_size = publisher_width * publisher_height * 3;

    // allocate decoder buffer, enough space for a full raw frame in the desired pixel format which is RGB in this example
    if( (decoder_buffer = malloc( decoded_frame_size )) == NULL )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- failed to allocate decoder buffer" );
        goto GRACEFUL_EXIT_STMNT;
    }

    // create GUI
    if( (gui = gui_init( NODE_NAME, publisher_width, publisher_height )) == NULL )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- failed to create GUI" );
        goto GRACEFUL_EXIT_STMNT;
    }

    // set the publisher GUID
    gui->image_publisher_guid = publisher_guid;


    // main event loop
    // loop until signaled (control-c)
    while( global_exit_signal == 0 )
    {
        // zero
        bytes_decoded = 0;

        // get message if one exist
        ps_msg_ref msg = g_async_queue_try_pop( msg_queue );

        // check if valid
        if( msg != PSYNC_MSG_REF_INVALID )
        {
            // cast
            const ps_image_data_msg * const image_data_msg = (ps_image_data_msg*) msg;

            // check if publisher is what we've initialized to using its GUID and pixel format
            if(
                    (image_data_msg->header.src_guid == publisher_guid) &&
                    (image_data_msg->pixel_format == publisher_format) )
            {
                // decode the data
                if( (ret = psync_video_decoder_decode(
                        &video_decoder,
                        image_data_msg->timestamp,
                        image_data_msg->data_buffer._buffer,
                        image_data_msg->data_buffer._length )) != DTC_NONE )
                {
                    psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_decoder_decode - ret: %d", ret );
                    goto GRACEFUL_EXIT_STMNT;
                }

                // copy the decoded bytes into our local buffer, this is the raw frame is our desired pixel format
                if( (ret = psync_video_decoder_copy_bytes(
                        &video_decoder,
                        decoder_buffer,
                        decoded_frame_size,
                        &bytes_decoded )) != DTC_NONE )
                {
                    psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_decoder_copy_bytes - ret: %d", ret );
                    goto GRACEFUL_EXIT_STMNT;
                }

                // if decoder has data available
                if( bytes_decoded != 0 )
                {
                    // update frame counter
                    gui->frame_cnt += 1;

                    // get timestamp
                    ps_timestamp now = 0;
                    (void) psync_get_timestamp( &now );

                    // get rx FPS
                    gui->rx_fps = (double) (now - last_rx_time) / 1000000.0;
                    gui->rx_fps = 1.0 / gui->rx_fps;

                    // update last timestamp
                    last_rx_time = now;

                    // update texture with new data and we're not in freeze-frame
                    if( gui->config.freeze_frame == 0 )
                    {
                        gui_update_image_data( gui, decoder_buffer, bytes_decoded );
                    }

                    // reset sleep ticker
                    sleep_tick = 0;
                }
            }

            // free
            if( (ret = psync_message_free( global_node_ref, &msg )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_free - ret: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }
        }

        // get timestamp
        if( (ret = psync_get_timestamp( &timestamp )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_timestamp - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // update gui
        gui_update( gui, timestamp, &time_to_draw );

        // disable sleep ticker if redraw time is less than 5 ms
        if( time_to_draw < 5000 )
        {
            // reset ticker
            sleep_tick = 0;
        }

        // check sleep ticker
        if( sleep_tick >= 10 )
        {
            // sleep ~1 ms
            usleep( 1000 );

            // reset to enabled
            sleep_tick = 10;
        }

        // increment sleep ticker
        sleep_tick++;
    }


    // using 'goto' to allow for an easy example exit
    GRACEFUL_EXIT_STMNT:
    global_exit_signal = 1;

    // unregister listener
    if( (ret = psync_message_unregister_listener( global_node_ref, image_data_msg_type )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_unregister_listener - ret: %d", ret );
    }

    // release GUI
    if( gui != NULL )
    {
        // free GUI
        gui_release( gui );
        free( gui );
        gui = NULL;
    }

    // release video decoder
    if( (ret = psync_video_decoder_release( &video_decoder )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_decoder_release - ret: %d", ret );
    }

    // flush queue
    flush_queue( msg_queue );

	// release core API
    if( (ret = psync_release( &global_node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }

    // free decoder buffer
    if( decoder_buffer != NULL )
    {
        free(decoder_buffer);
    }


	return EXIT_SUCCESS;
}
