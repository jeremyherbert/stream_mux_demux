#include <stdbool.h>
#include <stdint.h>
#include "stream_mux_demux.h"

static inline void demux_flush_output(demux_context_t *ctx) {
    if (ctx->working_buffer_size) {
        ctx->output_callback(ctx->current_output_channel, ctx->working_buffer, ctx->working_buffer_size, ctx->callback_param);
        ctx->working_buffer_size = 0;
    }
}

static inline void demux_push_to_outbuf(demux_context_t *ctx, uint8_t byte) {
    ctx->working_buffer[ctx->working_buffer_size++] = byte;

    if (ctx->working_buffer_size == ctx->working_buffer_max_size) {
        demux_flush_output(ctx);
    }
}

void demux_init(demux_context_t *ctx, uint8_t *working_buffer, size_t working_buffer_size, demux_output_callback_t output_callback, void *callback_param) {
    ctx->working_buffer = working_buffer;
    ctx->working_buffer_size = 0;
    ctx->working_buffer_max_size = working_buffer_size;
    ctx->output_callback = output_callback;
    ctx->current_output_channel = 0;
    ctx->callback_param = callback_param;

    ctx->next_byte_is_escaped = false;
    ctx->next_byte_is_channel_change = false;
}

void demux_input(demux_context_t *ctx, const uint8_t *payload, size_t payload_len) {
    if (!payload_len) return;

    for (size_t i=0; i<payload_len; i++) {
        if (payload[i] == STREAM_MUX_DEMUX_CHANNEL_CHANGE_MARKER) {
            ctx->next_byte_is_channel_change = true;
            demux_flush_output(ctx);
        } else if (payload[i] == STREAM_MUX_DEMUX_ESCAPE_MARKER) {
            ctx->next_byte_is_escaped = true;
        } else {
            uint8_t byte;
            if (ctx->next_byte_is_escaped) {
                byte = payload[i] ^ (1 << 5);
                ctx->next_byte_is_escaped = false;
            } else {
                byte = payload[i];
            }

            if (ctx->next_byte_is_channel_change) {
                ctx->current_output_channel = byte;
                ctx->next_byte_is_channel_change = false;
            } else {
                demux_push_to_outbuf(ctx, byte);
            }
        }
    }

    demux_flush_output(ctx);
}