
#ifndef PS_VIDEO_LOG_UTILS
#define PS_VIDEO_LOG_UTILS

typedef struct
{
    ps_node_ref node_ref;
    ps_msg_type image_data_msg_type;
    char in_file[PSYNC_DEFAULT_STRING_LEN];
    char out_file[PSYNC_DEFAULT_STRING_LEN];
    unsigned long long img_count;
    unsigned int initialized;

    ps_video_encoder video_encoder;
    unsigned char *enocded_buffer;
    unsigned long enocded_frame_size;
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
    enum uvc_frame_format * uvc_format);

int output_ppm(
    const ps_image_data_msg * const image_data_msg,
    context_s * const context);

int output_bmp(
    const ps_image_data_msg * const image_data_msg,
    context_s * const context);



#endif // PS_VIDEO_LOG_UTILS