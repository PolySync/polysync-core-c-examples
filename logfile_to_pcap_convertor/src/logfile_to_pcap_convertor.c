/*
 * Copyright (c) 2016 HARBRICK TECHNOLOGIES, INC
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * \example logfile_iterator.c
 *
 * Shows how to use the Logfile API routines to iterate over a Velodyne HDL
 * PolySync logfile (.plog file), outside the normal replay time domain.
 * 
 * Note: PCAP must be installed to the system: `apt-get install pcaputils`
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pcap/pcap.h>
#include <net/ethernet.h>

// API headers
#include "polysync_core.h"
#include "polysync_sdf.h"
#include "polysync_node.h"
#include "polysync_message.h"
#include "polysync_logfile.h"

#include "velodyne_hdl_driver.h"




// *****************************************************
// static global types/macros
// *****************************************************

//
typedef struct
{
    //
    // node reference
    ps_node_ref node_ref;
    //
    // message type for 'ps_byte_array_msg'
    ps_msg_type byte_array_msg_type;
    //
    // pcap handle
    pcap_t *pcap;
    //
    // pcap buffer
    pcap_dumper_t *dumper;
    //
    //
    char in_file[PSYNC_DEFAULT_STRING_LEN];
    //
    //
    char out_file[PSYNC_DEFAULT_STRING_LEN];
} context_s;




// *****************************************************
// static global data
// *****************************************************

/**
 * @brief PolySync node name.
 *
 */
static const char NODE_NAME[] = "velodyne-logfile-to-pcap";


/**
 * @brief PolySync 'ps_byte_array_msg' type name, which is where the Velodyne
 * data is stored within the .plog file.
 *
 */
static const char BYTE_ARRAY_MSG_NAME[] = "ps_byte_array_msg";


static unsigned char *global_buffer = NULL;
static unsigned long global_buffer_size = 0;




// *****************************************************
// static declarations
// *****************************************************

/**
 * @brief Logfile iterator callback.
 *
 * @warning When logfile is empty, expect:
 * \li \ref ps_logfile_attributes.data_count == 0
 * \li msg_type == \ref PSYNC_MSG_TYPE_INVALID
 * \li log_record == NULL
 *
 * @param [in] file_attributes Logfile attributes loaded by the logfile API.
 * @param [in] msg_type Message type identifier for the message in \ref ps_rnr_log_record.data, as seen by this data model.
 * @param [in] log_record Logfile record loaded by the logfile API.
 * @param [in] user_data A pointer to user data, provided by \ref psync_logfile_foreach_iterator.
 *
 */
static void logfile_iterator_callback(
        const ps_logfile_attributes * const file_attributes,
        const ps_msg_type msg_type,
        const ps_rnr_log_record * const log_record,
        void * const user_data );




// *****************************************************
// static definitions
// *****************************************************

//
static int resize_global_buffer( const unsigned long size )
{
    int ret = DTC_NONE;


    if( size > global_buffer_size )
    {
        global_buffer = realloc( global_buffer, size * sizeof(*global_buffer) );
        if( global_buffer == NULL )
        {
            printf( "failed to allocate buffer\n" );
            ret = DTC_MEMERR;
        }

        global_buffer_size = size;
    }


    return ret;
}


//
static void logfile_iterator_callback(
        const ps_logfile_attributes * const file_attributes,
        const ps_msg_type msg_type,
        const ps_rnr_log_record * const log_record,
        void * const user_data )
{
    context_s * const context = (context_s*) user_data;

    // ignore if we don't have any context
    if( context == NULL )
    {
        return;
    }

    // if logfile is empty, only attributes are provided
    if( log_record != NULL )
    {
        // get the PolySync message reference
        const ps_msg_ref msg = (ps_msg_ref) log_record->data;

        // 'ps_byte_array_msg' type is where the Velodyne data payload is stored
        // within the .plog file
        if( msg_type == context->byte_array_msg_type )
        {
            // UDP velodyne packet header
            // this must exist in the PCAP file in order for Velodyne's Veloview 
            // to visualize the data
            const unsigned char udp_header[] =
            {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x60, 0x76,
                0x88, 0x20, 0x11, 0x8C, 0x08, 0x00, 0x45, 0x00,
                0x04, 0xD2, 0x00, 0x00, 0x40, 0x00, 0xFF, 0x11,
                0xB4, 0xA9, 0xC0, 0xA8, 0x01, 0xC9, 0xFF, 0xFF,
                0xFF, 0xFF, 0x09, 0x40, 0x09, 0x40, 0x04, 0xBE,
                0x00, 0x00
            };

            //
            const ps_byte_array_msg * const byte_array_msg = (ps_byte_array_msg*) msg;

            //
            (void) resize_global_buffer( byte_array_msg->bytes._length + sizeof(udp_header) );

            // copy UDP header
            memcpy( global_buffer, udp_header, sizeof(udp_header) );

            // copy payload
            memcpy( &global_buffer[ sizeof(udp_header) ], (char*) byte_array_msg->bytes._buffer, (unsigned long) byte_array_msg->bytes._length );

            // create the pcap header
            struct pcap_pkthdr pcap_header;

            memset( &pcap_header, 0, sizeof(pcap_header) );

            // must set the pcap header values to create a valid packet
            pcap_header.caplen = (bpf_u_int32) byte_array_msg->bytes._length + sizeof(udp_header);
            pcap_header.len = pcap_header.caplen;

            // UTC micro second to struct timeval (second.microsecond)
            pcap_header.ts.tv_sec = byte_array_msg->header.timestamp / 1000000;
            pcap_header.ts.tv_usec = (byte_array_msg->header.timestamp % 1000000);

            // write
            pcap_dump( (u_char*) context->dumper, &pcap_header, (u_char*) global_buffer );
        }
    }
}




// *****************************************************
// main
// *****************************************************
int main( int argc, char **argv )
{
    // polysync return status
    int ret = DTC_NONE;

    // context data
    context_s context;


    memset( &context, 0, sizeof(context) );

    // first argument in input file path, output file path is (IN).pcap
    if( (argc == 2) && (strlen(argv[1]) > 0) )
    {
        strncpy( context.in_file, argv[1], sizeof(context.in_file) );
        snprintf( context.out_file, sizeof(context.out_file) , "%s.pcap", context.in_file );
    }
    else
    {
        printf( "usage\n$program <input_file>.plog\n" );
        return EXIT_FAILURE;
    }

    printf( "\n\n" );
    printf( "input file: '%s'\n", context.in_file );
    printf( "output file: '%s'\n", context.out_file );
    printf( "\n" );

    //
    context.pcap = pcap_open_dead( DLT_EN10MB, 65535 );

    //
    context.dumper = pcap_dump_open( context.pcap, context.out_file );

    // init core API
    ret = psync_init(
            NODE_NAME,
            PSYNC_NODE_TYPE_API_USER,
            PSYNC_DEFAULT_DOMAIN,
            PSYNC_SDF_ID_INVALID,
            PSYNC_INIT_FLAG_STDOUT_LOGGING,
            &context.node_ref );

    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_init - ret: %d", ret );
        (void) psync_release( &context.node_ref );
        return EXIT_FAILURE;
    }

    // get the message type for 'ps_byte_array_msg'
    ret = psync_message_get_type_by_name(
            context.node_ref,
            BYTE_ARRAY_MSG_NAME,
            &context.byte_array_msg_type );

    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_message_get_type_by_name - ret: %d", ret );
        (void) psync_release( &context.node_ref );
        return EXIT_FAILURE;
    }

    ps_msg_ref msg = NULL;
    ps_msg_type type = 0;
    (void) psync_message_get_type_by_name( context.node_ref, "ps_rnr_msg", &type );
    (void) psync_message_alloc( context.node_ref, type, &msg );
    (void) psync_message_publish( context.node_ref, msg );

    // initialize logfile API resources
    ret = psync_logfile_init( context.node_ref );

    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_logfile_init - ret: %d", ret );
        (void) psync_release( &context.node_ref );
        return EXIT_FAILURE;
    }

    // iterate over the logfile data, which executes the callback function for
    // each record in the .plog file
    ret = psync_logfile_foreach_iterator(
            context.node_ref,
            context.in_file,
            logfile_iterator_callback,
            &context );

    if( global_buffer != NULL )
    {
        free( global_buffer );
        global_buffer = NULL;
    }

    if( context.dumper != NULL )
    {
        pcap_dump_close( context.dumper );
    }

    if( context.pcap != NULL )
    {
        pcap_close( context.pcap );
    }

    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_logfile_foreach_iterator - ret: %d", ret );
        (void) psync_release( &context.node_ref );
        return EXIT_FAILURE;
    }

    // release logfile API resources
    ret = psync_logfile_release( context.node_ref );

    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_logfile_release - ret: %d", ret );
        (void) psync_release( &context.node_ref );
        return EXIT_FAILURE;
    }

    // release core API
    ret = psync_release( &context.node_ref );

    if( ret != DTC_NONE )
    {
        psync_log_error( "psync_release - ret: %d", ret );
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}
