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
    size_t bytes_in_buffer; 
    bool encode;// true if encoding, false if decoding
};

#endif //BASE_H

