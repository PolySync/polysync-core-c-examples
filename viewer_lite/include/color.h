/**
 * @file color.h
 * @brief Color Provider Interface.
 *
 */




#ifndef COLOR_H
#define	COLOR_H




void color_get_next_4d( GLdouble * const color_rgba );


void color_get_4d( const unsigned long long key, GLdouble * const color_rgba );




#endif	/* COLOR_H */
