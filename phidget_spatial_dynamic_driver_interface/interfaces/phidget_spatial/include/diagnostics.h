/**
 * @file phidget_spatial/include/diagnostics.h
 * @brief Phidget Spatial Diagnostic Utilities Interface.
 *
 */




#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H




#include "polysync_dynamic_driver.h"



//
int validate_interface(
        ps_dynamic_driver * const dynamic_driver );


//
int recover_dtc_nointerface(
        ps_dynamic_driver * const dynamic_driver );




#endif /* DIAGNOSTICS_H */
