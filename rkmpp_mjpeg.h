/*
 * RKMPP MJPEG Encoder/Decoder Library
 * 
 * A C library wrapper for Rockchip MPP hardware MJPEG encoding and decoding
 * Supports NV12 input format for encoding and NV12 output for decoding
 * 
 * Author: RKMPP MJPEG Library
 * License: MIT
 */

#ifndef RKMPP_MJPEG_H
#define RKMPP_MJPEG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* ============================================================================
 * Type Definitions and Constants
 * ============================================================================ */

/* Error codes */
typedef enum {
    RKMPP_OK = 0,                      /* Success */
    RKMPP_ERR_INVALID_PARAM = -1,      /* Invalid parameter */
    RKMPP_ERR_MEMORY = -2,             /* Memory allocation failed */
    RKMPP_ERR_INIT = -3,               /* Initialization failed */
    RKMPP_ERR_ENCODE = -4,             /* Encoding failed */
    RKMPP_ERR_DECODE = -5,             /* Decoding failed */
    RKMPP_ERR_TIMEOUT = -6,            /* Operation timeout */
    RKMPP_ERR_NOT_READY = -7,          /* Data not ready */
    RKMPP_ERR_UNKNOWN = -99            /* Unknown error */
} RkmppStatus;

/* Encoder/Decoder handle (opaque pointer) */
typedef struct RkmppEncoder RkmppEncoder;
typedef struct RkmppDecoder RkmppDecoder;

/* ============================================================================
 * MJPEG Encoder Interface
 * ============================================================================ */

/**
 * Encoder configuration structure
 */
typedef struct {
    uint32_t width;                    /* Image width in pixels */
    uint32_t height;                   /* Image height in pixels */
    uint32_t fps;                      /* Frames per second */
    uint32_t bitrate;                  /* Target bitrate in bps (0 for auto) */
    uint32_t quality;                  /* JPEG quality (0-100, default 80) */
    uint32_t gop;                      /* GOP size (for future use) */
} RkmppEncoderConfig;

/**
 * Create and initialize MJPEG encoder
 * 
 * @param config Encoder configuration
 * @return Encoder handle on success, NULL on failure
 */
RkmppEncoder* rkmpp_encoder_create(const RkmppEncoderConfig* config);

/**
 * Destroy encoder and release resources
 * 
 * @param encoder Encoder handle
 * @return RKMPP_OK on success, error code on failure
 */
RkmppStatus rkmpp_encoder_destroy(RkmppEncoder* encoder);

/**
 * Encode NV12 frame to MJPEG
 * 
 * @param encoder Encoder handle
 * @param nv12_data NV12 frame data (Y plane followed by UV plane)
 * @param nv12_size Total size of NV12 data in bytes
 * @param jpeg_data Output buffer for JPEG data
 * @param jpeg_size Maximum size of output buffer
 * @param jpeg_len Output parameter: actual size of encoded JPEG
 * @return RKMPP_OK on success, error code on failure
 */
RkmppStatus rkmpp_encoder_encode(
    RkmppEncoder* encoder,
    const uint8_t* nv12_data,
    uint32_t nv12_size,
    uint8_t* jpeg_data,
    uint32_t jpeg_size,
    uint32_t* jpeg_len
);

/**
 * Get encoder statistics
 * 
 * @param encoder Encoder handle
 * @param frames_encoded Output: total frames encoded
 * @param bytes_encoded Output: total bytes encoded
 * @return RKMPP_OK on success, error code on failure
 */
RkmppStatus rkmpp_encoder_get_stats(
    RkmppEncoder* encoder,
    uint64_t* frames_encoded,
    uint64_t* bytes_encoded
);

/* ============================================================================
 * MJPEG Decoder Interface
 * ============================================================================ */

/**
 * Decoder configuration structure
 */
typedef struct {
    uint32_t max_width;                /* Maximum image width */
    uint32_t max_height;               /* Maximum image height */
    uint32_t output_format;            /* Output format (currently only NV12) */
} RkmppDecoderConfig;

/**
 * Decoded frame information
 */
typedef struct {
    uint32_t width;                    /* Actual frame width */
    uint32_t height;                   /* Actual frame height */
    uint32_t format;                   /* Frame format (NV12) */
    uint64_t timestamp;                /* Frame timestamp */
} RkmppFrameInfo;

/**
 * Create and initialize MJPEG decoder
 * 
 * @param config Decoder configuration
 * @return Decoder handle on success, NULL on failure
 */
RkmppDecoder* rkmpp_decoder_create(const RkmppDecoderConfig* config);

/**
 * Destroy decoder and release resources
 * 
 * @param decoder Decoder handle
 * @return RKMPP_OK on success, error code on failure
 */
RkmppStatus rkmpp_decoder_destroy(RkmppDecoder* decoder);

/**
 * Decode MJPEG to NV12 frame
 * 
 * @param decoder Decoder handle
 * @param jpeg_data JPEG data buffer
 * @param jpeg_size Size of JPEG data
 * @param nv12_data Output buffer for NV12 data
 * @param nv12_size Maximum size of output buffer
 * @param nv12_len Output parameter: actual size of decoded NV12
 * @param frame_info Output parameter: decoded frame information
 * @return RKMPP_OK on success, error code on failure
 */
RkmppStatus rkmpp_decoder_decode(
    RkmppDecoder* decoder,
    const uint8_t* jpeg_data,
    uint32_t jpeg_size,
    uint8_t* nv12_data,
    uint32_t nv12_size,
    uint32_t* nv12_len,
    RkmppFrameInfo* frame_info
);

/**
 * Get decoder statistics
 * 
 * @param decoder Decoder handle
 * @param frames_decoded Output: total frames decoded
 * @param bytes_decoded Output: total bytes decoded
 * @return RKMPP_OK on success, error code on failure
 */
RkmppStatus rkmpp_decoder_get_stats(
    RkmppDecoder* decoder,
    uint64_t* frames_decoded,
    uint64_t* bytes_decoded
);

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

/**
 * Get required NV12 buffer size for given dimensions
 * 
 * @param width Image width
 * @param height Image height
 * @return Required buffer size in bytes
 */
uint32_t rkmpp_get_nv12_size(uint32_t width, uint32_t height);

/**
 * Get error message string
 * 
 * @param status Error code
 * @return Error message string
 */
const char* rkmpp_get_error_string(RkmppStatus status);

/**
 * Get library version
 * 
 * @return Version string (e.g., "1.0.0")
 */
const char* rkmpp_get_version(void);

#ifdef __cplusplus
}
#endif

#endif /* RKMPP_MJPEG_H */
