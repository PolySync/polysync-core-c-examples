/**
 * @file commander.h
 * @brief Commander Interface.
 *
 */




#ifndef COMMANDER_H
#define COMMANDER_H




#include "polysync_core.h"
#include "polysync_node_ref.h"

#include "joystick.h"




/**
 * @brief Maximum allowed throttle pedal position value. [normalized]
 *
 */
#define MAX_THROTTLE_PEDAL (0.3)


/**
 * @brief Maximum allowed brake pedal position value. [normalized]
 *
 */
#define MAX_BRAKE_PEDAL (0.8)


/**
 * @brief Minimum brake value to be considered enabled. [normalized]
 *
 * Throttle is disabled when brake value is greate than this value.
 *
 */
#define BRAKES_ENABLED_MIN (0.05)


/**
 * @brief Minimum allowed steering wheel angle value. [radians]
 *
 * Negative value means turning to the right.
 *
 */
#define MIN_STEERING_WHEEL_ANGLE (-M_PI * 2.0)


/**
 * @brief Maximum allowed steering wheel angle value. [radians]
 *
 * Positive value means turning to the left.
 *
 */
#define MAX_STEERING_WHEEL_ANGLE (M_PI * 2.0)


/**
 * @brief Maximum allowed absolute steering wheel angle rate value. [radians/second]
 *
 */
#define STEERING_WHEEL_ANGLE_RATE_LIMIT (M_PI_2)


/**
 * @brief Commander update interval. [microseconds]
 *
 */
#define COMMANDER_UPDATE_INTERVAL (50000)



/**
 * @brief Commander message set.
 *
 */
typedef struct
{
    //
    //
    ps_platform_brake_command_msg *brake_cmd; /*!< Platform brake command message. */
    //
    //
    ps_platform_throttle_command_msg *throttle_cmd; /*!< Platform throttle command message. */
    //
    //
    ps_platform_steering_command_msg *steer_cmd; /*!< Platform steering wheel command message. */
    //
    //
    ps_platform_gear_command_msg *gear_cmd; /*!< Platform gear command message. */
    //
    //
    ps_platform_turn_signal_command_msg *turn_signal_cmd; /*!< Platform turn signal command message. */
    //
    //
    ps_parameters_msg *parameters_cmd; /*!< Parameter(s) get/set command message. */
    //
    //
    unsigned int send_enable_controls; /*!< Flag indicating whether or not to send a enable-controls get/set command. */
    //
    //
    unsigned int send_disable_controls; /*!< Flag indicating whether or not to send a disable-controls get/set command. */
} commander_messages_s;




/**
 * @brief Wait for joystick throttle/brake values to be zero.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the joystick handle.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if joystick values safe.
 * \li \ref DTC_CONFIG if configuration invalid.
 * \li \ref DTC_UNAVAILABLE if joystick values NOT safe.
 *
 */
int commander_check_for_safe_joystick(
        joystick_device_s * const jstick );


/**
 * @brief Set control command messages to their safe state.
 *
 * @param [in] messages A pointer to \ref commander_messages_s which receives the safe state configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
int commander_set_safe(
        commander_messages_s * const messages );


/**
 * @brief Send E-Stop command.
 *
 * @param [in] node_ref Node reference.
 * @param [in] messages A pointer to \ref commander_messages_s which specifies the message set.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
int commander_send_estop(
        ps_node_ref node_ref,
        commander_messages_s * const messages );


/**
 * @brief Update commander.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the joystick handle.
 * @param [in] messages A pointer to \ref commander_messages_s which specifies the message set.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
int commander_joystick_update(
        joystick_device_s * const jstick,
        commander_messages_s * const messages );


/**
 * @brief Send commander commands.
 *
 * @param [in] node_ref Node reference.
 * @param [in] messages A pointer to \ref commander_messages_s which specifies the message set.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
int commander_send_commands(
        ps_node_ref node_ref,
        commander_messages_s * const messages );




#endif	/* COMMANDER_H */
