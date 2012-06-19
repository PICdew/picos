#ifndef BASE64_H
#define BASE64_H 1

#include <stdio.h>
#include <stdbool.h>

typedef long crc24;

struct base64_stream
{
    crc24 crc;// CRC24 of the stream

    FILE *output;// output off buffer

    char buffer[3];
    char outblock[4];
    size_t bytes_in_buffer;// Number of bytes in the input buffer
    size_t truncated_size;// If the output buffer (when decoding) is truncated, i.e. decoded stream size is not a multiple of 3 bytes, this value will be non-zero, containing the number of bytes to be included in the output buffer.
    bool encode;// true if encoding, false if decoding
};

#endif //BASE_H

