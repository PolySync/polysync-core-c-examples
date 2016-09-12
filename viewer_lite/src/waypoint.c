#include "gl_headers.h"
#include "render.h"
#include "gui.h"
#include "waypoint.h"



void zero_waypoints( waypoint_s * waypoints )
{
    for( int i = 0; i < MAX_WAYPOINTS; i++ )
    {
        waypoints[ i ].valid = 0;
    }
}


void add_new_waypoint( double x, double y, waypoint_s * waypoints )
{
    int i = 0;
    
    while( i < MAX_WAYPOINTS )
    {
        if( ! waypoints[ i ].valid )
        {
            waypoints[ i ].x = x;
            
            waypoints[ i ].y = y;
            
            waypoints[ i ].valid = 1;
            
            //printf( "waypoint %f %f\n", waypoints[ i ].x, waypoints[ i ].y );
            
            return;
        }
        
        i++;
    }
}


void draw_waypoints( waypoint_s * waypoints, gui_context_s * gui )
{
    int i = 0;
    
    while( i < MAX_WAYPOINTS )
    {
        if( waypoints[ i ].valid )
        {
            glPushMatrix();
            
            //glTranslated( waypoints[ i ].x, waypoints[ i ].y, 0.0 );
            
            // set entity line width
            glLineWidth( (GLfloat) gui->config.wireframe_width );

            // set color
            //glColor4dv( color );
            glColor3b( 0, 126, 0 );
            
            render_circle_2d( waypoints[ i ].x, waypoints[ i ].y, 5 );
            //render_circle_2d( -210, 210, 10 );
            
            //render_cross_2d( 0, 0, 20, 20 );
            
            printf( "waypoint render %f %f\n", waypoints[ i ].x, waypoints[ i ].y );
            
            glPopMatrix();
        }
        
        i++;
    }
}