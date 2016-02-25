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
 * @file ps_interface.h
 * @brief PolySync Interface.
 *
 * @todo fix docs.
 *
 */




#ifndef PS_INTERFACE_H
#define	PS_INTERFACE_H




#include <glib-2.0/glib.h>
#include "polysync_core.h"

#include "gui.h"





typedef struct
{
    //
    //
    ps_node_ref node;
    //
    //
    GAsyncQueue *msg_queue;
    //
    //
    ps_msg_type msg_type_radar_targets;
    //
    //
    ps_msg_type msg_type_lidar_points;
    //
    //
    ps_msg_type msg_type_objects;
} node_data_s;



/**
 * @brief Node name string.
 *
 */
extern const char       PS_NODE_NAME[];


/**
 * @brief PolySync radar targets message name.
 *
 */
extern const char       PS_RADAR_TARGETS_MSG_NAME[];


/**
 * @brief PolySync LiDAR points message name.
 *
 */
extern const char       PS_LIDAR_POINTS_MSG_NAME[];


/**
 * @brief PolySync objects message name.
 *
 */
extern const char       PS_OBJECTS_MSG_NAME[];




/**
 * @brief Initialize PolySync resources.
 *
 * Sets up data handler(s) and data queue for incoming messages.
 *
 * @return A newly created PolySync message queue on success, NULL on failure.
 *
 */
node_data_s *init_polysync( void );


/**
 * @brief Release PolySync resources.
 *
 * Free's any messages in the data queue.
 *
 * @param [in] msg_queue A pointer to GAsyncQueue which possibly contains PolySync messages.
 *
 */
void release_polysync( node_data_s * const node_data );


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
GList *ps_process_message( node_data_s * const node_data, const gui_context_s * const gui, GList * const parent_list, const unsigned long long update_time, unsigned int * const msg_read );




#endif	/* PS_INTERFACE_H */
