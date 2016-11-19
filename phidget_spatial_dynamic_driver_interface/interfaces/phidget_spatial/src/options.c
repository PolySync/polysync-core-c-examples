/**
 * @file phidget_spatial/src/options.c
 * @brief Phidget Spatial Option Handling Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "polysync_core.h"
#include "polysync_dynamic_interface.h"
#include "polysync_interface_util.h"
#include "interface_data.h"
#include "options.h"




/**
 * @brief PAL interface options string.
 *
 */
const char PALOPT_STRING[] = "dt";


/**
 * @brief PAL interface options character list.
 *
 * @note Order defines the indexing scheme.
 *
 */
const char PALOPT_CHARS[] =
{
    [PALOPT_DEBUG] = 'd',
    [PALOPT_TEST] = 't',
    '\0'
};


/**
 * @brief PAL interface options description list.
 *
 * @note Order must match the indexing scheme in \ref PALOPT_CHARS.
 *
 */
const char * const STDOPT_DESCRIPTIONS[] =
{
    [PALOPT_DEBUG] = "enable additional debugging output [optional]",
    [PALOPT_TEST] = "perform a validation test on the interface [optional]",
    NULL
};


/**
 * @brief PAL interface options help message list.
 *
 * @note Order must match the indexing scheme in \ref PALOPT_CHARS.
 *
 */
const char * const STDOPT_HELP_STRINGS[] =
{
    [PALOPT_DEBUG] = "-d",
    [PALOPT_TEST] = "-t",
    NULL
};




// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************


//
static int parse_option_debug( ps_dynamic_driver * const dynamic_driver );


//
static int parse_option_test( ps_dynamic_driver * const dynamic_driver );




// *****************************************************
// static definitions
// *****************************************************


//
static int parse_option_debug( ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;


    if( dynamic_driver == NULL )
    {
        ret = DTC_USAGE;
    }

    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );

    if( ret == DTC_NONE )
    {
        if( interface_data == NULL )
        {
            ret = DTC_USAGE;
        }
    }

    if( ret == DTC_NONE )
    {
        ret = util_get_option(
            dynamic_driver->node_config->arg_cnt,
            (char * const *) dynamic_driver->node_config->arg_list,
            PALOPT_STRING,
            PALOPT_CHARS[PALOPT_DEBUG],
            NULL,
            0 );
    }

    if( ret == DTC_NONE )
    {
        interface_data->palopts.options[PALOPT_DEBUG] = 1;
    }
    else if( ret == DTC_UNAVAILABLE )
    {
        interface_data->palopts.options[PALOPT_DEBUG] = 0;

        // okay
        ret = DTC_NONE;
    }
    else
    {
        psync_log_error( "error processing option '%c' - check help for usage",
                PALOPT_CHARS[ PALOPT_DEBUG ] );
    }


    return ret;
}


//
static int parse_option_test( ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;


    if( dynamic_driver == NULL )
    {
        ret = DTC_USAGE;
    }

    interface_data_s * const interface_data = GET_INTERFACE_DATA( dynamic_driver );

    if( ret == DTC_NONE )
    {
        if( interface_data == NULL )
        {
            ret = DTC_USAGE;
        }
    }

    if( ret == DTC_NONE )
    {
        ret = util_get_option(
            dynamic_driver->node_config->arg_cnt,
            (char * const *) dynamic_driver->node_config->arg_list,
            PALOPT_STRING,
            PALOPT_CHARS[PALOPT_TEST],
            NULL,
            0 );
    }

    if( ret == DTC_NONE )
    {
        interface_data->palopts.options[PALOPT_TEST] = 1;
    }
    else if( ret == DTC_UNAVAILABLE )
    {
        interface_data->palopts.options[PALOPT_TEST] = 0;

        // okay
        ret = DTC_NONE;
    }
    else
    {
        psync_log_error( "error processing option '%c' - check help for usage",
                PALOPT_CHARS[ PALOPT_TEST ] );
    }


    return ret;
}




// *****************************************************
// public definitions
// *****************************************************


//
int parse_options( ps_dynamic_driver * const dynamic_driver )
{
    int ret = DTC_NONE;


    if( dynamic_driver == NULL )
    {
        ret = DTC_USAGE;
    }

    if( ret == DTC_NONE )
    {
        ret = parse_option_debug( dynamic_driver );
    }

    if( ret == DTC_NONE )
    {
        ret = parse_option_test( dynamic_driver );
    }


    return ret;
}
