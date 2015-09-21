/**
 * @file ps_interface.c
 * @brief PolySync Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <glib-2.0/glib.h>
#include <math.h>

#include "polysync_core.h"
#include "common.h"
#include "drawable_type.h"
#include "gui.h"
#include "entity_manager.h"
#include "ps_interface.h"




// *****************************************************
// static global structures
// *****************************************************




// *****************************************************
// public global data
// *****************************************************


// docs in ps_interface.h
const char       PS_NODE_NAME[]     = "viewer-lite";




// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief Convert decibels to cross section.
 *
 * @param [in] db Decibels to convert.
 *
 * @return Cross section. [square meters]
 *
 */
static double db_to_cross_section( const double db );


/**
 * @brief PolySync message on-data handler.
 *
 * Enqueues new PolySync messages.
 *
 * @param [in] user_data A pointer to GAsyncQueue which specifies the data queue.
 * @param [in] msg_type Message type identifier for the message, as seen by the data model.
 * @param [in] message Message reference to be handled by the function.
 *
 */
static void psync_default_handler( void *usr_data, ps_msg_type msg_type, void *message );




// *****************************************************
// static definitions
// *****************************************************

//
static double db_to_cross_section( const double db )
{
    // cross section (m^2) = 10 ^ (db/10)
    double rcs = pow( 10, (db/10.0) );

    // cap
    if( rcs < 1.5 )
    {
        return 1.5;
    }
    else if( rcs > 25.0 )
    {
        return 25.0;
    }
    else
    {
        return rcs;
    }
}


//
static void psync_default_handler( void *usr_data, ps_msg_type msg_type, void *message )
{
    // local vars
    int                     ret                 = DTC_NONE;
    ps_node_flags           node_flags          = 0;
    ps_guid                 node_guid           = 0;
    ps_guid                 src_guid            = 0;
    void                    *copy               = NULL;

    // handle data reference
    GAsyncQueue *msg_queue = (GAsyncQueue*) usr_data;


    // ignore if not valid
    if( (msg_queue == NULL) || (message == NULL) )
    {
        return;
    }

    // get flags
    if( (ret = psync_get_node_flags( &node_flags )) != DTC_NONE )
    {
        return;
    }

    // active check
    if( (node_flags & (ps_node_flags) (1 << NODE_FLAG_HANDLERS_ACTIVE)) == 0 )
    {
        return;
    }

    // get this node's GUID
    if( (ret = psync_get_GUID( &node_guid ) ) != DTC_NONE )
    {
        return;
    }

    // get source GUID
    if( (ret = psync_message_get_source_guid( message, &src_guid ) ) != DTC_NONE )
    {
        return;
    }

    // ignore if produced by this node
    if( src_guid == node_guid )
    {
        return;
    }

    // get an instance
    if( (ret = psync_message_request_instance( msg_type, &copy ) ) != DTC_RET( DTC_NONE ) )
    {
        return;
    }

    // copy message into new message
    if( (ret = psync_message_copy( message, copy )) != DTC_RET( DTC_NONE ) )
    {
        return;
    }

    // enque
    g_async_queue_push( msg_queue, (gpointer) copy );
}




// *****************************************************
// public definitions
// *****************************************************

//
GAsyncQueue *init_polysync( void )
{
    // local vars
    GAsyncQueue *queue = NULL;


    // init polysync
    if( psync_init( PSYNC_NID_API, PS_NODE_NAME, 0 ) != DTC_NONE )
    {
        return NULL;
    }

    // create queue
    if( (queue = g_async_queue_new()) == NULL )
    {
        return NULL;
    }

    // register listener
    if( psync_message_register_listener( MSG_TYPE_OBJECT_STREAM , psync_default_handler, queue ) != DTC_NONE )
    {
        g_async_queue_unref( queue );
        return NULL;
    }

    // register listener
    if( psync_message_register_listener( MSG_TYPE_RADAR_TRACK_STREAM , psync_default_handler, queue ) != DTC_NONE )
    {
        g_async_queue_unref( queue );
        return NULL;
    }

    // register listener
    if( psync_message_register_listener( MSG_TYPE_LIDAR_POINT_STREAM , psync_default_handler, queue ) != DTC_NONE )
    {
        g_async_queue_unref( queue );
        return NULL;
    }

    // register listener
    if( psync_message_register_listener( MSG_TYPE_PLATFORM_MOTION , psync_default_handler, queue ) != DTC_NONE )
    {
        g_async_queue_unref( queue );
        return NULL;
    }

    // enable handlers
    if( psync_set_node_flags_bit( NODE_FLAG_HANDLERS_ACTIVE, 1 ) != DTC_NONE )
    {
        g_async_queue_unref( queue );
        return NULL;
    }


    // return new data queue
    return queue;
}


//
void release_polysync( GAsyncQueue * const msg_queue )
{
    // local vars
    void *element = NULL;


    // disable handlers
    psync_set_node_flags_bit( NODE_FLAG_HANDLERS_ACTIVE, 0 );

    // wait a little
    usleep( 100000 );

    // flush queue
    if( msg_queue != NULL )
    {
        // get elements in queue
        while( (element = g_async_queue_try_pop( msg_queue )) != NULL )
        {
            // release message
            psync_message_release_instance( &element );
        }

        // unref
        g_async_queue_unref( msg_queue );
    }

    // release polysync
    (void) psync_release( 0 );
}


//
GList *ps_parse_push_object_stream( const gui_context_s * const gui, const ps_object_stream_msg * const msg, GList * const parent_list, const unsigned long long update_time )
{
    if( (gui == NULL) || (msg == NULL) )
    {
        return NULL;
    }

    // local vars
    object_s                object;
    GList                   *list       = NULL;
    unsigned long           idx         = 0;
    ps_object_descriptor    *obj        = NULL;
    ps_node_type            node_type   = 0;


    // set list
    list = parent_list;

    // get node type
    (void) psync_get_node_type( msg->header.src_guid, &node_type );

    // for each object
    for( idx = 0; idx < (unsigned long) msg->objects._length; idx++ )
    {
        // cast
        obj = &msg->objects._buffer[ idx ];

        // init, object ID = obj ID
        entity_object_init( (unsigned long long) obj->id, &object );

        // defaults/unused
        object.radius = 1.25;
        object.adjusted_radius = 1.25;

        // parent ID = node GUID
        object.parent_id = (unsigned long long) msg->header.src_guid;

        // container ID = sensor SN
        object.container_id = (unsigned long long) msg->sensor_descriptor.serial_number;

        // timeout interval
        object.timeout_interval = DEFAULT_OBJECT_TIMEOUT;

        // update time
        object.update_time = update_time;

        // fusion algorithms get ellipses
        if( node_type == PSYNC_NID_FUSION_ALG )
        {
            object.primitive = PRIMITIVE_ELLIPSE;
        }
        else
        {
            // all other node types get rectangles
            object.primitive = PRIMITIVE_RECTANGLE;
        }

        // position x,y,z
        object.x = obj->pos[ 0 ];
        object.y = obj->pos[ 1 ];
        object.z = obj->pos[ 2 ];

        // velocity  x,y,z
        object.vx = obj->vel[ 0 ];
        object.vy = obj->vel[ 1 ];
        object.vz = obj->vel[ 2 ];

        // size  x,y,z
        object.length = obj->size[ 0 ];
        object.width = obj->size[ 1 ];
        object.height = obj->size[ 2 ];

		// gating stored in velocity std deviation
		object.gate_x = obj->vel_sigma[ 0 ];
		object.gate_y = obj->vel_sigma[ 1 ];
		object.gate_z = obj->vel_sigma[ 2 ];

		// position std deviation
		object.sigma_x = obj->pos_sigma[ 0 ];
		object.sigma_y = obj->pos_sigma[ 1 ];
		object.sigma_z = obj->pos_sigma[ 2 ];
		
        // orientation radians
        object.orientation = obj->orientation;

        // add/update list with object
        list = entity_object_update_copy( list, object.parent_id, object.container_id, &object );
    }


    // return parent list
    return list;
}


//
GList *ps_parse_push_radar_track_stream( const gui_context_s * const gui, const ps_radar_track_stream_msg * const msg, GList * const parent_list, const unsigned long long update_time )
{
    if( (gui == NULL) || (msg == NULL) )
    {
        return NULL;
    }

    // local vars
    object_s                object;
    GList                   *list       = NULL;
    unsigned long           idx         = 0;
    ps_radar_track          *track      = NULL;


    // set list
    list = parent_list;

    // for each track
    for( idx = 0; idx < (unsigned long) msg->tracks._length; idx++ )
    {
        // cast
        track = &msg->tracks._buffer[ idx ];

        // ignore not active
        if( track->status == TRACK_STATUS_NOT_ACTIVE )
        {
            continue;
        }

        // init, object ID = track ID
        entity_object_init( (unsigned long long) track->id, &object );

        // parent ID = node GUID
        object.parent_id = (unsigned long long) msg->header.src_guid;

        // container ID = sensor SN
        object.container_id = (unsigned long long) msg->sensor_descriptor.serial_number;

        // timeout interval
        object.timeout_interval = DEFAULT_OBJECT_TIMEOUT;

        // update time
        object.update_time = update_time;

        // primitive
        object.primitive = PRIMITIVE_CIRCLE;

        // default radius
        object.radius = 1.25;

        // adjusted radius = half the cross section
        object.adjusted_radius = db_to_cross_section( track->amplitude );
        object.adjusted_radius /= 2.0;

        // position x,y,z
        object.x = track->pos[ 0 ];
        object.y = track->pos[ 1 ];
        object.z = track->pos[ 2 ];

        // velocity  x,y,z
        object.vx = track->vel[ 0 ];
        object.vy = track->vel[ 1 ];
        object.vz = track->vel[ 2 ];

        // add/update list with object
        list = entity_object_update_copy( list, object.parent_id, object.container_id, &object );
    }


    // return parent list
    return list;
}


//
GList *ps_parse_push_lidar_point_stream( const gui_context_s * const gui, const ps_lidar_point_stream_msg * const msg, GList * const parent_list, const unsigned long long update_time )
{
    if( (gui == NULL) || (msg == NULL) )
    {
        return NULL;
    }

    // local vars
    object_s                object;
    GList                   *list       = NULL;
    unsigned long           idx         = 0;


    // ignore if no points
    if( msg->points._length == 0 )
    {
        return parent_list;
    }

    // set list
    list = parent_list;

    // init, object ID = 0
    entity_object_init( 0, &object );

    // parent ID = node GUID
    object.parent_id = (unsigned long long) msg->header.src_guid;

    // container ID = sensor SN
    object.container_id = (unsigned long long) msg->sensor_descriptor.serial_number;

    // timeout interval
    object.timeout_interval = DEFAULT_OBJECT_TIMEOUT;

    // update time
    object.update_time = update_time;

    // primitive
    object.primitive = PRIMITIVE_POINTS;

    // radius = point size / 2.0
    object.radius = 0.5;

    // create points
    if( (object.points_3d = g_try_new0( ps_point, msg->points._length )) == NULL)
    {
        return list;
    }

    // num points
    object.num_points = (unsigned long) msg->points._length;

    // for each point
    for( idx = 0; idx < object.num_points; idx++ )
    {
        // copy
        object.points_3d[ idx ].pos[ 0 ] = msg->points._buffer[ idx ].pos[ 0 ];
        object.points_3d[ idx ].pos[ 1 ] = msg->points._buffer[ idx ].pos[ 1 ];
        object.points_3d[ idx ].pos[ 2 ] = msg->points._buffer[ idx ].pos[ 2 ];
    }

    // add/update list with object
    list = entity_object_update_copy( list, object.parent_id, object.container_id, &object );


    // return parent list
    return list;
}


//
void ps_parse_platform_motion( const gui_context_s * const gui, const ps_platform_motion_msg * const msg, platform_data_s * const platform, const unsigned long long update_time )
{
    if( (gui == NULL) || (msg == NULL) )
    {
        return;
    }


    // no object ID
    platform->id = 0;

    // parent ID = node GUID
    platform->parent_id = (unsigned long long) msg->header.src_guid;

    // container ID = sensor SN
    platform->container_id = (unsigned long long) msg->sensor_descriptor.serial_number;

    // no timeout interval
    platform->timeout_interval = ENTITY_NO_TIMEOUT;

    // update time
    platform->update_time = update_time;

    // rotation rate x,y,z
    platform->wx = msg->rotation_rate[ 0 ];
    platform->wy = msg->rotation_rate[ 1 ];
    platform->wz = msg->rotation_rate[ 2 ];

    // is valid
    if( msg->valid_fields & PSYNC_PFMOTION_ROTATION_RATE_VALID )
    {
        platform->rotation_rate_valid = 1;
    }
    else
    {
        platform->rotation_rate_valid = 0;
    }

    // velocity x,y,z
    platform->vx = msg->velocity[ 0 ];
    platform->vy = msg->velocity[ 1 ];
    platform->vz = msg->velocity[ 2 ];

    // is valid
    if( msg->valid_fields & PSYNC_PFMOTION_VELOCITY_VALID )
    {
        platform->velocity_valid = 1;
    }
    else
    {
        platform->velocity_valid = 0;
    }

    // speed = |x/y velocity|
    platform->speed = sqrt( (platform->vx * platform->vx) + (platform->vy * platform->vy) );

    // calculate adjusted platfrom velocity
    platform->adjusted_vx = platform->speed;
    platform->adjusted_vy = platform->wz * platform->speed + platform->speed * tan(platform->wz * 0.05);
}


//
GList *ps_process_message( const gui_context_s * const gui, GAsyncQueue * const msg_queue, GList * const parent_list, const unsigned long long update_time, unsigned int * const msg_read )
{
    if( (gui == NULL) || (msg_read == NULL) )
    {
        return NULL;
    }

    if( msg_queue == NULL )
    {
        // nothing to do
        return parent_list;
    }

    // local vars
    void        *msg    = NULL;
    GList       *list   = NULL;
    ps_msg_type type    = 0;


    // set list
    list = parent_list;

    // check for message
    if( (msg = g_async_queue_try_pop( msg_queue )) != NULL )
    {
        // get message type
        if( psync_message_get_type( msg, &type ) == DTC_NONE )
        {
            // success

            // if not in freeze-frame
            if( gui->config.freeze_frame == 0 )
            {
                // process known types
                if( type == MSG_TYPE_OBJECT_STREAM )
                {
                    list = ps_parse_push_object_stream( gui, (const ps_object_stream_msg*) msg, list, update_time );
                }
                else if( type == MSG_TYPE_RADAR_TRACK_STREAM )
                {
                    list = ps_parse_push_radar_track_stream( gui, (const ps_radar_track_stream_msg*) msg, list, update_time );
                }
                else if( type == MSG_TYPE_LIDAR_POINT_STREAM )
                {
                    list = ps_parse_push_lidar_point_stream( gui, (const ps_lidar_point_stream_msg*) msg, list, update_time );
                }
                else if( type == MSG_TYPE_PLATFORM_MOTION )
                {
                    ps_parse_platform_motion( gui, (const ps_platform_motion_msg*) msg, (platform_data_s*) &gui->platform, update_time );
                }
            }
        }

        // release
        (void) psync_message_release_instance( &msg );
    }


    // return parent list
    return list;
}


//
void ps_register_listener( const ps_msg_type type, GAsyncQueue * const msg_queue )
{
    (void) psync_message_register_listener( type , psync_default_handler, msg_queue );
}


//
void ps_unregister_listener( const ps_msg_type type )
{
    (void) psync_message_unregister_listener( type );
}
