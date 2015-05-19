/******************************************************
** FILE: node.h
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
 * @example node.h
 *
 * Node Template Interface.
 *
 */


#ifndef NODE_H
#define	NODE_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>
#include <string.h>
#include <glib-2.0/glib.h>

// polysync
#include "polysync_core.h"

// node template interface
#include "polysync_node_template.h"


/**
 * @brief User's node data.
 *
 */
typedef struct
{
    //
    //
    GAsyncQueue                *received_message_queue; /**< Incoming message queue. */
    //
    //
    guint                       received_message_queue_enabled; /**< Flag indicating whether or not to enque incoming messages. */
    //
    //
    unsigned long               registered_message_types; /**< Bit flags set by command line args to indicate which message types to listen for. */
} node_data;




/**
 * @brief PolySync Message string table.
 *
 * Used for command line arguments.
 * table[ 0 ] -> PS_TOPIC_0 -> "PS_TRANSPORT_MSG"
 *
 */
extern const char               *MESSAGE_TYPE_STRING_TABLE[];





/**
 * @brief Register message listeners.
 *
 * Uses bit flags to indicate which message types to register listeners for.
 *
 * If bit at index (1 << message_type) is set, a listener is registered for that
 * type. All messages get placed into a common message queue.
 *
 * @param context Pointer to the node's context data.
 * @param enabled_message_types Bit flags indicating which message types to listen for.
 *
 * @return Error code, DTC_RET( DTC_NONE ) if success.
 *
 */
int node_register_message_listeners( ps_node_context *context, unsigned long enabled_message_types );


/**
 * @brief Poll the message queue.
 *
 * Returns immediately, message_ptr will be non-zero if message exist.
 * User must release message.
 *
 * @param context Pointer to the node's context data.
 * @param message_ptr Pointer to message pointer, set if message exist.
 *
 * @return Error code, DTC_RET( DTC_NONE ) if success.
 *
 */
int node_poll_for_message( ps_node_context *context, void **message_ptr );


/**
 * @brief Wait for message.
 *
 * Returns after message is available or timeout reached, message_ptr will be non-zero if message exist.
 * User must release message.
 *
 * @param context Pointer to the node's context data.
 * @param message_ptr Pointer to message pointer, set if message exist.
 * @param timeout Timeout in microseconds.
 *
 * @return Error code, DTC_RET( DTC_NONE ) if success.
 *
 */
int node_wait_for_message( ps_node_context *context, void **message_ptr, ps_timestamp timeout );


/**
 * @brief Free up any resources used by the user data field.
 *
 * @param configuration Pointer to the node's context.configuration_data.
 *
 * @return Error code, DTC_RET( DTC_NONE ) if success.
 *
 */
int node_release_user_data( ps_node_configuration_data *configuration );


/**
 * @brief Create node data resources used by the user data field.
 *
 * @param configuration Pointer to the node's context.configuration_data.
 *
 * @return Error code, DTC_RET( DTC_NONE ) if success.
 *
 */
int node_create_user_data( ps_node_configuration_data *configuration );


#endif	/* NODE_H */

