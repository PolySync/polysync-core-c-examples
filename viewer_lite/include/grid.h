/**
 * @file grid.h
 * @brief Grid Rendering Interface.
 *
 */




#ifndef GRID_H
#define	GRID_H




#include "gui.h"




//
void grid_draw_cartesian( const gui_context_s * const gui, const double range, const double step );


//
void grid_draw_radial( const gui_context_s * const gui, const double range, const double step );




#endif	/* GRID_H */
