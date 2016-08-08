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
 * \example get_set.c
 *
 * Get/Set Parameters Example.
 *
 * Shows how to use get/set system to get all parameters from all nodes.
 *
 * The example uses the standard PolySync node template and state machine.
 * Send the SIGINT (control-C on the keyboard) signal to the node/process to do a graceful shutdown.
 * See \ref polysync_node_template.h for more information.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// API headers
#include "polysync_core.h"
#include "polysync_node.h"
#include "polysync_sdf.h"
#include "polysync_message.h"
#include "polysync_node_template.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief Node flags to be OR'd with driver/interface flags.
 *
 * Provided by the compiler so Harbrick can add build-specifics as needed.
 *
 */
#ifndef NODE_FLAGS_VALUE
#define NODE_FLAGS_VALUE (0)
#endif


/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "polysync-get-set-c";


/**
 * @brief Parameters message type name.
 *
 */
static const char PARAMETERS_MSG_NAME[] = "ps_parameters_msg";




// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief Message "ps_parameters_msg" handler.
 *
 * Prints the parameter response information.
 *
 * @param [in] msg_type Message type identifier for the message, as seen by the data model.
 * @param [in] message Message reference to be handled by the function.
 * @param [in] user_data A pointer to void which specifies the internal data.
 *
 */
static void ps_parameters_msg__handler(
        const ps_msg_type msg_type,
        const ps_msg_ref const message,
        void * const user_data );


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




// *****************************************************
// static definitions
// *****************************************************

//
static void ps_parameters_msg__handler(
        const ps_msg_type msg_type,
        const ps_msg_ref const message,
        void * const user_data )
{
    // index
    unsigned long idx = 0;

    // cast to message
    const ps_parameters_msg * const parameters_msg = (ps_parameters_msg*) message;


    // only handle responses from other nodes
    if( parameters_msg->type != PARAMETER_MESSAGE_RESPONSE )
    {
        return;
    }

    // print details
    printf( "  received parameters message from node 0x%016llX (%llu)\n",
            (unsigned long long) parameters_msg->header.src_guid,
            (unsigned long long) parameters_msg->header.src_guid );

    // node does not support any parameters
    if( parameters_msg->parameters._length == 0 )
    {
        printf( "  - no parameters supported\n" );
    }

    // for each parameter in the response
    for( idx = 0; idx < parameters_msg->parameters._length; idx++ )
    {
        // get convenience ref
        const ps_parameter * const param = &parameters_msg->parameters._buffer[ idx ];

        printf( "- [%lu]\n", idx );
        printf( "    ID: 0x%llX (%llu)\n", (unsigned long long) param->id, (unsigned long long) param->id );
        printf( "    description: '%s'\n", param->description._buffer );
        printf( "    value type: %u\n", (unsigned int) param->value._d );
        printf( "    flags: 0x%X", (unsigned int) param->flags );

        // format flag bit strings if set
        if( param->flags != 0 )
        {
            // check bits
            if( param->flags & PSYNC_PARAMETER_FLAG_DISABLED )
            {
                printf( ", DISABLED" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_USERDEFINED )
            {
                printf( ", USER-DEFINED" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_ANALYTIC )
            {
                printf( ", ANALYTIC" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_READONLY_VALUE )
            {
                printf( ", READ-ONLY VALUE" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_READONLY_MIN )
            {
                printf( ", READ-ONLY MIN" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_READONLY_MAX )
            {
                printf( ", READ-ONLY MAX" );
            }
            if( param->flags & PSYNC_PARAMETER_FLAG_READONLY_STEP )
            {
                printf( ", READ-ONLY STEP" );
            }
            printf( "\n" );
        }
        else
        {
            printf( "\n" );
        }
    }

    printf( "\n" );
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
    node_config->flags = NODE_FLAGS_VALUE | PSYNC_INIT_FLAG_STDOUT_LOGGING;

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
    // local vars
    int ret = DTC_NONE;
    ps_msg_type msg_type = PSYNC_MSG_TYPE_INVALID;
    ps_msg_ref msg = PSYNC_MSG_REF_INVALID;


    // get parameters message type identifier
    ret = psync_message_get_type_by_name(
            node_ref,
            PARAMETERS_MSG_NAME,
            &msg_type );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_get_type_by_name returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // set default parameters message subscriber QoS to reliable
    ret = psync_node_set_subscriber_reliability_qos(
            node_ref,
            msg_type,
            RELIABILITY_QOS_RELIABLE );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_node_set_subscriber_reliability_qos returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // register subscriber for parameters message
    ret = psync_message_register_listener(
            node_ref,
            msg_type,
            ps_parameters_msg__handler,
            NULL );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_register_listener returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // create a parameters message
    ret = psync_message_alloc(
            node_ref,
            msg_type,
            &msg );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_alloc returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // set message data
    ps_parameters_msg *parameters_msg = (ps_parameters_msg*) msg;

    // create a local parameter structure to hold our get-all command
    ps_parameter parameter;
    memset( &parameter, 0, sizeof(parameter) );

    // invalid destination GUID will be handled by all nodes
    parameters_msg->dest_guid = PSYNC_GUID_INVALID;

    // set ID to reserved get-all ID
    parameter.id = PSYNC_PARAM_ID_ALL;

    // point message sequence buffer to the local variable since we just doing a get all
    parameters_msg->parameters._buffer = &parameter;
    parameters_msg->parameters._length = 1;
    parameters_msg->parameters._maximum = 1;

    // set type to get-all
    parameters_msg->type = PARAMETER_MESSAGE_GET_ALL;

    // set publish timestamp in message header
    ret = psync_get_timestamp( &parameters_msg->header.timestamp );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_get_timestamp returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    psync_log_message(
                LOG_LEVEL_INFO,
                "%s : (%u) -- sending get-set command to get all parameters from all nodes",
                __FILE__,
                __LINE__ );

    // publish parameters message
    ret = psync_message_publish(
            node_ref,
            msg );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_publish returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // free parameters message
    ret = psync_message_free(
            node_ref,
            &msg );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_message_free returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

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
    // do nothing, sleep for 10 milliseconds
    (void) psync_sleep_micro( 10000 );
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


//
static void on_ok(
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
