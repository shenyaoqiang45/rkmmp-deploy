# RKMPP MJPEG Library - API Reference

## Overview

The RKMPP MJPEG Library provides a comprehensive C API for hardware-accelerated MJPEG encoding and decoding on Rockchip platforms. This document describes all public functions, data types, and error codes.

## Table of Contents

1. [Data Types](#data-types)
2. [Error Codes](#error-codes)
3. [Encoder API](#encoder-api)
4. [Decoder API](#decoder-api)
5. [Utility Functions](#utility-functions)

## Data Types

### RkmppStatus

```c
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
```

Status codes returned by API functions.

### RkmppEncoderConfig

```c
typedef struct {
    uint32_t width;                    /* Image width in pixels */
    uint32_t height;                   /* Image height in pixels */
    uint32_t fps;                      /* Frames per second */
    uint32_t bitrate;                  /* Target bitrate in bps (0 for auto) */
    uint32_t quality;                  /* JPEG quality (0-100, default 80) */
    uint32_t gop;                      /* GOP size (for future use) */
} RkmppEncoderConfig;
```

Configuration structure for MJPEG encoder initialization.

**Parameters:**
- `width`: Image width in pixels (16-4096)
- `height`: Image height in pixels (16-4096)
- `fps`: Frames per second (1-120)
- `bitrate`: Target bitrate in bits per second (0 for automatic)
- `quality`: JPEG quality level (0-100, default 80)
- `gop`: Group of Pictures size (reserved for future use)

### RkmppDecoderConfig

```c
typedef struct {
    uint32_t max_width;                /* Maximum image width */
    uint32_t max_height;               /* Maximum image height */
    uint32_t output_format;            /* Output format (currently only NV12) */
} RkmppDecoderConfig;
```

Configuration structure for MJPEG decoder initialization.

**Parameters:**
- `max_width`: Maximum image width (16-4096)
- `max_height`: Maximum image height (16-4096)
- `output_format`: Output format (0 for NV12)

### RkmppFrameInfo

```c
typedef struct {
    uint32_t width;                    /* Actual frame width */
    uint32_t height;                   /* Actual frame height */
    uint32_t format;                   /* Frame format (NV12) */
    uint64_t timestamp;                /* Frame timestamp */
} RkmppFrameInfo;
```

Information about a decoded frame.

**Parameters:**
- `width`: Actual width of the decoded frame
- `height`: Actual height of the decoded frame
- `format`: Frame format (0 for NV12)
- `timestamp`: Frame timestamp in milliseconds

## Error Codes

| Code | Value | Description |
|------|-------|-------------|
| RKMPP_OK | 0 | Operation completed successfully |
| RKMPP_ERR_INVALID_PARAM | -1 | Invalid parameter passed to function |
| RKMPP_ERR_MEMORY | -2 | Memory allocation failed |
| RKMPP_ERR_INIT | -3 | Initialization failed |
| RKMPP_ERR_ENCODE | -4 | Encoding operation failed |
| RKMPP_ERR_DECODE | -5 | Decoding operation failed |
| RKMPP_ERR_TIMEOUT | -6 | Operation timeout |
| RKMPP_ERR_NOT_READY | -7 | Data not ready for processing |
| RKMPP_ERR_UNKNOWN | -99 | Unknown error occurred |

## Encoder API

### rkmpp_encoder_create()

```c
RkmppEncoder* rkmpp_encoder_create(const RkmppEncoderConfig* config);
```

Create and initialize a new MJPEG encoder instance.

**Parameters:**
- `config`: Pointer to encoder configuration structure

**Returns:**
- Encoder handle on success
- NULL on failure

**Example:**
```c
RkmppEncoderConfig config = {
    .width = 1920,
    .height = 1080,
    .fps = 30,
    .quality = 80
};

RkmppEncoder* encoder = rkmpp_encoder_create(&config);
if (!encoder) {
    fprintf(stderr, "Failed to create encoder\n");
    return -1;
}
```

### rkmpp_encoder_destroy()

```c
RkmppStatus rkmpp_encoder_destroy(RkmppEncoder* encoder);
```

Destroy encoder and release all associated resources.

**Parameters:**
- `encoder`: Encoder handle

**Returns:**
- RKMPP_OK on success
- Error code on failure

**Example:**
```c
RkmppStatus status = rkmpp_encoder_destroy(encoder);
if (status != RKMPP_OK) {
    fprintf(stderr, "Failed to destroy encoder\n");
}
```

### rkmpp_encoder_encode()

```c
RkmppStatus rkmpp_encoder_encode(
    RkmppEncoder* encoder,
    const uint8_t* nv12_data,
    uint32_t nv12_size,
    uint8_t* jpeg_data,
    uint32_t jpeg_size,
    uint32_t* jpeg_len);
```

Encode an NV12 frame to MJPEG format.

**Parameters:**
- `encoder`: Encoder handle
- `nv12_data`: Pointer to NV12 frame data
- `nv12_size`: Size of NV12 data in bytes
- `jpeg_data`: Output buffer for JPEG data
- `jpeg_size`: Maximum size of output buffer
- `jpeg_len`: Output parameter for actual JPEG size

**Returns:**
- RKMPP_OK on success
- Error code on failure

**Example:**
```c
uint32_t nv12_size = rkmpp_get_nv12_size(1920, 1080);
uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
uint8_t* jpeg_data = (uint8_t*)malloc(nv12_size);
uint32_t jpeg_len = 0;

// ... fill nv12_data with image data ...

RkmppStatus status = rkmpp_encoder_encode(
    encoder, nv12_data, nv12_size,
    jpeg_data, nv12_size, &jpeg_len);

if (status == RKMPP_OK) {
    printf("Encoded %u bytes of JPEG data\n", jpeg_len);
}
```

### rkmpp_encoder_get_stats()

```c
RkmppStatus rkmpp_encoder_get_stats(
    RkmppEncoder* encoder,
    uint64_t* frames_encoded,
    uint64_t* bytes_encoded);
```

Get encoder statistics.

**Parameters:**
- `encoder`: Encoder handle
- `frames_encoded`: Output parameter for total frames encoded
- `bytes_encoded`: Output parameter for total bytes encoded

**Returns:**
- RKMPP_OK on success
- Error code on failure

## Decoder API

### rkmpp_decoder_create()

```c
RkmppDecoder* rkmpp_decoder_create(const RkmppDecoderConfig* config);
```

Create and initialize a new MJPEG decoder instance.

**Parameters:**
- `config`: Pointer to decoder configuration structure

**Returns:**
- Decoder handle on success
- NULL on failure

### rkmpp_decoder_destroy()

```c
RkmppStatus rkmpp_decoder_destroy(RkmppDecoder* decoder);
```

Destroy decoder and release all associated resources.

**Parameters:**
- `decoder`: Decoder handle

**Returns:**
- RKMPP_OK on success
- Error code on failure

### rkmpp_decoder_decode()

```c
RkmppStatus rkmpp_decoder_decode(
    RkmppDecoder* decoder,
    const uint8_t* jpeg_data,
    uint32_t jpeg_size,
    uint8_t* nv12_data,
    uint32_t nv12_size,
    uint32_t* nv12_len,
    RkmppFrameInfo* frame_info);
```

Decode MJPEG data to NV12 frame.

**Parameters:**
- `decoder`: Decoder handle
- `jpeg_data`: Pointer to JPEG data
- `jpeg_size`: Size of JPEG data in bytes
- `nv12_data`: Output buffer for NV12 data
- `nv12_size`: Maximum size of output buffer
- `nv12_len`: Output parameter for actual NV12 size
- `frame_info`: Output parameter for frame information

**Returns:**
- RKMPP_OK on success
- Error code on failure

### rkmpp_decoder_get_stats()

```c
RkmppStatus rkmpp_decoder_get_stats(
    RkmppDecoder* decoder,
    uint64_t* frames_decoded,
    uint64_t* bytes_decoded);
```

Get decoder statistics.

**Parameters:**
- `decoder`: Decoder handle
- `frames_decoded`: Output parameter for total frames decoded
- `bytes_decoded`: Output parameter for total bytes decoded

**Returns:**
- RKMPP_OK on success
- Error code on failure

## Utility Functions

### rkmpp_get_nv12_size()

```c
uint32_t rkmpp_get_nv12_size(uint32_t width, uint32_t height);
```

Calculate required buffer size for NV12 frame.

**Parameters:**
- `width`: Image width in pixels
- `height`: Image height in pixels

**Returns:**
- Required buffer size in bytes

**Formula:**
```
NV12 size = width * height * 3 / 2
```

**Example:**
```c
uint32_t size = rkmpp_get_nv12_size(1920, 1080);
// Returns: 1920 * 1080 * 3 / 2 = 3110400 bytes
```

### rkmpp_get_error_string()

```c
const char* rkmpp_get_error_string(RkmppStatus status);
```

Get human-readable error message for status code.

**Parameters:**
- `status`: Error code

**Returns:**
- Error message string

**Example:**
```c
RkmppStatus status = rkmpp_encoder_encode(...);
if (status != RKMPP_OK) {
    printf("Error: %s\n", rkmpp_get_error_string(status));
}
```

### rkmpp_get_version()

```c
const char* rkmpp_get_version(void);
```

Get library version string.

**Returns:**
- Version string (e.g., "1.0.0")

**Example:**
```c
printf("RKMPP MJPEG Library version: %s\n", rkmpp_get_version());
```

## Thread Safety

The library is thread-safe for multiple encoder/decoder instances. However, a single encoder or decoder instance should not be accessed from multiple threads simultaneously without external synchronization.

## Memory Management

- Buffers passed to encoding/decoding functions must be allocated and managed by the caller
- The library does not take ownership of input buffers
- Output buffers must be large enough to hold the result
- Use `rkmpp_get_nv12_size()` to calculate required buffer sizes

## Performance Considerations

- Use hardware-accelerated encoding/decoding for optimal performance
- Allocate buffers once and reuse them for multiple frames
- Consider using buffer pools for high-throughput scenarios
- Monitor statistics using `rkmpp_encoder_get_stats()` and `rkmpp_decoder_get_stats()`

