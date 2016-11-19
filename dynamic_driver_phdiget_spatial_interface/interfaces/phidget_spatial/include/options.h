/**
 * @file phidget_spatial/include/options.h
 * @brief Phidget Spatial Option Handling Interface.
 *
 */




#ifndef OPTIONS_H
#define OPTIONS_H




#include "polysync_dynamic_driver.h"




/**
 * @brief PAL interface options string.
 *
 */
extern const char PALOPT_STRING[];


/**
 * @brief PAL interface options character list.
 *
 * @note Order defines the indexing scheme.
 *
 */
extern const char PALOPT_CHARS[];


/**
 * @brief PAL interface options description list.
 *
 * @note Order must match the indexing scheme in \ref PALOPT_CHARS.
 *
 */
extern const char * const STDOPT_DESCRIPTIONS[];


/**
 * @brief PAL interface options help message list.
 *
 * @note Order must match the indexing scheme in \ref PALOPT_CHARS.
 *
 */
extern const char * const STDOPT_HELP_STRINGS[];




/**
 * @brief Parse command line arguments.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which receives the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_CONFIG if an invalid argument was detected.
 *
 */
int parse_options( ps_dynamic_driver * const dynamic_driver );




#endif	/* OPTIONS_H */
