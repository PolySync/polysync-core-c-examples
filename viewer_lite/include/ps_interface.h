/**
 * @file ps_interface.h
 * @brief PolySync Interface.
 *
 */




#ifndef PS_INTERFACE_H
#define	PS_INTERFACE_H




#include <glib-2.0/glib.h>

#include "polysync_core.h"
#include "gui.h"




/**
 * @brief Node name string.
 *
 */
extern const char       PS_NODE_NAME[];




/**
 * @brief Initialize PolySync resources.
 *
 * Sets up data handler(s) and data queue for incoming messages.
 *
 * @return A newly created PolySync message queue on success, NULL on failure.
 *
 */
GAsyncQueue *init_polysync( void );


/**
 * @brief Release PolySync resources.
 *
 * Free's any messages in the data queue.
 *
 * @param [in] msg_queue A pointer to GAsyncQueue which possibly contains PolySync messages.
 *
 */
void release_polysync( GAsyncQueue * const msg_queue );


/**
 * @brief Parse \ref ps_object_stream_msg into GUI entities.
 *
 * Adds/updates the entities list with the message data.
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the configuration(s).
 * @param [in] msg A pointer to \ref ps_object_stream_msg which specifies the message to parse.
 * @param [in] parent_list A pointer to GList which specifies the parent entities list.
 * @param [in] update_time Update timestamp to give objects.
 *
 * @return The (possibly new) parent entities list, NULL means empty list.
 *
 */
GList *ps_parse_push_object_stream( const gui_context_s * const gui, const ps_object_stream_msg * const msg, GList * const parent_list, const unsigned long long update_time );


/**
 * @brief Parse \ref ps_radar_track_stream_msg into GUI entities.
 *
 * Adds/updates the entities list with the message data.
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the configuration(s).
 * @param [in] msg A pointer to \ref ps_radar_track_stream_msg which specifies the message to parse.
 * @param [in] parent_list A pointer to GList which specifies the parent entities list.
 * @param [in] update_time Update timestamp to give objects.
 *
 * @return The (possibly new) parent entities list, NULL means empty list.
 *
 */
GList *ps_parse_push_radar_track_stream( const gui_context_s * const gui, const ps_radar_track_stream_msg * const msg, GList * const parent_list, const unsigned long long update_time );


/**
 * @brief Parse \ref ps_lidar_point_stream_msg into GUI entities.
 *
 * Adds/updates the entities list with the message data.
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the configuration(s).
 * @param [in] msg A pointer to \ref ps_lidar_point_stream_msg which specifies the message to parse.
 * @param [in] parent_list A pointer to GList which specifies the parent entities list.
 * @param [in] update_time Update timestamp to give objects.
 *
 * @return The (possibly new) parent entities list, NULL means empty list.
 *
 */
GList *ps_parse_push_lidar_point_stream( const gui_context_s * const gui, const ps_lidar_point_stream_msg * const msg, GList * const parent_list, const unsigned long long update_time );


/**
 * @brief Parse \ref ps_platform_motion_msg into GUI entity.
 *
 * Updates platform data with message data.
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the configuration(s).
 * @param [in] msg A pointer to \ref ps_platform_motion_msg which specifies the message to parse.
 * @param [in] platform A pointer to \ref platform_data_s which specifies the platform data to update.
 * @param [in] update_time Update timestamp to set.
 *
 */
void ps_parse_platform_motion( const gui_context_s * const gui, const ps_platform_motion_msg * const msg, platform_data_s * const platform, const unsigned long long update_time );



/**
 * @brief Process a PolySync message.
 *
 * Dequeue a message (if any) from a data queue and process it into GUI objects
 * base on the type.
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the configuration(s).
 * @param [in] msg_queue A pointer to GAsyncQueue which specifies the message queue to read from.
 * @param [in] parent_list A pointer to GList which specifies the parent entities list.
 * @param [in] update_time Update timestamp to give objects.
 * @param [out] msg_read A pointer to unsigned int which receives the message processed status.
 * Value one means a message was processed. Value zero means no messages were available.
 *
 * @return The (possibly new) parent entities list, NULL means empty list.
 *
 */
GList *ps_process_message( const gui_context_s * const gui, GAsyncQueue * const msg_queue, GList * const parent_list, const unsigned long long update_time, unsigned int * const msg_read );




void ps_register_listener( const ps_msg_type type, GAsyncQueue * const msg_queue );


void ps_unregister_listener( const ps_msg_type type );




#endif	/* PS_INTERFACE_H */
