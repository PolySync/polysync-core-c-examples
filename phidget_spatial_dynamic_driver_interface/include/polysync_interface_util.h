/*
 * Copyright (c) 2016 PolySync
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
 * @file polysync_interface_util.h
 * @brief Dynamic Driver Utility Interface.
 *
 * PUBLIC_HEADER
 */




#ifndef POLYSYNC_INTERFACE_UTIL_H
#define POLYSYNC_INTERFACE_UTIL_H




#include "polysync_dynamic_driver.h"
#include <math.h>




/**
 * @brief Convert radians to degrees.
 *
 * angle = 180 / pi
 *
 */
#ifndef RAD_TO_DEG
#define RAD_TO_DEG(rad) ((rad)*(180.0/M_PI))
#endif


/**
 * @brief Convert degrees to radians.
 *
 * angle = pi / 180
 *
 */
#ifndef DEG_TO_RAD
#define DEG_TO_RAD(deg) ((deg)*(M_PI/180.0))
#endif


/**
 * @brief Minimum of a and b.
 *
 */
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif


/**
 * @brief Maximum of a and b.
 *
 */
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif


/**
 * @brief Absolute value of x.
 *
 */
#ifndef ABS
#define ABS(x) ((x)>0?(x):-(x))
#endif


/**
 * @brief Constrain amount to the range low:high.
 *
 */
#ifndef CONSTRAIN
#define CONSTRAIN(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif


/**
 * @brief Square of x.
 *
 */
#ifndef SQ
#define SQ(x) ((x)*(x))
#endif


/**
 * @brief Get the length of a static array.
 *
 */
#define PSYNC_STATIC_ARRAY_LENGTH(m_array) (sizeof(m_array) / sizeof(m_array[0]))


/**
 * @brief DDS sequence static initializer.
 *
 */
#define PSYNC_INIT_DDS_SEQUENCE(m_len,m_max,m_rel,m_buf) \
{ \
    ._length = m_len, \
    ._maximum = m_max, \
    ._release = m_rel, \
    ._buffer = m_buf \
}


/**
 * @brief Initialize a DDS sequence using a static array as the buffer.
 *
 */
#define PSYNC_INIT_DDS_SEQUENCE_STATIC_ARRAY(m_array) \
{ \
    ._length = PSYNC_STATIC_ARRAY_LENGTH(m_array), \
    ._maximum = PSYNC_STATIC_ARRAY_LENGTH(m_array), \
    ._release = 0, \
    ._buffer = &m_array[0] \
}


/**
 * @brief Initialize an empty DDS sequence.
 *
 */
#define PSYNC_INIT_DDS_SEQUENCE_NULL() \
    PSYNC_INIT_DDS_SEQUENCE(0,0,0,NULL)


/**
 * @brief Initialize a DDS character sequence using a static c-string as the buffer.
 *
 */
#define PSYNC_INIT_DDS_CHAR_SEQUENCE_STRING(m_u) \
    PSYNC_INIT_DDS_SEQUENCE(sizeof(m_u),sizeof(m_u),0,(DDS_char*)m_u)


/**
 * @brief SDF parameter value type (\ref ps_sdf_parameter_value) static initializer.
 *
 */
#define PSYNC_INIT_SDF_PARAMETER_TYPE(m_id,m_type) \
{ \
    .id = PSYNC_SDF_ID_INVALID, \
    .parameter = \
    { \
        .id = m_id, \
        .value = { ._d = m_type } \
    } \
}


/**
 * @brief SDF parameter unknown value (\ref ps_parameter_value) static initializer.
 *
 */
#define PSYNC_INIT_SDF_PARAMETER_VALUE_NA() \
{ \
    ._d = PARAMETER_VALUE_UNKNOWN, \
    ._u.ull_value = 0 \
}


/**
 * @brief SDF parameter unsigned long long value (\ref ps_parameter_value) static initializer.
 *
 */
#define PSYNC_INIT_SDF_PARAMETER_VALUE_ULL(m_u) \
{ \
    ._d = PARAMETER_VALUE_ULONGLONG, \
    ._u.ull_value = m_u \
}


/**
 * @brief SDF parameter long long value (\ref ps_parameter_value) static initializer.
 *
 */
#define PSYNC_INIT_SDF_PARAMETER_VALUE_LL(m_u) \
{ \
    ._d = PARAMETER_VALUE_LONGLONG, \
    ._u.ll_value = m_u \
}


/**
 * @brief SDF parameter double value (\ref ps_parameter_value) static initializer.
 *
 */
#define PSYNC_INIT_SDF_PARAMETER_VALUE_DOUBLE(m_u) \
{ \
    ._d = PARAMETER_VALUE_DOUBLE, \
    ._u.d_value = m_u \
}


/**
 * @brief SDF parameter string value (\ref ps_parameter_value) static initializer.
 *
 */
#define PSYNC_INIT_SDF_PARAMETER_VALUE_STRING(m_u) \
{ \
    ._d = PARAMETER_VALUE_STRING, \
    ._u.str_value = PSYNC_INIT_DDS_SEQUENCE(sizeof(m_u),sizeof(m_u),0,(DDS_char*)m_u) \
}


/**
 * @brief SDF parameter NULL string value (\ref ps_parameter_value) static initializer.
 *
 */
#define PSYNC_INIT_SDF_PARAMETER_VALUE_NULL_STRING() \
{ \
    ._d = PARAMETER_VALUE_STRING, \
    ._u.str_value = PSYNC_INIT_DDS_SEQUENCE(0,0,0,NULL) \
}


/**
 * @brief SDF core parameter (\ref ps_parameter) static initializer.
 *
 */
#define PSYNC_INIT_SDF_CORE_PARAMETER(m_id,m_flags,init_value,init_min,init_max,init_step) \
{ \
    .id = m_id, \
    .timestamp = 0, \
    .value = init_value, \
    .min = init_min, \
    .max = init_max, \
    .step = init_step, \
    .flags = (PSYNC_PARAMETER_FLAG_READONLY_MIN | PSYNC_PARAMETER_FLAG_READONLY_MAX | PSYNC_PARAMETER_FLAG_READONLY_STEP | m_flags), \
    .description = PSYNC_INIT_DDS_SEQUENCE(0,0,0,NULL), \
}


/**
 * @brief SDF custom parameter (\ref ps_parameter) static initializer.
 *
 */
#define PSYNC_INIT_SDF_CUSTOM_PARAMETER(m_id,m_flags,init_value,init_min,init_max,init_step,desc) \
{ \
    .id = m_id, \
    .timestamp = 0, \
    .value = init_value, \
    .min = init_min, \
    .max = init_max, \
    .step = init_step, \
    .flags = (PSYNC_PARAMETER_FLAG_USERDEFINED | PSYNC_PARAMETER_FLAG_READONLY_MIN | PSYNC_PARAMETER_FLAG_READONLY_MAX | PSYNC_PARAMETER_FLAG_READONLY_STEP | m_flags), \
    .description = PSYNC_INIT_DDS_SEQUENCE(sizeof(desc),sizeof(desc),0,(DDS_char*)desc), \
}


/**
 * @brief SDF IO configuration (\ref ps_sdf_io_configuration) static initializer.
 *
 */
#define PSYNC_INIT_SDF_IO_CONFIGURATION(params_len,params_buf) \
{ \
    .id = PSYNC_SDF_ID_INVALID, \
    .name = PSYNC_INIT_DDS_SEQUENCE(0,0,0,NULL), \
    .parameter_values = PSYNC_INIT_DDS_SEQUENCE(params_len,params_len,0,params_buf) \
}


/**
 * @brief SDF sensor configuration (\ref ps_sdf_sensor_configuration) static initializer.
 *
 */
#define PSYNC_INIT_SDF_SENSOR_CONFIGURATION(io_configs_len,io_configs_buf,params_len,params_buf) \
{ \
    .id = PSYNC_SDF_ID_INVALID, \
    .name = PSYNC_INIT_DDS_SEQUENCE(0,0,0,NULL), \
    .io_configs = PSYNC_INIT_DDS_SEQUENCE(io_configs_len,io_configs_len,0,io_configs_buf), \
    .parameter_values = PSYNC_INIT_DDS_SEQUENCE(params_len,params_len,0,params_buf) \
}


/**
 * @brief SDF node configuration (\ref ps_sdf_node_configuration) static initializer.
 *
 */
#define PSYNC_INIT_SDF_NODE_CONFIGURATION(sensor_configs_len,sensor_configs_buf,params_len,params_buf) \
{ \
    .id = PSYNC_SDF_ID_INVALID, \
    .name = PSYNC_INIT_DDS_SEQUENCE(0,0,0,NULL), \
    .enabled = 1, \
    .sensor_configs = PSYNC_INIT_DDS_SEQUENCE(sensor_configs_len,sensor_configs_len,0,sensor_configs_buf), \
    .parameter_values = PSYNC_INIT_DDS_SEQUENCE(params_len,params_len,0,params_buf) \
}


/**
 * @brief Parameter group option list item (\ref ps_parameter_group_option_list_item) static initializer.
 *
 */
#define PSYNC_INIT_PARAMETER_GROUP_OPTION_LIST_ITEM(m_label_id,m_value_id) \
{ \
    .label_id = m_label_id, \
    .value_id = m_value_id \
}


/**
 * @brief Parameter group list (\ref ps_parameter_group) static initializer.
 *
 */
#define PSYNC_INIT_PARAMETER_GROUP_LIST(m_id,m_name_seq,m_parent_id,m_toggle_id,m_id_seq) \
{ \
    .kind = PSYNC_PARAMETER_GROUP_LIST, \
    .list = \
    { \
        .id = m_id, \
        .parameter_list_id = 0, \
        .parent_id = m_parent_id, \
        .name = m_name_seq, \
        .toggle_parameter_id = m_toggle_id, \
        .item_ids = m_id_seq \
    } \
}


/**
 * @brief Parameter group option list (\ref ps_parameter_group) static initializer.
 *
 */
#define PSYNC_INIT_PARAMETER_GROUP_OPTION_LIST(m_id,m_name_seq,m_parent_id,m_value_id,m_options_seq) \
{ \
    .kind = PSYNC_PARAMETER_GROUP_OPTION_LIST, \
    .option_list = \
    { \
        .id = m_id, \
        .parameter_list_id = 0, \
        .parent_id = m_parent_id, \
        .name = m_name_seq, \
        .value_id = m_value_id, \
        .options = m_options_seq \
    } \
}


/**
 * @brief Parameter group value range (\ref ps_parameter_group) static initializer.
 *
 */
#define PSYNC_INIT_PARAMETER_GROUP_VALUE_RANGE(m_id,m_name_seq,m_parent_id,m_min_id,m_max_id) \
{ \
    .kind = PSYNC_PARAMETER_GROUP_VALUE_RANGE, \
    .value_range = \
    { \
        .id = m_id, \
        .parameter_list_id = 0, \
        .parent_id = m_parent_id, \
        .name = m_name_seq, \
        .min_id = m_min_id, \
        .max_id = m_max_id \
    } \
}


/**
 * @brief Parameter group bounded value range (\ref ps_parameter_group) static initializer.
 *
 */
#define PSYNC_INIT_PARAMETER_GROUP_VALUE_RANGE_BOUNDED(m_id,m_name_seq,m_parent_id,m_min_id,m_max_id) \
{ \
    .kind = PSYNC_PARAMETER_GROUP_VALUE_RANGE_BOUNDED, \
    .value_range = \
    { \
        .id = m_id, \
        .parameter_list_id = 0, \
        .parent_id = m_parent_id, \
        .name = m_name_seq, \
        .min_id = m_min_id, \
        .max_id = m_max_id \
    } \
}


/**
 * @brief Parameter group SDF exposure map (\ref ps_parameter_group_sdf_exposure_map) static initializer.
 *
 */
#define PSYNC_INIT_PARAMETER_GROUP_SDF_EXPOSURE_MAP(m_group_id,m_node_config_idx,m_sensor_config_idx,m_io_config_idx) \
{ \
    .group_id = m_group_id, \
    .node_config_index = m_node_config_idx, \
    .sensor_config_index = m_sensor_config_idx, \
    .io_config_index = m_io_config_idx \
}




/**
 * @brief Standard options string.
 *
 */
extern const char           STDOPT_STRING[];




// if optarg is null, does not check for opt
int util_get_option( const int argc, char * const * argv, const char * optstring, const char optchar, char * const arg_buffer, const unsigned long arg_buffer_len );


//
int util_string_to_sdf_key( const char * const string, unsigned long * const key );


//
void util_print_options_help( const char * const * help_strings, const char * const * descriptions, const unsigned long num_options );


//
void util_print_sensor_support_help(
        const ps_sensor_support_descriptor * const sensor_support_descriptors,
        const unsigned long num_descriptors );


//
void util_print_sdf_support_help( const ps_sdf_node_configuration * const sdf_node_configuration );


//
void util_print_parameters_help( const ps_parameter * const parameters, const unsigned long num_parameters );


//
void util_print_type_support_help(
        const char * const interface_name,
        const char * const * module_names,
        const unsigned long num_modules,
        const char * const * message_type_names,
        const unsigned long num_message_types );


//
int util_resize_octet_sequence( DDS_sequence_octet * const seq, const unsigned long max );


//
int util_resize_char_sequence( DDS_sequence_char * const seq, const unsigned long max );


//
int util_resize_parameter_sequence( DDS_sequence_ps_parameter * const seq, const unsigned long max );


//
int util_resize_parameter_value_sequence( DDS_sequence_ps_parameter_value * const seq, const unsigned long max );


// search all entities for the given parameter id
ps_sdf_parameter_value * util_search_for_sdf_parameter( const ps_sdf_node_configuration * const node_config, const ps_parameter_id id );


// search all entities for the given parameter id
ps_parameter * util_search_for_parameter( const ps_sdf_node_configuration * const node_config, const ps_parameter_id id );


//
int util_copy_parameter_value( const ps_parameter_value * const in, ps_parameter_value * const out );


//
int util_copy_parameter( const ps_parameter * const in, ps_parameter * const out );


//
int util_get_parameter_value_string( const ps_parameter_value * const value, char * const buffer, const unsigned long buffer_len );


// update SDF table if needed
int util_verify_sdf_sensor_support(
        const ps_sensor_support_descriptor * const sensor_support_descriptors,
        const unsigned long num_descriptors );


// update SDF table if needed
int util_verify_sdf_parameter_support( const ps_parameter * const parameters, const unsigned long num_parameters );


//
int util_get_time_since( const ps_timestamp past_time, ps_timestamp * const difference );


//
int util_verify_api_version(
        const unsigned int major,
        const unsigned int minor );


//
int util_verify_module_version(
        const ps_node_ref const node_ref,
        const char * name,
        const unsigned int major,
        const unsigned int minor );


//
void util_flush_msg_queue(
        ps_node_ref const node_ref,
        void * const queue,
        const unsigned long max );




#endif	/* POLYSYNC_INTERFACE_UTIL_H */
