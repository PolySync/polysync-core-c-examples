/**
 * \example shared_image_data_viewer.c
 *
 * Shared Memory Image Data Viewer Example.
 *
 * Shows how to decode and view encoded and raw
 * image data received over a PolySync Shared Memory Queue.
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
#include "polysync_shdmem.h"

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
static const char NODE_NAME[] = "shared-image-data-viewer";




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




// *****************************************************
// main
// *****************************************************


int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // GUI data
    gui_context_s *gui = NULL;

    // shared memory queue
    psync_shdmem_queue shdmem_queue = PSYNC_SHDMEM_QUEUE_INVALID;

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

    // shared memory message buffer
    unsigned char *shdmem_buffer = NULL;

    // size in bytes of the shared memory buffer
    unsigned long shdmem_buffer_size = 0;

    // shared memory key
    unsigned long shdmem_key = 0;

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

    // check for shared memory key argument
    if( (argc < 2) || (strlen(argv[1]) <= 0) )
    {
        printf( "usage: %s <shared_mem_key>\n", NODE_NAME );
        return EXIT_FAILURE;
    }

    // convert to int
    shdmem_key = (unsigned long) atol(argv[1]);

	// init core API
    if( (ret = psync_init(
            NODE_NAME,
            PSYNC_NODE_TYPE_API_USER,
            PSYNC_DEFAULT_DOMAIN,
            PSYNC_SDF_KEY_INVALID,
            PSYNC_INIT_FLAG_STDOUT_LOGGING,
            &global_node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        return EXIT_FAILURE;
    }

    // nodes typically should shutdown after handling SIGINT
    // hook up the control-c signal handler, sets exit signaled flag
    signal( SIGINT, sig_handler );

    // allow signals to interrupt
    siginterrupt( SIGINT, 1 );

    printf( "attaching to shared memory queue with key: 0x%lX (%lu)\n",
            shdmem_key,
            shdmem_key );

    // attach to existing shared memory queue
    if( (ret = psync_shdmem_queue_attach( shdmem_key, &shdmem_queue )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_shdmem_queue_attach - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // arbitrary large size
    shdmem_buffer_size = (1280 * 1080 * 3);

    // allocate shared memory buffer, enough space for a full raw frame
    if( (shdmem_buffer = malloc( shdmem_buffer_size )) == NULL )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- failed to allocate shared memory buffer" );
        goto GRACEFUL_EXIT_STMNT;
    }

    printf( "waiting for image data on shared memory queue with key: 0x%lX (%lu)\n",
            shdmem_key,
            shdmem_key );

    // while we haven't seen a valid image data message
    while( publisher_format == PIXEL_FORMAT_INVALID )
    {
        // check for data
        if( psync_shdmem_queue_try_pop( &shdmem_queue, shdmem_buffer, shdmem_buffer_size ) == DTC_NONE )
        {
            // cast header
            ps_shdmem_message_header * const header =
                    (ps_shdmem_message_header*) &shdmem_buffer[ 0 ];

            // cast message
            ps_shdmem_image_data_msg * const message =
                    (ps_shdmem_image_data_msg*) &shdmem_buffer[ sizeof(*header) ];

            // check for supported image data message type
            if( header->msg_type == PSYNC_SHDMEM_MSG_TYPE_IMAGE_DATA )
            {
                unsigned int producer_valid = 0;
                char format_string[256];

                // check for supported pixel format
                if( message->pixel_format == PIXEL_FORMAT_H264 )
                {
                    producer_valid = 1;
                    strncpy( format_string, "H264 (encoded)", sizeof(format_string) );
                }
                else if( message->pixel_format == PIXEL_FORMAT_MJPEG )
                {
                    producer_valid = 1;
                    strncpy( format_string, "MJPEG (encoded)", sizeof(format_string) );
                }
                else if( message->pixel_format == PIXEL_FORMAT_RGB24 )
                {
                    producer_valid = 1;
                    strncpy( format_string, "RGB (raw)", sizeof(format_string) );
                }

                // get image information if supported
                if( producer_valid != 0 )
                {
                    printf( "found shared memory image data producer - pixel_format: '%s'\n",
                            format_string );

                    // set publisher format
                    publisher_format = message->pixel_format;

                    // set publisher pixel format and w/h
                    publisher_width = (unsigned long) message->width;
                    publisher_height = (unsigned long) message->height;
                }
            }
        }

        // check for exit
        if( global_exit_signal != 0 )
        {
            goto GRACEFUL_EXIT_STMNT;
        }
    }

    // if viewing an encoded pixel format
    if( publisher_format != PIXEL_FORMAT_RGB24 )
    {
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
    }

    // set the decoded frame size, RGB
    decoded_frame_size = (publisher_width * publisher_height * 3);

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

    // enabled help
    gui->config.help_visible = 1;


    // main event loop
    // loop until signaled (control-c)
    while( global_exit_signal == 0 )
    {
        // zero
        bytes_decoded = 0;

        // pointer to the default image buffer
        const unsigned char * buffer_ptr = decoder_buffer;

        // check for data
        ret = psync_shdmem_queue_try_pop( &shdmem_queue, shdmem_buffer, shdmem_buffer_size );

		if( ret == DTC_NONE )
        {
            // cast header
            ps_shdmem_message_header * const header =
                    (ps_shdmem_message_header*) &shdmem_buffer[ 0 ];

            // cast message
            ps_shdmem_image_data_msg * const message =
                    (ps_shdmem_image_data_msg*) &shdmem_buffer[ sizeof(*header) ];

            // check for supported image data message type
            if( header->msg_type == PSYNC_SHDMEM_MSG_TYPE_IMAGE_DATA )
            {
                // check if publisher is what we've initialized to using its and pixel format
                if( message->pixel_format == publisher_format )
                {
                    // if receiving an encoded pixel format
                    if( publisher_format != PIXEL_FORMAT_RGB24 )
                    {
                        // decode the data
                        if( (ret = psync_video_decoder_decode(
                                &video_decoder,
                                message->timestamp,
                                &shdmem_buffer[ sizeof(*header) + sizeof(*message) ],
                                message->data_size )) != DTC_NONE )
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
                    }
                    else
                    {
                        // point buffer to the raw image data, no decoding needed
                        buffer_ptr = &shdmem_buffer[ sizeof(*header) + sizeof(*message) ];
                        bytes_decoded = (unsigned long) message->data_size;
                    }

                    // if decoder/buffer has data available
                    if( bytes_decoded != 0 )
                    {
                        // update frame counter
                        if( message->frame_id != 0 )
                        {
                            gui->frame_cnt = (unsigned long) message->frame_id;
                        }
                        else
                        {
                            // default to using local indexing
                            gui->frame_cnt += 1;
                        }

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
                            gui_update_image_data( gui, buffer_ptr, bytes_decoded );
                        }

                        // reset sleep ticker
                        sleep_tick = 0;
                    }
                }
            }
        }
		else if( ret != DTC_UNAVAILABLE )                                                                                                                                                             
        {                                                                                                                                                                                             
            global_exit_signal = 1;                                                                                                                                                                   
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

	// release core API
    if( (ret = psync_release( &global_node_ref )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }

    // free shared memory buffer
    if( shdmem_buffer != NULL )
    {
        free(shdmem_buffer);
    }

    // free decoder buffer
    if( decoder_buffer != NULL )
    {
        free(decoder_buffer);
    }


	return EXIT_SUCCESS;
}
