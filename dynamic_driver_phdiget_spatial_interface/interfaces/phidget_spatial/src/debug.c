/**
 * @file phidget_spatial/src/debug.c
 * @brief Phidget Spatial Debugging Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "polysync_core.h"

#include "interface_data.h"
#include "debug.h"




// *****************************************************
// static global types/macros
// *****************************************************




// *****************************************************
// static global data
// *****************************************************




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
void debug_print_imu_msg(
        const ps_imu_msg * msg )
{
    if( msg != NULL )
    {
        printf( "ps_imu_msg\n" );

        printf( "  header.type: %lu\n",
                (unsigned long) msg->header.type );

        printf( "  header.timestamp: %llu\n",
                (unsigned long long) msg->header.timestamp );

        printf( "  header.src_guid: 0x%016llX (%llu)\n",
                (unsigned long long) msg->header.src_guid,
                (unsigned long long) msg->header.src_guid );

        printf( "  sensor_descriptor.id: %llu\n",
                (unsigned long long) msg->sensor_descriptor.id );

        printf( "  sensor_descriptor.type: %lu\n",
                (unsigned long) msg->sensor_descriptor.type );

        printf( "  sensor_descriptor.transform.parent_id: %llu\n",
                (unsigned long long) msg->sensor_descriptor.transform.parent_id );

        printf( "  timestamp: %llu\n",
                (unsigned long long) msg->timestamp );

        printf( "  native_timestamp.value: %llu\n",
                (unsigned long long) msg->native_timestamp.value );

        printf( "  rotation_rate[X]: %f rad/sec\n",
                (double) msg->rotation_rate[ X_INDEX ] );

        printf( "  rotation_rate[Y]: %f rad/sec\n",
                (double) msg->rotation_rate[ Y_INDEX ] );

        printf( "  rotation_rate[Z]: %f rad/sec\n",
                (double) msg->rotation_rate[ Z_INDEX ] );

        printf( "  acceleration[X]: %f m/sec^2\n",
                (double) msg->acceleration[ X_INDEX ] );

        printf( "  acceleration[Y]: %f m/sec^2\n",
                (double) msg->acceleration[ Y_INDEX ] );

        printf( "  acceleration[Z]: %f m/sec^2\n",
                (double) msg->acceleration[ Z_INDEX ] );
    }
}
