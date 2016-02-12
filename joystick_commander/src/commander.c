/**
 * @file commander.c
 * @brief Commander Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>

#include "polysync_core.h"
#include "polysync_message.h"
#include "polysync_node.h"
#include "joystick.h"
#include "commander.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief Throttle axis index.
 *
 */
#define JSTICK_AXIS_THROTTLE (5)


/**
 * @brief Brake axis index.
 *
 */
#define JSTICK_AXIS_BRAKE (2)


/**
 * @brief Steering axis index.
 *
 */
#define JSTICK_AXIS_STEER (3)


/**
 * @brief Turn signal disable button index.
 *
 */
#define JSTICK_BUTTON_DISABLE_TURN_SIGNAL (2)


/**
 * @brief Left turn signal button index.
 *
 */
#define JSTICK_BUTTON_LEFT_TURN_SIGNAL (4)


/**
 * @brief Right turn signal button index.
 *
 */
#define JSTICK_BUTTON_RIGHT_TURN_SIGNAL (5)


/**
 * @brief Honk horn button index.
 *
 */
#define JSTICK_BUTTON_HONK_HORN (0)


/**
 * @brief Shift to park gear button index.
 *
 */
#define JSTICK_BUTTON_GEAR_SHIFT_PARK (3)


/**
 * @brief Shift to drive gear button index.
 *
 */
#define JSTICK_BUTTON_GEAR_SHIFT_DRIVE (1)


/**
 * @brief Convert degrees to radians.
 *
 */
#define m_radians(deg) ((deg)*(M_PI/180.0))


/**
 * @brief Convert radians to degrees.
 *
 */
#define m_degrees(rad) ((rad)*(180.0/M_PI))


/**
 * @brief Absolute value.
 *
 */
#define m_abs(x) ((x)>0?(x):-(x))




// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief Update speed setpoint.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the joystick handle.
 * @param [out] speed A pointer to double which receives the setpoint value. [meters/second]
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int update_speed_setpoint(
        joystick_device_s * const jstick,
        double * const speed );


/**
 * @brief Update curvature setpoint.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the joystick handle.
 * @param [out] speed A pointer to double which receives the setpoint value. [1/meters]
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int update_curvature_setpoint(
        joystick_device_s * const jstick,
        double * const curvature );


/**
 * @brief Update gear selection.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the joystick handle.
 * @param [out] gear_position A pointer to \ref ps_gear_position_kind which receives the desired gear position command.
 * Value \ref GEAR_POSITION_INVALID means no command.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int update_gear_selection(
        joystick_device_s * const jstick,
        ps_gear_position_kind * const gear_position );


/**
 * @brief Update turn signal selection.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the joystick handle.
 * @param [out] turn_signal A pointer to \ref ps_platform_turn_signal_kind which receives the desired turn signal command.
 * Value \ref PLATFORM_TURN_SIGNAL_INVALID means no command.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int update_turn_signal_selection(
        joystick_device_s * const jstick,
        ps_platform_turn_signal_kind * const turn_signal );


/**
 * @brief Set control command message 'ps_platform_control_msg' to its safe state.
 *
 * @param [in] control_msg A pointer to \ref ps_platform_control_msg which receives the safe state values.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int set_safe_platform_control_msg(
        ps_platform_control_msg * const control_msg );


/**
 * @brief Set control command message 'ps_platform_gear_command_msg' to its safe state.
 *
 * @param [in] control_msg A pointer to \ref ps_platform_gear_command_msg which receives the safe state values.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int set_safe_platform_gear_command_msg(
        ps_platform_gear_command_msg * const gear_msg );




// *****************************************************
// static definitions
// *****************************************************

//
static int update_speed_setpoint(
        joystick_device_s * const jstick,
        double * const speed )
{
    // local vars
    int ret = DTC_NONE;
    int axis_position = 0;


    // read axis position
    ret = jstick_get_axis(
            jstick,
            JSTICK_AXIS_THROTTLE,
            &axis_position );

    // if succeeded
    if( ret == DTC_NONE )
    {
        // set speed set point - scale to 0:max
        (*speed) = jstick_normalize_axis_position(
                axis_position,
                0.0,
                MAX_SPEED );
    }


    return ret;
}


//
static int update_curvature_setpoint(
        joystick_device_s * const jstick,
        double * const curvature )
{
    // local vars
    int ret = DTC_NONE;
    int axis_position = 0;


    // read axis position
    ret = jstick_get_axis(
            jstick,
            JSTICK_AXIS_STEER,
            &axis_position );

    // if succeeded
    if( ret == DTC_NONE )
    {
        // set curvature set point - scale to max:min
        // note that this is inverting the sign of the joystick axis
        (*curvature) = jstick_normalize_axis_position(
                axis_position,
                MAX_CURVATURE,
                MIN_CURVATURE );
    }


    return ret;
}


//
static int update_gear_selection(
        joystick_device_s * const jstick,
        ps_gear_position_kind * const gear_position )
{
    // local vars
    int ret = DTC_NONE;
    unsigned int btn_state = JOYSTICK_BUTTON_STATE_NOT_PRESSED;

    // invalidate
    (*gear_position) = GEAR_POSITION_INVALID;

    // read shift-to-drive button state
    ret |= jstick_get_button(
            jstick,
            JSTICK_BUTTON_GEAR_SHIFT_DRIVE,
            &btn_state );

    // if succeeded
    if( ret == DTC_NONE )
    {
        // if button depressed
        if( btn_state == JOYSTICK_BUTTON_STATE_PRESSED )
        {
            // set gear position command to drive
            (*gear_position) = GEAR_POSITION_DRIVE;
        }
    }

    // read shift-to-park button state
    ret |= jstick_get_button(
            jstick,
            JSTICK_BUTTON_GEAR_SHIFT_PARK,
            &btn_state );

    // if succeeded
    if( ret == DTC_NONE )
    {
        // if button depressed
        if( btn_state == JOYSTICK_BUTTON_STATE_PRESSED )
        {
            // set gear position command to park
            // note that this takes precedence over drive gear command
            (*gear_position) = GEAR_POSITION_PARK;
        }
    }

    // set return if anything failed
    if( ret != DTC_NONE )
    {
        // joystick get routine failed
        ret = DTC_IOERR;
    }


    return ret;
}


//
static int update_turn_signal_selection(
        joystick_device_s * const jstick,
        ps_platform_turn_signal_kind * const turn_signal )
{
    // local vars
    int ret = DTC_NONE;
    unsigned int btn_state = JOYSTICK_BUTTON_STATE_NOT_PRESSED;


    // invalidate
    (*turn_signal) = PLATFORM_TURN_SIGNAL_INVALID;

    // read left-signal button state
    ret |= jstick_get_button(
            jstick,
            JSTICK_BUTTON_LEFT_TURN_SIGNAL,
            &btn_state );

    // if succeeded
    if( ret == DTC_NONE )
    {
        // if button depressed
        if( btn_state == JOYSTICK_BUTTON_STATE_PRESSED )
        {
            // set turn signal command to left
            (*turn_signal) = PLATFORM_TURN_SIGNAL_LEFT;
        }
    }

    // read right-signal button state
    ret |= jstick_get_button(
            jstick,
            JSTICK_BUTTON_RIGHT_TURN_SIGNAL,
            &btn_state );

    // if succeeded
    if( ret == DTC_NONE )
    {
        // if button depressed
        if( btn_state == JOYSTICK_BUTTON_STATE_PRESSED )
        {
            // set turn signal command to right
            // note that this takes precedence over left signal command
            (*turn_signal) = PLATFORM_TURN_SIGNAL_RIGHT;
        }
    }

    // read clear-turn-signal button state
    ret |= jstick_get_button(
            jstick,
            JSTICK_BUTTON_DISABLE_TURN_SIGNAL,
            &btn_state );

    // if succeeded
    if( ret == DTC_NONE )
    {
        // if button depressed
        if( btn_state == JOYSTICK_BUTTON_STATE_PRESSED )
        {
            // set turn signal command to none
            // note that this takes precedence over left/right signal command
            (*turn_signal) = PLATFORM_TURN_SIGNAL_NONE;
        }
    }

    // set return if anything failed
    if( ret != DTC_NONE )
    {
        // joystick get routine failed
        ret = DTC_IOERR;
    }


    return ret;
}


//
static int set_safe_platform_control_msg(
        ps_platform_control_msg * const control_msg )
{
    // local vars
    int ret = DTC_NONE;


    // if message valid
    if( control_msg != NULL )
    {
        // clear destination GUID
        control_msg->dest_guid = PSYNC_GUID_INVALID;

        // clear e-stop
        control_msg->e_stop = PSYNC_EMERGENCY_STOP_DISABLED;

        // zero speed
        control_msg->speed = (DDS_float) 0.0;

        // set acceleration limit
        control_msg->acceleration_limit = (DDS_float) ACCELERATION_LIMIT;

        // set acceleration limit
        control_msg->decceleration_limit = (DDS_float) DECELERATION_LIMIT;

        // zero curvature
        control_msg->curvature = (DDS_float) 0.0;

        // set max curvature rate
        control_msg->max_curvature_rate = (DDS_float) CURVATURE_RATE_LIMIT;

        // update timestamp
        ret = psync_get_timestamp( &control_msg->timestamp );
    }


    return ret;
}


//
static int set_safe_platform_gear_command_msg(
        ps_platform_gear_command_msg * const gear_msg )
{
    // local vars
    int ret = DTC_NONE;


    // if message valid
    if( gear_msg != NULL )
    {
        // clear destination GUID
        gear_msg->dest_guid = PSYNC_GUID_INVALID;

        // clear e-stop
        gear_msg->e_stop = PSYNC_EMERGENCY_STOP_DISABLED;

        // invalidate command
        gear_msg->gear_position = GEAR_POSITION_INVALID;

        // update timestamp
        ret = psync_get_timestamp( &gear_msg->timestamp );
    }


    return ret;
}


//
static int set_safe_platform_turn_signal_msg(
        ps_platform_turn_signal_command_msg * const turn_signal_msg )
{
    // local vars
    int ret = DTC_NONE;


    // if message valid
    if( turn_signal_msg != NULL )
    {
        // clear destination GUID
        turn_signal_msg->dest_guid = PSYNC_GUID_INVALID;

        // clear e-stop
        turn_signal_msg->e_stop = PSYNC_EMERGENCY_STOP_DISABLED;

        // invalidate command
        turn_signal_msg->turn_signal = PLATFORM_TURN_SIGNAL_INVALID;

        // update timestamp
        ret = psync_get_timestamp( &turn_signal_msg->timestamp );
    }


    return ret;
}




// *****************************************************
// public definitions
// *****************************************************

//
int commander_set_safe(
        commander_messages_s * const messages )
{
    // local vars
    int ret = DTC_NONE;


    // if control message valid
    if( messages->control_msg != NULL )
    {
        // set safe state
        ret |= set_safe_platform_control_msg( messages->control_msg );
    }

    // if gear command message valid
    if( messages->gear_msg != NULL )
    {
        // set safe state
        ret |= set_safe_platform_gear_command_msg( messages->gear_msg );
    }

    // if turn signal command message valid
    if( messages->turn_signal_msg != NULL )
    {
        // set safe state
        ret |= set_safe_platform_turn_signal_msg( messages->turn_signal_msg );
    }

    // handle DTC
    if( ret != DTC_NONE )
    {
        // treat as a configuration error
        ret = DTC_CONFIG;
    }


    return ret;
}


//
int commander_send_estop(
        ps_node_ref node_ref,
        commander_messages_s * const messages )
{
    // local vars
    int ret = DTC_NONE;


    // start from safe state
    ret |= commander_set_safe( messages );

    // enable e-stop
    messages->control_msg->e_stop = 1;

    // update publish timestamp
    ret |= psync_get_timestamp( &messages->control_msg->header.timestamp );

    // publish
    ret |= psync_message_publish( node_ref, messages->control_msg );

    // handle return if anything failed
    if( ret != DTC_NONE )
    {
        // return protocol DTC
        ret = DTC_PROTOCOL;
    }


    return ret;
}


//
int commander_joystick_update(
        joystick_device_s * const jstick,
        commander_messages_s * const messages )
{
    // local vars
    int ret = DTC_NONE;
    double speed_setpoint = 0.0;
    double curvature_setpoint = 0.0;
    ps_gear_position_kind gear_position = GEAR_POSITION_INVALID;
    ps_platform_turn_signal_kind turn_signal = PLATFORM_TURN_SIGNAL_INVALID;


    // update joystick readings
    jstick_update( jstick );

    // start from safe state
    ret |= commander_set_safe( messages );

    // update sample timestamp
    ret |= psync_get_timestamp( &messages->control_msg->timestamp );

    // copy to other command messages
    messages->gear_msg->timestamp = messages->control_msg->timestamp;
    messages->turn_signal_msg->timestamp = messages->control_msg->timestamp;

    // update speed set point
    ret |= update_speed_setpoint( jstick, &speed_setpoint );

    // update steering curvature set point
    ret |= update_curvature_setpoint( jstick, &curvature_setpoint );

    // update gear selection
    ret |= update_gear_selection( jstick, &gear_position );

    // update turn signal selection
    ret |= update_turn_signal_selection( jstick, &turn_signal );

    // handle return if anything failed
    if( ret == DTC_NONE )
    {
        // success

        // set message data
        messages->control_msg->speed = (DDS_float) speed_setpoint;
        messages->control_msg->curvature = (DDS_float) curvature_setpoint;
        messages->gear_msg->gear_position = gear_position;
        messages->turn_signal_msg->turn_signal = turn_signal;
    }
    else
    {
        // return protocol DTC
        ret = DTC_PROTOCOL;
    }


    return ret;
}


//
int commander_send_commands(
        ps_node_ref node_ref,
        commander_messages_s * const messages )
{
    // local vars
    int ret = DTC_NONE;
    ps_timestamp now = 0;


    // update current timestamp
    ret |= psync_get_timestamp( &now );

    // update control message publish timestamp
    messages->control_msg->header.timestamp = now;

    // publish control message
    ret |= psync_message_publish( node_ref, messages->control_msg );

    // if gear position command set
    if( messages->gear_msg->gear_position != GEAR_POSITION_INVALID )
    {
        // update gear position command message publish timestamp
        messages->gear_msg->header.timestamp = now;

        // publish gear position command message
        ret |= psync_message_publish( node_ref, messages->gear_msg );
    }

    // if turn signal command set
    if( messages->turn_signal_msg->turn_signal != PLATFORM_TURN_SIGNAL_INVALID )
    {
        // update turn signal command message publish timestamp
        messages->turn_signal_msg->header.timestamp = now;

        // publish turn signal command message
        ret |= psync_message_publish( node_ref, messages->turn_signal_msg );
    }

    // handle return if anything failed
    if( ret != DTC_NONE )
    {
        // return protocol DTC
        ret = DTC_PROTOCOL;
    }


    return ret;
}
