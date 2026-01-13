/*
 * Utility Functions
 */

#include <stdio.h>
#include <string.h>
#include "rkmpp_mjpeg.h"

#define RKMPP_VERSION "1.0.0"

/**
 * Get required NV12 buffer size
 * NV12: Y plane (width * height) + UV plane (width * height / 2)
 * Total: width * height * 3 / 2
 */
uint32_t rkmpp_get_nv12_size(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0) {
        return 0;
    }
    
    return (width * height * 3) / 2;
}

/**
 * Get error message string
 */
const char* rkmpp_get_error_string(RkmppStatus status)
{
    switch (status) {
        case RKMPP_OK:
            return "Success";
        case RKMPP_ERR_INVALID_PARAM:
            return "Invalid parameter";
        case RKMPP_ERR_MEMORY:
            return "Memory allocation failed";
        case RKMPP_ERR_INIT:
            return "Initialization failed";
        case RKMPP_ERR_ENCODE:
            return "Encoding failed";
        case RKMPP_ERR_DECODE:
            return "Decoding failed";
        case RKMPP_ERR_TIMEOUT:
            return "Operation timeout";
        case RKMPP_ERR_NOT_READY:
            return "Data not ready";
        case RKMPP_ERR_UNKNOWN:
        default:
            return "Unknown error";
    }
}

/**
 * Get library version
 */
const char* rkmpp_get_version(void)
{
    return RKMPP_VERSION;
}
