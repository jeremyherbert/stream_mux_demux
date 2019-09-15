## stream_mux_demux

The code in this repository is a simple C library that allows multiplexing and demultiplexing of 256 individual byte streams 
(called "channels") over a single byte stream. The intended usage is in things like microcontroller UARTs. This implementation
uses only static allocation methods (you pass a pointer to a work buffer).

There are no checksumming or reliability mechanisms in place.

See main.cpp for test code.