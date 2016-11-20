/**
 * @file phidget_spatial/include/getset.h
 * @brief Phidget Spatial Get/Set Interface.
 *
 */




#ifndef GETSET_H
#define GETSET_H




#include "polysync_dynamic_driver.h"




/**
 * @brief Register get/set variable mappings.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which receives the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 * \li \ref DTC_USAGE if arguments invalid.
 *
 */
int register_getset_variables( ps_dynamic_driver * const dynamic_driver );




#endif	/* GETSET_H */
