/*
 * MJPEG Decoder Internal Implementation
 */

#ifndef DECODER_INTERNAL_H
#define DECODER_INTERNAL_H

#include <stdint.h>
#include <pthread.h>

/* Forward declaration */
typedef struct MppCtx MppCtx;
typedef struct MppApi MppApi;
typedef struct MppBuffer MppBuffer;
typedef struct MppFrame MppFrame;
typedef struct MppPacket MppPacket;
typedef struct MppBufferGroup MppBufferGroup;

/**
 * Internal decoder context structure
 */
struct RkmppDecoder {
    /* MPP core objects */
    MppCtx* mpp_ctx;                   /* MPP context */
    MppApi* mpi;                       /* MPP interface */
    MppBufferGroup* frame_group;       /* Buffer group for output frames */
    MppBufferGroup* packet_group;      /* Buffer group for input packets */
    
    /* Configuration */
    uint32_t max_width;
    uint32_t max_height;
    uint32_t output_format;
    
    /* Statistics */
    uint64_t frames_decoded;
    uint64_t bytes_decoded;
    
    /* Synchronization */
    pthread_mutex_t lock;
    
    /* State */
    int initialized;
    int eos_received;
};

/**
 * Initialize decoder internal structures
 */
int decoder_init_mpp(struct RkmppDecoder* decoder);

/**
 * Configure decoder parameters
 */
int decoder_configure(struct RkmppDecoder* decoder);

/**
 * Cleanup decoder resources
 */
void decoder_cleanup_mpp(struct RkmppDecoder* decoder);

#endif /* DECODER_INTERNAL_H */
