/* SPDX-License-Identifier: MIT */

#include <vector>
#include <ctime>
#include <cstdlib>
#include <cassert>

#include "stream_mux_demux.h"

std::vector<std::vector<uint8_t>> output_data(256);
std::vector<std::vector<uint8_t>> test_data(256);
std::vector<size_t> test_data_index(256, 0);

void mux_output_callback(const uint8_t *payload, size_t payload_len, void *callback_param) {
    auto *demux = (demux_context_t *)callback_param;

  //  printf("got multiplexed data:\n");
//    for (size_t i=0; i<payload_len; i++) {
//        printf("%02X ", payload[i]);
//    }
//    printf("\n\n");

    demux_input(demux, payload, payload_len);
}

void demux_output_callback(uint8_t channel, const uint8_t *payload, size_t payload_len, void *callback_param) {
   // printf("pushing %i bytes to channel %i\n", payload_len, channel);
    for (size_t i=0; i<payload_len; i++) {
//        printf("%02X ", payload[i]);
        output_data[channel].push_back(payload[i]);
    }
//    printf("\n\n");
}

int main() {
    uint8_t demux_buffer[10];
    demux_context_t demux;
    demux_init(&demux, &demux_buffer[0], 10, demux_output_callback, nullptr);

    uint8_t mux_buffer[10];
    mux_context_t mux;
    mux_init(&mux, &mux_buffer[0], 10, mux_output_callback, &demux);

    srand(time(nullptr));

    uint8_t data_to_send[25];

    while (1) {
        for (size_t i=0; i<256; i++) {
            output_data[i].clear();
            test_data[i].clear();
            test_data_index[i] = 0;

            // generate random data to send
            size_t size = (rand() % 99) + 1;
            test_data[i].resize(size);
            for (size_t j=0; j<size; j++) {
                test_data[i][j] = rand() % 256;
            }
        }

        while (1) {
            for (size_t i=1; i<256; i++) {
                size_t size_to_send = rand() % 25;

                if (size_to_send > (test_data[i].size() - test_data_index[i])) {
                    size_to_send = test_data[i].size() - test_data_index[i];
                }

               // if (size_to_send > 0) {
                    //printf("input from channel %i of size %i\n", i, size_to_send);
                    for (size_t j=0; j<size_to_send; j++) {
                        assert(i < 256);
                        data_to_send[j] = test_data[i][test_data_index[i] + j];
                        //printf("%02X ", data_to_send[i]);
                    }
                    //printf("\n\n");

                    mux_input(&mux, i, &data_to_send[0], size_to_send, false);
                    test_data_index[i] += size_to_send;
                //}
            }

            bool all_done = true;
            for (size_t i=1; i<256; i++) {
                if (test_data[i].size() != test_data_index[i]) {
                    all_done = false;
                }
            }
            if (all_done) break;
        }

        for (size_t i=1; i<256; i++) {
            assert(test_data[i] == output_data[i]);
            assert(!test_data[i].empty());
        }
        //printf("OK!\n");
    }

    return 0;
}