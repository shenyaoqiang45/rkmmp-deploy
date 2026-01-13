/*
 * Integration Tests - Encoder/Decoder Pipeline
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rkmpp_mjpeg.h"

#define TEST_PASS(name) printf("✓ PASS: %s\n", name)
#define TEST_FAIL(name) printf("✗ FAIL: %s\n", name)

/**
 * Test 1: Encode-Decode pipeline
 */
void test_encode_decode_pipeline(void)
{
    uint32_t width = 640;
    uint32_t height = 480;
    
    /* Create encoder */
    RkmppEncoderConfig enc_config = {
        .width = width,
        .height = height,
        .fps = 30,
        .bitrate = 0,
        .quality = 80,
        .gop = 0
    };
    
    RkmppEncoder* encoder = rkmpp_encoder_create(&enc_config);
    if (!encoder) {
        TEST_FAIL("encode_decode_pipeline (encoder creation)");
        return;
    }
    
    /* Create decoder */
    RkmppDecoderConfig dec_config = {
        .max_width = width,
        .max_height = height,
        .output_format = 0
    };
    
    RkmppDecoder* decoder = rkmpp_decoder_create(&dec_config);
    if (!decoder) {
        TEST_FAIL("encode_decode_pipeline (decoder creation)");
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Allocate buffers */
    uint32_t nv12_size = rkmpp_get_nv12_size(width, height);
    uint8_t* nv12_input = (uint8_t*)malloc(nv12_size);
    uint8_t* jpeg_buffer = (uint8_t*)malloc(nv12_size);
    uint8_t* nv12_output = (uint8_t*)malloc(nv12_size);
    
    if (!nv12_input || !jpeg_buffer || !nv12_output) {
        TEST_FAIL("encode_decode_pipeline (buffer allocation)");
        free(nv12_output);
        free(jpeg_buffer);
        free(nv12_input);
        rkmpp_decoder_destroy(decoder);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Fill input with test pattern */
    memset(nv12_input, 64, nv12_size);
    
    /* Encode */
    uint32_t jpeg_len = 0;
    RkmppStatus status = rkmpp_encoder_encode(
        encoder, nv12_input, nv12_size,
        jpeg_buffer, nv12_size, &jpeg_len
    );
    
    if (status != RKMPP_OK || jpeg_len == 0) {
        TEST_FAIL("encode_decode_pipeline (encoding)");
        free(nv12_output);
        free(jpeg_buffer);
        free(nv12_input);
        rkmpp_decoder_destroy(decoder);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Decode */
    uint32_t nv12_len = 0;
    RkmppFrameInfo frame_info = {0};
    status = rkmpp_decoder_decode(
        decoder, jpeg_buffer, jpeg_len,
        nv12_output, nv12_size, &nv12_len, &frame_info
    );
    
    if (status != RKMPP_OK || nv12_len == 0) {
        TEST_FAIL("encode_decode_pipeline (decoding)");
        free(nv12_output);
        free(jpeg_buffer);
        free(nv12_input);
        rkmpp_decoder_destroy(decoder);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Validate output */
    if (frame_info.width != width || frame_info.height != height) {
        TEST_FAIL("encode_decode_pipeline (frame dimensions)");
        free(nv12_output);
        free(jpeg_buffer);
        free(nv12_input);
        rkmpp_decoder_destroy(decoder);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    free(nv12_output);
    free(jpeg_buffer);
    free(nv12_input);
    rkmpp_decoder_destroy(decoder);
    rkmpp_encoder_destroy(encoder);
    
    TEST_PASS("encode_decode_pipeline");
}

/**
 * Test 2: Multiple frames encoding
 */
void test_multiple_frames_encoding(void)
{
    uint32_t width = 320;
    uint32_t height = 240;
    int num_frames = 10;
    
    RkmppEncoderConfig config = {
        .width = width,
        .height = height,
        .fps = 30,
        .bitrate = 0,
        .quality = 80,
        .gop = 0
    };
    
    RkmppEncoder* encoder = rkmpp_encoder_create(&config);
    if (!encoder) {
        TEST_FAIL("multiple_frames_encoding");
        return;
    }
    
    uint32_t nv12_size = rkmpp_get_nv12_size(width, height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    uint8_t* jpeg_data = (uint8_t*)malloc(nv12_size);
    
    if (!nv12_data || !jpeg_data) {
        TEST_FAIL("multiple_frames_encoding");
        free(jpeg_data);
        free(nv12_data);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    /* Encode multiple frames */
    for (int i = 0; i < num_frames; i++) {
        memset(nv12_data, (i * 25) % 256, nv12_size);
        
        uint32_t jpeg_len = 0;
        RkmppStatus status = rkmpp_encoder_encode(
            encoder, nv12_data, nv12_size,
            jpeg_data, nv12_size, &jpeg_len
        );
        
        if (status != RKMPP_OK) {
            TEST_FAIL("multiple_frames_encoding");
            free(jpeg_data);
            free(nv12_data);
            rkmpp_encoder_destroy(encoder);
            return;
        }
    }
    
    /* Check statistics */
    uint64_t frames_encoded = 0, bytes_encoded = 0;
    RkmppStatus status = rkmpp_encoder_get_stats(encoder, &frames_encoded, &bytes_encoded);
    
    if (status != RKMPP_OK || frames_encoded != num_frames) {
        TEST_FAIL("multiple_frames_encoding (stats)");
        free(jpeg_data);
        free(nv12_data);
        rkmpp_encoder_destroy(encoder);
        return;
    }
    
    free(jpeg_data);
    free(nv12_data);
    rkmpp_encoder_destroy(encoder);
    
    TEST_PASS("multiple_frames_encoding");
}

/**
 * Test 3: Multiple frames decoding
 */
void test_multiple_frames_decoding(void)
{
    uint32_t width = 320;
    uint32_t height = 240;
    int num_frames = 10;
    
    RkmppDecoderConfig config = {
        .max_width = width,
        .max_height = height,
        .output_format = 0
    };
    
    RkmppDecoder* decoder = rkmpp_decoder_create(&config);
    if (!decoder) {
        TEST_FAIL("multiple_frames_decoding");
        return;
    }
    
    uint32_t jpeg_size = 10000;
    uint8_t* jpeg_data = (uint8_t*)malloc(jpeg_size);
    uint32_t nv12_size = rkmpp_get_nv12_size(width, height);
    uint8_t* nv12_data = (uint8_t*)malloc(nv12_size);
    
    if (!jpeg_data || !nv12_data) {
        TEST_FAIL("multiple_frames_decoding");
        free(nv12_data);
        free(jpeg_data);
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    /* Decode multiple frames */
    for (int i = 0; i < num_frames; i++) {
        memset(jpeg_data, (i * 25) % 256, jpeg_size);
        
        uint32_t nv12_len = 0;
        RkmppFrameInfo frame_info = {0};
        RkmppStatus status = rkmpp_decoder_decode(
            decoder, jpeg_data, jpeg_size,
            nv12_data, nv12_size, &nv12_len, &frame_info
        );
        
        if (status != RKMPP_OK) {
            TEST_FAIL("multiple_frames_decoding");
            free(nv12_data);
            free(jpeg_data);
            rkmpp_decoder_destroy(decoder);
            return;
        }
    }
    
    /* Check statistics */
    uint64_t frames_decoded = 0, bytes_decoded = 0;
    RkmppStatus status = rkmpp_decoder_get_stats(decoder, &frames_decoded, &bytes_decoded);
    
    if (status != RKMPP_OK || frames_decoded != num_frames) {
        TEST_FAIL("multiple_frames_decoding (stats)");
        free(nv12_data);
        free(jpeg_data);
        rkmpp_decoder_destroy(decoder);
        return;
    }
    
    free(nv12_data);
    free(jpeg_data);
    rkmpp_decoder_destroy(decoder);
    
    TEST_PASS("multiple_frames_decoding");
}

/**
 * Test 4: Utility functions
 */
void test_utility_functions(void)
{
    /* Test NV12 size calculation */
    uint32_t size_640_480 = rkmpp_get_nv12_size(640, 480);
    if (size_640_480 != 640 * 480 * 3 / 2) {
        TEST_FAIL("utility_functions (NV12 size)");
        return;
    }
    
    /* Test error string */
    const char* err_str = rkmpp_get_error_string(RKMPP_OK);
    if (!err_str || strcmp(err_str, "Success") != 0) {
        TEST_FAIL("utility_functions (error string)");
        return;
    }
    
    /* Test version */
    const char* version = rkmpp_get_version();
    if (!version || strlen(version) == 0) {
        TEST_FAIL("utility_functions (version)");
        return;
    }
    
    TEST_PASS("utility_functions");
}

/**
 * Test 5: Concurrent encoder/decoder
 */
void test_concurrent_encoder_decoder(void)
{
    uint32_t width = 640;
    uint32_t height = 480;
    
    /* Create multiple encoders and decoders */
    RkmppEncoderConfig enc_config = {
        .width = width,
        .height = height,
        .fps = 30,
        .bitrate = 0,
        .quality = 80,
        .gop = 0
    };
    
    RkmppDecoderConfig dec_config = {
        .max_width = width,
        .max_height = height,
        .output_format = 0
    };
    
    RkmppEncoder* encoders[3];
    RkmppDecoder* decoders[3];
    
    for (int i = 0; i < 3; i++) {
        encoders[i] = rkmpp_encoder_create(&enc_config);
        if (!encoders[i]) {
            TEST_FAIL("concurrent_encoder_decoder");
            for (int j = 0; j < i; j++) {
                rkmpp_encoder_destroy(encoders[j]);
                rkmpp_decoder_destroy(decoders[j]);
            }
            return;
        }
        
        decoders[i] = rkmpp_decoder_create(&dec_config);
        if (!decoders[i]) {
            TEST_FAIL("concurrent_encoder_decoder");
            rkmpp_encoder_destroy(encoders[i]);
            for (int j = 0; j < i; j++) {
                rkmpp_encoder_destroy(encoders[j]);
                rkmpp_decoder_destroy(decoders[j]);
            }
            return;
        }
    }
    
    /* Clean up */
    for (int i = 0; i < 3; i++) {
        rkmpp_encoder_destroy(encoders[i]);
        rkmpp_decoder_destroy(decoders[i]);
    }
    
    TEST_PASS("concurrent_encoder_decoder");
}

/**
 * Run all integration tests
 */
int main(int argc, char* argv[])
{
    printf("=== RKMPP MJPEG Integration Test Suite ===\n\n");
    
    test_encode_decode_pipeline();
    test_multiple_frames_encoding();
    test_multiple_frames_decoding();
    test_utility_functions();
    test_concurrent_encoder_decoder();
    
    printf("\n=== Tests Complete ===\n");
    
    return 0;
}
