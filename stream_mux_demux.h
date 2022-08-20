/* SPDX-License-Identifier: MIT */

#ifndef STREAM_MUX_DEMUX_STREAM_MUX_DEMUX_H
#define STREAM_MUX_DEMUX_STREAM_MUX_DEMUX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef STREAM_MUX_DEMUX_CHANNEL_CHANGE_MARKER
#define STREAM_MUX_DEMUX_CHANNEL_CHANGE_MARKER 0x8E
#endif

#ifndef STREAM_MUX_DEMUX_ESCAPE_MARKER
#define STREAM_MUX_DEMUX_ESCAPE_MARKER 0x8D
#endif

/***************/

typedef void (*mux_output_callback_t)(const uint8_t *payload, size_t payload_len, void *callback_param);

typedef struct {
    mux_output_callback_t output_callback;
    uint8_t current_output_channel;
    void *callback_param;

    uint8_t *working_buffer;
    size_t working_buffer_current_size;
    size_t working_buffer_max_size;
} mux_context_t;

/***************/

typedef void (*demux_output_callback_t)(uint8_t channel, const uint8_t *payload, size_t payload_len,
                                        void *callback_param);

typedef struct {
    demux_output_callback_t output_callback;
    uint8_t current_output_channel;
    void *callback_param;

    uint8_t *working_buffer;
    size_t working_buffer_current_size;
    size_t working_buffer_max_size;

    bool next_byte_is_escaped;
    bool next_byte_is_channel_change;
} demux_context_t;

/***************/

// init a multiplexer context with a working buffer and an output callback
void mux_init(mux_context_t *ctx, uint8_t *working_buffer, size_t working_buffer_size,
              mux_output_callback_t output_callback, void *callback_param);

// send data from a channel into a multiplexer
void mux_input(mux_context_t *ctx, uint8_t channel, const uint8_t *payload, size_t payload_len,
               bool force_send_channel_change_msg);

// init a demultiplexer with a working buffer and an output callback
void demux_init(demux_context_t *ctx, uint8_t *working_buffer, size_t working_buffer_size,
                demux_output_callback_t output_callback, void *callback_param);

// send multiplexed data into the demultiplexer for extracting into channels
void demux_input(demux_context_t *ctx, const uint8_t *payload, size_t payload_len);

#ifdef __cplusplus
}
#endif

#endif //STREAM_MUX_DEMUX_STREAM_MUX_DEMUX_H
