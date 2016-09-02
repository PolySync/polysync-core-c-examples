/**
 * @file velodyne_hdl_driver.h
 * @brief Velodyne HDL Hardware Driver.
 *
 * All native multi-byte values are little endian unless specified otherwise.
 *
 * Default UDP data port is 2368, web interface on port 80.
 * Default IP is 192.168.2.202.
 *
 * Uses the name space 'VELO' to shorthand the key word 'VELODYNE' on macros.
 *
 * PUBLIC_HEADER
 */




#ifndef VELODYNE_HDL_DRIVER_H
#define	VELODYNE_HDL_DRIVER_H




#include <inttypes.h>

#include "polysync_core.h"
#include "polysync_socket.h"




/**
 * @brief Default Velodyne HDL Ethernet UDP port number. [int]
 *
 */
#define VELO_HDL_DEFAULT_UDP_PORT (2368)


/**
 * @brief Maximum total size of Velodyne HDL message. [bytes]
 *
 */
#define VELO_HDL_MESSAGE_MAX_SIZE (1206)


/**
 * @brief Maximum detection range. [meters]
 *
 */
#define VELO_HDL32_MAX_RANGE (120.0)


/**
 * @brief Number of rotational increments. [1/100 degrees/bit]
 *
 */
#define VELO_HDL_ROTATION_ANGLE_COUNT (36000)


/**
 * @brief Maximum number of vertical lasers supported. [uint32_t]
 *
 */
#define VELO_HDL_LASERS_MAX (64)


/**
 * @brief Number of lasers in the Velodyne HDL32E. [uint32_t]
 *
 */
#define VELO_HDL32E_LASER_COUNT (32)


/**
 * @brief Number of \ref velodyne_hdl_laser_return_s structures in a \ref velodyne_hdl_firing_data_s structure. [uint32_t]
 *
 */
#define VELO_HDL_LASER_PER_FIRING (32)


/**
 * @brief Number of Velodyne HDL32E \ref velodyne_hdl_firing_data_s structures in a \ref velodyne_hdl_message_s structure. [uint32_t]
 *
 */
#define VELO_HDL32E_FIRING_PER_MESSAGE (12)


/**
 * @brief Block identifier value indicating data is in the 0-31 range. [uint16_t]
 *
 * Start offset in the laser return data and validate firing data.
 *
 * Valid block identifier value.
 *
 */
#define VELO_HDL_BLOCK_ID_0_TO_31 (0xEEFF)


/**
 * @brief Block identifier value indicating data is in the 32-63 range. [uint16_t]
 *
 * Used to offset the laser return data and validate firing data.
 *
 * Valid block identifier value.
 *
 */
#define VELO_HDL_BLOCK_ID_32_TO_63 (0xDDFF)


/**
 * @brief Invalid \ref velodyne_hdl_laser_return_s.distance value. [uint16_t]
 *
 * Means no return within maximum range or invalid return.
 *
 */
#define VELO_HDL_LASER_RETURN_DISTANCE_INVALID (0x0000)


/**
 * @brief Lowest \ref velodyne_hdl_laser_return_s.intensity value. [uint8_t]
 *
 * Lowest return energy.
 *
 */
#define VELO_HDL_LASER_RETURN_INTENSITY_MIN (0x00)


/**
 * @brief Highest \ref velodyne_hdl_laser_return_s.intensity value. [uint8_t]
 *
 * Highest return energy.
 *
 */
#define VELO_HDL_LASER_RETURN_INTENSITY_MAX (0xFF)


/**
 * @brief Convert uint16_t rotational position to double angle. [degrees]
 *
 */
#define VELO_HDL32E_ROTATION_TO_ANGLE( mangle ) ((((double) mangle) / 100.0))


/**
 * @brief Number of rotation units per degree. [uint16_t]
 *
 */
#define VELO_HDL32E_ROTATION_PER_DEGREE (100)


/**
 * @brief Convert uint16_t return distance to double. [meters]
 *
 */
#define VELO_HDL32E_DISTANCE_TO_METER( mrange ) ((((double) mrange) * 0.002))




// enforce 1 byte alignment so we can do linear packing
#pragma pack(push)
#pragma pack(1)


/**
 * @brief Velodyne HDL laser return data.
 *
 * Provides laser return distance and intensity information.
 *
 */
typedef struct
{
    //
    //
    uint16_t distance; /*!< Distance.
                        * Value \ref VELO_HDL_LASER_RETURN_DISTANCE_INVALID means invalid laser return. [2 millimeters/bit] */
    //
    //
    uint8_t intensity; /*!< Intensity. See \ref VELO_HDL_LASER_RETURN_INTENSITY_MIN and \ref VELO_HDL_LASER_RETURN_INTENSITY_MAX. */
} velodyne_hdl_laser_return_s;


/**
 * @brief Velodyne HDL firing data.
 *
 * Provides a block of vertical laser return information for a rotation angle.
 *
 */
typedef struct
{
    //
    //
    uint16_t block_id; /*!< Block idetifier. Identifies the information block.
                        * See \ref VELO_HDL_BLOCK_ID_0_TO_31 and \ref VELO_HDL_BLOCK_ID_32_TO_63 for valid values. */
    //
    //
    uint16_t rotational_pos; /*!< Rotation angle. [1/100 degrees/bit] */
    //
    //
    velodyne_hdl_laser_return_s laser_returns[VELO_HDL_LASER_PER_FIRING]; /*!< Laser returns. */
} velodyne_hdl_firing_data_s;



/**
 * @brief Velodyne HDL message data.
 *
 * Provides a list of firing data blocks and timing information.
 *
 */
typedef struct
{
    //
    //
    velodyne_hdl_firing_data_s firing_data[VELO_HDL32E_FIRING_PER_MESSAGE]; /*!< Firing data blocks. */
    //
    //
    uint32_t gps_timestamp; /*!< GPS synchronized timestamp. Time since the top of the hour. [microseconds] */
    //
    //
    uint8_t reserved_0; /*!< Reserved data. */
    //
    //
    uint8_t reserved_1; /*!< Reserved data. */
} velodyne_hdl_message_s;


// restore alignment
#pragma pack(pop)


/**
 * @brief Velodyne HDL32E laser correction data.
 *
 * Contains pre-calculated laser correction data.
 *
 */
typedef struct
{
    //
    //
    double azimuth; /*!< Azimuth. */
    //
    //
    double vertical; /*!< Vertical. [degrees] */
    //
    //
    double distance; /*!< Distance. */
    //
    //
    double vertical_offset; /*!< Vertical offset. */
    //
    //
    double horizontal_offset; /*!< Horizontal offset. */
    //
    //
    double sin_vertical; /*!< Sine vertical. */
    //
    //
    double cos_vertical; /*!< Cosine vertical. */
    //
    //
    double sin_vertical_offset; /*!< Sine vertical offset. */
    //
    //
    double cos_vertical_offset; /*!< Cosine vertical offset. */
} velodyne_hdl32e_laser_correction_s;


/**
 * @brief Velodyne HDL32E corrections data.
 *
 */
typedef struct
{
    //
    //
    velodyne_hdl32e_laser_correction_s laser_corrections[VELO_HDL_LASERS_MAX]; /*!< Correction for each laser. */
    //
    //
    double cos_table[VELO_HDL_ROTATION_ANGLE_COUNT]; /*!< Lookup table for cosine(rotational_position) . */
    //
    //
    double sin_table[VELO_HDL_ROTATION_ANGLE_COUNT]; /*!< Lookup table for sine(rotational_position) . */
} velodyne_hdl32e_corrections_s;




/**
 * @brief Initialize Velodyne HDL32E corrections data using an array of vertical corrections.
 *
 * @param [in] laser_vertical_corrections A pointer to double which specifies the input vertical corrections array.
 * @param [in] num_lasers Number of vertical corrections/lasers in the provided array.
 * @param [out] corrections A pointer to \ref velodyne_hdl32e_corrections_s which receives the initialization.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if frame is valid.
 * \li \ref DTC_USAGE if arguments invalid.
 *
 */
int velodyne_hdl32e_init_corrections(
        const double * const laser_vertical_corrections,
        const unsigned long num_lasers,
        velodyne_hdl32e_corrections_s * const corrections );


/**
 * @brief Configure UDP socket for Velodyne HDL device communication.
 *
 * @note Expects a valid TCP socket, created by the user.
 *
 * @note Sets the non-blocking IO socket option.
 *
 * @param [in] sock A pointer to \ref ps_socket which receives the configuration.
 * @param [in] address A pointer to char buffer which specifies the device IP address.
 * Value \ref PSYNC_SOCKET_ADDRESS_ANY is acceptable but not recommended.
 * @parm [in] port Port number to bind the socket to.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_CONFIG if socket/configuration is invalid.
 *
 */
int velodyne_hdl_configure_socket(
        ps_socket * const sock,
        const char * const address,
        const unsigned long port );


/**
 * @brief Check if byte buffer is a valid Velodyne HDL message.
 *
 * Checks for a valid buffer size.
 *
 * @param [in] buffer A pointer to char buffer which receives the validation.
 * @param [in] buffer_len Number of bytes in the buffer. [bytes]
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if frame is valid.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_DATAERR if message is invalid.
 *
 */
int velodyne_hdl_is_message_valid(
        const unsigned char * const buffer,
        const unsigned long buffer_len );


/**
 * @brief Check if data is a valid Velodyne HDL firing data.
 *
 * Checks for a valid block ID and firing rotation position.
 *
 * @param [in] firing_data A pointer to \ref velodyne_hdl_firing_data_s which receives the validation.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if frame is valid.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_DATAERR if data is invalid.
 *
 */
int velodyne_hdl_is_firing_data_valid(
        const velodyne_hdl_firing_data_s * const firing_data );


/**
 * @brief Check if data is a valid Velodyne HDL laser return.
 *
 * Checks for a valid return distance.
 *
 * @param [in] laser_return A pointer to \ref velodyne_hdl_laser_return_s which receives the validation.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if frame is valid.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_DATAERR if data is invalid.
 *
 */
int velodyne_hdl_is_laser_return_valid(
        const velodyne_hdl_laser_return_s * const laser_return );


/**
 * @brief Poll for an available valid Velodyne HDL message.
 *
 * Calls \ref velodyne_hdl_is_message_valid if a frame was received.
 *
 * @param [in] sock A pointer to \ref ps_socket which holds the configuration.
 * @param [out] buffer A pointer to unsigned char buffer which receives the data read.
 * @param [in] buffer_len Length of provided buffer.
 * @param [out] bytes_read A pointer to unsigned long which receives the count value.
 * @param [out] timestamp A pointer to \ref ps_timestamp which receives the receive timestamp value.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if frame is valid.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if timeout expired and/or no message available when in non-blocking mode (see \ref psync_socket_recv_from).
 * \li \ref DTC_DATAERR if frame is invalid.
 *
 */
int velodyne_hdl_read_message(
        ps_socket * const sock,
        unsigned char * const buffer,
        const unsigned long buffer_len,
        unsigned long * const bytes_read,
        ps_timestamp * const timestamp );




#endif	/* VELODYNE_HDL_DRIVER_H */
