/**
 * @file node.c
 * @brief Node Layer Source.
 *
 * Joystick device: Logitech Gamepad F310
 * Brake controls: left trigger
 * Throttle controls: right trigger
 * Steering controls: right stick
 * Left turn signal: left trigger button
 * Right turn signal: right trigger button
 * Clear turn signal: 'X' button
 * Shift to park: 'Y' button
 * Shift to drive: 'B' button
 * Enable controls: 'A' button
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_node.h"
#include "polysync_message.h"
#include "polysync_node_template.h"

#include "node.h"
#include "joystick.h"
#include "commander.h"




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
 * @brief Node data.
 *
 * Serves as a top-level container for the application's data structures.
 *
 */
typedef struct
{
    //
    //
    joystick_device_s joystick; /*!< Joystick handle. */
    //
    //
    ps_timestamp last_commander_update; /*!< Last commander update timestamp. [microseconds] */
    //
    //
    commander_messages_s messages; /*!< Commander message set. */
} node_data_s;




// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Platform brake command message type name.
 *
 */
static const char BRAKE_COMMAND_MSG_NAME[] = "ps_platform_brake_command_msg";


/**
 * @brief Platform throttle command message type name.
 *
 */
static const char THROTTLE_COMMAND_MSG_NAME[] = "ps_platform_throttle_command_msg";


/**
 * @brief Platform steering command message type name.
 *
 */
static const char STEERING_COMMAND_MSG_NAME[] = "ps_platform_steering_command_msg";


/**
 * @brief Gear position command message type name.
 *
 */
static const char GEAR_POSITION_COMMAND_MSG_NAME[] = "ps_platform_gear_command_msg";


/**
 * @brief Turn signal command message type name.
 *
 */
static const char TURN_SIGNAL_COMMAND_MSG_NAME[] = "ps_platform_turn_signal_command_msg";


/**
 * @brief Warning string.
 *
 */
static const char WARNING_STRING[] =
"\nWARNING: example is built for "
"the Joystick device: Logitech Gamepad F310\n"
"Brake controls: left trigger\n"
"Throttle controls: right trigger\n"
"Steering controls: right stick\n"
"Left turn signal: left trigger button\n"
"Right turn signal: right trigger button\n"
"Clear turn signal: 'X' button\n"
"Shift to park: 'Y' button\n"
"Shift to drive: 'B' button\n"
"Enable controls: 'A' button\n\n";




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


/**
 * @brief Commander update loop.
 *
 * Called by on_warn and on_ok.
 *
 * @param [in] node_ref Node reference.
 * @param [in] state A pointer to \ref ps_diagnostic_state which stores the current state of the node.
 * @param [in] node_data A pointer to \ref node_data_s which specifies the configuration.
 *
 */
static void update_loop(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        node_data_s * const node_data );




// *****************************************************
// static definitions
// *****************************************************

//
static int set_configuration(
        ps_node_configuration_data * const node_config )
{
    // local vars
    node_data_s *node_data = NULL;

    const char default_node_name[] = "joystick-commander";


    // show warning string
    printf( WARNING_STRING );
    
    // set defaults
    // node type
    node_config->node_type = PSYNC_NODE_TYPE_API_USER;

    // set node domain
    node_config->domain_id = PSYNC_DEFAULT_DOMAIN;

    // set node SDF key
    node_config->sdf_key = PSYNC_SDF_KEY_INVALID;

    // set node flags
    node_config->flags = NODE_FLAGS_VALUE | PSYNC_INIT_FLAG_STDOUT_LOGGING;

    // set user data
    node_config->user_data = NULL;

    // set node name
    memset( node_config->node_name, 0, sizeof(node_config->node_name) );
    strncpy( node_config->node_name, default_node_name, sizeof(node_config->node_name) );

    // create node data
    if( (node_data = malloc( sizeof(*node_data) )) == NULL )
    {
        psync_log_message( LOG_LEVEL_ERROR, "node : (%u) -- failed to create node data", __LINE__ );
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
        psync_log_message( LOG_LEVEL_ERROR,
                "node : (%u) -- invalid node context",
                __LINE__ );
        psync_node_activate_fault( node_ref, DTC_USAGE, NODE_STATE_FATAL );
        return;
    }

    // get brake command message type
    if( (ret = psync_message_get_type_by_name(
            node_ref,
            BRAKE_COMMAND_MSG_NAME,
            &msg_type )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get brake command message
    if( (ret = psync_message_alloc(
            node_ref,
            msg_type,
            (ps_msg_ref*) &node_data->messages.brake_cmd )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get throttle command message type
    if( (ret = psync_message_get_type_by_name(
            node_ref,
            THROTTLE_COMMAND_MSG_NAME,
            &msg_type )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get throttle command message
    if( (ret = psync_message_alloc(
            node_ref,
            msg_type,
            (ps_msg_ref*) &node_data->messages.throttle_cmd )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get steering command message type
    if( (ret = psync_message_get_type_by_name(
            node_ref,
            STEERING_COMMAND_MSG_NAME,
            &msg_type )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get steering command message
    if( (ret = psync_message_alloc(
            node_ref,
            msg_type,
            (ps_msg_ref*) &node_data->messages.steer_cmd )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get gear position message type
    if( (ret = psync_message_get_type_by_name(
            node_ref,
            GEAR_POSITION_COMMAND_MSG_NAME,
            &msg_type )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get gear position message
    if( (ret = psync_message_alloc(
            node_ref,
            msg_type,
            (ps_msg_ref*) &node_data->messages.gear_cmd )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get turn signal message type
    if( (ret = psync_message_get_type_by_name(
            node_ref,
            TURN_SIGNAL_COMMAND_MSG_NAME,
            &msg_type )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get turn signal message
    if( (ret = psync_message_alloc(
            node_ref,
            msg_type,
            (ps_msg_ref*) &node_data->messages.turn_signal_cmd )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // init joystick subsystem
    if( (ret = jstick_init_subsystem()) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // set commander safe state
    if( (ret = commander_set_safe(
            &node_data->messages )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        return;
    }

    // get number of joysticks visible
    const int num_joysticks = jstick_get_num_devices();

    // if any are available
    if( num_joysticks > 0 )
    {
        // device GUID
        joystick_guid_s js_guid;

        // get GUID of device at index
        if( (ret = jstick_get_guid_at_index(
                0,
                &js_guid )) != DTC_NONE )
        {
            psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
            return;
        }

        psync_log_message( LOG_LEVEL_DEBUG,
                "node : found %d devices -- connecting to device at system index %lu - GUID: %s",
                num_joysticks,
                0,
                js_guid.ascii_string );

        // connect to first device
        if( (ret = jstick_open( 0, &node_data->joystick )) != DTC_NONE )
        {
            psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
            return;
        }

        // wait for safe state
        psync_log_message( LOG_LEVEL_INFO,
                "node : wait for joystick controls to zero" );
        do
        {
            ret = commander_check_for_safe_joystick( &node_data->joystick );

            if( ret == DTC_UNAVAILABLE )
            {
                // wait a little for the next try
                (void) psync_sleep_micro( COMMANDER_UPDATE_INTERVAL );
            }
            else if( ret != DTC_NONE )
            {
                psync_log_message( LOG_LEVEL_ERROR,
                        "node : (%u) -- failed to wait for joystick to zero the control values",
                        __LINE__ );
                psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );

                // return now
                break;
            }
        }
        while( ret != DTC_NONE );
    }
    else
    {
        psync_log_message( LOG_LEVEL_ERROR,
                "node : (%u) -- no joystick/devices available on the host",
                __LINE__ );
        psync_node_activate_fault( node_ref, DTC_USAGE, NODE_STATE_FATAL );
    }
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
        // set commander safe state
        (void) commander_set_safe(
            &node_data->messages );

        // close device if needed
        jstick_close( &node_data->joystick );

        // free messages
        (void) psync_message_free(
                node_ref,
                (ps_msg_ref*) &node_data->messages.brake_cmd );
        (void) psync_message_free(
                node_ref,
                (ps_msg_ref*) &node_data->messages.throttle_cmd );
        (void) psync_message_free(
                node_ref,
                (ps_msg_ref*) &node_data->messages.steer_cmd );
        (void) psync_message_free(
                node_ref,
                (ps_msg_ref*) &node_data->messages.gear_cmd );
        (void) psync_message_free(
                node_ref,
                (ps_msg_ref*) &node_data->messages.turn_signal_cmd );

        // zero
        memset( node_data, 0, sizeof(*node_data) );

        // free
        free( node_data );
        node_data = NULL;
    }

    // release joystick subsystem
    jstick_release_subsystem();
}


//
static void on_error(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // cast node data
    node_data_s * const node_data = (node_data_s*) user_data;


    // if node data valid
    if( node_data != NULL )
    {
        // send e-stop
        (void) commander_send_estop(
                node_ref,
                &node_data->messages );
    }
}


//
static void on_fatal(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // cast node data
    node_data_s * const node_data = (node_data_s*) user_data;


    // if node data valid
    if( node_data != NULL )
    {
        // send e-stop
        (void) commander_send_estop(
                node_ref,
                &node_data->messages );
    }
}


//
static void on_warn(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // cast node data
    node_data_s * const node_data = (node_data_s*) user_data;


    // if node data not valid
    if( node_data == NULL )
    {
        // activate DTC
        psync_node_activate_fault( node_ref, DTC_NOINTERFACE, NODE_STATE_FATAL );

        // return now
        return;
    }

    // if messages not valid
    if(
            (node_data->messages.brake_cmd == PSYNC_MSG_REF_INVALID) ||
            (node_data->messages.throttle_cmd == PSYNC_MSG_REF_INVALID) ||
            (node_data->messages.steer_cmd == PSYNC_MSG_REF_INVALID) ||
            (node_data->messages.gear_cmd == PSYNC_MSG_REF_INVALID) ||
            (node_data->messages.turn_signal_cmd == PSYNC_MSG_REF_INVALID) )
    {
        // activate DTC
        psync_node_activate_fault( node_ref, DTC_NOINTERFACE, NODE_STATE_FATAL );

        // return now
        return;
    }

    // do update loop
    update_loop( node_ref, state, node_data );
}


//
static void on_ok(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        void * const user_data )
{
    // cast node data
    node_data_s * const node_data = (node_data_s*) user_data;


    // if node data not valid
    if( node_data == NULL )
    {
        // activate DTC
        psync_node_activate_fault( node_ref, DTC_NOINTERFACE, NODE_STATE_FATAL );

        // return now
        return;
    }

    // if messages not valid
    if(
            (node_data->messages.brake_cmd == PSYNC_MSG_REF_INVALID) ||
            (node_data->messages.throttle_cmd == PSYNC_MSG_REF_INVALID) ||
            (node_data->messages.steer_cmd == PSYNC_MSG_REF_INVALID) ||
            (node_data->messages.gear_cmd == PSYNC_MSG_REF_INVALID) ||
            (node_data->messages.turn_signal_cmd == PSYNC_MSG_REF_INVALID) )
    {
        // activate DTC
        psync_node_activate_fault( node_ref, DTC_NOINTERFACE, NODE_STATE_FATAL );

        // return now
        return;
    }

    // do update loop
    update_loop( node_ref, state, node_data );
}


//
static void update_loop(
        ps_node_ref const node_ref,
        const ps_diagnostic_state * const state,
        node_data_s * const node_data )
{
    // local vars
    int ret = DTC_NONE;
    ps_timestamp now = 0;


    // get current time
    if( (ret = psync_get_timestamp( &now )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        (void) commander_send_estop( node_ref, &node_data->messages );
        return;
    }

    // check for time to update
    if( (now - node_data->last_commander_update) >= COMMANDER_UPDATE_INTERVAL )
    {
        // update commander state using joystick
        if( (ret = commander_joystick_update(
                &node_data->joystick,
                &node_data->messages )) != DTC_NONE )
        {
            psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
            (void) commander_send_estop( node_ref, &node_data->messages );
            return;
        }

        // publish commander controls
        if( (ret = commander_send_commands(
                node_ref,
                &node_data->messages )) != DTC_NONE )
        {
            psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
            (void) commander_send_estop( node_ref, &node_data->messages );
            return;
        }

        // update last publish time
        node_data->last_commander_update = now;
    }

    // wait a little
    if( (ret = psync_sleep_micro( NODE_SLEEP_TICK_INTERVAL )) != DTC_NONE )
    {
        psync_node_activate_fault( node_ref, ret, NODE_STATE_FATAL );
        (void) commander_send_estop( node_ref, &node_data->messages );
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
