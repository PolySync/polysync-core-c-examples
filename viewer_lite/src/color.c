/**
 * @file color.c
 * @brief Color Provider Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gl_headers.h"
#include "common.h"
#include "drawable_type.h"
#include "color.h"




// *****************************************************
// static global structures
// *****************************************************




// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Color table indexer.
 *
 */
static unsigned long long       global_key = 0;

/**
 * @brief Color table.
 *
 */
static const GLdouble           COLOR_TABLE[][4] =
{
    //
    //
    { 1.0, 0.64, 0.08, 1.0 }, /*!< Orange. */
    //
    //
    { 0.22f, 0.82f, 0.22f, 1.0 }, /*!< Green. */
    //
    //
    { 0.18f, 0.84f, 0.80f, 1.0 }, /*!< Lite blue. */
    //
    //
    { 0.89f, 0.09f, 0.09f, 1.0 }, /*!< Red. */
    //
    //
    { 0.60f, 0.89f, 0.09f, 1.0 }, /*!< Green-yellow. */
    //
    //
    { 0.0f, 0.08f, 1.0f, 1.0 }, /*!< Dark blue. */
    //
    //
    { 0.80f, 0.0f, 1.0f, 1.0 } /*!< Purple. */
};




// *****************************************************
// static declarations
// *****************************************************




// *****************************************************
// static definitions
// *****************************************************




// *****************************************************
// public definitions
// *****************************************************

//
void color_get_next_4d( GLdouble * const color_rgba )
{
    if( color_rgba == NULL )
    {
        return;
    }

    // local vars
    const unsigned long long num_colors = (sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0]));

    // set key
    global_key = ((global_key + 1) % num_colors);

    // set color
    memcpy( color_rgba, COLOR_TABLE[global_key], sizeof(COLOR_TABLE[global_key]) );
}


//
void color_get_4d( const unsigned long long key, GLdouble * const color_rgba )
{
    if( color_rgba == NULL )
    {
        return;
    }

    // local vars
    const unsigned long long num_colors = (sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0]));

    // get key
    const unsigned long long id_key = (key % num_colors);

    // set color
    memcpy( color_rgba, COLOR_TABLE[id_key], sizeof(COLOR_TABLE[id_key]) );
}
