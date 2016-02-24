/*
 * Copyright (c) 2015 HARBRICK TECHNOLOGIES, INC
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
 * \example reader_writer.c
 *
 * Reader/Writer Example.
 *
 * Shows how to use publish/subscribe routines.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>

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
static const char NODE_NAME[] = "polysync-reader-writer";




// *****************************************************
// static declarations
// *****************************************************

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
 * \li "on_init" - Called once after node transitions into the INIT state.
 * \li "on_release" - Called once on node exit.
 * \li "on_warn" - Called continously while in WARN state.
 * \li "on_error" - Called continously while in ERROR state.
 * \li "on_fatal" - Called once after node transitions into the FATAL state before terminating.
 * \li "on_ok" - Called continously while in OK state.
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
 * \li "on_init" - Called once after node transitions into the INIT state.
 * \li "on_release" - Called once on node exit.
 * \li "on_warn" - Called continously while in WARN state.
 * \li "on_error" - Called continously while in ERROR state.
 * \li "on_fatal" - Called once after node transitions into the FATAL state before terminating.
 * \li "on_ok" - Called continously while in OK state.
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
 * \li "on_init" - Called once after node transitions into the INIT state.
 * \li "on_release" - Called once on node exit.
 * \li "on_warn" - Called continously while in WARN state.
 * \li "on_error" - Called continously while in ERROR state.
 * \li "on_fatal" - Called once after node transitions into the FATAL state before terminating.
 * \li "on_ok" - Called continously while in OK state.
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
 * \li "on_init" - Called once after node transitions into the INIT state.
 * \li "on_release" - Called once on node exit.
 * \li "on_warn" - Called continously while in WARN state.
 * \li "on_error" - Called continously while in ERROR state.
 * \li "on_fatal" - Called once after node transitions into the FATAL state before terminating.
 * \li "on_ok" - Called continously while in OK state.
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
 * \li "on_init" - Called once after node transitions into the INIT state.
 * \li "on_release" - Called once on node exit.
 * \li "on_warn" - Called continously while in WARN state.
 * \li "on_error" - Called continously while in ERROR state.
 * \li "on_fatal" - Called once after node transitions into the FATAL state before terminating.
 * \li "on_ok" - Called continously while in OK state.
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
 * \li "on_init" - Called once after node transitions into the INIT state.
 * \li "on_release" - Called once on node exit.
 * \li "on_warn" - Called continously while in WARN state.
 * \li "on_error" - Called continously while in ERROR state.
 * \li "on_fatal" - Called once after node transitions into the FATAL state before terminating.
 * \li "on_ok" - Called continously while in OK state.
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
    // do nothing
}


//
static void on_release(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // do nothing
}


//
static void on_error(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // do nothing
}


//
static void on_fatal(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // do nothing
}


//
static void on_warn(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // do nothing
}


//
static void on_ok(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // do nothing
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
