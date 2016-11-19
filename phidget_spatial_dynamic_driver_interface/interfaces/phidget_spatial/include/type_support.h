/**
 * @file phidget_spatial/include/type_support.h
 * @brief Phidget Spatial Generic Type Support Interface.
 *
 */




#ifndef TYPE_SUPPORT_H
#define TYPE_SUPPORT_H




#include "polysync_dynamic_driver.h"




/**
 * @brief PAL interface name.
 *
 * For PolySync provided nodes, this should match what the licensing framework uses.
 *
 */
extern const char INTERFACE_NAME[];


/**
 * @brief PAL interface description.
 *
 * Generic human readable description for the interface.
 *
 */
extern const char INTERFACE_DESCRIPTION[];


/**
 * @brief PAL interface knowledge base link.
 *
 * Link to the online documentation for this device
 *
 */
extern const char INTERFACE_SUPPORT_LINK[];


/**
 * @brief Data model modules used by the PAL interface.
 *
 * See \ref NUM_MODULES for number of elements.
 *
 */
extern const char * const INTERFACE_MODULES[];


/**
 * @brief Message type names used by the PAL interface.
 *
 * See \ref NUM_MESSAGE_TYPE_NAMES for number of elements.
 *
 */
extern const char * const INTERFACE_MESSAGE_TYPE_NAMES[];


/**
* @brief IMU message type name.
*
*/
extern const char IMU_MSG_NAME[];



/**
 * @brief Initialize type support.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which receives the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_CONFIG if SDF configuration invalid.
 * \li \ref DTC_OSERR if system missing core resources.
 *
 */
int init_type_support( ps_dynamic_driver * const dynamic_driver );


/**
 * @brief Release type support.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which receives the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 * \li \ref DTC_USAGE if arguments invalid.
 *
 */
int release_type_support( ps_dynamic_driver * const dynamic_driver );


/**
 * @brief Initialize RnR support.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which receives the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_OSERR if system missing core resources.
 *
 */
int init_rnr_support( ps_dynamic_driver * const dynamic_driver );


/**
 * @brief Initialize message meta data.
 *
 * @param [in] dynamic_driver A pointer to \ref ps_dynamic_driver which receives the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_OSERR if system missing core resources.
 *
 */
int init_message_meta_data( ps_dynamic_driver * const dynamic_driver );




#endif	/* TYPE_SUPPORT_H */
