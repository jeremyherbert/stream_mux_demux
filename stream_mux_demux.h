//
// Created by Jeremy Herbert on 14/9/19.
//

#ifndef STREAM_MUX_DEMUX_STREAM_MUX_DEMUX_H
#define STREAM_MUX_DEMUX_STREAM_MUX_DEMUX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define STREAM_MUX_DEMUX_CHANNEL_CHANGE_MARKER 0x8E
#define STREAM_MUX_DEMUX_ESCAPE_MARKER 0x8D

/***************/

typedef void (*mux_output_callback_t)(const uint8_t *payload, size_t payload_len, void *callback_param);

typedef struct {
    mux_output_callback_t output_callback;
    uint8_t current_output_channel;
    void *callback_param;

    uint8_t *working_buffer;
    size_t working_buffer_size;
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
    size_t working_buffer_size;
    size_t working_buffer_max_size;

    bool next_byte_is_escaped;
    bool next_byte_is_channel_change;
} demux_context_t;

/***************/

void
mux_init(mux_context_t *ctx, uint8_t *working_buffer, size_t working_buffer_size, mux_output_callback_t output_callback,
         void *callback_param);
void mux_input(mux_context_t *ctx, uint8_t channel, const uint8_t *payload, size_t payload_len,
               bool force_send_channel_change_msg);

void demux_init(demux_context_t *ctx, uint8_t *working_buffer, size_t working_buffer_size,
                demux_output_callback_t output_callback, void *callback_param);
void demux_input(demux_context_t *ctx, const uint8_t *payload, size_t payload_len);

#ifdef __cplusplus
};
#endif

#endif //STREAM_MUX_DEMUX_STREAM_MUX_DEMUX_H
