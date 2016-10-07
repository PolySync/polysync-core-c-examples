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
 * \example rnr_node.c
 *
 * Record and Replay Participant Node Example.
 *
 * Shows how to use RnR and Logfile APIs to handle record/replay operations.
 *
 * It will log example ps_event_msg's when recording.
 *
 * The example uses the standard PolySync node template and state machine.
 * Send the SIGINT (control-C on the keyboard) signal to the node/process to do a graceful shutdown.
 * See \ref polysync_node_template.h for more information.
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
#include "polysync_node.h"
#include "polysync_message.h"
#include "polysync_sdf.h"
#include "polysync_logfile.h"
#include "polysync_rnr.h"
#include "polysync_node_template.h"




// *****************************************************
// static global types/macros
// *****************************************************


/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "polysync-rnr-node-c";


/**
 * @brief How often to log message data. [Hertz]
 *
 */
static const ps_timestamp WRITE_FREQ = 10;


/**
 * @brief TODO message type name.
 *
 */
static const char RNR_MSG_NAME[] = "ps_rnr_msg";

#warning "TODO - docs on these constants and docs on declarations/updates"
/**
 * @brief Event message type name.
 *
 */
static const char EVENT_MSG_NAME[] = "ps_event_msg";


//
static GAsyncQueue *msg_handler_queue = NULL;




// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief Message "ps_rnr_msg" handler.
 *
 * Enqueues the message to be processed in the on_ok() function.
 *
 * @param [in] msg_type Message type identifier for the message, as seen by the data model.
 * @param [in] message Message reference to be handled by the function.
 * @param [in] user_data A pointer to void which specifies the internal data.
 *
 */
static void ps_rnr_msg__handler(
        const ps_msg_type msg_type,
        const ps_msg_ref const message,
        void * const user_data );


/**
 * @brief Publish a node's RnR status at a constant rate.
 *
 * Uses the current time to determine whether or not to actually publish a message.
 *
 * @param [in] node_ref Node reference.
 * @param [in/out] tx_timestamp A pointer to \ref ps_timestamp which contains the last publish timestamp.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int publish_rnr_status(
        ps_node_ref node_ref,
        ps_timestamp * const tx_timestamp );


/**
 * @brief Log a 'ps_event_msg' at a constant rate.
 *
 * Uses the current time to determine whether or not to actually log a message.
 *
 * @param [in] node_ref Node reference.
 * @param [in/out] logged_timestamp A pointer to \ref ps_timestamp which contains the last logged timestamp.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int log_event_message(
        ps_node_ref node_ref,
        ps_timestamp * const logged_timestamp );


/**
 * @brief Process a replay message if one exist in the replay message queue.
 *
 * Prints the message details to stdout.
 *
 * @param [in] node_ref Node reference.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int poll_for_replay_message(
        ps_node_ref node_ref );


/**
 * @brief Node template set configuration callback function.
 *
 * If the host provides command line arguments they will be set, and available
 * for parsing (ie getopts).
 *
 * @note Returning a DTC other than DTC_NONE will cause the node to transition
 * into the fatal state and terminate.
 *
 * @param [in] node_config A pointer to \ref ps_node_configuration_data which specifies the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int set_configuration(
        ps_node_configuration_data * const node_config );


/**
 * @brief Node template on_init callback function.
 *
 * Called once after node transitions into the INIT state.
 *
 * @param [in] node_ref Node reference, provided by node template API.
 * @param [in] state A pointer to \ref ps_diagnostic_state which stores the current state of the node.
 * @param [in] user_data A pointer to user data, provided by user during configuration.
 *
 */
static void on_init(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data );


/**
 * @brief Node template on_release callback function.
 *
 * Called once on node exit.
 *
 * @param [in] node_ref Node reference, provided by node template API.
 * @param [in] state A pointer to \ref ps_diagnostic_state which stores the current state of the node.
 * @param [in] user_data A pointer to user data, provided by user during configuration.
 *
 */
static void on_release(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data );


/**
 * @brief Node template on_ok callback function.
 *
 * Called continously while in OK state.
 *
 * @param [in] node_ref Node reference, provided by node template API.
 * @param [in] state A pointer to \ref ps_diagnostic_state which stores the current state of the node.
 * @param [in] user_data A pointer to user data, provided by user during configuration.
 *
 */
static void on_ok(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data );


/**
 * @brief Node template on_error callback function.
 *
 * Called continously while in ERROR state.
 *
 * @param [in] node_ref Node reference, provided by node template API.
 * @param [in] state A pointer to \ref ps_diagnostic_state which stores the current state of the node.
 * @param [in] user_data A pointer to user data, provided by user during configuration.
 *
 */
static void on_error(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data );


/**
 * @brief Node template on_fatal callback function.
 *
 * Called once after node transitions into the FATAL state before terminating.
 *
 * @param [in] node_ref Node reference, provided by node template API.
 * @param [in] state A pointer to \ref ps_diagnostic_state which stores the current state of the node.
 * @param [in] user_data A pointer to user data, provided by user during configuration.
 *
 */
static void on_fatal(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data );


/**
 * @brief Node template on_warn callback function.
 *
 * Called continously while in WARN state.
 *
 * @param [in] node_ref Node reference, provided by node template API.
 * @param [in] state A pointer to \ref ps_diagnostic_state which stores the current state of the node.
 * @param [in] user_data A pointer to user data, provided by user during configuration.
 *
 */
static void on_warn(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data );




// *****************************************************
// static definitions
// *****************************************************


//
static void ps_rnr_msg__handler(
        const ps_msg_type msg_type,
        const ps_msg_ref const message,
        void * const user_data )
{
    int ret = 0;

    // expects the node_ref to be provided by user_data
    if( user_data == NULL )
    {
        psync_log_error( "invalid node reference" );
        ret = DTC_CONFIG;
    }

    // expects valid message queue
    if( msg_handler_queue == NULL )
    {
        psync_log_error( "invalid message handler queue" );
        ret = DTC_CONFIG;
    }

    ps_node_ref node_ref = (ps_node_ref) user_data;
    ps_msg_ref msg_copy = PSYNC_MSG_REF_INVALID;
    ps_guid my_guid = PSYNC_GUID_INVALID;
    ps_guid src_guid = PSYNC_GUID_INVALID;

    // get our node's GUID
    if( ret == DTC_NONE )
    {
        ret = psync_node_get_guid(
                node_ref,
                &my_guid );
        if( ret != DTC_NONE )
        {
            psync_log_error( "psync_node_get_guid returned DTC %d", ret );
        }
    }

    // get the GUID of the message publisher (in message.header.src_guid)
    if( ret == DTC_NONE )
    {
        ret = psync_message_get_source_guid(
                message,
                &src_guid );
        if( ret != DTC_NONE )
        {
            psync_log_error( "psync_message_get_source_guid returned DTC %d", ret );
        }
    }

    // create a new message of the same type
    if( ret == DTC_NONE )
    {
        ret = psync_message_alloc(
                node_ref,
                msg_type,
                &msg_copy );
        if( ret != DTC_NONE )
        {
            psync_log_error( "psync_message_alloc returned DTC %d", ret );
        }
    }

    // copy message
    if( ret == DTC_NONE )
    {
        ret = psync_message_copy(
                node_ref,
                message,
                msg_copy );
        if( ret != DTC_NONE )
        {
            psync_log_error( "psync_message_copy returned DTC %d", ret );
        }
    }

    // enqueue
    if( (msg_copy != PSYNC_MSG_REF_INVALID) && (msg_handler_queue != NULL) )
    {
        g_async_queue_push(
                msg_handler_queue,
                (gpointer) msg_copy );
    }
}


//
static int publish_rnr_status(
        ps_node_ref node_ref,
        ps_timestamp * const tx_timestamp )
{
    int ret = DTC_NONE;
    ps_timestamp now = 0;

    if( node_ref == PSYNC_NODE_REF_INVALID )
    {
        psync_log_error( "invalid node reference" );
        ret = DTC_CONFIG;
    }

    if( tx_timestamp == NULL )
    {
        psync_log_error( "invalid arguments" );
        ret = DTC_CONFIG;
    }

    // get the current time
    if( ret == DTC_NONE )
    {
        ret = psync_get_timestamp( &now );
    }

    // check if we should publish again
    if( ret == DTC_NONE )
    {
        if( (now - *tx_timestamp) >= PSYNC_PLOG_REPLAY_PUBLISH_INTERVAL )
        {
            ps_msg_ref msg = PSYNC_MSG_REF_INVALID;
            ps_msg_type msg_type = PSYNC_MSG_TYPE_INVALID;

            // get RnR message type identifier
            if( ret == DTC_NONE )
            {
                ret = psync_message_get_type_by_name(
                        node_ref,
                        RNR_MSG_NAME,
                        &msg_type );
            }

            // create RnR message
            if( ret == DTC_NONE )
            {
                ret = psync_message_alloc(
                        node_ref,
                        msg_type,
                        &msg );
            }

            // fill RnR message using logfile API
            if( ret == DTC_NONE )
            {
                ret = psync_logfile_fill_rnr_status_message(
                        node_ref,
                        (ps_rnr_msg*) msg );
            }

            // update last publish timestamp
            if( ret == DTC_NONE )
            {
                ret = psync_get_timestamp( tx_timestamp );
            }

            // publish
            if( ret == DTC_NONE )
            {
                ret = psync_message_publish(
                        node_ref,
                        msg );
            }

            // free message
            if( ret == DTC_NONE )
            {
                ret = psync_message_free(
                        node_ref,
                        &msg );
            }
        }
    }

    return ret;
}


//
static int log_event_message(
        ps_node_ref node_ref,
        ps_timestamp * const logged_timestamp )
{
    int ret = DTC_NONE;
    ps_timestamp now = 0;

    if( node_ref == PSYNC_NODE_REF_INVALID )
    {
        psync_log_error( "invalid node reference" );
        ret = DTC_CONFIG;
    }

    if( logged_timestamp == NULL )
    {
        psync_log_error( "invalid arguments" );
        ret = DTC_CONFIG;
    }

    // get the current time
    if( ret == DTC_NONE )
    {
        ret = psync_get_timestamp( &now );
    }

    // check if we should log a message again
    if( ret == DTC_NONE )
    {
        const ps_timestamp log_interval = (ps_timestamp) (1000000ULL / WRITE_FREQ);

        if( (now - *logged_timestamp) >= log_interval )
        {
            ps_msg_ref msg = PSYNC_MSG_REF_INVALID;
            ps_msg_type msg_type = PSYNC_MSG_TYPE_INVALID;

            // get event message type identifier
            if( ret == DTC_NONE )
            {
                ret = psync_message_get_type_by_name(
                        node_ref,
                        EVENT_MSG_NAME,
                        &msg_type );
            }

            // create event message
            if( ret == DTC_NONE )
            {
                ret = psync_message_alloc(
                        node_ref,
                        msg_type,
                        &msg );
            }

            // update last publish timestamp
            if( ret == DTC_NONE )
            {
                ret = psync_get_timestamp( logged_timestamp );
            }

            // fill event message with arbitrary data
            if( ret == DTC_NONE )
            {
                ps_event_msg * const event_msg = (ps_event_msg*) msg;

                event_msg->id = 0;
                event_msg->dest_guid = 0;
                event_msg->header.timestamp = *logged_timestamp;
            }

            // log
            if( ret == DTC_NONE )
            {
                ret = psync_logfile_write_message(
                        node_ref,
                        msg );
            }

            // free message
            if( ret == DTC_NONE )
            {
                ret = psync_message_free(
                        node_ref,
                        &msg );
            }
        }
    }

    return ret;
}


//
static int poll_for_replay_message(
        ps_node_ref node_ref )
{
    int ret = DTC_NONE;

    // holds a pointer to the logfile API message queue
    static GAsyncQueue *replay_queue = NULL;

    // get a pointer to the logfile reader queue
    // only need to get the pointer once after calling psync_logfile_init()
    if( replay_queue == NULL )
    {
        ret = psync_logfile_get_replay_msg_queue(
                node_ref,
                &replay_queue );
        if( replay_queue == NULL )
        {
            psync_log_error( "failed to get logfile replay message queue" );
            ret = DTC_MEMERR;
        }
    }

    // check for a replay message
    ps_msg_ref replay_msg = PSYNC_MSG_REF_INVALID;
    if( ret == DTC_NONE )
    {
        replay_msg = g_async_queue_try_pop( replay_queue );
    }

    // print message details
    if( replay_msg != PSYNC_MSG_REF_INVALID )
    {
        ps_msg_type msg_type = PSYNC_MSG_TYPE_INVALID;
        ps_msg_header msg_header;
        char msg_type_name[512];

        printf( "received replay message from logfile API queue\n" );

        if( ret == DTC_NONE )
        {
            ret = psync_message_get_type(
                    replay_msg,
                    &msg_type );

            printf( "  msg_type: %lu\n", (unsigned long) msg_type );
        }

        if( ret == DTC_NONE )
        {
            ret = psync_message_get_name_by_type(
                    node_ref,
                    msg_type,
                    msg_type_name,
                    sizeof(msg_type_name) );

            printf( "  type_name: '%s'\n", msg_type_name );
        }

        if( ret == DTC_NONE )
        {
            ret = psync_message_get_header(
                    replay_msg,
                    &msg_header );

            printf( "  msg_header.timestamp: %llu\n",
                    (unsigned long long) msg_header.timestamp );
        }
    }

    // free message
    if( ret == DTC_NONE )
    {
        ret = psync_message_free(
                node_ref,
                &replay_msg );
    }

    return ret;
}


//
static int set_configuration(
        ps_node_configuration_data * const node_config )
{
    // set node configuration default values

    // node type
    node_config->node_type = PSYNC_NODE_TYPE_API_USER;

    // set node domain
    node_config->domain_id = PSYNC_DEFAULT_DOMAIN;

    // set node SDF key
    node_config->sdf_key = PSYNC_SDF_ID_INVALID;

    // set node flags
    node_config->flags = 0;

    // set user data
    node_config->user_data = NULL;

    // set node name
    memset( node_config->node_name, 0, sizeof(node_config->node_name) );
    strncpy( node_config->node_name, NODE_NAME, sizeof(node_config->node_name) );

    return DTC_NONE;
}


//
static void on_init(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    int ret = DTC_NONE;
    ps_msg_type msg_type = PSYNC_MSG_TYPE_INVALID;

    // create message handler queue
    msg_handler_queue = g_async_queue_new();
    if( msg_handler_queue == NULL )
    {
        psync_log_error( "failed to create message handler queue" );
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // initialize the logfile API
    ret = psync_logfile_init( node_ref );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_logfile_init returned DTC %d", ret );
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // configure logfile API to provide replay messages to a queue in addition to publishing to the bus
    ret = psync_logfile_enable_output_queue(
            node_ref,
            1 );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_logfile_enable_output_queue returned DTC %d", ret );
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get RnR message type identifier
    ret = psync_message_get_type_by_name(
            node_ref,
            RNR_MSG_NAME,
            &msg_type );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_message_get_type_by_name returned DTC %d", ret );
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // set subscriber QoS to reliable
    ret = psync_node_set_subscriber_reliability_qos(
            node_ref,
            msg_type,
            RELIABILITY_QOS_RELIABLE );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_node_set_subscriber_reliability_qos returned DTC %d", ret );
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // register subscriber for RnR message
    ret = psync_message_register_listener(
            node_ref,
            msg_type,
            ps_rnr_msg__handler,
            (void*) node_ref );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_message_register_listener returned DTC %d", ret );
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }
}


//
static void on_release(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    ps_msg_type msg_type = PSYNC_MSG_TYPE_INVALID;

    // get RnR message type identifier
    (void) psync_message_get_type_by_name(
            node_ref,
            RNR_MSG_NAME,
            &msg_type );

    // unregister subscriber for RnR message
    if( msg_type != PSYNC_MSG_TYPE_INVALID )
    {
        (void) psync_message_unregister_listener(
                node_ref,
                msg_type );
    }

    // release logfile API
    (void) psync_logfile_release( node_ref );

    // flush message handler queue
    if( msg_handler_queue != NULL )
    {
        ps_msg_ref msg = g_async_queue_try_pop( msg_handler_queue );

        while( msg != PSYNC_MSG_REF_INVALID )
        {
            (void) psync_message_free(
                    node_ref,
                    &msg );

            msg = g_async_queue_try_pop( msg_handler_queue );
        }

        g_async_queue_unref( msg_handler_queue );
        msg_handler_queue = NULL;
    }
}


//
static void on_ok(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    int ret = DTC_NONE;
    ps_logfile_mode_kind logfile_mode = LOGFILE_MODE_INVALID;
    ps_logfile_state_kind logfile_state = LOGFILE_STATE_INVALID;

    // holds the last publish timestamp of our RnR status message
    static ps_timestamp last_rnr_publish_time = 0;

    // holds the last logged event message timestamp
    static ps_timestamp last_event_logged_time = 0;

    // check for any incoming RnR messages from the handler queue
    if( msg_handler_queue != NULL )
    {
        ps_msg_ref rx_msg = g_async_queue_try_pop( msg_handler_queue );

        // we are only expected RnR messages
        if( rx_msg != PSYNC_MSG_REF_INVALID )
        {
            // cast and process the RnR message
            ps_rnr_msg * const rnr_msg = (ps_rnr_msg*) rx_msg;

            ret = psync_logfile_handle_rnr_command(
                    node_ref,
                    rnr_msg );
            if( ret != DTC_NONE )
            {
                psync_log_error( "psync_logfile_handle_rnr_command returned DTC %d", ret );
                psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
            }

            // free message
            ret = psync_message_free(
                    node_ref,
                    &rx_msg );
            if( ret != DTC_NONE )
            {
                psync_log_error( "psync_message_free returned DTC %d", ret );
                psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
            }
        }
    }

    // publish our RnR status at a constant rate (can be done in a worker thread)
    ret = publish_rnr_status(
            node_ref,
            &last_rnr_publish_time );
    if( ret != DTC_NONE )
    {
        psync_log_error( "publish_rnr_status returned DTC %d", ret );
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
    }

    // get the current logfile mode
    ret = psync_logfile_get_mode(
            node_ref,
            &logfile_mode );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_logfile_get_mode returned DTC %d", ret );
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
    }

    // get the current logfile state
    ret = psync_logfile_get_state(
            node_ref,
            &logfile_state );
    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_logfile_get_state returned DTC %d", ret );
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
    }

    // log a ps_event_msg message if we're in record mode and actively recording
    if( logfile_mode == LOGFILE_MODE_WRITE )
    {
        if( logfile_state == LOGFILE_STATE_ENABLED )
        {
            ret = log_event_message(
                    node_ref,
                    &last_event_logged_time );
        }
    }
    // if we are in replay mode, then poll the logfile API's reader queue
    // for a new replay message
    else if( logfile_mode == LOGFILE_MODE_READ )
    {
        ret = poll_for_replay_message( node_ref );
        if( ret != DTC_NONE )
        {
            psync_log_error( "poll_for_replay_message returned DTC %d", ret );
            psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        }
    }

    // sleep for 1 milliseconds to prevent loading up the CPU
    (void) psync_sleep_micro( 1000 );
}


//
static void on_error(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // do nothing, sleep for 10 milliseconds
    (void) psync_sleep_micro( 10000 );
}


//
static void on_fatal(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // do nothing, sleep for 10 milliseconds
    (void) psync_sleep_micro( 10000 );
}


//
static void on_warn(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // do nothing, sleep for 10 milliseconds
    (void) psync_sleep_micro( 10000 );
}




// *****************************************************
// public definitions
// *****************************************************


//
int main( int argc, char **argv )
{
    // callback data
    ps_node_callbacks callbacks;

    // zero
    memset( &callbacks, 0, sizeof(callbacks) );

    // set callbacks
    callbacks.set_config = &set_configuration;
    callbacks.on_init = &on_init;
    callbacks.on_release = &on_release;
    callbacks.on_warn = &on_warn;
    callbacks.on_error = &on_error;
    callbacks.on_fatal = &on_fatal;
    callbacks.on_ok = &on_ok;

    // use PolySync main entry, this will give execution context to node template machine
    return( psync_node_main_entry( &callbacks, argc, argv ) );
}
