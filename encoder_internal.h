/*
 * MJPEG Encoder Internal Implementation
 */

#ifndef ENCODER_INTERNAL_H
#define ENCODER_INTERNAL_H

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
 * Internal encoder context structure
 */
struct RkmppEncoder {
    /* MPP core objects */
    MppCtx* mpp_ctx;                   /* MPP context */
    MppApi* mpi;                       /* MPP interface */
    MppBufferGroup* frame_group;       /* Buffer group for input frames */
    MppBufferGroup* packet_group;      /* Buffer group for output packets */
    
    /* Configuration */
    uint32_t width;
    uint32_t height;
    uint32_t fps;
    uint32_t bitrate;
    uint32_t quality;
    
    /* Statistics */
    uint64_t frames_encoded;
    uint64_t bytes_encoded;
    
    /* Synchronization */
    pthread_mutex_t lock;
    
    /* State */
    int initialized;
    int eos_sent;
};

/**
 * Initialize encoder internal structures
 */
int encoder_init_mpp(struct RkmppEncoder* encoder);

/**
 * Configure encoder parameters
 */
int encoder_configure(struct RkmppEncoder* encoder);

/**
 * Cleanup encoder resources
 */
void encoder_cleanup_mpp(struct RkmppEncoder* encoder);

#endif /* ENCODER_INTERNAL_H */
