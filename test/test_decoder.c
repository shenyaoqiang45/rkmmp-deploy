/*
 * MJPEG Decoder Test Cases
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rkmpp_mjpeg.h"

/* Test utilities */
#define TEST_PASS(name) printf("✓ PASS: %s\n", name)
#define TEST_FAIL(name) printf("✗ FAIL: %s\n", name)

/**
 * Test 1: Create and destroy decoder
 */
void test_decoder_create_destroy(void)
{
    RkmppDecoderConfig config = {
        .max_width = 1920,
        .max_height = 1080,
        .output_format = 0
    };
    
    RkmppDecoder* decoder = rkmpp_decoder_create(&config);
    if (!decoder) {
        TEST_FAIL("decoder_create_destroy");
        return;
    }
    
    RkmppStatus status = rkmpp_decoder_destroy(decoder);
    if (status != RKMPP_OK) {
        TEST_FAIL("decoder_create_destroy");
        return;
    }
    
    TEST_PASS("decoder_create_destroy");
}

/**
 * Test 2: Invalid configuration
 */
void test_decoder_invalid_config(void)
{
    /* Test NULL config */
    RkmppDecoder* decoder = rkmpp_decoder_create(NULL);
    if (decoder != NULL) {
        TEST_FAIL("decoder_invalid_config (NULL config)");
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    /* Test invalid resolution (too small) */
    RkmppDecoderConfig config = {
        .max_width = 8,
        .max_height = 8,
        .output_format = 0
    };
    
    decoder = rkmpp_decoder_create(&config);
    if (decoder != NULL) {
        TEST_FAIL("decoder_invalid_config (small resolution)");
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    /* Test invalid resolution (too large) */
    config.max_width = 8192;
    config.max_height = 8192;
    
    decoder = rkmpp_decoder_create(&config);
    if (decoder != NULL) {
        TEST_FAIL("decoder_invalid_config (large resolution)");
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    TEST_PASS("decoder_invalid_config");
}

/**
 * Test 3: Decode frame
 */
void test_decoder_decode_frame(void)
{
    RkmppDecoderConfig config = {
        .max_width = 640,
        .max_height = 480,
        .output_format = 0
    };
    
    RkmppDecoder* decoder = rkmpp_decoder_create(&config);
    if (!decoder) {
        TEST_FAIL("decoder_decode_frame");
        return;
    }
    
    /* Allocate JPEG buffer (mock JPEG data) */
    uint32_t jpeg_size = 10000;
    uint8_t* jpeg_data = (uint8_t*)malloc(jpeg_size);
    if (!jpeg_data) {
        TEST_FAIL("decoder_decode_frame");
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    /* Fill with test pattern */
    memset(jpeg_data, 0xFF, jpeg_size);
    
    /* Allocate NV12 output buffer */
    uint32_t nv12_size = rkmpp_get_nv12_size(config.max_width, config.max_height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    if (!nv12_data) {
        TEST_FAIL("decoder_decode_frame");
        free(jpeg_data);
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    uint32_t nv12_len = 0;
    RkmppFrameInfo frame_info = {0};
    
    RkmppStatus status = rkmpp_decoder_decode(
        decoder, jpeg_data, jpeg_size,
        nv12_data, nv12_size, &nv12_len, &frame_info
    );
    
    if (status != RKMPP_OK || nv12_len == 0) {
        TEST_FAIL("decoder_decode_frame");
        free(nv12_data);
        free(jpeg_data);
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    free(nv12_data);
    free(jpeg_data);
    rkmpp_decoder_destroy(decoder);
    
    TEST_PASS("decoder_decode_frame");
}

/**
 * Test 4: Decode with invalid parameters
 */
void test_decoder_decode_invalid(void)
{
    RkmppDecoderConfig config = {
        .max_width = 640,
        .max_height = 480,
        .output_format = 0
    };
    
    RkmppDecoder* decoder = rkmpp_decoder_create(&config);
    if (!decoder) {
        TEST_FAIL("decoder_decode_invalid");
        return;
    }
    
    uint32_t jpeg_size = 10000;
    uint8_t* jpeg_data = (uint8_t*)malloc(jpeg_size);
    uint32_t nv12_size = rkmpp_get_nv12_size(config.max_width, config.max_height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    uint32_t nv12_len = 0;
    RkmppFrameInfo frame_info = {0};
    
    /* Test NULL decoder */
    RkmppStatus status = rkmpp_decoder_decode(
        NULL, jpeg_data, jpeg_size,
        nv12_data, nv12_size, &nv12_len, &frame_info
    );
    if (status != RKMPP_ERR_INVALID_PARAM) {
        TEST_FAIL("decoder_decode_invalid (NULL decoder)");
        free(nv12_data);
        free(jpeg_data);
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    /* Test NULL JPEG data */
    status = rkmpp_decoder_decode(
        decoder, NULL, jpeg_size,
        nv12_data, nv12_size, &nv12_len, &frame_info
    );
    if (status != RKMPP_ERR_INVALID_PARAM) {
        TEST_FAIL("decoder_decode_invalid (NULL JPEG data)");
        free(nv12_data);
        free(jpeg_data);
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    /* Test zero JPEG size */
    status = rkmpp_decoder_decode(
        decoder, jpeg_data, 0,
        nv12_data, nv12_size, &nv12_len, &frame_info
    );
    if (status != RKMPP_ERR_INVALID_PARAM) {
        TEST_FAIL("decoder_decode_invalid (zero JPEG size)");
        free(nv12_data);
        free(jpeg_data);
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    free(nv12_data);
    free(jpeg_data);
    rkmpp_decoder_destroy(decoder);
    
    TEST_PASS("decoder_decode_invalid");
}

/**
 * Test 5: Get decoder statistics
 */
void test_decoder_get_stats(void)
{
    RkmppDecoderConfig config = {
        .max_width = 640,
        .max_height = 480,
        .output_format = 0
    };
    
    RkmppDecoder* decoder = rkmpp_decoder_create(&config);
    if (!decoder) {
        TEST_FAIL("decoder_get_stats");
        return;
    }
    
    uint64_t frames = 0, bytes = 0;
    RkmppStatus status = rkmpp_decoder_get_stats(decoder, &frames, &bytes);
    
    if (status != RKMPP_OK) {
        TEST_FAIL("decoder_get_stats");
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    /* Initial stats should be zero */
    if (frames != 0 || bytes != 0) {
        TEST_FAIL("decoder_get_stats (initial stats)");
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    /* Decode a frame and check stats */
    uint32_t jpeg_size = 10000;
    uint8_t* jpeg_data = (uint8_t*)malloc(jpeg_size);
    uint32_t nv12_size = rkmpp_get_nv12_size(config.max_width, config.max_height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    uint32_t nv12_len = 0;
    RkmppFrameInfo frame_info = {0};
    
    memset(jpeg_data, 0xFF, jpeg_size);
    
    status = rkmpp_decoder_decode(
        decoder, jpeg_data, jpeg_size,
        nv12_data, nv12_size, &nv12_len, &frame_info
    );
    
    if (status == RKMPP_OK) {
        status = rkmpp_decoder_get_stats(decoder, &frames, &bytes);
        if (status != RKMPP_OK || frames != 1 || bytes == 0) {
            TEST_FAIL("decoder_get_stats (after decode)");
            free(nv12_data);
            free(jpeg_data);
            rkmpp_decoder_destroy(decoder);
            return;
        }
    }
    
    free(nv12_data);
    free(jpeg_data);
    rkmpp_decoder_destroy(decoder);
    
    TEST_PASS("decoder_get_stats");
}

/**
 * Test 6: Frame info validation
 */
void test_decoder_frame_info(void)
{
    RkmppDecoderConfig config = {
        .max_width = 640,
        .max_height = 480,
        .output_format = 0
    };
    
    RkmppDecoder* decoder = rkmpp_decoder_create(&config);
    if (!decoder) {
        TEST_FAIL("decoder_frame_info");
        return;
    }
    
    uint32_t jpeg_size = 10000;
    uint8_t* jpeg_data = (uint8_t*)malloc(jpeg_size);
    uint32_t nv12_size = rkmpp_get_nv12_size(config.max_width, config.max_height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    uint32_t nv12_len = 0;
    RkmppFrameInfo frame_info = {0};
    
    memset(jpeg_data, 0xFF, jpeg_size);
    
    RkmppStatus status = rkmpp_decoder_decode(
        decoder, jpeg_data, jpeg_size,
        nv12_data, nv12_size, &nv12_len, &frame_info
    );
    
    if (status != RKMPP_OK) {
        TEST_FAIL("decoder_frame_info");
        free(nv12_data);
        free(jpeg_data);
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    /* Validate frame info */
    if (frame_info.width == 0 || frame_info.height == 0) {
        TEST_FAIL("decoder_frame_info (invalid dimensions)");
        free(nv12_data);
        free(jpeg_data);
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    free(nv12_data);
    free(jpeg_data);
    rkmpp_decoder_destroy(decoder);
    
    TEST_PASS("decoder_frame_info");
}

/**
 * Test 7: Multiple resolutions
 */
void test_decoder_multiple_resolutions(void)
{
    uint32_t resolutions[][2] = {
        {320, 240},
        {640, 480},
        {1280, 720},
        {1920, 1080},
        {2560, 1440}
    };
    
    int num_resolutions = sizeof(resolutions) / sizeof(resolutions[0]);
    
    for (int i = 0; i < num_resolutions; i++) {
        RkmppDecoderConfig config = {
            .max_width = resolutions[i][0],
            .max_height = resolutions[i][1],
            .output_format = 0
        };
        
        RkmppDecoder* decoder = rkmpp_decoder_create(&config);
        if (!decoder) {
            printf("✗ FAIL: decoder_multiple_resolutions (%ux%u)\n",
                   config.max_width, config.max_height);
            return;
        }
        
        rkmpp_decoder_destroy(decoder);
    }
    
    TEST_PASS("decoder_multiple_resolutions");
}

/**
 * Run all decoder tests
 */
int main(int argc, char* argv[])
{
    printf("=== RKMPP MJPEG Decoder Test Suite ===\n\n");
    
    test_decoder_create_destroy();
    test_decoder_invalid_config();
    test_decoder_decode_frame();
    test_decoder_decode_invalid();
    test_decoder_get_stats();
    test_decoder_frame_info();
    test_decoder_multiple_resolutions();
    
    printf("\n=== Tests Complete ===\n");
    
    return 0;
}
