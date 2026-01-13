# RKMPP MJPEG Library - Usage Guide

## Introduction

This guide provides step-by-step instructions for using the RKMPP MJPEG Library in your projects. It covers installation, compilation, and practical usage examples.

## Table of Contents

1. [Installation](#installation)
2. [Building the Library](#building-the-library)
3. [Integration into Your Project](#integration-into-your-project)
4. [Basic Usage Examples](#basic-usage-examples)
5. [Advanced Topics](#advanced-topics)
6. [Troubleshooting](#troubleshooting)

## Installation

### Prerequisites

Before using the RKMPP MJPEG Library, ensure you have:

- A Rockchip-based system (RK3588, RK3568, RK3399, etc.)
- Linux kernel with MPP driver support
- Rockchip MPP library installed
- GCC compiler
- CMake (version 3.10 or later)

### System Setup

1. **Install Rockchip MPP library:**

   ```bash
   git clone https://github.com/rockchip-linux/mpp.git
   cd mpp
   mkdir build
   cd build
   cmake ..
   make
   sudo make install
   ```

2. **Set environment variables:**

   ```bash
   export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
   export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
   ```

## Building the Library

### Clone the Repository

```bash
git clone <repository_url>
cd rkmpp_mjpeg_lib
```

### Build Steps

1. **Create build directory:**

   ```bash
   mkdir build
   cd build
   ```

2. **Configure with CMake:**

   ```bash
   cmake ..
   ```

3. **Compile:**

   ```bash
   make
   ```

4. **Run tests:**

   ```bash
   ctest
   ```

5. **Install (optional):**

   ```bash
   sudo make install
   ```

## Integration into Your Project

### Linking the Library

#### Using CMake

Add to your `CMakeLists.txt`:

```cmake
# Find the library
find_library(RKMPP_MJPEG_LIB rkmpp_mjpeg)
find_path(RKMPP_MJPEG_INCLUDE rkmpp_mjpeg.h)

# Include directories
include_directories(${RKMPP_MJPEG_INCLUDE})

# Add your executable
add_executable(my_app main.c)

# Link the library
target_link_libraries(my_app ${RKMPP_MJPEG_LIB})
```

#### Using GCC Directly

```bash
gcc -o my_app main.c -lrkmpp_mjpeg -I/path/to/include -L/path/to/lib -lpthread
```

### Header Inclusion

```c
#include "rkmpp_mjpeg.h"
```

## Basic Usage Examples

### Example 1: Simple MJPEG Encoding

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rkmpp_mjpeg.h"

int main(void)
{
    // Configuration
    RkmppEncoderConfig config = {
        .width = 1920,
        .height = 1080,
        .fps = 30,
        .quality = 80
    };
    
    // Create encoder
    RkmppEncoder* encoder = rkmpp_encoder_create(&config);
    if (!encoder) {
        fprintf(stderr, "Failed to create encoder\n");
        return -1;
    }
    
    // Allocate buffers
    uint32_t nv12_size = rkmpp_get_nv12_size(config.width, config.height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    uint8_t* jpeg_data = (uint8_t*)malloc(nv12_size);
    
    if (!nv12_data || !jpeg_data) {
        fprintf(stderr, "Memory allocation failed\n");
        rkmpp_encoder_destroy(encoder);
        return -1;
    }
    
    // Fill with test pattern (in real code, read from camera/file)
    memset(nv12_data, 128, nv12_size);
    
    // Encode frame
    uint32_t jpeg_len = 0;
    RkmppStatus status = rkmpp_encoder_encode(
        encoder, nv12_data, nv12_size,
        jpeg_data, nv12_size, &jpeg_len
    );
    
    if (status == RKMPP_OK) {
        printf("Successfully encoded %u bytes of JPEG\n", jpeg_len);
        
        // Save to file
        FILE* fp = fopen("output.jpg", "wb");
        if (fp) {
            fwrite(jpeg_data, 1, jpeg_len, fp);
            fclose(fp);
        }
    } else {
        fprintf(stderr, "Encoding failed: %s\n", rkmpp_get_error_string(status));
    }
    
    // Cleanup
    free(jpeg_data);
    free(nv12_data);
    rkmpp_encoder_destroy(encoder);
    
    return 0;
}
```

### Example 2: Simple MJPEG Decoding

```c
#include <stdio.h>
#include <stdlib.h>
#include "rkmpp_mjpeg.h"

int main(void)
{
    // Configuration
    RkmppDecoderConfig config = {
        .max_width = 1920,
        .max_height = 1080
    };
    
    // Create decoder
    RkmppDecoder* decoder = rkmpp_decoder_create(&config);
    if (!decoder) {
        fprintf(stderr, "Failed to create decoder\n");
        return -1;
    }
    
    // Read JPEG file
    FILE* fp = fopen("input.jpg", "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open input file\n");
        rkmpp_decoder_destroy(decoder);
        return -1;
    }
    
    fseek(fp, 0, SEEK_END);
    uint32_t jpeg_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    uint8_t* jpeg_data = (uint8_t*)malloc(jpeg_size);
    fread(jpeg_data, 1, jpeg_size, fp);
    fclose(fp);
    
    // Allocate output buffer
    uint32_t nv12_size = rkmpp_get_nv12_size(config.max_width, config.max_height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    
    // Decode
    uint32_t nv12_len = 0;
    RkmppFrameInfo frame_info;
    RkmppStatus status = rkmpp_decoder_decode(
        decoder, jpeg_data, jpeg_size,
        nv12_data, nv12_size, &nv12_len, &frame_info
    );
    
    if (status == RKMPP_OK) {
        printf("Successfully decoded frame: %ux%u\n", 
               frame_info.width, frame_info.height);
        printf("NV12 data size: %u bytes\n", nv12_len);
    } else {
        fprintf(stderr, "Decoding failed: %s\n", rkmpp_get_error_string(status));
    }
    
    // Cleanup
    free(nv12_data);
    free(jpeg_data);
    rkmpp_decoder_destroy(decoder);
    
    return 0;
}
```

### Example 3: Real-time Encoding Loop

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rkmpp_mjpeg.h"

int main(void)
{
    RkmppEncoderConfig config = {
        .width = 640,
        .height = 480,
        .fps = 30,
        .quality = 80
    };
    
    RkmppEncoder* encoder = rkmpp_encoder_create(&config);
    if (!encoder) return -1;
    
    uint32_t nv12_size = rkmpp_get_nv12_size(config.width, config.height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    uint8_t* jpeg_data = (uint8_t*)malloc(nv12_size);
    
    // Encode 30 frames
    for (int i = 0; i < 30; i++) {
        // In real code: capture frame from camera
        memset(nv12_data, (i * 8) % 256, nv12_size);
        
        uint32_t jpeg_len = 0;
        RkmppStatus status = rkmpp_encoder_encode(
            encoder, nv12_data, nv12_size,
            jpeg_data, nv12_size, &jpeg_len
        );
        
        if (status == RKMPP_OK) {
            printf("Frame %d: %u bytes\n", i, jpeg_len);
        }
        
        // Simulate frame rate
        usleep(1000000 / config.fps);
    }
    
    // Print statistics
    uint64_t frames, bytes;
    rkmpp_encoder_get_stats(encoder, &frames, &bytes);
    printf("Total: %lu frames, %lu bytes\n", frames, bytes);
    
    // Cleanup
    free(jpeg_data);
    free(nv12_data);
    rkmpp_encoder_destroy(encoder);
    
    return 0;
}
```

## Advanced Topics

### Buffer Management

For high-performance applications, consider pre-allocating buffers:

```c
// Allocate once
uint32_t nv12_size = rkmpp_get_nv12_size(width, height);
uint8_t* nv12_buffer = (uint8_t*)malloc(nv12_size);
uint8_t* jpeg_buffer = (uint8_t*)malloc(nv12_size);

// Reuse for multiple frames
for (int i = 0; i < num_frames; i++) {
    // Fill nv12_buffer with frame data
    
    uint32_t jpeg_len = 0;
    rkmpp_encoder_encode(encoder, nv12_buffer, nv12_size,
                        jpeg_buffer, nv12_size, &jpeg_len);
    
    // Process jpeg_buffer
}

// Free once
free(jpeg_buffer);
free(nv12_buffer);
```

### Error Handling

Always check return values:

```c
RkmppStatus status = rkmpp_encoder_encode(...);
if (status != RKMPP_OK) {
    fprintf(stderr, "Error: %s\n", rkmpp_get_error_string(status));
    // Handle error appropriately
}
```

### Statistics Monitoring

Track performance:

```c
uint64_t frames_encoded, bytes_encoded;
rkmpp_encoder_get_stats(encoder, &frames_encoded, &bytes_encoded);

double avg_size = (double)bytes_encoded / frames_encoded;
printf("Average frame size: %.2f KB\n", avg_size / 1024.0);
```

## Troubleshooting

### Common Issues

**Issue: Library not found**

```
error: cannot find -lrkmpp_mjpeg
```

**Solution:** Ensure the library is installed and LD_LIBRARY_PATH is set:

```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

**Issue: Encoder/Decoder creation fails**

**Solution:** Check configuration parameters:
- Width/height must be between 16 and 4096
- FPS must be between 1 and 120
- Quality must be between 0 and 100

**Issue: Encoding produces no output**

**Solution:** Ensure:
- Input buffer is properly filled with NV12 data
- Output buffer is large enough
- Encoder is properly initialized

### Debug Output

Enable debug information by checking error codes:

```c
RkmppStatus status = rkmpp_encoder_create(&config);
if (!encoder) {
    printf("Encoder creation failed\n");
    printf("Error: %s\n", rkmpp_get_error_string(status));
}
```

## Performance Tips

1. **Reuse buffers** to avoid repeated allocations
2. **Use appropriate quality settings** (80 is a good default)
3. **Monitor statistics** to identify bottlenecks
4. **Consider resolution** - lower resolutions encode faster
5. **Use hardware acceleration** - ensure MPP driver is loaded

## Next Steps

- Review the [API Reference](API_REFERENCE.md) for detailed function documentation
- Check the test cases in the `test/` directory for more examples
- Explore the source code in `src/` for implementation details

