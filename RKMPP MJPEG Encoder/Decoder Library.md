# RKMPP MJPEG Encoder/Decoder Library

This project provides a C library for hardware-accelerated MJPEG encoding and decoding on Rockchip platforms using the Media Process Platform (MPP). It offers a simple and clean API for converting NV12 frames to MJPEG streams and vice versa.

## Features

- **Hardware Acceleration**: Leverages Rockchip's VPU for high-performance encoding and decoding.
- **NV12 <-> MJPEG**: Supports NV12 as the raw image format and MJPEG as the compressed format.
- **Simple C API**: Easy-to-use functions for creating, configuring, and using encoders and decoders.
- **Cross-Platform**: Designed to be portable across different Rockchip-based systems.
- **Comprehensive Test Suite**: Includes unit and integration tests to ensure correctness and stability.

## Project Structure

```
. 
├── build/                # Build directory
├── docs/                 # Documentation files
├── include/              # Public and internal header files
├── src/                  # Source code for the library
├── test/                 # Test cases
├── CMakeLists.txt        # Main CMake build file
└── README.md             # This file
```

## Building the Library

### Prerequisites

- A C compiler (e.g., GCC)
- CMake (version 3.10 or later)
- The Rockchip MPP library (not included, must be installed on the target system)

### Build Steps

1.  **Clone the repository**:

    ```bash
    git clone <repository_url>
    cd rkmpp_mjpeg_lib
    ```

2.  **Create a build directory**:

    ```bash
    mkdir build
    cd build
    ```

3.  **Run CMake and build**:

    ```bash
    cmake ..
    make
    ```

4.  **Run tests**:

    ```bash
    ctest
    ```

## API Usage

### Encoder Example

```c
#include "rkmpp_mjpeg.h"

// 1. Configure the encoder
 RkmppEncoderConfig enc_config = {
    .width = 1920,
    .height = 1080,
    .fps = 30,
    .quality = 80
};

// 2. Create the encoder
RkmppEncoder* encoder = rkmpp_encoder_create(&enc_config);

// 3. Prepare input and output buffers
uint32_t nv12_size = rkmpp_get_nv12_size(1920, 1080);
uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
uint8_t* jpeg_data = (uint8_t*)malloc(nv12_size);

// ... (fill nv12_data with image data)

// 4. Encode the frame
uint32_t jpeg_len = 0;
RkmppStatus status = rkmpp_encoder_encode(
    encoder, nv12_data, nv12_size,
    jpeg_data, nv12_size, &jpeg_len
);

// 5. Clean up
rkmpp_encoder_destroy(encoder);
free(nv12_data);
free(jpeg_data);
```

### Decoder Example

```c
#include "rkmpp_mjpeg.h"

// 1. Configure the decoder
RkmppDecoderConfig dec_config = {
    .max_width = 1920,
    .max_height = 1080
};

// 2. Create the decoder
RkmppDecoder* decoder = rkmpp_decoder_create(&dec_config);

// 3. Prepare input and output buffers
// ... (load jpeg_data and jpeg_size from a file or network)

// 4. Decode the frame
 RkmppFrameInfo frame_info;
 RkmppStatus status = rkmpp_decoder_decode(
    decoder, jpeg_data, jpeg_size,
    nv12_data, nv12_size, &nv12_len, &frame_info
);

// 5. Clean up
rkmpp_decoder_destroy(decoder);
// ... (free buffers)
```

## Documentation

API documentation can be generated using Doxygen. Ensure Doxygen is installed, then run:

```bash
doxygen Doxyfile
```

The documentation will be generated in the `docs/html` directory.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
