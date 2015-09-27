/**
 * @file ps_interface.c
 * @brief PolySync Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <glib-2.0/glib.h>
#include "polysync_core.h"
#include "polysync_node.h"
#include "polysync_sdf.h"
#include "polysync_message.h"

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


//
const char       PS_RADAR_TARGETS_MSG_NAME[] = "ps_radar_targets_msg";




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
 * @param [in] msg_type Message type identifier for the message, as seen by the data model.
 * @param [in] message Message reference to be handled by the function.
 * @param [in] user_data A pointer to void which specifies the internal data.
 *
 */
static void psync_default_handler( const ps_msg_type msg_type, const ps_msg_ref const message, void * const user_data );


/**
 * @brief Parse \ref ps_radar_track_stream_msg into GUI entities.
 *
 * Adds/updates the entities list with the message data.
 *
 * @param [in] gui A pointer to \ref gui_context_s which specifies the configuration(s).
 * @param [in] msg A pointer to \ref ps_radar_targets_msg which specifies the message to parse.
 * @param [in] parent_list A pointer to GList which specifies the parent entities list.
 * @param [in] update_time Update timestamp to give objects.
 *
 * @return The (possibly new) parent entities list, NULL means empty list.
 *
 */
static GList *ps_parse_push_radar_targets( const gui_context_s * const gui, const ps_radar_targets_msg * const msg, GList * const parent_list, const unsigned long long update_time );




// *****************************************************
// static definitions
// *****************************************************

//
static double clamp_cross_section( const double cs )
{
    // clamp
    if( cs < 1.5 )
    {
        return 1.5;
    }
    else if( cs > 20.0 )
    {
        return 20.0;
    }
    else
    {
        return cs;
    }
}

//
static double db_to_cross_section( const double db )
{
    // cross section (m^2) = 10 ^ (db/10)
    double rcs_sqrt = sqrt( pow( 10, (db/10.0) ) );

    // clamp
    return clamp_cross_section( rcs_sqrt );
}


//
static void psync_default_handler( const ps_msg_type msg_type, const ps_msg_ref const message, void * const user_data )
{
    // local vars
    node_data_s     *node_data  = NULL;
    ps_msg_ref      msg_copy    = PSYNC_MSG_REF_INVALID;
    ps_guid         node_guid   = PSYNC_GUID_INVALID;
    ps_guid         src_guid    = PSYNC_GUID_INVALID;
    ps_node_flags   node_flags  = 0;


    // cast
    node_data = (node_data_s*) user_data;

    // ignore if not valid
    if( (node_data == NULL) || (message == NULL) || (node_data->msg_queue == NULL) )
    {
        return;
    }

    // get flags
    if( psync_node_get_flags( node_data->node, &node_flags ) != DTC_NONE )
    {
        return;
    }

    // active check
    if( (node_flags & (ps_node_flags) (1 << NODE_FLAG_HANDLERS_ENABLED)) == 0 )
    {
        return;
    }

    // get this node's GUID
    if( psync_node_get_guid( node_data->node, &node_guid ) != DTC_NONE )
    {
        return;
    }

    // get source GUID
    if( psync_message_get_source_guid( message, &src_guid ) != DTC_NONE )
    {
        return;
    }

    // ignore if produced by this node
    if( src_guid == node_guid )
    {
        return;
    }

    // get an instance
    if( psync_message_request_instance( node_data->node, msg_type, &msg_copy ) != DTC_NONE )
    {
        return;
    }

    // copy message into new message
    if( psync_message_copy( node_data->node, message, msg_copy ) != DTC_NONE )
    {
        return;
    }

    // enque
    g_async_queue_push( node_data->msg_queue, (gpointer) msg_copy );
}


//
static GList *ps_parse_push_radar_targets( const gui_context_s * const gui, const ps_radar_targets_msg * const msg, GList * const parent_list, const unsigned long long update_time )
{
    if( (gui == NULL) || (msg == NULL) )
    {
        return NULL;
    }

    // local vars
    object_s                object;
    GList                   *list       = NULL;
    unsigned long           idx         = 0;
    ps_radar_target         *target     = NULL;


    // set list
    list = parent_list;

    // for each track
    for( idx = 0; idx < (unsigned long) msg->targets._length; idx++ )
    {
        // cast
        target = &msg->targets._buffer[ idx ];

        // ignore invalid targets
        if(
                (target->track_status == TRACK_STATUS_NO_TRACK)
                || (target->track_status == TRACK_STATUS_NO_TRACK)
                )
        {
            continue;
        }

        // init, object ID = track ID
        entity_object_init( (unsigned long long) target->id, &object );

        // parent ID = node GUID
        object.parent_id = (unsigned long long) msg->header.src_guid;

        // container ID = sensor SN
        object.container_id = (unsigned long long) msg->sensor_descriptor.id;

        // timeout interval
        object.timeout_interval = 230000;

        // update time
        object.update_time = update_time;

        // primitive
        object.primitive = PRIMITIVE_CIRCLE;

        // default radius
        object.radius = 1.25;

        // if amplitude provided
        if( target->cross_section != PSYNC_RADAR_CROSS_SECTION_NOT_AVAILABLE )
        {
            // use rcs
            object.adjusted_radius = clamp_cross_section( sqrt( target->cross_section ) );
        }
        else if( target->amplitude != PSYNC_AMPLITUDE_NOT_AVAILABLE )
        {
            object.adjusted_radius = db_to_cross_section( target->amplitude );
        }
        else
        {
            object.adjusted_radius = 0.3;
        }

        // position x,y,z
        object.x = target->position[ 0 ];
        object.y = target->position[ 1 ];
        object.z = target->position[ 2 ];

        // size x,y,z
        if( target->size[ 0 ] != PSYNC_SIZE_NOT_AVAILABLE )
        {
            object.length = target->size[ 0 ];
        }
        if( target->size[ 1 ] != PSYNC_SIZE_NOT_AVAILABLE )
        {
            object.width = target->size[ 1 ];
        }
        if( target->size[ 2 ] != PSYNC_SIZE_NOT_AVAILABLE )
        {
            object.height = target->size[ 2 ];
        }

        // velocity  x,y,z
        if( target->velocity[ 0 ] != PSYNC_VELOCITY_NOT_AVAILABLE )
        {
            object.vx = target->velocity[ 0 ];
        }
        if( target->velocity[ 1 ] != PSYNC_VELOCITY_NOT_AVAILABLE )
        {
            object.vy = target->velocity[ 1 ];
        }
        if( target->velocity[ 2 ] != PSYNC_VELOCITY_NOT_AVAILABLE )
        {
            object.vz = target->velocity[ 2 ];
        }

        // add/update list with object
        list = entity_object_update_copy( list, object.parent_id, object.container_id, &object );
    }


    // return parent list
    return list;
}




// *****************************************************
// public definitions
// *****************************************************

//
node_data_s *init_polysync( void )
{
    // local vars
    node_data_s *node_data = NULL;

    // create
    if( (node_data = malloc( sizeof(*node_data) )) == NULL )
    {
        return NULL;
    }

    // zero
    memset( node_data, 0, sizeof(*node_data) );
    node_data->node = PSYNC_NODE_REF_INVALID;
    node_data->msg_type_radar_targets = PSYNC_MSG_TYPE_INVALID;

    // init polysync
    if( psync_init( PS_NODE_NAME, PSYNC_NODE_TYPE_API_USER, PSYNC_DEFAULT_DOMAIN, PSYNC_SDF_KEY_INVALID, 0, &node_data->node ) != DTC_NONE )
    {
        free( node_data );
        return NULL;
    }

    // create queue
    if( (node_data->msg_queue = g_async_queue_new()) == NULL )
    {
        (void) psync_release( &node_data->node, 0 );
        free( node_data );
        return NULL;
    }

    // disable handlers
    if( psync_node_set_flag( node_data->node, NODE_FLAG_HANDLERS_ENABLED, 0 ) != DTC_NONE )
    {
        (void) psync_release( &node_data->node, 0 );
        g_async_queue_unref( node_data->msg_queue );
        free( node_data );
        return NULL;
    }

    // check for 'sensor' module
#warning "TODO add psync get module info routine"

    // check version ?

    // get type
    if( psync_message_get_type_by_name( node_data->node, PS_RADAR_TARGETS_MSG_NAME, &node_data->msg_type_radar_targets ) != DTC_NONE )
    {
        (void) psync_release( &node_data->node, 0 );
        g_async_queue_unref( node_data->msg_queue );
        free( node_data );
        return NULL;
    }

    // register listener
    if( psync_message_register_listener( node_data->node, node_data->msg_type_radar_targets , psync_default_handler, node_data ) != DTC_NONE )
    {
        (void) psync_release( &node_data->node, 0 );
        g_async_queue_unref( node_data->msg_queue );
        free( node_data );
        return NULL;
    }

    // enable handlers
    if( psync_node_set_flag( node_data->node, NODE_FLAG_HANDLERS_ENABLED, 1 ) != DTC_NONE )
    {
        (void) psync_release( &node_data->node, 0 );
        g_async_queue_unref( node_data->msg_queue );
        free( node_data );
        return NULL;
    }


    // return new node data
    return node_data;
}


//
void release_polysync( node_data_s * const node_data )
{
    if( node_data == NULL )
    {
        return;
    }

    // local vars
    ps_msg_ref msg = PSYNC_MSG_REF_INVALID;


    // disable handlers
    (void) psync_node_set_flag( node_data->node, NODE_FLAG_HANDLERS_ENABLED, 0 );

    // wait a little
    usleep( 100000 );

    // flush queue
    if( node_data->msg_queue != NULL )
    {
        // get elements in queue
        while( (msg = g_async_queue_try_pop( node_data->msg_queue )) != NULL )
        {
            // release message
            (void) psync_message_release_instance( node_data->node, &msg );
        }

        // unref
        g_async_queue_unref( node_data->msg_queue );
    }

    // release polysync
    (void) psync_release( &node_data->node, 0 );
}


//
GList *ps_process_message( node_data_s * const node_data, const gui_context_s * const gui, GList * const parent_list, const unsigned long long update_time, unsigned int * const msg_read )
{
    if( (node_data == NULL) || (gui == NULL) || (msg_read == NULL) )
    {
        raise( SIGINT );
        return NULL;
    }

    if( node_data->msg_queue == NULL )
    {
        // nothing to do
        return parent_list;
    }

    // local vars
    ps_msg_ref   msg    = PSYNC_MSG_REF_INVALID;
    GList       *list   = NULL;
    ps_msg_type type    = 0;


    // set list
    list = parent_list;

    // check for message
    if( (msg = g_async_queue_try_pop( node_data->msg_queue )) != NULL )
    {
        // get message type
        if( psync_message_get_type( msg, &type ) == DTC_NONE )
        {
            // success

            // if not in freeze-frame
            if( gui->config.freeze_frame == 0 )
            {
                // process known types
                if( type == node_data->msg_type_radar_targets )
                {
                    list = ps_parse_push_radar_targets( gui, (const ps_radar_targets_msg*) msg, list, update_time );
                }
            }
        }

        // release
        (void) psync_message_release_instance( node_data->node, &msg );
    }


    // return parent list
    return list;
}

#warning "TODO - un/reg listeners"
//
void ps_register_listener( const ps_msg_type type, GAsyncQueue * const msg_queue )
{
    //(void) psync_message_register_listener( type , psync_default_handler, msg_queue );
}


//
void ps_unregister_listener( const ps_msg_type type )
{
    //(void) psync_message_unregister_listener( type );
}
