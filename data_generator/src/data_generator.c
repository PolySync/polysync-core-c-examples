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
 * \example data_generator.c
 *
 * Data Generator Example.
 *
 * Shows how to publish various provided sensor data messages.
 *
 * The example uses the standard PolySync node template and state machine.
 * Send the SIGINT (control-C on the keyboard) signal to the node/process to do a graceful shutdown.
 * See \ref polysync_node_template.h for more information.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_node.h"
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
 * @brief Default update interval for all generators. [microseconds]
 *
 */
#define GENERATOR_UPDATE_INTERVAL (50000)




/**
 * @brief Node data.
 *
 * Serves as a top-level container for the application's data structures.
 *
 */
typedef struct
{
    //
    //
    ps_msg_ref radar_targets_msg; /*!< 'ps_radar_targets_msg' message. */
    //
    //
    ps_msg_ref lidar_points_msg; /*!< 'ps_lidar_points_msg' message. */
    //
    //
    ps_msg_ref objects_msg; /*!< 'ps_objects_msg' message. */
} node_data_s;




// *****************************************************
// static global data
// *****************************************************

/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "polysync-data-generator-c";


/**
 * @brief PolySync radar targets message name.
 *
 */
static const char RADAR_TARGETS_MSG_NAME[] = "ps_radar_targets_msg";


/**
 * @brief PolySync LiDAR points message name.
 *
 */
static const char LIDAR_POINTS_MSG_NAME[] = "ps_lidar_points_msg";


/**
 * @brief PolySync objects message name.
 *
 */
static const char OBJECTS_MSG_NAME[] = "ps_objects_msg";




// *****************************************************
// public declarations
// *****************************************************

/**
 * @brief Initialize radar targets generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_radar_targets_msg which receives the initialization.
 *
 */
void radar_targets_init( const ps_timestamp time, ps_radar_targets_msg * const msg );


/**
 * @brief Update radar targets generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_radar_targets_msg which receives the update.
 *
 */
void radar_targets_update( const ps_timestamp time, ps_radar_targets_msg * const msg );


/**
 * @brief Initialize LiDAR points generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_lidar_points_msg which receives the initialization.
 *
 */
void lidar_points_init( const ps_timestamp time, ps_lidar_points_msg * const msg );


/**
 * @brief Update LiDAR points generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_lidar_points_msg which receives the update.
 *
 */
void lidar_points_update( const ps_timestamp time, ps_lidar_points_msg * const msg );


/**
 * @brief Initialize objects generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_objects_msg which receives the initialization.
 *
 */
void objects_init( const ps_timestamp time, ps_objects_msg * const msg );


/**
 * @brief Update objects generator.
 *
 * @param [in] time Current timestamp. [microseconds]
 * @param [in] msg A pointer to \ref ps_objects_msg which receives the update.
 *
 */
void objects_update( const ps_timestamp time, ps_objects_msg * const msg );




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
static int set_configuration(
        ps_node_configuration_data * const node_config )
{
    // local vars
    node_data_s *node_data = NULL;


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

    // create node data
    if( (node_data = malloc( sizeof(*node_data) )) == NULL )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- failed to allocate node data structure",
                __FILE__,
                __LINE__ );

        return DTC_MEMERR;
    }

    // zero
    memset( node_data, 0, sizeof(*node_data) );

    // set user data pointer to our top-level node data
    // this will get passed around to the various interface routines
    node_config->user_data = (void*) node_data;


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
    node_data_s *node_data = NULL;


    // cast
    node_data = (node_data_s*) user_data;

    // check reference since other routines don't
    if( node_data == NULL )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- invalid node context",
                __FILE__,
                __LINE__ );

        psync_node_activate_fault( node_ref, DTC_USAGE, NODE_STATE_FATAL );
        return;
    }

    // get radar targets message type identifier
    ret = psync_message_get_type_by_name(
            node_ref,
            RADAR_TARGETS_MSG_NAME,
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

    // create radar targets message
    ret = psync_message_alloc(
            node_ref,
            msg_type,
            &node_data->radar_targets_msg );

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

    // get lidar points message type identifier
    ret = psync_message_get_type_by_name(
            node_ref,
            LIDAR_POINTS_MSG_NAME,
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

    // create lidar points message
    ret = psync_message_alloc(
            node_ref,
            msg_type,
            &node_data->lidar_points_msg );

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

    // get objects message type identifier
    ret = psync_message_get_type_by_name(
            node_ref,
            OBJECTS_MSG_NAME,
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

    // create objects message
    ret = psync_message_alloc(
            node_ref,
            msg_type,
            &node_data->objects_msg );

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

    // get current timestamp
    ps_timestamp current_time = 0;
    ret = psync_get_timestamp( &current_time );

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

    // init radar targets generator and resources
    radar_targets_init(
            current_time,
            (ps_radar_targets_msg*) node_data->radar_targets_msg );

    // init lidar points generator and resources
    lidar_points_init(
            current_time,
            (ps_lidar_points_msg*) node_data->lidar_points_msg );

    // init objects generator and resources
    objects_init(
            current_time,
            (ps_objects_msg*) node_data->objects_msg );
}


//
static void on_release(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // local vars
    node_data_s *node_data = NULL;


    // cast
    node_data = (node_data_s*) user_data;

    // if valid
    if( node_data != NULL )
    {
        // free messages
        (void) psync_message_free(
                node_ref,
                (ps_msg_ref*) &node_data->radar_targets_msg );
        (void) psync_message_free(
                node_ref,
                (ps_msg_ref*) &node_data->lidar_points_msg );
        (void) psync_message_free(
                node_ref,
                (ps_msg_ref*) &node_data->objects_msg );

        // free
        free( node_data );
        node_data = NULL;
    }
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
    // local vars
    int ret = DTC_NONE;
    ps_timestamp current_time = 0;
    node_data_s *node_data = NULL;

    // cast
    node_data = (node_data_s*) user_data;

    // check reference since other routines don't
    if( node_data == NULL )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- invalid node context",
                __FILE__,
                __LINE__ );

        psync_node_activate_fault( node_ref, DTC_USAGE, NODE_STATE_FATAL );
        return;
    }

    // get current timestamp
    ret = psync_get_timestamp( &current_time );

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

    // update radar targets
    radar_targets_update(
            current_time,
            (ps_radar_targets_msg*) node_data->radar_targets_msg );

    // publish radar targets message
    ret = psync_message_publish(
            node_ref,
            node_data->radar_targets_msg );

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

    // update lidar points
    lidar_points_update(
            current_time,
            (ps_lidar_points_msg*) node_data->lidar_points_msg );

    // publish lidar points message
    ret = psync_message_publish(
            node_ref,
            node_data->lidar_points_msg );

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

    // update objects
    objects_update(
            current_time,
            (ps_objects_msg*) node_data->objects_msg );

    // publish objects message
    ret = psync_message_publish(
            node_ref,
            node_data->objects_msg );

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

    // wait for next update
    ret = psync_sleep_micro( GENERATOR_UPDATE_INTERVAL );

    // activate fatal error and return if failed
    if( ret != DTC_NONE )
    {
        psync_log_message(
                LOG_LEVEL_ERROR,
                "%s : (%u) -- psync_sleep_micro returned DTC %d",
                __FILE__,
                __LINE__,
                ret );

        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }
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
