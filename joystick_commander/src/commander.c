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
 * @brief Update brake pedal setpoint.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the joystick handle.
 * @param [out] speed A pointer to double which receives the setpoint value. [normalized]
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int update_brake_setpoint(
        joystick_device_s * const jstick,
        double * const brake );


/**
 * @brief Update throttle pedal setpoint.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the joystick handle.
 * @param [out] speed A pointer to double which receives the setpoint value. [normalized]
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int update_throttle_setpoint(
        joystick_device_s * const jstick,
        double * const throttle );


/**
 * @brief Update steering wheel angle setpoint.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the joystick handle.
 * @param [out] speed A pointer to double which receives the setpoint value. [radians]
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int update_steering_setpoint(
        joystick_device_s * const jstick,
        double * const angle );


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
 * @brief Set control command message 'ps_platform_brake_command_msg' to its safe state.
 *
 * @param [in] control_msg A pointer to \ref ps_platform_brake_command_msg which receives the safe state values.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int set_safe_brake_command_msg(
        ps_platform_brake_command_msg * const brake_msg );


/**
 * @brief Set control command message 'ps_platform_throttle_command_msg' to its safe state.
 *
 * @param [in] control_msg A pointer to \ref ps_platform_throttle_command_msg which receives the safe state values.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int set_safe_throttle_command_msg(
        ps_platform_throttle_command_msg * const throttle_msg );


/**
 * @brief Set control command message 'ps_platform_steering_command_msg' to its safe state.
 *
 * @param [in] control_msg A pointer to \ref ps_platform_steering_command_msg which receives the safe state values.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int set_safe_steering_command_msg(
        ps_platform_steering_command_msg * const steering_msg );


/**
 * @brief Set control command message 'ps_platform_gear_command_msg' to its safe state.
 *
 * @param [in] control_msg A pointer to \ref ps_platform_gear_command_msg which receives the safe state values.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int set_safe_gear_command_msg(
        ps_platform_gear_command_msg * const gear_msg );


/**
 * @brief Set E-Stop flag in all commander messages.
 *
 * @param [in] messages A pointer to \ref commander_messages_s which specifies the message set.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
static int set_estop(
        commander_messages_s * const messages );




// *****************************************************
// static definitions
// *****************************************************

//
static int update_brake_setpoint(
        joystick_device_s * const jstick,
        double * const brake )
{
    // local vars
    int ret = DTC_NONE;
    int axis_position = 0;


    // read axis position
    ret = jstick_get_axis(
            jstick,
            JSTICK_AXIS_BRAKE,
            &axis_position );

    // if succeeded
    if( ret == DTC_NONE )
    {
        // set brake set point - scale to 0:max
        (*brake) = jstick_normalize_axis_position(
                axis_position,
                0.0,
                MAX_BRAKE_PEDAL );
    }


    return ret;
}


//
static int update_throttle_setpoint(
        joystick_device_s * const jstick,
        double * const throttle )
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
        // set throttle set point - scale to 0:max
        (*throttle) = jstick_normalize_axis_position(
                axis_position,
                0.0,
                MAX_THROTTLE_PEDAL );
    }


    return ret;
}


//
static int update_steering_setpoint(
        joystick_device_s * const jstick,
        double * const angle )
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
        // set steering wheel angle set point - scale to max:min
        // note that this is inverting the sign of the joystick axis
        (*angle) = jstick_normalize_axis_position(
                axis_position,
                MAX_STEERING_WHEEL_ANGLE,
                MIN_STEERING_WHEEL_ANGLE );
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
static int set_safe_brake_command_msg(
        ps_platform_brake_command_msg * const brake_msg )
{
    // local vars
    int ret = DTC_NONE;


    // if message valid
    if( brake_msg != NULL )
    {
        // clear destination GUID
        brake_msg->dest_guid = PSYNC_GUID_INVALID;

        // clear e-stop
        brake_msg->e_stop = PSYNC_EMERGENCY_STOP_DISABLED;

        // disable
        brake_msg->enabled = 0;

        // BOO disabled
        brake_msg->boo_enabled = 0;

        // invalidate
        brake_msg->brake_command_type = BRAKE_COMMAND_INVALID;

        // zero
        brake_msg->brake_command = (DDS_float) 0.0f;

        // update timestamp
        ret = psync_get_timestamp( &brake_msg->timestamp );
    }


    return ret;
}


//
static int set_safe_throttle_command_msg(
        ps_platform_throttle_command_msg * const throttle_msg )
{
    // local vars
    int ret = DTC_NONE;


    // if message valid
    if( throttle_msg != NULL )
    {
        // clear destination GUID
        throttle_msg->dest_guid = PSYNC_GUID_INVALID;

        // clear e-stop
        throttle_msg->e_stop = PSYNC_EMERGENCY_STOP_DISABLED;

        // disable
        throttle_msg->enabled = 0;

        // invalidate
        throttle_msg->throttle_command_type = THROTTLE_COMMAND_INVALID;

        // zero
        throttle_msg->throttle_command = (DDS_float) 0.0f;

        // update timestamp
        ret = psync_get_timestamp( &throttle_msg->timestamp );
    }


    return ret;
}


//
static int set_safe_steering_command_msg(
        ps_platform_steering_command_msg * const steering_msg )
{
    // local vars
    int ret = DTC_NONE;


    // if message valid
    if( steering_msg != NULL )
    {
        // clear destination GUID
        steering_msg->dest_guid = PSYNC_GUID_INVALID;

        // clear e-stop
        steering_msg->e_stop = PSYNC_EMERGENCY_STOP_DISABLED;

        // disable
        steering_msg->enabled = 0;

        // invalidate
        steering_msg->steering_command_kind = STEERING_COMMAND_INVALID;

        // zero
        steering_msg->steering_wheel_angle = (DDS_float) 0.0f;

        // zero
        steering_msg->max_steering_wheel_rotation_rate = (DDS_float) 0.0f;

        // update timestamp
        ret = psync_get_timestamp( &steering_msg->timestamp );
    }


    return ret;
}


//
static int set_safe_gear_command_msg(
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
static int set_safe_turn_signal_msg(
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


//
static int set_estop(
        commander_messages_s * const messages )
{
    // local vars
    int ret = DTC_NONE;


    // if brake command message valid
    if( messages->brake_cmd != NULL )
    {
        // set estop
        messages->brake_cmd->e_stop = 1;
    }

    // if throttle command message valid
    if( messages->throttle_cmd != NULL )
    {
        // set estop
        messages->throttle_cmd->e_stop = 1;
    }

    // if steering command message valid
    if( messages->steer_cmd != NULL )
    {
        // set estop
        messages->steer_cmd->e_stop = 1;
    }

    // if gear command message valid
    if( messages->gear_cmd != NULL )
    {
        // set estop
        messages->gear_cmd->e_stop = 1;
    }

    // if turn signal command message valid
    if( messages->turn_signal_cmd != NULL )
    {
        // set estop
        messages->turn_signal_cmd->e_stop = 1;
    }

    // handle DTC
    if( ret != DTC_NONE )
    {
        // treat as a configuration error
        ret = DTC_CONFIG;
    }


    return ret;
}




// *****************************************************
// public definitions
// *****************************************************

//
int commander_check_for_safe_joystick(
        joystick_device_s * const jstick )
{
    // local vars
    int ret = DTC_NONE;
    double brake_setpoint = 0.0;
    double throttle_setpoint = 0.0;


    // update joystick readings
    ret |= jstick_update( jstick );

    // update brake set point
    ret |= update_brake_setpoint( jstick, &brake_setpoint );

    // update throttle set point
    ret |= update_throttle_setpoint( jstick, &throttle_setpoint );

    // handle DTC
    if( ret != DTC_NONE )
    {
        // configuration error
        ret = DTC_CONFIG;
    }

    // if succeeded
    if( ret == DTC_NONE )
    {
        // if throttle not zero
        if( throttle_setpoint > 0.0 )
        {
            // invalidate
            ret = DTC_UNAVAILABLE;
        }

        // if brake not zero
        if( brake_setpoint > 0.0 )
        {
            // invalidate
            ret = DTC_UNAVAILABLE;
        }
    }


    return ret;
}


//
int commander_set_safe(
        commander_messages_s * const messages )
{
    // local vars
    int ret = DTC_NONE;


    // if brake command message valid
    if( messages->brake_cmd != NULL )
    {
        // set safe state
        ret |= set_safe_brake_command_msg( messages->brake_cmd );
    }

    // if throttle command message valid
    if( messages->throttle_cmd != NULL )
    {
        // set safe state
        ret |= set_safe_throttle_command_msg( messages->throttle_cmd );
    }

    // if steering command message valid
    if( messages->steer_cmd != NULL )
    {
        // set safe state
        ret |= set_safe_steering_command_msg( messages->steer_cmd );
    }

    // if gear command message valid
    if( messages->gear_cmd != NULL )
    {
        // set safe state
        ret |= set_safe_gear_command_msg( messages->gear_cmd );
    }

    // if turn signal command message valid
    if( messages->turn_signal_cmd != NULL )
    {
        // set safe state
        ret |= set_safe_turn_signal_msg( messages->turn_signal_cmd );
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


    // start with safe state
    ret |= commander_set_safe( messages );

    // enable e-stop
    ret |= set_estop( messages );

    // if throttle message valid
    if( messages->throttle_cmd != NULL )
    {
        // update publish timestamp
        ret |= psync_get_timestamp(
                &messages->throttle_cmd->header.timestamp );

        // publish
        ret |= psync_message_publish(
                node_ref,
                messages->throttle_cmd );
    }

    // if brake message valid
    if( messages->brake_cmd != NULL )
    {
        // update publish timestamp
        ret |= psync_get_timestamp(
                &messages->brake_cmd->header.timestamp );

        // publish
        ret |= psync_message_publish(
                node_ref,
                messages->brake_cmd );
    }

    // if steering message valid
    if( messages->steer_cmd != NULL )
    {
        // update publish timestamp
        ret |= psync_get_timestamp(
                &messages->steer_cmd->header.timestamp );

        // publish
        ret |= psync_message_publish(
                node_ref,
                messages->steer_cmd );
    }

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
    double brake_setpoint = 0.0;
    double throttle_setpoint = 0.0;
    double steering_setpoint = 0.0;
    ps_gear_position_kind gear_position = GEAR_POSITION_INVALID;
    ps_platform_turn_signal_kind turn_signal = PLATFORM_TURN_SIGNAL_INVALID;


    // update joystick readings
    ret |= jstick_update( jstick );

    // start from safe state
    ret |= commander_set_safe( messages );

    // update set points if succeeded
    if( ret == DTC_NONE )
    {
        // update brake set point
        ret |= update_brake_setpoint( jstick, &brake_setpoint );

        // update throttle set point
        ret |= update_throttle_setpoint( jstick, &throttle_setpoint );

        // update steering wheel angle set point
        ret |= update_steering_setpoint( jstick, &steering_setpoint );

        // update gear selection
        ret |= update_gear_selection( jstick, &gear_position );

        // update turn signal selection
        ret |= update_turn_signal_selection( jstick, &turn_signal );
    }

    // don't allow throttle if gear command is to be sent
    if( gear_position != GEAR_POSITION_INVALID )
    {
        throttle_setpoint = 0.0;
    }

    // don't allow throttle if brakes are applied
    if( brake_setpoint > BRAKES_ENABLED_MIN )
    {
        throttle_setpoint = 0.0;
    }

    // copy into command messages if succeeded
    if( ret == DTC_NONE )
    {
        // validate control kinds
        messages->brake_cmd->brake_command_type = BRAKE_COMMAND_PEDAL;
        messages->throttle_cmd->throttle_command_type = THROTTLE_COMMAND_PEDAL;
        messages->steer_cmd->steering_command_kind = STEERING_COMMAND_ANGLE;

        // enable controls
        messages->brake_cmd->enabled = 1;
        messages->throttle_cmd->enabled = 1;
        messages->steer_cmd->enabled = 1;

        // copy set points
        messages->brake_cmd->brake_command = (DDS_float) brake_setpoint;
        messages->throttle_cmd->throttle_command = (DDS_float) throttle_setpoint;
        messages->steer_cmd->steering_wheel_angle = (DDS_float) steering_setpoint;

        // steering rate limit
        messages->steer_cmd->max_steering_wheel_rotation_rate = (DDS_float) STEERING_WHEEL_ANGLE_RATE_LIMIT;

        // copy state commands
        messages->gear_cmd->gear_position = gear_position;
        messages->turn_signal_cmd->turn_signal = turn_signal;
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

    // set publish timestamp
    messages->brake_cmd->header.timestamp = now;
    messages->throttle_cmd->header.timestamp = now;
    messages->steer_cmd->header.timestamp = now;
    messages->gear_cmd->header.timestamp = now;
    messages->turn_signal_cmd->header.timestamp = now;

    // publish brake command message
    ret |= psync_message_publish( node_ref, messages->brake_cmd );

    // publish throttle command message
    ret |= psync_message_publish( node_ref, messages->throttle_cmd );

    // publish steering command message
    ret |= psync_message_publish( node_ref, messages->steer_cmd );

    // if gear position command set
    if( messages->gear_cmd->gear_position != GEAR_POSITION_INVALID )
    {
        // publish gear position command message
        ret |= psync_message_publish( node_ref, messages->gear_cmd );
    }

    // if turn signal command set
    if( messages->turn_signal_cmd->turn_signal != PLATFORM_TURN_SIGNAL_INVALID )
    {
        // publish turn signal command message
        ret |= psync_message_publish( node_ref, messages->turn_signal_cmd );
    }

    // handle return if anything failed
    if( ret != DTC_NONE )
    {
        // return protocol DTC
        ret = DTC_PROTOCOL;
    }


    return ret;
}
