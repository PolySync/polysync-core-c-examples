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
 * @file ps_interface.h
 * @brief PolySync Interface.
 *
 */




#ifndef PS_INTERFACE_H
#define PS_INTERFACE_H




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
    ps_msg_type msg_type_platform_motion;
    //
    //
    waypoint_s current_vehicle_position;
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
extern const char       PS_PLATFORM_MOTION_MSG_NAME[];




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




#endif	/* PS_INTERFACE_H */
