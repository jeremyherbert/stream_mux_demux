/* SPDX-License-Identifier: MIT */

#include "stream_mux_demux.h"

static inline void mux_flush_output(mux_context_t *ctx) {
    if (ctx->working_buffer_current_size) {
        ctx->output_callback(ctx->working_buffer, ctx->working_buffer_current_size, ctx->callback_param);
        ctx->working_buffer_current_size = 0;
    }
}

static inline void mux_push_to_outbuf(mux_context_t *ctx, uint8_t byte) {
    ctx->working_buffer[ctx->working_buffer_current_size++] = byte;

    if (ctx->working_buffer_current_size == ctx->working_buffer_max_size) {
        mux_flush_output(ctx);
    }
}

static inline void mux_change_channel(mux_context_t *ctx, uint8_t new_channel) {
    mux_push_to_outbuf(ctx, STREAM_MUX_DEMUX_CHANNEL_CHANGE_MARKER);

    if (new_channel == STREAM_MUX_DEMUX_CHANNEL_CHANGE_MARKER || new_channel == STREAM_MUX_DEMUX_ESCAPE_MARKER) {
        mux_push_to_outbuf(ctx, STREAM_MUX_DEMUX_ESCAPE_MARKER);
        mux_push_to_outbuf(ctx, new_channel ^ (1 << 5));
    } else {
        mux_push_to_outbuf(ctx, new_channel);
    }

    ctx->current_output_channel = new_channel;
}

void mux_init(mux_context_t *ctx, uint8_t *working_buffer, size_t working_buffer_size, mux_output_callback_t output_callback, void *callback_param) {
    ctx->working_buffer = working_buffer;
    ctx->working_buffer_current_size = 0;
    ctx->working_buffer_max_size = working_buffer_size;
    ctx->output_callback = output_callback;
    ctx->current_output_channel = 0;
    ctx->callback_param = callback_param;
}

void mux_input(mux_context_t *ctx, uint8_t channel, const uint8_t *payload, size_t payload_len,
               bool force_send_channel_change_msg) {

    if (force_send_channel_change_msg) {
        mux_change_channel(ctx, ctx->current_output_channel);
        mux_flush_output(ctx);
    }

    if (!payload_len) return;

    // change output channel if necessary
    if (ctx->current_output_channel != channel) {
        mux_change_channel(ctx, channel);
    }

    for (size_t i=0; i<payload_len; i++) {
        if (payload[i] == STREAM_MUX_DEMUX_CHANNEL_CHANGE_MARKER || payload[i] == STREAM_MUX_DEMUX_ESCAPE_MARKER) {
            mux_push_to_outbuf(ctx, STREAM_MUX_DEMUX_ESCAPE_MARKER);
            mux_push_to_outbuf(ctx, payload[i] ^ (1 << 5));
        } else {
            mux_push_to_outbuf(ctx, payload[i]);
        }
    }
    mux_flush_output(ctx);
}
