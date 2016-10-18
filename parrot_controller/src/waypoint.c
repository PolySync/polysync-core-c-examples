/**
 * @file waypoint.c
 * @brief Stores and iterates through waypoints.
 *
 */




#include "gl_headers.h"
#include "render.h"
#include "gui.h"
#include "waypoint.h"



void zero_waypoints( waypoint_s * const waypoints )
{
    for( int i = 0; i < MAX_WAYPOINTS; i++ )
    {
        waypoints[ i ].valid = 0;
    }
}


void add_new_waypoint( const double x, const double y, waypoint_s * const waypoints )
{
    int i = 0;
    
    while( i < MAX_WAYPOINTS )
    {
        if( ! waypoints[ i ].valid )
        {
            waypoints[ i ].x = x;
            
            waypoints[ i ].y = y;
            
            waypoints[ i ].valid = 1;
            
            return;
        }
        
        i++;
    }
}


void draw_waypoints( const waypoint_s * const waypoints, gui_context_s * const gui )
{
    int i = 0;
    
    while( i < MAX_WAYPOINTS )
    {
        if( waypoints[ i ].valid )
        {
            glPushMatrix();
            
            // set entity line width
            glLineWidth( (GLfloat) gui->config.wireframe_width );

            // set color
            glColor3b( 0, 126, 0 );
            
            render_circle_2d( waypoints[ i ].x, waypoints[ i ].y, 5 );
            
            if( i < MAX_WAYPOINTS - 1 && waypoints[ i + 1 ].valid )
            {
                render_line_between_points( 
                        waypoints[ i ].x,  
                        waypoints[ i ].y, 
                        waypoints[ i + 1 ].x, 
                        waypoints[ i + 1 ].y );
            }
            
            glPopMatrix();
        }
        
        i++;
    }
}
