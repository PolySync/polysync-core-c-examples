/******************************************************
** FILE: bus_monitor.c
**
** ABSTRACT:
**
** DOCUMENTS:
**
** AUTHOR:
** JL
** Harbrick LLC
**
** NOTES:
**
*******************************************************/



/**
 * @example bus_monitor.c
 *
 * Bus Monitor Example node.
 *
 */


#include "node.h"




// *****************************************************
// public global data
// *****************************************************

const char                      *NODE_NAME          = "bus_monitor";
const ps_node_type              NODE_TYPE           = PSYNC_NID_API;
unsigned int                    NODE_STDOUT_ENABLED = 1;




// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief PolySync Message processing routines.
 *
 * Just prints out a message for the given type.
 *
 */
static int bm_process_message( ps_node_context *context, void *message );





// *****************************************************
// static definitions
// *****************************************************

//
static int bm_process_message( ps_node_context *context, void *message )
{
    if( !message )
    {
        psync_log_message( LOG_LEVEL_ERROR, "bm_process_message -- bad arg" );
        return DTC_USAGE;
    }

    // local vars
    int             ret             = DTC_NONE;
    ps_msg_type     msg_type        = 0;
    ps_timestamp    msg_timestamp   = 0;


    // get message type
    if( (ret = psync_message_get_type( message, &msg_type )) != DTC_NONE )
    {
        return ret;
    }

    // get timestamp
    if( (ret = psync_message_get_timestamp( message, &msg_timestamp )) != DTC_NONE )
    {
        return ret;
    }

    // print
    if( msg_type < PSYNC_MESSAGE_COUNT )
    {
        printf( "type: %s - timestamp: %llu\n", MESSAGE_TYPE_STRING_TABLE[ msg_type ], msg_timestamp );
    }
    else
    {
        printf( "unknown type\n" );
    }


    return DTC_NONE;
}







// *****************************************************
// public definitions
// *****************************************************

//
int node_set_configuration_data( ps_node_configuration_data *configuration )
{
    // local vars
    int         ret             = DTC_NONE;
    node_data   *data           = NULL;

    // flags from command line arguments
    int         opt_char        = 0;
    int         subopt_char     = 0;
    char        *subopt         = NULL;
    char        *subopt_val     = NULL;

    unsigned long   registered_message_types = 0;


    // parse command line arguments
    while( (opt_char = getopt( configuration->node_argc, configuration->node_argv, "ht:" )) != -1 )
    {
        if( opt_char == 'h' )
        {
            printf( "\nusage: bus_monitor [options]\n\n" );
            printf( "%s\t\t\t%s\n", "-h", "show help" );
            printf( "%s\t%s\n", "-t MSG_TYPE_EVENT,...", "message types to register listeners for" );
            printf( "\nex: bus_monitor -t MSG_TYPE_EVENT,MSG_TYPE_PLATFORM_MOTION\n\n" );

            // signal an exit
            raise( SIGINT );

            // done
            return DTC_NONE;
        }
        else if( opt_char == 't' )
        {
            //
            // each bit represents a flag indicating whether or not to register a listener for that type

            // parse subopts
            subopt = optarg;
            while( *subopt != '\0' )
            {
                subopt_char = getsubopt( &subopt, (char * const *) MESSAGE_TYPE_STRING_TABLE, &subopt_val );
                if( (subopt_char >= 0) && (subopt_char < PSYNC_MESSAGE_COUNT) )
                {
                    // set flag
                    registered_message_types |= (unsigned long) (1 << subopt_char);
                }
            }
        }
    }

    // create user data
    if( (ret = node_create_user_data( configuration )) != DTC_NONE )
	{
		return ret;
	}

    // get reference
    data = (node_data*) configuration->user_data;

    // set user data
    data->registered_message_types = registered_message_types;


    return DTC_NONE;
}


//
int node_on_init( ps_node_context *context, ps_diagnostic_state *current_state, void *user_data )
{
    // local vars
    int         ret             = DTC_NONE;
    node_data   *data           = NULL;


    // get reference
    data = (node_data*) context->configuration_data.user_data;


    // register listeners
    if( (ret = node_register_message_listeners( context, data->registered_message_types )) != DTC_NONE )
	{
        psync_log_message( LOG_LEVEL_ERROR, "node_on_init -- node_register_message_listeners failed" );
		return ret;
	}

    return DTC_NONE;
}


//
int node_on_release( ps_node_context *context, ps_diagnostic_state *current_state, void *user_data )
{
    // local vars
    int     ret     = DTC_NONE;

    psync_log_message( LOG_LEVEL_DEBUG, "node_on_release" );

    // release user data
    if( (ret = node_release_user_data( &context->configuration_data )) != DTC_NONE )
	{
        psync_log_message( LOG_LEVEL_ERROR, "node_on_init -- node_release_user_data failed" );
		return ret;
	}


    return DTC_NONE;
}


//
int node_on_warn( ps_node_context *context, ps_diagnostic_state *current_state, void *user_data )
{
    psync_log_message( LOG_LEVEL_DEBUG, "node_on_warn -- TEST" );

    psync_sleep_micro( 500000 );

    return DTC_NONE;
}


//
int node_on_error( ps_node_context *context, ps_diagnostic_state *current_state, void *user_data )
{
    psync_log_message( LOG_LEVEL_DEBUG, "node_on_error -- TEST" );

    psync_sleep_micro( 500000 );

    return DTC_NONE;
}


//
int node_on_fatal( ps_node_context *context, ps_diagnostic_state *current_state, void *user_data )
{
    psync_log_message( LOG_LEVEL_DEBUG, "node_on_fatal -- TEST" );

    psync_sleep_micro( 500000 );

    return DTC_NONE;
}


//
int node_on_ok( ps_node_context *context, ps_diagnostic_state *current_state, void *user_data )
{
    // local vars
    int     ret                 = DTC_NONE;
    // incoming message data pointer
    void    *message_ptr        = NULL;


    // check the message queue
    if( (ret = node_poll_for_message( context, &message_ptr )) != DTC_NONE )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node_on_ok -- node_poll_for_message failed" );
		return ret;
    }


    // if message is available
    if( message_ptr )
    {
        // process message
        if( (ret = bm_process_message( context, message_ptr )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "node_on_ok -- node_poll_for_message failed" );
            return ret;
        }

        // release message
        if( (ret = psync_message_release_instance( &message_ptr )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "node_on_ok -- psync_message_release_instance failed" );
            return ret;
        }
    }
    else
    {
        // wait a little
        if( (ret = psync_sleep_micro( 5000 )) != DTC_NONE )
        {
            psync_log_message( LOG_LEVEL_ERROR, "node_on_ok -- psync_sleep_micro failed" );
            return ret;
        }
    }


    return DTC_NONE;
}
