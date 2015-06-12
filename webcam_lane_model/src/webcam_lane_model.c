/******************************************************
** FILE: webcam_lane_model.c
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
 * @example webcam_lane_model.c
 *
 * Example API project to read, modify, and encode image data
 * from /dev/video0 based on lane model information,
 * enforcing RGB24 input format.
 *
 */


#include "webcam_lane_model.h"






// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Node name.
 *
 * @note Set by user.
 *
 */
static const char               *node_name                  = "webcam_lane_model";

/**
 * @brief Node UID.
 *
 * @note Set by user.
 *
 */
static const ps_node_type       psync_nid                   = PSYNC_NID_API;

/**
 * @brief Node GUID.
 *
 * @note Set by API.
 *
 */
static ps_guid                  psync_guid                  = 0;

/**
 * @brief Flag indicating exit signal was caught.
 *
 */
static sig_atomic_t             exit_signaled               = 0;

/**
 * @brief Image device name.
 *
 */
static const char               *image_device_name          = "/dev/video0";

/**
 * @brief Image device handle.
 *
 */
static ps_image_device         image_device_handle;

/**
 * @brief Video stream encoder.
 *
 */
static ps_video_encoder         video_encoder;


/**
 * @brief Video stream message.
 *
 */
static ps_video_stream_msg      *video_stream_msg           = NULL;

/**
 * @brief Lane model message.
 *
 */
static ps_lane_model_msg        *lane_model_msg             = NULL;






// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief Signal handler.
 *
 */
static void sig_handler( int signal );

/**
 * @brief Color image data pixels.
 *
 * @warning Image data must in RGB24.
 *
 */
static void update_image_data( unsigned char *pixels, unsigned int width, unsigned int height );








// *****************************************************
// static definitions
// *****************************************************

//
static void sig_handler( int signal )
{
	//if( signal == SIGINT )
	exit_signaled = 1;
}


//
static void update_image_data( unsigned char *pixels, unsigned int width, unsigned int height )
{
    // sanity check
    if( !pixels || (width == 0) || (height == 0) )
        return;

    // index in buffer
    unsigned int index = 0;
    unsigned int line_stop = width;

    // when to stop the line
    if( height < width )
        line_stop = height;

    // draw a diagonal red line
    for( index = 0; index < line_stop; index++ )
    {
        // R
        pixels[ index*3 + index*width*3 ] = 0xFF;
        // G
        pixels[ 1 + index*3 + index*width*3 ] = 0x00;
        // B
        pixels[ 2 + index*3 + index*width*3 ] = 0x00;
    }
}








// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;
    // flag to enable stdout logs in addition to the normal syslog output
    unsigned int stdout_logging_enabled = 1;
    // data flag
    unsigned int data_ready = 0;


    // zero
    memset( &image_device_handle, 0, sizeof(image_device_handle) );
    memset( &video_encoder, 0, sizeof(video_encoder) );


    // init polysync
    if( (ret = psync_init( psync_nid, node_name, stdout_logging_enabled )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_init - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // hook up the control-c signal handler, sets exit_signaled flag
    signal( SIGINT, sig_handler );

    // allow signal to interrupt
    siginterrupt( SIGINT, 1 );


    // get this node's GUID
    if( (ret = psync_get_GUID( &psync_guid )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_get_GUID - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // request instance
    if( (ret = psync_message_request_instance( MSG_TYPE_VIDEO_STREAM, (void**) &video_stream_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // request instance
    if( (ret = psync_message_request_instance( MSG_TYPE_LANE_MODEL, (void**) &lane_model_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_request_instance - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }








    // init image device handle
    // specify RGB to have the image data converted to desired format when data is ready
    if( (ret = psync_image_device_init( &image_device_handle, image_device_name, PIXEL_FORMAT_RGB24 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_image_device_init - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }

    // init video stream encoder, using image device info
    if( (ret = psync_video_encoder_init( &video_encoder, image_device_handle.width, image_device_handle.height, image_device_handle.video_format, COMPRESSION_PROFILE_LOW_QUALITY )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_encoder_init - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }




    // set constants
    video_stream_msg->sensor_descriptor.serial_number = 123;
    video_stream_msg->width = image_device_handle.width;
    video_stream_msg->height = image_device_handle.height;
    video_stream_msg->format = image_device_handle.video_format;





    // start image device data capture
    if( (ret = psync_image_device_start_capture( &image_device_handle )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_image_device_start_capture - ret: %d", ret );
        goto GRACEFUL_EXIT_STMNT;
    }


    //
	while(1)
	{

		// check for an exit signal
		if( exit_signaled )
		{
			psync_log_message( LOG_LEVEL_DEBUG, "frame_grabber : main -- got exit signal" );
			goto GRACEFUL_EXIT_STMNT;
		}

        // zero
        data_ready = 0;



        // TODO
        // update lane model




        // poll for image device data
        if( (ret = psync_image_device_poll_frame( &image_device_handle, NULL, &video_stream_msg->header.timestamp, &data_ready )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_image_device_poll_frame - ret: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }



        // if image data is ready
        if( data_ready )
        {
            // draw some data on the image
            // TODO - use lane model information
            update_image_data( image_device_handle.active_buffer, image_device_handle.width, image_device_handle.height );


            // encode data
            if( (ret = psync_video_encoder_encode_image( &video_encoder, image_device_handle.active_buffer, &data_ready )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_encoder_encode_image - ret: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }

            // if encoder has data ready, set to number of bytes in coder buffer
            if( data_ready )
            {
                // put encoded bytes in video stream message
                if( (ret = psync_video_stream_set_data( video_stream_msg, video_encoder.buffer, data_ready )) != DTC_NONE )
                {
                    psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_stream_set_data - ret: %d", ret );
                    goto GRACEFUL_EXIT_STMNT;
                }

                // publish encoded video stream message
                if( (ret = psync_message_publish_instance( (void*) video_stream_msg )) != DTC_NONE )
                {
                    psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish_instance - ret: %d", ret );
                    goto GRACEFUL_EXIT_STMNT;
                }
            }


            // publish lane model message
            if( (ret = psync_message_publish_instance( (void*) lane_model_msg )) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_publish_instance - ret: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }
        }



        // wait a little
        psync_sleep_micro( 10000 );
	}// end while


// using 'goto' as an easy example exit
GRACEFUL_EXIT_STMNT:
	exit_signaled = 1;



    // if image device is active
    if( image_device_handle.is_init )
    {
        // stop image device data capture
        if( (ret = psync_image_device_stop_capture( &image_device_handle )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_image_device_stop_capture - ret: %d", ret );
        }

        // release
        if( (ret = psync_image_device_release( &image_device_handle )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_image_device_release - ret: %d", ret );
        }
    }


    // if encoder is active
    if( video_encoder.is_active )
    {
        // release
        if( (ret = psync_video_encoder_release( &video_encoder )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_encoder_release - ret: %d", ret );
        }
    }


    // release instance
    if( (ret = psync_message_release_instance( (void**) &video_stream_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance - ret: %d", ret );
    }

    // release instance
    if( (ret = psync_message_release_instance( (void**) &lane_model_msg )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance - ret: %d", ret );
    }



    // release polysync
    if( (ret = psync_release( 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- psync_release - ret: %d", ret );
    }


	return 0;
}
