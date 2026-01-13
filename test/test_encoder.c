/*
 * MJPEG Encoder Test Cases
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
 * Test 1: Create and destroy encoder
 */
void test_encoder_create_destroy(void)
{
    RkmppEncoderConfig config = {
        .width = 1920,
        .height = 1080,
        .fps = 30,
        .bitrate = 0,
        .quality = 80,
        .gop = 0
    };
    
    RkmppEncoder* encoder = rkmpp_encoder_create(&config);
    if (!encoder) {
        TEST_FAIL("encoder_create_destroy");
        return;
    }
    
    RkmppStatus status = rkmpp_encoder_destroy(encoder);
    if (status != RKMPP_OK) {
        TEST_FAIL("encoder_create_destroy");
        return;
    }
    
    TEST_PASS("encoder_create_destroy");
}

/**
 * Test 2: Invalid configuration
 */
void test_encoder_invalid_config(void)
{
    /* Test NULL config */
    RkmppEncoder* encoder = rkmpp_encoder_create(NULL);
    if (encoder != NULL) {
        TEST_FAIL("encoder_invalid_config (NULL config)");
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Test invalid resolution (too small) */
    RkmppEncoderConfig config = {
        .width = 8,
        .height = 8,
        .fps = 30,
        .bitrate = 0,
        .quality = 80,
        .gop = 0
    };
    
    encoder = rkmpp_encoder_create(&config);
    if (encoder != NULL) {
        TEST_FAIL("encoder_invalid_config (small resolution)");
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Test invalid FPS */
    config.width = 1920;
    config.height = 1080;
    config.fps = 0;
    
    encoder = rkmpp_encoder_create(&config);
    if (encoder != NULL) {
        TEST_FAIL("encoder_invalid_config (invalid FPS)");
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Test invalid quality */
    config.fps = 30;
    config.quality = 150;
    
    encoder = rkmpp_encoder_create(&config);
    if (encoder != NULL) {
        TEST_FAIL("encoder_invalid_config (invalid quality)");
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    TEST_PASS("encoder_invalid_config");
}

/**
 * Test 3: Encode frame
 */
void test_encoder_encode_frame(void)
{
    RkmppEncoderConfig config = {
        .width = 640,
        .height = 480,
        .fps = 30,
        .bitrate = 0,
        .quality = 80,
        .gop = 0
    };
    
    RkmppEncoder* encoder = rkmpp_encoder_create(&config);
    if (!encoder) {
        TEST_FAIL("encoder_encode_frame");
        return;
    }
    
    /* Allocate NV12 buffer */
    uint32_t nv12_size = rkmpp_get_nv12_size(config.width, config.height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    if (!nv12_data) {
        TEST_FAIL("encoder_encode_frame");
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Fill with test pattern */
    memset(nv12_data, 128, nv12_size);
    
    /* Allocate JPEG output buffer */
    uint8_t* jpeg_data = (uint8_t*)malloc(nv12_size);
    if (!jpeg_data) {
        TEST_FAIL("encoder_encode_frame");
        free(nv12_data);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    uint32_t jpeg_len = 0;
    RkmppStatus status = rkmpp_encoder_encode(
        encoder, nv12_data, nv12_size,
        jpeg_data, nv12_size, &jpeg_len
    );
    
    if (status != RKMPP_OK || jpeg_len == 0) {
        TEST_FAIL("encoder_encode_frame");
        free(jpeg_data);
        free(nv12_data);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    free(jpeg_data);
    free(nv12_data);
    rkmpp_encoder_destroy(encoder);
    
    TEST_PASS("encoder_encode_frame");
}

/**
 * Test 4: Encode with invalid parameters
 */
void test_encoder_encode_invalid(void)
{
    RkmppEncoderConfig config = {
        .width = 640,
        .height = 480,
        .fps = 30,
        .bitrate = 0,
        .quality = 80,
        .gop = 0
    };
    
    RkmppEncoder* encoder = rkmpp_encoder_create(&config);
    if (!encoder) {
        TEST_FAIL("encoder_encode_invalid");
        return;
    }
    
    uint32_t nv12_size = rkmpp_get_nv12_size(config.width, config.height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    uint8_t* jpeg_data = (uint8_t*)malloc(nv12_size);
    uint32_t jpeg_len = 0;
    
    /* Test NULL encoder */
    RkmppStatus status = rkmpp_encoder_encode(
        NULL, nv12_data, nv12_size,
        jpeg_data, nv12_size, &jpeg_len
    );
    if (status != RKMPP_ERR_INVALID_PARAM) {
        TEST_FAIL("encoder_encode_invalid (NULL encoder)");
        free(jpeg_data);
        free(nv12_data);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Test NULL data */
    status = rkmpp_encoder_encode(
        encoder, NULL, nv12_size,
        jpeg_data, nv12_size, &jpeg_len
    );
    if (status != RKMPP_ERR_INVALID_PARAM) {
        TEST_FAIL("encoder_encode_invalid (NULL data)");
        free(jpeg_data);
        free(nv12_data);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Test buffer too small */
    status = rkmpp_encoder_encode(
        encoder, nv12_data, 100,
        jpeg_data, nv12_size, &jpeg_len
    );
    if (status != RKMPP_ERR_INVALID_PARAM) {
        TEST_FAIL("encoder_encode_invalid (buffer too small)");
        free(jpeg_data);
        free(nv12_data);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    free(jpeg_data);
    free(nv12_data);
    rkmpp_encoder_destroy(encoder);
    
    TEST_PASS("encoder_encode_invalid");
}

/**
 * Test 5: Get encoder statistics
 */
void test_encoder_get_stats(void)
{
    RkmppEncoderConfig config = {
        .width = 640,
        .height = 480,
        .fps = 30,
        .bitrate = 0,
        .quality = 80,
        .gop = 0
    };
    
    RkmppEncoder* encoder = rkmpp_encoder_create(&config);
    if (!encoder) {
        TEST_FAIL("encoder_get_stats");
        return;
    }
    
    uint64_t frames = 0, bytes = 0;
    RkmppStatus status = rkmpp_encoder_get_stats(encoder, &frames, &bytes);
    
    if (status != RKMPP_OK) {
        TEST_FAIL("encoder_get_stats");
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Initial stats should be zero */
    if (frames != 0 || bytes != 0) {
        TEST_FAIL("encoder_get_stats (initial stats)");
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Encode a frame and check stats */
    uint32_t nv12_size = rkmpp_get_nv12_size(config.width, config.height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    uint8_t* jpeg_data = (uint8_t*)malloc(nv12_size);
    uint32_t jpeg_len = 0;
    
    memset(nv12_data, 128, nv12_size);
    
    status = rkmpp_encoder_encode(
        encoder, nv12_data, nv12_size,
        jpeg_data, nv12_size, &jpeg_len
    );
    
    if (status == RKMPP_OK) {
        status = rkmpp_encoder_get_stats(encoder, &frames, &bytes);
        if (status != RKMPP_OK || frames != 1 || bytes == 0) {
            TEST_FAIL("encoder_get_stats (after encode)");
            free(jpeg_data);
            free(nv12_data);
            rkmpp_encoder_destroy(encoder);
            return;
        }
    }
    
    free(jpeg_data);
    free(nv12_data);
    rkmpp_encoder_destroy(encoder);
    
    TEST_PASS("encoder_get_stats");
}

/**
 * Test 6: Multiple resolutions
 */
void test_encoder_multiple_resolutions(void)
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
        RkmppEncoderConfig config = {
            .width = resolutions[i][0],
            .height = resolutions[i][1],
            .fps = 30,
            .bitrate = 0,
            .quality = 80,
            .gop = 0
        };
        
        RkmppEncoder* encoder = rkmpp_encoder_create(&config);
        if (!encoder) {
            printf("✗ FAIL: encoder_multiple_resolutions (%ux%u)\n",
                   config.width, config.height);
            return;
        }
        
        rkmpp_encoder_destroy(encoder);
    }
    
    TEST_PASS("encoder_multiple_resolutions");
}

/**
 * Run all encoder tests
 */
int main(int argc, char* argv[])
{
    printf("=== RKMPP MJPEG Encoder Test Suite ===\n\n");
    
    test_encoder_create_destroy();
    test_encoder_invalid_config();
    test_encoder_encode_frame();
    test_encoder_encode_invalid();
    test_encoder_get_stats();
    test_encoder_multiple_resolutions();
    
    printf("\n=== Tests Complete ===\n");
    
    return 0;
}
