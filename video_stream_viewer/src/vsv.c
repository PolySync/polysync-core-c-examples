/******************************************************
** FILE: vsv.c
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
 * @example vsv.c
 *
 * Video Stream Viewer.
 *
 * This example listens for video stream messages, decodes, and displays
 * the image data.
 *
 * The program expects the (-g) command line argument when multiple sources exist on bus.
 * This specifies the
 * source node GUID to listen for, allowing for multiple reader/writers on
 * bus. This is a 16 upper case hex string, ie 000300008F120263. If this is
 * not present, filtering is not used.
 *
 * Use (-h) to get a detailed usage description:\n
 * @code
 * $./video_stream_viewer -h
 * @endcode
 *
 * \n
 *
 * Example usage with example webcam_lane_model project as source generator.\n
 * Get GUID of image data source node:
 * @code
 * $./webcam_lane_model
 * ...
 * DEBUG: polysync_core : psync_init -- node GUID: 0x000300008F120263 - 0d844427330454115
 * ...
 * @endcode
 *
 * Run viewer, listening for data from source node by GUID:
 * @code
 * $./video_stream_viewer -o -g 000300008F120263
 * @endcode
 *
 */


#include "vsv_common.h"


// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Node name.
 *
 * @note Set by user.
 *
 */
const char                      *node_name                  = "vsv";

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
static sig_atomic_t				exit_signaled               = 0;

/**
 * @brief Video stream message queue.
 *
 */
static GAsyncQueue              *video_stream_queue         = NULL;

/**
 * @brief Video Stream Viewer context.
 *
 */
static _vsv_data                 vsv_data;

/**
 * @brief Video stream decoder.
 *
 */
static ps_video_decoder         video_decoder;

/**
 * @brief Window title string.
 *
 */
static char                     window_title[ PSYNC_DEFAULT_STRING_LEN ];





// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief Signal handler.
 *
 */
static void sig_handler( int signal );


/**
 * @brief Init resources.
 *
 * @param stdout_enabled Flag indicating the use of stdout logging.
 *
 */
static int init( unsigned int stdout_enabled );

/**
 * @brief Release resources.
 *
 */
static int release();

/**
 * @brief Video stream message callback.
 *
 * @param usr_data User data pointer.
 * @param msg_type Message type.
 * @param topic_data Message data.
 *
 */
static void psync_handler_video_stream( void *usr_data, ps_msg_type msg_type, void *topic_data );

/**
 * @brief Process video stream.
 *
 * @param video_stream Message pointer.
 * @param frame_ready Flag indicating a new frame is ready.
 *
 */
static int process_video_stream( ps_video_stream_msg *video_stream, unsigned int *frame_ready );

/**
 * @brief Get GUID value from string.
 *
 * @param guid_hex_string Formatted GUID string.
 *
 */
static ps_guid get_GUID( const char *guid_hex_string );






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
static int init( unsigned int stdout_enabled )
{
    // local vars
    int ret = 0;

    // zero
    memset( &vsv_data, 0, sizeof(vsv_data) );
    memset( &video_decoder, 0, sizeof(video_decoder) );


	// init polysync
	if( (ret = psync_init( psync_nid, node_name, stdout_enabled )) != DTC_NONE )
	{
        psync_log_message( LOG_LEVEL_ERROR, "init -- psync_init failed - error_code: %d", ret );
		return ret;
	}

    // disable handlers, etc
    if( (ret = psync_set_node_flags_bit( NODE_FLAG_HANDLERS_ACTIVE, 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "init -- psync_set_node_flags_bit failed - error_code: %d", ret );
        return ret;
    }


    // create queue
    video_stream_queue = g_async_queue_new();

    // error check
    if( !video_stream_queue )
    {
        psync_log_message( LOG_LEVEL_ERROR, "init -- failed to create queue" );
        return ret;
    }

    // get this nodes GUID
    if( (ret = psync_get_GUID( &psync_guid )) != DTC_NONE )
	{
        psync_log_message( LOG_LEVEL_ERROR, "init -- psync_get_GUID failed - error_code: %d", ret );
		return ret;
	}

    // register a listener for video stream viewer messages
    if( (ret = psync_message_register_listener( MSG_TYPE_VIDEO_STREAM , psync_handler_video_stream, video_stream_queue ) ) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "init -- psync_message_register_listener failed - error_code: %d", ret );
        return ret;
    }

    // enable listener/handlers
    if( (ret = psync_set_node_flags_bit( NODE_FLAG_HANDLERS_ACTIVE, 1 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "init -- psync_set_node_flags_bit failed - error_code: %d", ret );
        return ret;
    }

	// hook up the control-c signal handler, sets exit_signaled flag
	signal( SIGINT, sig_handler );

	// allow signals to interrupt
	siginterrupt( SIGINT, 1 );


	return DTC_NONE;
}


//
static int release()
{
	// local vars
    void            *q_element      = NULL;
    int             ret             = DTC_NONE;
    unsigned int    stale_topics    = 0;


    // disable handlers, etc
    if( (ret = psync_set_node_flags_bit( NODE_FLAG_HANDLERS_ACTIVE, 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "release -- psync_set_node_flags_bit failed - error_code: %d", ret );
    }

    // wait a little
    psync_sleep_micro( 10000 );

    // flush queue
    stale_topics = 0;
    if( video_stream_queue )
        q_element = g_async_queue_try_pop( video_stream_queue );
    while( q_element )
    {
        stale_topics++;

        // release instance
        if( (ret = psync_message_release_instance( &q_element ) ) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "release -- psync_message_release_instance failed - error_code: %d", ret );
            // don't return, attempt to continue
        }

        // get next element if any
        q_element = g_async_queue_try_pop( video_stream_queue );
    }
    q_element = NULL;
    if( video_stream_queue )
        g_async_queue_unref( video_stream_queue );
    video_stream_queue = NULL;

    // inform
    if( stale_topics )
    {
        psync_log_message( LOG_LEVEL_INFO, "release -- released %u stale messages in queue: VS", stale_topics );
    }


    // release API
    if( (ret = psync_release( 0 )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "release -- psync_release failed - error_code: %d", ret );
    }

	return DTC_NONE;
}


//
static void psync_handler_video_stream( void *usr_data, ps_msg_type msg_type, void *topic_data )
{
    // local vars
    int                     ret         = DTC_NONE;
    ps_node_flags           node_flags  = 0;

    // storage for new message for the queue
    void                    *message    = NULL;



    // get queue
    GAsyncQueue             *queue      = (GAsyncQueue*) usr_data;

    // cast
    ps_video_stream_msg     *stream     = (ps_video_stream_msg*) topic_data;


    // get flags
    if( (ret = psync_get_node_flags( &node_flags )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "psync_handler_video_stream -- psync_get_node_flags failed - error_code: %d", ret );
        return;
    }

    // active check
    if( (node_flags & (ps_node_flags) (1 << NODE_FLAG_HANDLERS_ACTIVE)) == 0 )
    {
        return;
    }

    // src_guid check
    // NOTE: this is set before handler is registered, and should not be used outside this context
    if( (vsv_data.src_guid != 0) && (stream->header.src_guid != vsv_data.src_guid) )
    {
        // only want stream messages from the node we were registered to listen to
        return;
    }


    // get an instance
    if( (ret = psync_message_request_instance( msg_type, &message ) ) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "psync_handler_video_stream -- psync_message_request_instance failed - error_code: %d", ret );
        return;
    }

    // copy message into new message
    if( (ret = psync_message_copy( topic_data, message )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "psync_handler_video_stream -- psync_message_copy failed - error_code: %d", ret );
        return;
    }

    // enqueue
    if( queue )
        g_async_queue_push( queue, (gpointer) message );

    // de-ref
    message = NULL;
}


//
static int process_video_stream( ps_video_stream_msg *video_stream, unsigned int *frame_ready )
{
    if( !video_stream || !frame_ready )
    {
        psync_log_message( LOG_LEVEL_ERROR, "process_video_stream -- bad arg" );
        return DTC_USAGE;
    }


    // local vars
    int             ret             = 0;
    unsigned short  frame_width     = 0;
    unsigned short  frame_height    = 0;
    unsigned long   buffer_size     = 0;
    unsigned char   *buffer_ptr     = NULL;


    // set desired format
    ps_pixel_format_kind viewer_format = PIXEL_FORMAT_BGR24;


    // zero
    (*frame_ready) = 0;


    // update context data
    vsv_data.frame_cnt = video_stream->frame_cnt;
    frame_width = video_stream->width;
    frame_height = video_stream->height;



    // if first time getting message data, init resources
    if( vsv_data.is_init == 0 )
    {
        // create decoder
        if( (ret = psync_video_decoder_init( &video_decoder, frame_width, frame_height, viewer_format )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "process_video_stream -- psync_video_decoder_init failed" );
            return ret;
        }

        // update
        vsv_data.width = frame_width;
        vsv_data.height = frame_height;

        // set flag
        vsv_data.is_init = 1;

        // init SDL
        if( SDL_Init( SDL_INIT_VIDEO) < 0 )
        {
            psync_log_message( LOG_LEVEL_ERROR, "process_video_stream -- SDL_Init failed" );
            return DTC_SOFTWARE ;
        }

        // create window name
        strncpy( vsv_data.window_title, window_title, sizeof(window_title) );
        SDL_WM_SetCaption( vsv_data.window_title, "VSV" );


        // create window surface
        if( viewer_format == PIXEL_FORMAT_RGB24 )
            vsv_data.surface = SDL_SetVideoMode( vsv_data.width, vsv_data.height, 24, SDL_SWSURFACE | SDL_RESIZABLE );
        else if( viewer_format == PIXEL_FORMAT_BGR24 )
            vsv_data.surface = SDL_SetVideoMode( vsv_data.width, vsv_data.height, 24, SDL_SWSURFACE | SDL_RESIZABLE );
        else if( viewer_format == PIXEL_FORMAT_YUYV422 )
            vsv_data.surface = SDL_SetVideoMode( vsv_data.width, vsv_data.height, 16, SDL_SWSURFACE | SDL_RESIZABLE );
        else
            vsv_data.surface = SDL_SetVideoMode( vsv_data.width, vsv_data.height, 24, SDL_SWSURFACE | SDL_RESIZABLE );

        // error check
        if( !vsv_data.surface )
        {
            psync_log_message( LOG_LEVEL_ERROR, "process_video_stream -- SDL_SetVideoMode failed" );
            return DTC_SOFTWARE ;
        }


        psync_log_message( LOG_LEVEL_DEBUG, "process_video_stream -- w: %u - h: %u", frame_width, frame_height );
    }


    // get a pointer to the video stream message buffer
    if( (ret = psync_video_stream_get_data( video_stream, &buffer_ptr, &buffer_size )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "process_video_stream -- psync_video_stream_get_data failed" );
        return ret;
    }


    // decode image data in buffer
    // NOTE: not capturing errors here, let av utils attempt to recover
    if( (ret = psync_video_decoder_decode_image( &video_decoder, buffer_ptr, buffer_size, frame_ready )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_WARN, "process_video_stream -- psync_video_decoder_decode_image failed" );
        //return ret;
    }


    // if new frame is built
    if( (*frame_ready) != 0 )
    {
        // lock screen surface
        SDL_LockSurface( vsv_data.surface );

        // copy data into surface
        if( (ret = psync_video_decoder_copy_buffer( &video_decoder, (unsigned char*) vsv_data.surface->pixels )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "process_video_stream -- psync_video_decoder_copy_buffer failed" );
            return ret;
        }

        // unlock
        SDL_UnlockSurface( vsv_data.surface );


        // check rotation flag
        if( vsv_data.flip_image )
        {
            // image bounding region
            SDL_Rect rec;
            memset( &rec, 0, sizeof(rec) );

            // set rect
            rec.w = vsv_data.surface->w;
            rec.h = vsv_data.surface->h;

            // rotate surface
            SDL_Surface *rot_img = rotozoomSurface( vsv_data.surface, 180.0, 1.0, 0 );

            // update window surface
            SDL_FillRect( vsv_data.surface, &rec, 0 );
            SDL_BlitSurface( rot_img, NULL, vsv_data.surface, &rec );
        }

        // draw window surface
        SDL_UpdateRect( vsv_data.surface, 0, 0, 0, 0 );
    }


    return DTC_NONE;
}


//
static ps_guid get_GUID( const char *guid_hex_string )
{
    if( !guid_hex_string )
        return 0;

    // local vars
    const char      *pos                = NULL;
    ps_guid         guid                = 0;
    unsigned char   guid_buffer[ 8 ]    = { 0 };
    size_t          cnt                 = 0;


    // set pos
    pos = guid_hex_string;


    // decode base-16 (hex) string into byte array
    // NOTE: no error checking or sanitization done
    for( cnt = 0; cnt < 8; cnt++ )
    {
        sscanf( pos, "%2hhX", &guid_buffer[ 7 - cnt ] );
        pos += (2 * sizeof(char));
    }

    memcpy( &guid, guid_buffer, sizeof(guid) );

    return guid;
}




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // local vars
    int                                 ret                 = DTC_NONE;
    // generic sleep ticker
    int                                 do_sleep            = 0;
    unsigned int                        frame_ready         = 0;
    void                                *queue_element      = NULL;
    SDL_Event                           sdl_event;

    // flags from command line arguments
    unsigned int                        stdout_enabled      = 0;
    unsigned int                        flip_enabled        = 0;
    ps_guid                             src_guid_arg        = 0;




    // copy default window title
    strncpy( window_title, DEF_WIN_TITLE, sizeof(window_title) );


    // parse command line arguments
    {
        // arg
        int c = 0;

        // parse
        while( (c = getopt( argc, argv, "hofk:g:t:" )) != -1 )
        {
            if( c == 'h' )
            {
                printf( "-o\t\tenable stdout logging\n" );
                printf( "-f\t\trotate image 180 degrees\n" );
                printf( "-g <N>\t\tsource GUID, MUST be 16 char upper case hex, format \"%%016llX\"\n" );
                printf( "-t \"<S>\"\twindow title, double quoted\n" );
                printf( "* press ESC to exit\n" );
                return 0;
            }
            else if( c == 'o' )
            {
                stdout_enabled = 1;
            }
            else if( c == 'f' )
            {
                flip_enabled = 1;
            }
            else if( c == 'g' )
            {
                // get GUID
                src_guid_arg = get_GUID( optarg );
            }
            else if( c == 't' )
            {
                // get custom window title
                strncpy( window_title, optarg, sizeof(window_title) );
            }
        }
    }


    // init resources
    if( (ret = init( stdout_enabled )) != DTC_NONE )
	{
        psync_log_message( LOG_LEVEL_ERROR, "main -- init failed - error_code: %d", ret );
		goto GRACEFUL_EXIT_STMNT;
	}

    // get GUID
    vsv_data.src_guid = src_guid_arg;

    // flag check
    if( flip_enabled )
        vsv_data.flip_image = 1;

    // log
    psync_log_message( LOG_LEVEL_DEBUG, "main -- source GUID: 0x%llX - 0d%llu", vsv_data.src_guid, vsv_data.src_guid );
    psync_log_message( LOG_LEVEL_DEBUG, "main -- win_title: %s", window_title );



    //
	while(1)
	{
        // zero
        frame_ready = 0;

		// check for an exit signal
		if( exit_signaled )
		{
			psync_log_message( LOG_LEVEL_DEBUG, "main -- exit signaled" );
			goto GRACEFUL_EXIT_STMNT;
		}



        // look for any video stream messages
        queue_element = g_async_queue_try_pop( video_stream_queue );
        if( queue_element )
        {
            // reset ticker
            do_sleep = 0;

            // cast
            ps_video_stream_msg *msg = (ps_video_stream_msg*) queue_element;

            // process data
            if( (ret = process_video_stream( msg, &frame_ready ) ) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- process_video_stream failed - error_code: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }


            // release instance
            if( (ret = psync_message_release_instance( &queue_element ) ) != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR, "main -- psync_message_release_instance failed - error_code: %d", ret );
                goto GRACEFUL_EXIT_STMNT;
            }

            // deref
            msg = NULL;
            queue_element = NULL;
        }


        //
        // check for keyboard events
        SDL_PollEvent( &sdl_event );

        // set flag is ESC event
        if( sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.sym == SDLK_ESCAPE )
            exit_signaled = 1;
        else if( sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.sym == SDLK_f )
            vsv_data.flip_image = !vsv_data.flip_image;



        // sleep ticker
        if( do_sleep >= 50 )
        {
            psync_sleep_micro( 500 );
            do_sleep = 50;
        }
        do_sleep++;

	}// end while


// using 'goto' for an easy example exit
    GRACEFUL_EXIT_STMNT:
	exit_signaled = 1;

    // log
    psync_log_message( LOG_LEVEL_WARN, "main -- node exiting" );


    // release
    if( vsv_data.is_init )
    {
        SDL_FreeSurface( vsv_data.surface );
        SDL_Quit();
    }

    // release
    if( video_decoder.is_active )
    {
        //
        if( (ret = psync_video_decoder_release( &video_decoder )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_video_decoder_release failed - error_code: %d", ret );
        }
    }

	// release
    if( (ret = release()) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- release failed - error_code: %d", ret );
    }

    // done
	return 0;
}
