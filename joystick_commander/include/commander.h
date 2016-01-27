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
 * @brief Minimum allowed steering curvature value. [1/meters]
 *
 * 0.05 [1/meters] ==> road radius of 20 [meters]
 *
 */
#define MIN_CURVATURE (-0.05)


/**
 * @brief Maximum allowed steering curvature value. [1/meters]
 *
 * 0.05 [1/meters] ==> road radius of 20 [meters]
 *
 */
#define MAX_CURVATURE (0.05)


/**
 * @brief Maximum allowed absolute steering curvature rate value. [1/meters^2]
 *
 */
#define CURVATURE_RATE_LIMIT (0.0001)


/**
 * @brief Maximum allowed speed value. [meters/second]
 *
 */
#define MAX_SPEED (5.0)


/**
 * @brief Maximum allowed acceleration value. [meters/second^2]
 *
 */
#define ACCELERATION_LIMIT (2.0)


/**
 * @brief Maximum allowed deceleration value. [meters/second^2]
 *
 */
#define DECELERATION_LIMIT (0.0)


/**
 * @brief Commander update interval. [microseconds]
 *
 */
#define COMMANDER_UPDATE_INTERVAL (20000)



/**
 * @brief Commander message set.
 *
 */
typedef struct
{
    //
    //
    ps_platform_control_msg *control_msg; /*!< Platform control message. */
    //
    //
    ps_platform_gear_command_msg *gear_msg; /*!< Platform gear command message. */
    //
    //
    ps_platform_turn_signal_command_msg *turn_signal_msg; /*!< Platform turn signal command message. */
} commander_messages_s;



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
