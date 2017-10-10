
#ifndef PS_VIDEO_LOG_UTILS
#define PS_VIDEO_LOG_UTILS

typedef enum {
    OUTPUT_BMP = 1,
    OUTPUT_PPM
} output_format_e;

typedef struct
{
    ps_node_ref node_ref;
    ps_msg_type image_data_msg_type;
    unsigned long long img_count;
    output_format_e output_format;
    char output_dir[1024];
    char logfile_path[1024];
} context_s;

#pragma pack(push,1)
typedef struct {
    uint16_t bitmap_type;
    int32_t file_size;
    int16_t reserved1;
    int16_t reserved2;
    uint32_t offset_bits;
} bitmap_file_header;

typedef struct {
    uint32_t size_header;
    uint32_t width;
    uint32_t height;
    int16_t planes;
    int16_t bit_count;
    uint32_t compression;
    uint32_t image_size;
    uint32_t ppm_x;
    uint32_t ppm_y;
    uint32_t clr_used;
    uint32_t clr_important;
} bitmap_image_header;
#pragma pack(pop)

int set_uvc_frame_format(
    const ps_pixel_format_kind ps_format,
    enum uvc_frame_format * const uvc_format);

/**
 * @brief Write ps_image_data_msg to ppm (portable pixmap) image file.
 *
 * @param [in] image_data_msg Valid non-null ps_image_data_msg whose values will populate the image file.
 * @param [in] context application context data.
 *
 * @return DTC code:
 * \li DTC_NONE (zero) if success.
*/
int output_ppm(
    const ps_image_data_msg * const image_data_msg,
    context_s * const context);

/**
 * @brief Write ps_image_data_msg to ppm (bitmap) image file.
 *
 * @param [in] image_data_msg Valid non-null ps_image_data_msg whose values will populate the image file.
 * @param [in] context application context data.
 *
 * @return DTC code:
 * \li DTC_NONE (zero) if success.
*/
int output_bmp(
    const ps_image_data_msg * const image_data_msg,
    context_s * const context);

/**
 * @brief Initialize application context.
 *
 * @param [in] context application context data.
 * @param [in] logfile_path Path to logfile. If NULL logfile path must be set by the user.
 *
 * @return DTC code:
 * \li DTC_NONE (zero) if success.
*/
int init_context(context_s * const context, const char * logfile_path);

#endif // PS_VIDEO_LOG_UTILS