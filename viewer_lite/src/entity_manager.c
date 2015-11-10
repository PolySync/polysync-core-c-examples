/**
 * @file entity_manager.c
 * @brief Object Entity Manager Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gl_headers.h"
#include "common.h"
#include "drawable_type.h"
#include "render.h"
#include "color.h"
#include "entity_manager.h"




// *****************************************************
// static global structures
// *****************************************************




// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************




// *****************************************************
// static definitions
// *****************************************************

//
static void object_release_function( gpointer data )
{
    // cast
    object_s *obj = (object_s*) data;


    // if valid
    if( obj != NULL )
    {
        // free points
        if( obj->points_3d != NULL )
        {
            g_free( obj->points_3d );
        }

        // free
        g_free( obj );
    }
}


//
static void container_release_function( gpointer data )
{
    // cast
    object_container_s *cntnr = (object_container_s*) data;


    // if valid
    if( cntnr != NULL )
    {
        // if objects exist
        if( cntnr->objects != NULL )
        {
            // free each object in container
            g_list_free_full( cntnr->objects, object_release_function );
        }

        // free
        g_free( cntnr );
    }
}


//
static void parent_release_function( gpointer data )
{
    // cast
    object_container_parent_s *parent = (object_container_parent_s*) data;


    // if valid
    if( parent != NULL )
    {
        // if containers exist
        if( parent->containers != NULL )
        {
            // free each container in parent
            g_list_free_full( parent->containers, container_release_function );
        }

        // free
        g_free( parent );
    }
}


//
static gint object_id_compare_function( gconstpointer a, gconstpointer b )
{
    // cast
    const object_s *obj = (const object_s*) a;
    const unsigned long long *id = (const unsigned long long*) b;


    // compare
    if( obj->id < (*id) )
    {
        // a comes before b
        return (gint) -1;
    }
    else if( obj->id > (*id) )
    {
        // a comes after b
        return (gint) 1;
    }
    else
    {
        // a equals b
        return (gint) 0;
    }
}


//
static gint container_id_compare_function( gconstpointer a, gconstpointer b )
{
    // cast
    const object_container_s *cntnr = (const object_container_s*) a;
    const unsigned long long *id = (const unsigned long long*) b;


    // compare
    if( cntnr->id < (*id) )
    {
        // a comes before b
        return (gint) -1;
    }
    else if( cntnr->id > (*id) )
    {
        // a comes after b
        return (gint) 1;
    }
    else
    {
        // a equals b
        return (gint) 0;
    }
}


//
static gint parent_id_compare_function( gconstpointer a, gconstpointer b )
{
    // cast
    const object_container_parent_s *parent = (const object_container_parent_s*) a;
    const unsigned long long *id = (const unsigned long long*) b;


    // compare
    if( parent->id < (*id) )
    {
        // a comes before b
        return (gint) -1;
    }
    else if( parent->id > (*id) )
    {
        // a comes after b
        return (gint) 1;
    }
    else
    {
        // a equals b
        return (gint) 0;
    }
}


//
static GList *timeout_objects( GList * const object_list, const unsigned long long compare_time )
{
    if( object_list == NULL )
    {
        return NULL;
    }

    if( compare_time == 0 )
    {
        return object_list;
    }

    // local vars
    GList   *list       = NULL;
    GList   *element    = NULL;
    GList   *next       = NULL;


    // set list
    list = object_list;
    element = list;

    // for each object
    while( element != NULL )
    {
        // cast
        object_s *obj = (object_s*) element->data;

        // get next, since current may be deleted need to get next reference before
        next = element->next;

        // if valid
        if( obj != NULL )
        {
            // if timeouts enabled
            if( obj->timeout_interval != ENTITY_NO_TIMEOUT )
            {
                // if object is in future or timeout exceeded
                if( (obj->update_time > compare_time) || ((compare_time - obj->update_time) >= obj->timeout_interval) )
                {
                    // delete object link, update list
                    list = g_list_remove_link( list, element );

                    // free container
                    object_release_function( element->data );

                    // free link
                    g_list_free( element );
                }
            }
        }

        // next element
        element = next;
    }


    // return parent list
    return list;
}


//
static GList *timeout_containers( GList * const container_list, const unsigned long long compare_time )
{
    if( container_list == NULL )
    {
        return NULL;
    }

    if( compare_time == 0 )
    {
        return container_list;
    }

    // local vars
    GList   *list       = NULL;
    GList   *element    = NULL;
    GList   *next       = NULL;


    // set list
    list = container_list;
    element = list;

    // for each container
    while( element != NULL )
    {
        // cast
        object_container_s *cntnr = (object_container_s*) element->data;

        // get next, since current may be deleted need to get next reference before
        next = element->next;

        // if valid
        if( cntnr != NULL )
        {
            // if timeouts enabled
            if( cntnr->timeout_interval != ENTITY_NO_TIMEOUT )
            {
                // if container is in future or timeout exceeded
                if( (cntnr->update_time > compare_time) || ((compare_time - cntnr->update_time) >= cntnr->timeout_interval) )
                {
                    // delete container link, update list
                    list = g_list_remove_link( list, element );

                    // free container
                    container_release_function( element->data );

                    // free link
                    g_list_free( element );
                }
                else
                {
                    // check the container's objects
                    cntnr->objects = timeout_objects( cntnr->objects, compare_time );
                }
            }
        }

        // next element
        element = next;
    }


    // return parent list
    return list;
}




// *****************************************************
// public definitions
// *****************************************************

//
object_s *entity_object_new( const unsigned long long id )
{
    // local vars
    object_s *obj = NULL;


    // create
    if( (obj = g_try_new0( object_s, 1 )) == NULL)
    {
        return NULL;
    }

    // init
    entity_object_init( id, obj );


    // return object memory
    return obj;
}


//
object_s *entity_object_new_copy( const object_s * const copy )
{
    // local vars
    object_s *obj = NULL;


    // create
    if( (obj = g_try_new0( object_s, 1 )) == NULL)
    {
        return NULL;
    }

    // zero
    memset( obj, 0, sizeof(*obj) );

    // copy
    if( copy != NULL )
    {
        memcpy( obj, copy, sizeof(*obj) );
    }


    // return object memory
    return obj;
}


//
void entity_object_init( const unsigned long long id, object_s * const object )
{
    if( object == NULL )
    {
        return;
    }


    // zero
    memset( object, 0, sizeof(*object) );

    // id
    object->id = id;

    // default timeout
    object->timeout_interval = DEFAULT_OBJECT_TIMEOUT;

    // defaults
    color_get_4d( id, object->color_rgba );
    object->length = 1.0;
    object->width = 1.0;
    object->height = 1.0;
    object->radius = 1.0;
}


//
object_container_s *entity_container_new( const unsigned long long id )
{
    // local vars
    object_container_s *cntnr = NULL;


    // create
    if( (cntnr = g_try_new0( object_container_s, 1 )) == NULL)
    {
        return NULL;
    }

    // zero
    memset( cntnr, 0, sizeof(*cntnr) );

    // id
    cntnr->id = id;

    // default timeout
    cntnr->timeout_interval = DEFAULT_CONTAINER_TIMEOUT;

    // defaults
    color_get_next_4d( cntnr->color_rgba );


    // return container memory
    return cntnr;
}


//
object_container_parent_s *entity_parent_new( const unsigned long long id )
{
    // local vars
    object_container_parent_s *parent = NULL;


    // create
    if( (parent = g_try_new0( object_container_parent_s, 1 )) == NULL)
    {
        return NULL;
    }

    // zero
    memset( parent, 0, sizeof(*parent) );

    // id
    parent->id = id;

    // default timeout
    parent->timeout_interval = DEFAULT_PARENT_TIMEOUT;

    // defaults
    color_get_next_4d( parent->color_rgba );


    // return parent memory
    return parent;
}


//
void entity_release_all( GList * const parent_list )
{
    if( parent_list == NULL )
    {
        return;
    }

    // free list and data elements
    g_list_free_full( parent_list, parent_release_function );
}


//
GList *entity_update_timeouts( GList * const parent_list, const unsigned long long compare_time )
{
    if( parent_list == NULL )
    {
        return NULL;
    }

    if( compare_time == 0 )
    {
        return parent_list;
    }

    // local vars
    GList   *list       = NULL;
    GList   *element    = NULL;
    GList   *next       = NULL;


    // set list
    list = parent_list;
    element = list;

    // for each parent
    while( element != NULL )
    {
        // cast
        object_container_parent_s *parent = (object_container_parent_s*) element->data;

        // get next, since current may be deleted need to get next reference before
        next = element->next;

        // if valid
        if( parent != NULL )
        {
            // if timeouts enabled
            if( parent->timeout_interval != ENTITY_NO_TIMEOUT )
            {
                // if parent is in future or timeout exceeded
                if( (parent->update_time > compare_time) || ((compare_time - parent->update_time) >= parent->timeout_interval) )
                {
                    // delete parent link, update list
                    list = g_list_remove_link( list, element );

                    // free parent
                    parent_release_function( element->data );

                    // free link
                    g_list_free( element );
                }
                else
                {
                    // check containers and their children elements
                    parent->containers = timeout_containers( parent->containers, compare_time );
                }
            }
        }

        // next element
        element = next;
    }


    // return parent list
    return list;
}


//
object_s *entity_container_search_by_id( const object_container_s * const container, const unsigned long long obj_id )
{
    if( container == NULL )
    {
        return NULL;
    }

    // local vars
    GList *element = NULL;


    // check if objects exist
    if( container->objects != NULL )
    {
        // search object list
        element = g_list_find_custom( container->objects, &obj_id, &object_id_compare_function );
    }

    // return object if found
    if( (element != NULL) && (element->data != NULL) )
    {
        return ((object_s*) element->data);
    }
    else
    {
        // no found
        return NULL;
    }
}


//
object_container_s *entity_parent_search_by_id( const object_container_parent_s * const parent, const unsigned long long container_id )
{
    if( parent == NULL )
    {
        return NULL;
    }

    // local vars
    GList *element = NULL;


    // check if containers exist
    if( parent->containers != NULL )
    {
        // search container list
        element = g_list_find_custom( parent->containers, &container_id, &container_id_compare_function );
    }

    // return container if found
    if( (element != NULL) && (element->data != NULL) )
    {
        return ((object_container_s*) element->data);
    }
    else
    {
        // no found
        return NULL;
    }
}


//
object_container_parent_s *entity_parent_list_search_by_id( const GList * const parent_list, const unsigned long long parent_id )
{
    if( parent_list == NULL )
    {
        return NULL;
    }

    // local vars
    GList *element = NULL;


    // search parent list
    element = g_list_find_custom( (GList*) parent_list, &parent_id, &parent_id_compare_function );

    // return parent if found
    if( (element != NULL) && (element->data != NULL) )
    {
        return ((object_container_parent_s*) element->data);
    }
    else
    {
        // no found
        return NULL;
    }
}


//
GList *entity_object_update_copy( GList * const parent_list, const unsigned long long parent_id, const unsigned long long container_id, const object_s * const object )
{
    // local vars
    GList *list = NULL;
    object_container_parent_s *parent = NULL;
    object_container_s *cntnr = NULL;
    object_s *obj = NULL;


    // set list
    list = parent_list;

    // search for parent
    if( (parent = entity_parent_list_search_by_id( list, parent_id )) == NULL )
    {
        // create new parent
        if( (parent = entity_parent_new( parent_id )) == NULL )
        {
            printf( "object : (%u) -- failed to create parent\n", __LINE__ );
            return NULL;
        }

        // create new container
        if( (cntnr = entity_container_new( container_id )) == NULL )
        {
            printf( "object : (%u) -- failed to create container\n", __LINE__ );
            return NULL;
        }

        // create new object
        if( (obj = entity_object_new( object->id )) == NULL )
        {
            printf( "object : (%u) -- failed to create object\n", __LINE__ );
            return NULL;
        }

        // add object to container
        cntnr->objects = g_list_append( cntnr->objects, obj );

        // add container to parent
        parent->containers = g_list_append( parent->containers, cntnr );

        // add parent to list
        list = g_list_append( list, parent );
    }
    // search for container
    else if( (cntnr = entity_parent_search_by_id( parent, container_id )) == NULL )
    {
        // create new container
        if( (cntnr = entity_container_new( container_id )) == NULL )
        {
            printf( "object : (%u) -- failed to create container\n", __LINE__ );
            return NULL;
        }

        // create new object
        if( (obj = entity_object_new( object->id )) == NULL )
        {
            printf( "object : (%u) -- failed to create object\n", __LINE__ );
            return NULL;
        }

        // add object to container
        cntnr->objects = g_list_append( cntnr->objects, obj );

        // add container to parent
        parent->containers = g_list_append( parent->containers, cntnr );
    }
    // search for object
    else if( (obj = entity_container_search_by_id( cntnr, object->id )) == NULL )
    {
        // create new object
        if( (obj = entity_object_new( object->id )) == NULL )
        {
            printf( "object : (%u) -- failed to create object\n", __LINE__ );
            return NULL;
        }

        // add object to container
        cntnr->objects = g_list_append( cntnr->objects, obj );
    }

    // free points if any before we overwrite
    if( obj->points_3d != NULL )
    {
        g_free( obj->points_3d );
        obj->points_3d = NULL;
    }

    // copy object
    memcpy( obj, object, sizeof(*obj) );

    // update time
    obj->update_time = object->update_time;
    cntnr->update_time = object->update_time;
    parent->update_time = object->update_time;


    // return parent list
    return list;
}


//
void entity_draw_object( const gui_context_s * const gui, const object_s * const object, const GLdouble * const color_rgba )
{
    if( (gui == NULL) || (object == NULL) )
    {
        return;
    }

    // local vars
    const GLdouble *color = color_rgba;

    // ignore if disabled
    if( (gui->config.circle_visible == 0) && (object->primitive == PRIMITIVE_CIRCLE) )
    {
        return;
    }
    else if( (gui->config.rectangle_visible == 0) && (object->primitive == PRIMITIVE_RECTANGLE) )
    {
        return;
    }
    else if( (gui->config.ellipse_visible == 0) && (object->primitive == PRIMITIVE_ELLIPSE) )
    {
        return;
    }
    else if( (gui->config.points_visible == 0) && (object->primitive == PRIMITIVE_POINTS) )
    {
        return;
    }

    // if color not provided, use objects color
    if( color == NULL )
    {
        color = object->color_rgba;
    }

    // check view mode
    if( gui->config.view_mode == VIEW_MODE_BIRDSEYE )
    {
        // save state
        glPushMatrix();

        // translate to center
        glTranslated( object->x, object->y, 0.0 );

        // if velocity vectors visible
        if( gui->config.velocity_vectors_visible != 0 )
        {
            // default line width
            glLineWidth( (GLfloat) GUI_DEFAULT_LINE_WIDTH );

            // set color with alpha
            glColor4d( color[0], color[1], color[2], 0.6 );

            // begin lines
            glBegin( GL_LINES );

            // draw relative vector
            glVertex2d( 0.0, 0.0 );
            glVertex2d( object->vx, object->vy );

            glEnd();
            // end lines
        }

        // rotate orientation
        glRotated( RAD_2_DEG(object->orientation), 0.0, 0.0, 1.0 );

        // set entity line width
        glLineWidth( (GLfloat) gui->config.wireframe_width );

        // set color
        glColor4dv( color );

        // check primitive
        if( object->primitive == PRIMITIVE_CIRCLE )
        {
            // check for adjusted radius usage
            if( gui->config.adjusted_circle_radius != 0 )
            {
                // render
                render_circle_2d( 0.0, 0.0, object->adjusted_radius );

                // render cross at center
                render_cross_2d( 0.0, 0.0, 0.5, 0.5 );
            }
            else
            {
                // render
                render_circle_2d( 0.0, 0.0, object->radius );

                // render cross at center
                render_cross_2d( 0.0, 0.0, 0.5, 0.5 );
            }

            // restore state
            glPopMatrix();
        }
        else if( object->primitive == PRIMITIVE_CROSS )
        {
            // render
            render_cross_2d( 0.0, 0.0, object->length, object->width );

            // restore state
            glPopMatrix();
        }
        else if( object->primitive == PRIMITIVE_RECTANGLE )
        {
            // render
            render_rectangle_2d( 0.0, 0.0, object->length, object->width );

            // restore state
            glPopMatrix();
        }
        else if( object->primitive == PRIMITIVE_TRIANGLE )
        {
            // render
            render_triangle_2d( 0.0, 0.0, object->length, object->width );

            // restore state
            glPopMatrix();
        }
        else if( object->primitive == PRIMITIVE_ELLIPSE )
        {
            // render gated ellipse
            render_ellipse_2d( 0.0, 0.0, object->gate_x, object->gate_y );

			// rectangle is transparent
			glColor4d( color[0], color[1], color[2], 0.4 );

			// render size rectangle
            render_rectangle_2d( 0.0, 0.0, object->length, object->width );

            // restore state
            glPopMatrix();
        }
        else if( object->primitive == PRIMITIVE_POINTS )
        {
            // NOTE: must be last in list since it pops state
            unsigned long idx = 0;

            // restore state
            glPopMatrix();

            // if points exist
            if( (object->points_3d != NULL) && (object->num_points > 0) )
            {
                // set point size
                glPointSize( (GLfloat) (object->radius * 2.0) );

                // begin points
                glBegin( GL_POINTS );

                // for each point
                for( idx = 0; idx < object->num_points; idx++ )
                {
                    // vertex
                    glVertex2d( object->points_3d[ idx ].position[ 0 ], object->points_3d[ idx ].position[ 1 ] );
                }

                // end points
                glEnd();
            }
        }
    }
}


//
void entity_draw_container( const gui_context_s * const gui, const object_container_s * const container, const GLdouble * const color_rgba )
{
    if( (gui == NULL) || (container == NULL) )
    {
        return;
    }

    // local vars
    GList *element = NULL;


    // for each object
    element = container->objects;
    while( element != NULL )
    {
        // check if color provided
        if( color_rgba != NULL )
        {
            // draw object using provided color
            entity_draw_object( gui, (const object_s*) element->data, color_rgba );
        }
        else
        {
            // check color mode
            if( gui->config.color_mode == COLOR_MODE_CONTAINER_ID )
            {
                // draw object using containers color
                entity_draw_object( gui, (const object_s*) element->data, container->color_rgba );
            }
            else
            {
                // COLOR_MODE_OBJECT_ID

                // draw object using its color
                entity_draw_object( gui, (const object_s*) element->data, ((const object_s*) element->data)->color_rgba );
            }
        }

        // next element
        element = element->next;
    }
}


//
void entity_draw_parent( const gui_context_s * const gui, const object_container_parent_s * const parent )
{
    if( (gui == NULL) || (parent == NULL) )
    {
        return;
    }

    // local vars
    GList *element = NULL;


    // for each container
    element = parent->containers;
    while( element != NULL )
    {
        // check color mode
        if( gui->config.color_mode == COLOR_MODE_PARENT_ID )
        {
            // draw container and children using this parents color
            entity_draw_container( gui, (const object_container_s*) element->data, parent->color_rgba );
        }
        else
        {
            // COLOR_MODE_CONTAINER_ID
            // COLOR_MODE_OBJECT_ID

            // draw container and children using given color mode
            entity_draw_container( gui, (const object_container_s*) element->data, NULL );
        }

        // next element
        element = element->next;
    }
}


//
void entity_draw_all( const gui_context_s * const gui, const GList * const parent_list )
{
    if( (gui == NULL) || (parent_list == NULL) )
    {
        return;
    }

    // local vars
    GList *element = NULL;


    // for each parent
    element = (GList*) parent_list;
    while( element != NULL )
    {
        // draw parent
        entity_draw_parent( gui, (const object_container_parent_s*) element->data );

        // next element
        element = element->next;
    }
}
