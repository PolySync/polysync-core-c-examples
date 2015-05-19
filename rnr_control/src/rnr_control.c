/******************************************************
** FILE: rnr_control.c
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
 * \example rnr_control.c
 *
 * Record and Replay Control.
 *
 * Shows how to use the rnr convenience routines to control the record and replay
 * abilities of PolySync nodes.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include <stdarg.h>
#include <getopt.h>
#include <string.h>
#include <glib-2.0/glib.h>


#include <polysync_core.h>

// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Node name.
 *
 * @note Set by user.
 *
 */
const char                      *node_name                  = "rnr_control";

/**
 * @brief Node Type.
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
 * @brief Flag indicating exit signal was caught.
 *
 */
static ps_logger_packet_msg     *logger_packet_msg          = NULL;



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






// *****************************************************
// source device static definitions
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


	// init polysync
	if( (ret = psync_init( psync_nid, node_name, stdout_enabled )) != DTC_RET( DTC_NONE ) )
	{
        psync_log_message( LOG_LEVEL_ERROR, "init -- psync_init failed - dtc_code: %d", ret );
		return ret;
	}

	// set logger packet message publishers to have RELIABLE QoS
    if( (ret = psync_set_publisher_reliability_qos( MSG_TYPE_LOGGER_PACKET, RELIABILITY_QOS_RELIABLE )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "init -- psync_set_subscriber_reliability_qos - dtc_code: %d", ret );
        return ret;
    }

    // request message
    if( (ret = psync_message_request_instance( MSG_TYPE_LOGGER_PACKET, (void**) &logger_packet_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "init -- psync_message_request_instance failed - dtc_code: %d", ret );
        return ret;
    }


    // get this nodes GUID
    if( (ret = psync_get_GUID( &psync_guid )) != DTC_RET( DTC_NONE ) )
	{
        psync_log_message( LOG_LEVEL_ERROR, "init -- psync_get_GUID failed - dtc_code: %d", ret );
		return ret;
	}


	// hook up the control-c signal handler, sets exit_signaled flag
	signal( SIGINT, sig_handler );

	// allow signals to interrupt
	siginterrupt( SIGINT, 1 );


	return DTC_RET( DTC_NONE );
}


//
static int release()
{
	// local vars
    int             ret             = DTC_RET( DTC_NONE );


    // release message
    if( (ret = psync_message_release_instance( (void**) &logger_packet_msg )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "release -- psync_message_release_instance failed - dtc_code: %d", ret );
    }

    // release API
    if( (ret = psync_release( 0 )) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "release -- psync_release failed - dtc_code: %d", ret );
    }

	return DTC_RET( DTC_NONE );
}




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // local vars
    int                     ret                 = DTC_RET( DTC_NONE );

    // flags from command line arguments
    unsigned int            stdout_enabled      = 0;
    unsigned int            record_enabled      = 0;
    ps_timestamp            session_time        = 0;
    ps_timestamp            start_time          = 0;
    unsigned int            start_time_is_abs   = 0;
    unsigned int            shutdown_logger     = 0;





    // parse command line arguments
    {
        // arg
        int c = 0;

        // parse
        while( (c = getopt( argc, argv, "horqt:s:S:" )) != -1 )
        {
            if( c == 'h' )
            {
                printf( "-o\t\tenable stdout logging\n" );
                printf( "-r\t\tenable record mode\n" );
                printf( "-q\t\tshutdown loggers\n" );
                printf( "-t <N>\t\tset the session timestamp\n" );
                printf( "-s <N>\t\tset relative start time [microseconds]\n" );
                printf( "-S <N>\t\tset absolute UTC start time, takes precedence over relative [microseconds]\n" );
                return 0;
            }
            else if( c == 'o' )
            {
                stdout_enabled = 1;
            }
            else if( c == 'r' )
            {
                record_enabled = 1;
            }
            else if( c == 'q' )
            {
                shutdown_logger = 1;
            }
            else if( c == 't' )
            {
                // get val
                session_time = (unsigned long long) atoll( optarg );
            }
            else if( c == 's' )
            {
                // get val
                start_time = (unsigned long long) atoll( optarg );

                // clear flag
                start_time_is_abs = 0;
            }
            else if( c == 'S' )
            {
                // get val
                start_time = (unsigned long long) atoll( optarg );

                // set flag
                start_time_is_abs = 1;
            }
//            else if( c == 't' )
//            {
//                // get custom window title
//                strncpy( window_title, optarg, sizeof(window_title) );
//            }
        }
    }





    // init resources
    if( (ret = init( stdout_enabled )) != DTC_RET( DTC_NONE ) )
	{
        psync_log_message( LOG_LEVEL_ERROR, "main -- init failed - dtc_code: %d", ret );
		goto GRACEFUL_EXIT_STMNT;
	}


    // log
    //psync_log_message( LOG_LEVEL_DEBUG, "main -- source GUID: 0x%llX - 0d%llu", (unsigned long long) vsv_context.src_guid, (unsigned long long) vsv_context.src_guid );
    //psync_log_message( LOG_LEVEL_DEBUG, "main -- win_title: %s", window_title );




    //
    // check for shutdown command
    if( shutdown_logger )
    {
        psync_log_message( LOG_LEVEL_DEBUG, "main -- shutting down loggers" );

        //
        // set message data and publish command
        if( (ret = psync_rnr_set_mode( logger_packet_msg, LOGGER_MODE_OFF, 0 )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_rnr_set_mode - dtc_code: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        // done
        goto GRACEFUL_EXIT_STMNT;
    }


    //
    // check for record mode
    if( record_enabled )
    {
        psync_log_message( LOG_LEVEL_DEBUG, "main -- enabling record mode - session_time: %llu", session_time );

        //
        // set message data and publish command
        if( (ret = psync_rnr_set_mode( logger_packet_msg, LOGGER_MODE_WRITE, session_time )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_rnr_set_mode - dtc_code: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        //
        // let app process message
        usleep( 10000 );

        psync_log_message( LOG_LEVEL_DEBUG, "main -- start_time: %llu", start_time );

        //
        // log start time
        if( start_time_is_abs )
        {
            //
            // get date string without newline
            char tmp_string[ 256 ] = {0};

            time_t expr_time = (time_t) start_time;
            struct tm *expr_date = gmtime( &expr_time );
            strncpy( tmp_string, asctime( expr_date ), sizeof(tmp_string) );
            char *nl = strchr( tmp_string, '\n' );
            if( nl )
                (*nl) = '\0';

            psync_log_message( LOG_LEVEL_DEBUG, "main -- start_time is absolute (UTC): %s", tmp_string );
        }
        else
        {
            double seconds = (double) start_time;
            seconds /= 1000000.0;
            psync_log_message( LOG_LEVEL_DEBUG, "main -- start_time is relative (sec): %.4f", seconds );
        }

        //
        // set message data and publish command
        if( (ret = psync_rnr_enable_active_mode( logger_packet_msg, 1, start_time, !start_time_is_abs )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_rnr_enable_active_mode - dtc_code: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }
    }
    else
    {
        //
        // default to replay mode

        psync_log_message( LOG_LEVEL_DEBUG, "main -- enabling replay mode - session_time: %llu", session_time );

        //
        // set message data and publish command
        if( (ret = psync_rnr_set_mode( logger_packet_msg, LOGGER_MODE_REPLAY, session_time )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_rnr_set_mode - dtc_code: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }

        //
        // let app process message
        usleep( 10000 );

        psync_log_message( LOG_LEVEL_DEBUG, "main -- start_time: %llu", start_time );

        //
        // log start time
        if( start_time_is_abs )
        {
            //
            // get date string without newline
            char tmp_string[ 256 ] = {0};

            time_t expr_time = (time_t) start_time;
            struct tm *expr_date = gmtime( &expr_time );
            strncpy( tmp_string, asctime( expr_date ), sizeof(tmp_string) );
            char *nl = strchr( tmp_string, '\n' );
            if( nl )
                (*nl) = '\0';

            psync_log_message( LOG_LEVEL_DEBUG, "main -- start_time is absolute (UTC): %s", tmp_string );
        }
        else
        {
            double seconds = (double) start_time;
            seconds /= 1000000.0;
            psync_log_message( LOG_LEVEL_DEBUG, "main -- start_time is relative (sec): %.4f", seconds );
        }

        //
        // set message data and publish command
        if( (ret = psync_rnr_enable_active_mode( logger_packet_msg, 1, start_time, !start_time_is_abs )) != DTC_RET( DTC_NONE ) )
        {
            psync_log_message( LOG_LEVEL_ERROR, "main -- psync_rnr_enable_active_mode - dtc_code: %d", ret );
            goto GRACEFUL_EXIT_STMNT;
        }
    }




    //
    // using 'goto' for an easy example exit
    GRACEFUL_EXIT_STMNT:
	exit_signaled = 1;



    // log
    psync_log_message( LOG_LEVEL_DEBUG, "main -- node exiting" );


	// release
    if( (ret = release()) != DTC_RET( DTC_NONE ) )
    {
        psync_log_message( LOG_LEVEL_ERROR, "main -- release failed - dtc_code: %d", ret );
    }

    // done
	return 0;
}
